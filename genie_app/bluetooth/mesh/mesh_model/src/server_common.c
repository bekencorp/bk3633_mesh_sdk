// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <errno.h>

#include "aos/kernel.h"
#include "api/mesh.h"
#include "mesh_config.h"
#include "access.h"
#include "server_common.h"
#if CONFIG_BLE_MESH_TIME_SCENE_SERVER

/**
 * According to Mesh Model Spec:
 * If the Transition Time field is not present and the Generic Default Transition
 * Time state is supported, the Generic Default Transition Time state shall be
 * used. Otherwise the transition shall be instantaneous.
 */
#define INSTANTANEOUS_TRANS_TIME      0

struct net_buf_simple *bt_mesh_alloc_buf(uint16_t size)
{
    struct net_buf_simple *buf = NULL;
    uint8_t *data = NULL;

    buf = (struct net_buf_simple *)aos_calloc(1, sizeof(struct net_buf_simple) + size);
    if (!buf) {
        BT_ERR("%s, Out of memory", __func__);
        return NULL;
    }

    data = (uint8_t *)buf + sizeof(struct net_buf_simple);

    buf->data = data;
    buf->len = 0;
    buf->size = size;
    return buf;
}

void bt_mesh_free_buf(struct net_buf_simple *buf)
{
    if (buf) {
        aos_free(buf);
    }
}


void bt_mesh_server_alloc_ctx(struct k_work *work)
{
    /**
     * This function is used to allocate memory for storing "struct bt_mesh_msg_ctx"
     * of the received messages, because some server models will callback the "struct
     * bt_mesh_msg_ctx" info to the application layer after a certain delay.
     * Here we use the allocated heap memory to store the "struct bt_mesh_msg_ctx".
     */
    __ASSERT(work, "Invalid parameter");
    if (!work->_reserved) {
        work->_reserved = aos_calloc(1, sizeof(struct bt_mesh_msg_ctx));
        __ASSERT(work->_reserved, "Out of memory");
    }
}

bool bt_mesh_is_server_recv_last_msg(struct bt_mesh_last_msg_info *last,
                                     uint8_t tid, uint16_t src, uint16_t dst, int64_t *now)
{
    *now = k_uptime_get();

    /* Currently we only compare msg info which dst is set to a unicast address */
    if (!BT_MESH_ADDR_IS_UNICAST(dst)) {
        return false;
    }

    if (last->tid == tid && last->src == src && last->dst == dst &&
            (*now - last->timestamp <= K_SECONDS(6))) {
        return true;
    }

    return false;
}

void bt_mesh_server_update_last_msg(struct bt_mesh_last_msg_info *last,
                                    uint8_t tid, uint16_t src, uint16_t dst, int64_t *now)
{
    /* Currently we only update msg info which dst is set to a unicast address */
    if (!BT_MESH_ADDR_IS_UNICAST(dst)) {
        return;
    }

    last->tid = tid;
    last->src = src;
    last->dst = dst;
    last->timestamp = *now;
    return;
}

struct net_buf_simple *bt_mesh_server_get_pub_msg(struct bt_mesh_model *model, uint16_t msg_len)
{
    struct net_buf_simple *buf = NULL;

    if (model == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return NULL;
    }

    if (model->pub == NULL || model->pub->msg == NULL ||
            model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        BT_DBG("No publication support, model id 0x%04x", model->id);
        return NULL;
    }

    buf = model->pub->msg;
    if (buf->size < msg_len) {
        BT_ERR("Too small publication msg size %d, model id 0x%04x",
                buf->size, model->id);
        return NULL;
    }

    return buf;
}

int bt_mesh_get_light_lc_trans_time(struct bt_mesh_model *model, uint8_t *trans_time)
{
    struct bt_mesh_light_lc_srv *srv = NULL;
    uint32_t value = 0U;

    if (model == NULL || trans_time == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (model->id != BT_MESH_MODEL_ID_LIGHT_LC_SRV) {
        BT_ERR("Invalid a Light LC Server 0x%04x", model->id);
        return -EINVAL;
    }

    srv = (struct bt_mesh_light_lc_srv *)model->user_data;
    if (srv == NULL) {
        BT_ERR("Invalid Light LC Server user data");
        return -EINVAL;
    }

    /**
     * 1. Set transition time to 0x54 for BQB test case MESH/SR/LLC/BV-04-C.
     *    Light LC Property Set: 0x3C, 0x004E20 -> Light LC Time Run On
     *    Light LC Property Set: 0x37, 0x004E20 -> Light LC Time Fade On
     *    Light LC Property Set: 0x39, 0x004E20 -> Light LC Time Fade Standby Manual
     *
     * 2. Set transition time to 0x0 for BQB test case MESH/SR/LLC/BV-08-C.
     *
     * TODO: Based on Light LC state and choose property property value as the
     * transition time. Currently directly use Light LC Time Run On property value.
     * Unit: Millisecond, range: [0, 16777214(0xFFFFFE)]
     */
    value = srv->lc->prop_state.time_run_on & 0xFFFFFF;

    /**
     * Convert value into Default Transition Time state format.
     * 0b00: 0 ~ 6.2s, 100 millisecond step resolution
     * 0b01: 0 ~ 62s,  1 second step resolution
     * 0b10: 0 ~ 620s, 10 seconds step resolution
     * 0b11: 0 ~ 620m, 10 minutes step resolution
     */
    if (value <= 6200) {
        *trans_time = (0 << 6) | (value / 100);
    } else if (value <= 62000) {
        *trans_time = (1 << 6) | (value / 1000);
    } else if (value <= 620000) {
        *trans_time = (2 << 6) | (value / 10000);
    } else {
        *trans_time = (3 << 6) | (value / 600000);
    }

    return 0;
}

uint8_t bt_mesh_get_default_trans_time(struct bt_mesh_model *model)
{
    /**
     * 1. If a Generic Default Transition Time Server model is present on the
     *    main element of the model, that model instance shall be used.
     * 2. If a Generic Default Transition Time Server model is not present on
     *    the main element of the model, then the Generic Default Transition
     *    Time Server model instance that is present on the element with the
     *    largest address that is smaller than the address of the main element
     *    of the node shall be used; if no model instance is present on any
     *    element with an address smaller than the address of the main element,
     *    then the Generic Default Transition Time Server is not supported.
     */
    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    struct bt_mesh_gen_def_trans_time_srv *state = NULL;
    uint16_t primary_addr = bt_mesh_primary_addr();
    struct bt_mesh_model *srv = NULL;

    for (uint16_t addr = element->addr; addr >= primary_addr; addr--) {
        element = bt_mesh_elem_find(addr);
        if (element) {
            srv = bt_mesh_model_find(element, BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV);
            if (srv) {
                state = (struct bt_mesh_gen_def_trans_time_srv *)srv->user_data;
                if (state) {
                    return state->state.trans_time;
                }
            }
        }
    }

    return INSTANTANEOUS_TRANS_TIME;
}

void bt_mesh_server_stop_transition(struct bt_mesh_state_transition *transition)
{
    memset(transition, 0x0, offsetof(struct bt_mesh_state_transition, flag));
    if (atomic_test_and_clear_bit(transition->flag, BLE_MESH_TRANS_TIMER_START)) {
        k_delayed_work_cancel(&transition->timer);
    }
}

void bt_mesh_server_calc_remain_time(struct bt_mesh_state_transition *transition)
{
    uint8_t steps = 0U, resolution = 0U;
    int32_t duration_remainder = 0;
    int64_t now = 0;

    if (transition->just_started) {
        transition->remain_time = transition->trans_time;
    } else {
        now = k_uptime_get();
        duration_remainder = transition->total_duration -
                             (now - transition->start_timestamp);
        if (duration_remainder > 620000) {
            /* > 620 seconds -> resolution = 0b11 [10 minutes] */
            resolution = 0x03;
            steps = duration_remainder / 600000;
        } else if (duration_remainder > 62000) {
            /* > 62 seconds -> resolution = 0b10 [10 seconds] */
            resolution = 0x02;
            steps = duration_remainder / 10000;
        } else if (duration_remainder > 6200) {
            /* > 6.2 seconds -> resolution = 0b01 [1 seconds] */
            resolution = 0x01;
            steps = duration_remainder / 1000;
        } else if (duration_remainder > 0) {
            /* <= 6.2 seconds -> resolution = 0b00 [100 ms] */
            resolution = 0x00;
            steps = duration_remainder / 100;
        } else {
            resolution = 0x00;
            steps = 0x00;
        }

        transition->remain_time = (resolution << 6) | steps;
    }
}

int bt_mesh_server_get_optional(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf,
                                uint8_t *trans_time, uint8_t *delay,
                                bool *optional)
{
    if (model == NULL || buf == NULL || trans_time == NULL ||
            delay == NULL || optional == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (buf->len != 0x00 && buf->len != 0x02) {
        BT_ERR("Invalid optional message length %d", buf->len);
        return -EINVAL;
    }

    /* Currently we only get optional msg info which dst is set to a unicast address */
    if (!BT_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
        *trans_time = 0U;
        *delay = 0U;
        *optional = false;
        return 0;
    }

    /* No optional fields are available */
    if (buf->len == 0x00) {
        if (model->id == BT_MESH_MODEL_ID_LIGHT_LC_SRV) {
            /**
             * Both messages(i.e. Light LC OnOff Set/Set Unack) may optionally include
             * a Transition Time field indicating the transition time to the target state.
             * If the Transition Time is not included, the Light LC Server shall use
             * its appropriate transition times defined by the Light LC Property states.
             */
            if (bt_mesh_get_light_lc_trans_time(model, trans_time)) {
                BT_ERR("Failed to get Light LC transition time");
                return -EIO;
            }
        } else {
            *trans_time = bt_mesh_get_default_trans_time(model);
        }
        *delay = 0U;
        *optional = false;
        return 0;
    }

    /* Optional fields are available */
    *trans_time = net_buf_simple_pull_u8(buf);
    if ((*trans_time & 0x3F) == 0x3F) {
        BT_ERR("Invalid Transaction Number of Steps 0x3f");
        return -EINVAL;
    }

    *delay = net_buf_simple_pull_u8(buf);
    *optional = true;
    return 0;
}

void transition_time_values(struct bt_mesh_state_transition *transition,
                                   uint8_t trans_time, uint8_t delay)
{
    transition->trans_time = trans_time;
    transition->delay = delay;

    if (trans_time == 0U) {
        return;
    }

    tt_values_calculator(transition);
    transition->quo_tt = transition->total_duration / transition->counter;
}

void tt_values_calculator(struct bt_mesh_state_transition *transition)
{
    uint8_t steps_multiplier = 0U, resolution = 0U;

    resolution = (transition->trans_time >> 6);
    steps_multiplier = (transition->trans_time & 0x3F);

    switch (resolution) {
    case 0: /* 100ms */
        transition->total_duration = steps_multiplier * 100;
        break;
    case 1: /* 1 second */
        transition->total_duration = steps_multiplier * 1000;
        break;
    case 2: /* 10 seconds */
        transition->total_duration = steps_multiplier * 10000;
        break;
    case 3: /* 10 minutes */
        transition->total_duration = steps_multiplier * 600000;
        break;
    }

    transition->counter = ((float) transition->total_duration / 100);

    if (transition->counter > BLE_MESH_DEVICE_SPECIFIC_RESOLUTION) {
        transition->counter = BLE_MESH_DEVICE_SPECIFIC_RESOLUTION;
    }
}

void bt_mesh_server_start_transition(struct bt_mesh_state_transition *transition)
{
    if (transition->delay) {
        k_delayed_work_submit(&transition->timer, K_MSEC(5 * transition->delay));
        atomic_set_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
    } else {
        k_work_submit(&transition->timer.work);
    }
}
#endif /* CONFIG_BLE_MESH_SERVER_MODEL */
