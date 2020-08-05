/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <hal/hal.h>
#include <bluetooth.h>
#include <api/mesh.h>
#include <api/genie_mesh.h>

#include "genie_app.h"
#include "net.h"
#include "transport.h"

#include "genie_mesh.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "ali_dfu.h"

extern u32_t get_mesh_pbadv_time(void);


#define MESH_TRNSATION_CYCLE 100

#define MESH_PROVISIONING_TIMEOUT 60*1000

static struct bt_mesh_prov prov;
static struct bt_mesh_comp comp;

extern struct bt_mesh_elem elements[];
extern S_ELEM_STATE g_elem_state[];

extern uint8_t get_vendor_element_num(void);
extern void user_prov_complete(u16_t net_idx, u16_t addr);
extern void user_prov_reset(void);

uint8_t g_indication_flag;

static struct k_timer g_pbadv_timer;
static struct k_timer g_prov_timer;
static struct k_timer g_indc_timer;

#ifdef CONFIG_MESH_MODEL_TRANS
static void _mesh_timer_stop(S_MODEL_STATE *p_state)
{
    k_timer_stop(&p_state->delay_timer);
    k_timer_stop(&p_state->trans_timer);
}

static void _mesh_delay_timer_cb(void *p_timer, void *p_arg)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;

    _mesh_timer_stop(p_state);
    genie_event(GENIE_EVT_SDK_DELAY_END, p_arg);
}

static void _clear_trans_para(S_MODEL_STATE *p_state)
{
    p_state->trans = 0;
    p_state->trans_start_time = 0;
    p_state->trans_end_time = 0;
}

static void _mesh_trans_timer_cycle(void *p_timer, void *p_arg)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;

    _mesh_timer_stop(p_state);

    //do cycle
    genie_event(GENIE_EVT_SDK_TRANS_CYCLE, p_arg);
    //BT_DBG(">>>>>%d %d", (u32_t)cur_time, (u32_t)p_elem->state.trans_end_time);

    if(p_state->trans == 0) {
        genie_event(GENIE_EVT_SDK_TRANS_END, p_arg);
    }
    else {
        k_timer_start(&p_state->trans_timer, MESH_TRNSATION_CYCLE);
    }
}

static uint8_t _calc_cur_state(S_ELEM_STATE * p_elem)
{
    S_MODEL_STATE *p_state = &p_elem->state;
    u32_t cur_time = k_uptime_get();
    uint8_t cycle = 0;

    //stop cycle when timeout
    if(cur_time <= p_state->trans_end_time - MESH_TRNSATION_CYCLE) {
        uint16_t step = (p_state->trans_end_time - cur_time)/MESH_TRNSATION_CYCLE;
    
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if(p_state->onoff[T_CUR] == 0 && p_state->onoff[T_TAR] == 1) {
            p_state->onoff[T_CUR] = 1;
        } else if(p_state->onoff[T_CUR] == 1 && p_state->onoff[T_TAR] == 0) {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            //turn off when lightness is zero
            if(p_state->actual[T_CUR] == 0)
#endif
                p_state->onoff[T_CUR] = 0;
        }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if(p_state->actual[T_CUR] != p_state->actual[T_TAR]) {
            if(p_state->actual[T_TAR] > p_state->actual[T_CUR]) {
                p_state->actual[T_CUR] += (p_state->actual[T_TAR]-p_state->actual[T_CUR])/step;
            } else {
                p_state->actual[T_CUR] -= (p_state->actual[T_CUR]-p_state->actual[T_TAR])/step;
            }
            //model_bind_operation(B_LIGHTNESS_ID, p_elem, T_CUR);
            cycle = 1;
        }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if(p_state->temp[T_CUR] != p_state->temp[T_TAR]) {
            if(p_state->temp[T_TAR] > p_state->temp[T_CUR]) {
                p_state->temp[T_CUR] += (p_state->temp[T_TAR]-p_state->temp[T_CUR])/step;
            } else {
                p_state->temp[T_CUR] -= (p_state->temp[T_CUR]-p_state->temp[T_TAR])/step;
            }
            cycle = 1;
        }
#endif
    }
#if 0
    BT_DBG("next: %d->%d|%02x->%02x|%02x->%02x", p_state->onoff[T_CUR], p_state->onoff[T_TAR],
        p_state->actual[T_CUR], p_state->actual[T_TAR], p_state->temp[T_CUR], p_state->temp[T_TAR]);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if(p_state->onoff[T_CUR] == p_state->onoff[T_TAR])
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if(p_state->actual[T_CUR] == p_state->actual[T_TAR])
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if(p_state->temp[T_CUR] == p_state->temp[T_TAR])
#endif
        cycle = 0;

    //BT_DBG("cycle %d", cycle);
    if(cycle == 0) {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        //BT_DBG("onoff %d->%d", p_state->onoff[T_CUR], p_state->onoff[T_TAR]);
        p_state->onoff[T_CUR] = p_state->onoff[T_TAR];
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        //BT_DBG("actual %02x->%02x", p_state->actual[T_CUR], p_state->actual[T_TAR]);
        p_state->actual[T_CUR] = p_state->actual[T_TAR];
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        //BT_DBG("temp %02x->%02x", p_state->temp[T_CUR], p_state->temp[T_TAR]);
        p_state->temp[T_CUR] = p_state->temp[T_TAR];
#endif
        p_state->trans = 0;
    }
    return cycle;
}
#endif

#if 0   //publish
#define MESH_PUB_FLAG_ONOFF 0x01

extern struct bt_mesh_elem elements[];
void mesh_publish(S_ELEM_STATE *p_elem, uint8_t flag)
{
    struct bt_mesh_model *model = NULL;

    if(flag & MESH_PUB_FLAG_ONOFF) {
        model = bt_mesh_model_find(&elements[p_elem->main_elem_num], BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
        gen_onoff_publication(model);
    }
}
#endif


static void _genie_pbadv_timer_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_SDK_MESH_PBADV_TIMEOUT, NULL);
}

static void _genie_pbadv_timer_start(void)
{
    static uint8_t inited = 0;
    u32_t timeout = get_mesh_pbadv_time();

    if(!inited) {
        k_timer_init(&g_pbadv_timer, _genie_pbadv_timer_cb, NULL);
        inited = 1;
    }
    k_timer_start(&g_pbadv_timer, timeout);
}

static void _genie_pbadv_timer_stop(void)
{
    k_timer_stop(&g_pbadv_timer);
}

static void _genie_mesh_disable_adv(void)
{
    bt_mesh_prov_disable(BT_MESH_PROV_ADV);
    bt_mesh_prov_disable(BT_MESH_PROV_GATT);
}

static void _genie_pbadv_start_silent_adv()
{
    genie_tri_tuple_set_silent_adv();
    unprov_beacon_interval_set(K_SECONDS(60));
}

static void _genie_prov_timer_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_SDK_MESH_PROV_TIMEOUT, NULL);
}

static void _genie_prov_timer_start(void)
{
    static uint8_t inited = 0;

    if(!inited) {
        k_timer_init(&g_prov_timer, _genie_prov_timer_cb, NULL);
        inited = 1;
    }
    k_timer_start(&g_prov_timer, MESH_PROVISIONING_TIMEOUT);
}

static void _genie_prov_timer_stop(void)
{
    k_timer_stop(&g_prov_timer);
}

static s16_t _genie_trans_step(S_MODEL_STATE *p_state)
{
#if 0
    s16_t temp_step = 0;
    s16_t actual_step = 0;
    u32_t cur_time = k_uptime_get();

    if (!(p_state->temp_trans_step || p_state->actual_trans_step) ) {
        s32_t temp_delta = 0;
        s32_t actual_delta = 0;
        /* calculate trans_step if in 1st transation cycle */
        if ((cur_time - p_state->trans_start_time) < MESH_TRNSATION_CYCLE) {
            temp_delta = (s32_t) p_state->temp[T_TAR] - (s32_t) p_state->temp[T_CUR];
            if (temp_delta)
                p_state->temp_trans_step =
                    temp_delta / ((u16_t) ((p_state->trans_end_time - p_state->trans_start_time) / MESH_TRNSATION_CYCLE));

            actual_delta = (s32_t) p_state->actual[T_TAR] - (s32_t) p_state->actual[T_CUR];
            if (actual_delta)
                p_state->actual_trans_step =
                    actual_delta / ((u16_t) ((p_state->trans_end_time - p_state->trans_start_time) / MESH_TRNSATION_CYCLE));
            BT_DBG("temp_delta:%d, actual_delta:%d, temp_step:%d, actual_step:%d", temp_delta, actual_delta, p_state->temp_trans_step, p_state->actual_trans_step);
        }
        p_state->trans_last_time = cur_time;

        /* if target and actual equals with each other, return non-value value to indicate that transition is done directly */
        if (temp_delta || actual_delta) // Shall we use actual_trans_step and temp_trans_step instead?
            return 0;
        else
            return 1;
    }


    temp_step = (s16_t)(((float)(cur_time - p_state->trans_last_time)/MESH_TRNSATION_CYCLE) * p_state->temp_trans_step);
    p_state->temp[T_CUR] += temp_step;

    actual_step = (s16_t)(((float)(cur_time - p_state->trans_last_time)/MESH_TRNSATION_CYCLE) * p_state->actual_trans_step);
    p_state->actual[T_CUR] += actual_step;

    p_state->trans_last_time = cur_time;
#endif
    return 0;
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
static void _poweron_indicate_cb(void *p_timer, void *args)
{
    _genie_prov_timer_stop();
    genie_event(GENIE_EVT_SDK_MESH_PWRON_INDC, NULL);
}

static void poweron_indicate_start(void)
{
    static uint8_t inited = 0;
    uint16_t random_time;

    if(!inited) {
        k_timer_init(&g_indc_timer, _poweron_indicate_cb, NULL);
        inited = 1;
    }
    bt_rand(&random_time, 1);
#ifdef CONFIG_MESH_MODEL_TRANS
    random_time = 2000 + 8000*random_time/255;
#else
    random_time = 500 + 9500*random_time/255;
#endif
    BT_DBG("indicate random(%d)ms", random_time);
    k_timer_start(&g_indc_timer, random_time);
}
#endif
static u16_t _poweron_indicate_dev_up_event (void)
{
    vnd_model_msg reply_msg;
    uint8_t payload[3] = {0};

    payload[0] = DEVICE_EVENT_T & 0xff;
    payload[1] = (DEVICE_EVENT_T >> 8) & 0xff;
    payload[2] = (uint8_t)EL_DEV_UP_T;

    reply_msg.opid = VENDOR_OP_ATTR_INDICATE;
    reply_msg.tid = 0;
    reply_msg.data = payload;
    reply_msg.len = 3;
    reply_msg.p_elem = elements;
    reply_msg.retry = VENDOR_MODEL_MSG_DFT_RETRY_TIMES;

    genie_vendor_model_msg_send(&reply_msg);

    return 0;
}


static u16_t _genie_indicate_hw_reset_event (void)
{
    vnd_model_msg reply_msg;
    uint8_t payload[3] = {0};

    payload[0] = DEVICE_EVENT_T & 0xff;
    payload[1] = (DEVICE_EVENT_T >> 8) & 0xff;
    payload[2] = (uint8_t)EL_HW_RESET_T;

    genie_vendor_model_msg_send(&reply_msg);
    reply_msg.opid = VENDOR_OP_ATTR_INDICATE;
    reply_msg.tid = 0;
    reply_msg.data = payload;
    reply_msg.len = 3;
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

    genie_vendor_model_msg_send(&reply_msg);

    return 0;
}

static u16_t genie_vnd_msg_handle(vnd_model_msg *p_msg){
    uint8_t *p_data = NULL;
    printk("vendor model message received\n");
    if (!p_msg)
        return -1;
    p_data = p_msg->data;
    printk("opcode:0x%x, tid:%d, len:%d", p_msg->opid, p_msg->tid, p_msg->len);
    if (p_data && p_msg->len)
        printk("payload: %s", bt_hex(p_data, p_msg->len));

    switch (p_msg->opid) {
        case VENDOR_OP_ATTR_INDICATE:
        {
            u16_t attr_type = *p_data++ + (*p_data++ << 8);
            if (attr_type == DEVICE_EVENT_T) {
                uint8_t event_id = *p_data;
                switch (event_id) {
                    case EL_DEV_UP_T:
                        poweron_indicate_start(); // When receiving genie's device up status, indication device's state and device up event in sequence
                        break;
                    default:
                        break;
                }
            }

            break;
        }
        default:
            break;
    }

    return 0;
}

#if 0
uint8_t vendor_indication_buff(S_ELEM_STATE *p_elem_state, uint8_t *p_buff, uint8_t len)
{
    vnd_model_msg reply_msg;

    reply_msg.opid = VENDOR_OP_ATTR_INDICATE;
    reply_msg.tid = 0;
    reply_msg.data = p_buff;
    reply_msg.len = len;
    reply_msg.p_elem = &elements[p_elem_state->elem_index];
    reply_msg.retry = VENDOR_MODEL_MSG_DFT_RETRY_TIMES;

    genie_vendor_model_msg_send(&reply_msg);
    return 0;
}
#endif

void mdoel_standart_event(S_ELEM_STATE *p_elem)
{
    E_GENIE_EVENT next_event = GENIE_EVT_SDK_ACTION_DONE;

    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (void *)p_elem);

#ifdef CONFIG_MESH_MODEL_TRANS
    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            next_event = GENIE_EVT_SDK_DELAY_START;
        } else {
            next_event = GENIE_EVT_SDK_TRANS_START;
        }
    }
#endif
    genie_event(next_event, (void *)p_elem);
}

uint8_t get_seg_count(uint16_t msg_len)
{
    if(msg_len <= 11) {
        return 1;
    } else {
        msg_len -= 8;
        if(msg_len % 12) {
            return msg_len/12+2;
        } else {
            return msg_len/12+1;
        }
    }
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
/*
indication onoff/lightness/temperature/power on
*/
void standart_indication(S_ELEM_STATE *p_elem)
{
    uint8_t buff[14];
    uint16_t i = 0;
    uint8_t cur_indication_flag = g_indication_flag;
    uint8_t seg_count = 0;

    BT_DBG("start flag %02x", g_indication_flag);

    if(cur_indication_flag & INDICATION_FLAG_POWERON) {
        g_indication_flag &= ~INDICATION_FLAG_POWERON;
        // the device will indication all states when powerup
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        cur_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        cur_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        cur_indication_flag |= INDICATION_FLAG_CTL;
#endif
    } else {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if(g_indication_flag & INDICATION_FLAG_ONOFF) {
            g_indication_flag &= ~INDICATION_FLAG_ONOFF;
        }
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if(g_indication_flag & INDICATION_FLAG_LIGHTNESS) {
            g_indication_flag &= ~INDICATION_FLAG_LIGHTNESS;
        }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if(g_indication_flag & INDICATION_FLAG_CTL) {
            g_indication_flag &= ~INDICATION_FLAG_CTL;
        }
#endif
    }
    BT_DBG("real flag %02x", cur_indication_flag);
    
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if(cur_indication_flag & INDICATION_FLAG_ONOFF) {
        buff[i++] = ONOFF_T & 0xff;
        buff[i++] = (ONOFF_T >> 8) & 0xff;
        buff[i++] = p_elem->state.onoff[T_CUR];
    }
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if(cur_indication_flag & INDICATION_FLAG_LIGHTNESS) {
        buff[i++] = LIGHTNESS_T & 0xff;
        buff[i++] = (LIGHTNESS_T >> 8) & 0xff;
        buff[i++] = p_elem->state.actual[T_CUR] & 0xff;
        buff[i++] = (p_elem->state.actual[T_CUR] >> 8) & 0xff;
    }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if(cur_indication_flag & INDICATION_FLAG_CTL) {
        buff[i++] = TEMPERATURE_T & 0xff;
        buff[i++] = (TEMPERATURE_T >> 8) & 0xff;
        buff[i++] = p_elem->state.temp[T_CUR] & 0xff;
        buff[i++] = (p_elem->state.temp[T_CUR] >> 8) & 0xff;
    }
#endif
    if(cur_indication_flag & INDICATION_FLAG_POWERON) {
        buff[i++] = DEVICE_EVENT_T & 0xff;
        buff[i++] = (DEVICE_EVENT_T >> 8) & 0xff;
        buff[i++] = EL_DEV_UP_T;
        cur_indication_flag &= ~INDICATION_FLAG_POWERON;
    }
    BT_DBG("end flag %02x", g_indication_flag);

    if(i) {
        vnd_model_msg reply_msg;
        seg_count = get_seg_count(i + 4);

        reply_msg.opid = VENDOR_OP_ATTR_INDICATE;
        reply_msg.tid = 0;
        reply_msg.data = buff;
        reply_msg.len = i;
        reply_msg.p_elem = &elements[p_elem->elem_index];
        reply_msg.retry_period = 120 * seg_count + 300;
        if(seg_count > 1) {
            reply_msg.retry_period *= 2;
        }
        reply_msg.retry = VENDOR_MODEL_MSG_DFT_RETRY_TIMES;

        genie_vendor_model_msg_send(&reply_msg);
    }

    return 0;
}
#endif


void genie_group_list_init()
{
    if(genie_flash_read_sub(g_sub_list) != GENIE_FLASH_SUCCESS) {
        mesh_group_init(g_sub_list);
        genie_flash_write_sub(g_sub_list);
    }
    BT_DBG("0x%04x 0x%04x 0x%04x 0x%04x", (g_sub_list)[0], (g_sub_list)[1], (g_sub_list)[2], (g_sub_list)[3]);
}

void genie_event(E_GENIE_EVENT event, void *p_arg)
{
    static uint8_t in_prov = 0;
    E_GENIE_EVENT next_event = event;
    uint8_t ignore_user_event = 0;

    GENIE_MESH_EVENT_PRINT(event);

    switch(event) {
        case GENIE_EVT_SW_RESET:
            //call user event first
            user_event(GENIE_EVT_SW_RESET, p_arg);
            ignore_user_event = 1;
        
            genie_flash_reset_system();
            bt_mesh_adv_stop();
            bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
            next_event = GENIE_EVT_SDK_MESH_PBADV_START;
            break;
        case GENIE_EVT_HW_RESET_START:
        {
            _genie_indicate_hw_reset_event(); // Indicate hardware reset event to cloud
            genie_reset_timer();
            
            break;
        }
        case GENIE_EVT_HW_RESET_DONE:
        {
            //call user event first
            user_event(GENIE_EVT_HW_RESET_DONE, p_arg);
            ignore_user_event = 1;

            genie_reset_clear_reset_cnt();
            //restart adv
            bt_mesh_reset();
            genie_flash_reset_system();
            bt_mesh_adv_stop();
            bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
            next_event = GENIE_EVT_SDK_MESH_PBADV_START;
        }
        case GENIE_EVT_SDK_MESH_INIT:
        {
            //check provsioning status
            uint16_t addr;
            uint32_t seq;

            p_arg = &g_elem_state[0];
            if(genie_flash_read_addr(&addr) == GENIE_FLASH_SUCCESS && genie_flash_read_seq(&seq) == GENIE_FLASH_SUCCESS){
                BT_DBG("addr(0x%04x) seq(%d)", addr, seq);
                if(genie_flash_read_para(&bt_mesh) == GENIE_FLASH_SUCCESS){
                    bt_mesh_setup(seq, addr);
                    genie_group_list_init();
                    genie_mesh_setup();
                    printk(F_GREEN ">>>proved<<<\n" F_END);

                    if (genie_reset_get_flag()) {
                        genie_reset_set_flag(0);
                        ignore_user_event = 1;
                        next_event = GENIE_EVT_HW_RESET_START;
                        break;
                    } else {
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
                        poweron_indicate_start();
#endif
                    }
                    break;
                }
            }
            printk(F_RED ">>>unprovisioned<<<\n" F_END);
            bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
            next_event = GENIE_EVT_SDK_MESH_PBADV_START;
            break;
        }

        case GENIE_EVT_SDK_MESH_PBADV_START:
            _genie_pbadv_timer_start();
            break;
        case GENIE_EVT_SDK_MESH_PBADV_TIMEOUT:
            _genie_pbadv_timer_stop();
            bt_mesh_prov_disable(BT_MESH_PROV_GATT);
            next_event = GENIE_EVT_SDK_MESH_SILENT_START;
            break;

        case GENIE_EVT_SDK_MESH_SILENT_START:
            _genie_pbadv_start_silent_adv();
            break;

        case GENIE_EVT_SDK_MESH_PROV_START:
            if(in_prov == 0) {
                in_prov = 1;
                /* disable adv timer */
                _genie_pbadv_timer_stop();
                /* enable prov timer */
                _genie_prov_timer_start();
            }
            break;

        case GENIE_EVT_SDK_MESH_PROV_DATA:
        {
            uint16_t addr = *(uint16_t *)p_arg;
            // genie_flash_write_addr(addr);
            break;
        }

        case GENIE_EVT_SDK_MESH_PROV_TIMEOUT:
            /* disable prov timer */
            next_event = GENIE_EVT_SDK_MESH_PROV_FAIL;
            break;

        case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
            next_event = GENIE_EVT_SDK_STATE_SYNC;
            break;
        case GENIE_EVT_SDK_STATE_SYNC:
            g_indication_flag |= INDICATION_FLAG_ONOFF | INDICATION_FLAG_LIGHTNESS | INDICATION_FLAG_CTL;
            next_event = GENIE_EVT_SDK_INDICATE;
            break;
        case GENIE_EVT_SDK_MESH_PROV_FAIL:
            in_prov = 0;
            /* clean prov data */
            /* restart adv timer */
            _genie_pbadv_timer_start();
            break;

        case GENIE_EVT_SDK_APPKEY_ADD:
        {
            uint8_t status = *(uint8_t *)p_arg;
            if(in_prov) {
                in_prov = 0;
                /* disable prov timer */
                _genie_prov_timer_stop();
                if(status == 0) {
                    // genie_flash_write_para(&bt_mesh);
                    next_event = GENIE_EVT_SDK_MESH_PROV_SUCCESS;
                    p_arg = &g_elem_state[0];
                } else {
                    next_event = GENIE_EVT_SDK_MESH_PROV_FAIL;
                }
            } else {
                if(status == 0) {
                    // genie_flash_write_para(&bt_mesh);
                }
            }
            break;
        }

        case GENIE_EVT_SDK_APPKEY_DEL:
        case GENIE_EVT_SDK_APPKEY_UPDATE:
        case GENIE_EVT_SDK_NETKEY_ADD:
        case GENIE_EVT_SDK_NETKEY_DEL:
        case GENIE_EVT_SDK_NETKEY_UPDATE:
        //case GENIE_EVT_SDK_IVI_UPDATE:
        {
            uint8_t status = *(uint8_t *)p_arg;
            if(status == 0) {
                // genie_flash_write_para(&bt_mesh);
            }
            break;
        }
        case GENIE_EVT_SDK_SUB_ADD:
        {
            // genie_flash_write_sub((g_sub_list));
            break;
        }
        case GENIE_EVT_SDK_SUB_DEL:
        {
            break;
        }
        case GENIE_EVT_SDK_SEQ_UPDATE:
        {
            // genie_flash_write_seq(bt_mesh.seq);
            break;
        }

        case GENIE_EVT_SDK_ANALYZE_MSG:
        {
            S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;
            next_event = GENIE_EVT_SDK_ACTION_DONE;

#ifdef CONFIG_MESH_MODEL_TRANS
            if(p_state->trans || p_state->delay) {
                if(p_state->delay) {
                    next_event = GENIE_EVT_SDK_DELAY_START;
                } else {
                    next_event = GENIE_EVT_SDK_TRANS_START;
                }
            }
#endif
            break;
        }
#ifdef CONFIG_MESH_MODEL_TRANS
        case GENIE_EVT_SDK_DELAY_START:
        {
            S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;
            _mesh_timer_stop(p_state);
            k_timer_start(&p_state->delay_timer, p_state->delay * 5);
            break;
        }

        case GENIE_EVT_SDK_DELAY_END:
        {
            S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;
            u32_t cur_time = k_uptime_get();

            p_state->delay = 0;

            if(p_state->trans == 0 || cur_time >= p_state->trans_end_time) {
                _clear_trans_para(p_state);
                next_event = GENIE_EVT_SDK_ACTION_DONE;
            } else {
                next_event = GENIE_EVT_SDK_TRANS_START;
            }
            break;
        }

        case GENIE_EVT_SDK_TRANS_START:
        {
            S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;
            u32_t cur_time = k_uptime_get();

            _mesh_timer_stop(p_state);

            //check time
            if(cur_time >= p_state->trans_end_time - MESH_TRNSATION_CYCLE) {
                next_event = GENIE_EVT_SDK_TRANS_END;
            } else {
                //start cycle
                k_timer_start(&p_state->trans_timer, MESH_TRNSATION_CYCLE);
                BT_DBG("start trans %p", &p_state->trans_timer);
            }
            break;
        }

        case GENIE_EVT_SDK_TRANS_CYCLE:
        {
            if(_calc_cur_state((S_ELEM_STATE *)p_arg) == 0) {
                ((S_ELEM_STATE *)p_arg)->state.trans = 0;
            }
            break;
        }

        case GENIE_EVT_SDK_TRANS_END:
        {
            S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;
            //clear paras
            _clear_trans_para(p_state);
            //action done
            next_event = GENIE_EVT_SDK_ACTION_DONE;
            break;
        }
#endif

        case GENIE_EVT_SDK_ACTION_DONE:
        {
            S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_arg)->state;
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
            BT_DBG("onoff cur(%d) tar(%d)", p_state->onoff[T_CUR], p_state->onoff[T_TAR]);

            if(p_state->onoff[T_CUR] != p_state->onoff[T_TAR]) {
                p_state->onoff[T_CUR] = p_state->onoff[T_TAR];
            }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            BT_DBG("actual cur(%04x) tar(%04x)", p_state->actual[T_CUR], p_state->actual[T_TAR]);

            if(p_state->actual[T_CUR] != p_state->actual[T_TAR]) {
                p_state->actual[T_CUR] = p_state->actual[T_TAR];
            }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            BT_DBG("temp cur(%04x) tar(%04x)", p_state->temp[T_CUR], p_state->temp[T_TAR]);

            if(p_state->temp[T_CUR] != p_state->temp[T_TAR]) {
                p_state->temp[T_CUR] = p_state->temp[T_TAR];
            }
#endif

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            next_event = GENIE_EVT_SDK_INDICATE;
#endif
            break;
        }
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        case GENIE_EVT_SDK_MESH_PWRON_INDC:
        {
            g_indication_flag |= INDICATION_FLAG_POWERON;
            next_event = GENIE_EVT_SDK_INDICATE;
            p_arg = &g_elem_state[0];
            break;
        }
        case GENIE_EVT_SDK_INDICATE:
        {
            if(g_indication_flag) {
                standart_indication((S_ELEM_STATE *)p_arg);
            }
            break;
        }
        case GENIE_EVT_SDK_VENDOR_MSG:
            BT_DBG("vendor message received");
            genie_vnd_msg_handle((vnd_model_msg *)p_arg);
            break;
#endif

        default:
            break;
    }

    if(!ignore_user_event)
        user_event(event, p_arg);

    if(next_event != event) {
        genie_event(next_event, p_arg);
    }
}

s16_t genie_vendor_model_msg_send(vnd_model_msg *p_vendor_msg) {
    s16_t r = -1;
    uint8_t opid = 0;

    if (!p_vendor_msg)
        return r;

    opid = p_vendor_msg->opid;
    BT_DBG("opcode:0x%x, tid:%d, len:%d", opid, p_vendor_msg->tid, p_vendor_msg->len);

#if 0
    // vnedor confirm message contains 0 extra data except opid
    if (!p_vendor_msg->data && !p_vendor_msg->len)
        return r;
#endif
    BT_DBG("payload: %s", p_vendor_msg->len ? bt_hex(p_vendor_msg->data, p_vendor_msg->len) : "empty");

    switch (opid) {
        case VENDOR_OP_ATTR_STATUS:
        case VENDOR_OP_ATTR_INDICATE:
        case VENDOR_OP_ATTR_INDICATE_TG:
        case VENDOR_OP_ATTR_TRANS_MSG:
            vendor_model_msg_send(p_vendor_msg);
            break;
        default:
            BT_WARN("unknown opid:0x%x", opid);
            break;
    }
    return 0;
}

#if 0
s16_t genie_light_action_notify(S_ELEM_STATE *p_elem) {

    s16_t ret = -1;

#if 0
    if (!p_elem) {
        BT_WARN("invalid p_arg");
        ret = -1;
    } else {
        S_MESH_POWERUP *p_powerup = &((S_ELEM_STATE *)p_elem)->powerup;
        S_MESH_STATE *p_state = &((S_ELEM_STATE *)p_elem)->state;

        if (p_state->onoff[T_TAR]) {
            p_state->trans_start_time = k_uptime_get();
            p_state->trans_end_time = p_state->trans_start_time + 1000; // unit:ms
            p_powerup->last_onoff = p_state->onoff[T_CUR];
            genie_event(GENIE_EVT_SDK_TRANS_START, p_elem);
        } else {
            genie_event(GENIE_EVT_SDK_ACTION_DONE, p_elem);
            ret = 0;
        }
    }
#endif
    return ret;
}
#endif

uint8_t elem_state_init(uint8_t state_count, S_ELEM_STATE *p_elem)
{
    uint8_t i = 0;

    while(i < state_count) {
        p_elem[i].elem_index = i;
#ifdef CONFIG_MESH_MODEL_TRANS
        //memcpy(&elem[i].powerup, &f_power_up[i], sizeof(S_MESH_POWERUP));
        k_timer_init(&p_elem[i].state.delay_timer, _mesh_delay_timer_cb, &p_elem[i]);
        k_timer_init(&p_elem[i].state.trans_timer, _mesh_trans_timer_cycle, &p_elem[i]);
#endif
#ifdef CONFIG_ALI_SIMPLE_MODLE
        p_elem[i].state.onoff[T_TAR] = GEN_ONOFF_DEFAULT;
        p_elem[i].state.actual[T_TAR] = LIGHTNESS_DEFAULT;
        p_elem[i].state.temp[T_TAR] = CTL_TEMP_DEFAULT;
#ifdef CONFIG_MESH_MODEL_TRANS
        p_elem[i].state.trans = 0x41;
        p_elem[i].state.delay = 100;
        if(p_elem[i].state.trans) {
            p_elem[i].state.trans_start_time = k_uptime_get() + p_elem[i].state.delay * 5;
            p_elem[i].state.trans_end_time = p_elem[i].state.trans_start_time + get_transition_time(p_elem[i].state.trans);
        }
#endif
#endif
        i++;
    }
    BT_DBG("+ done");
    return 0;
}

#ifdef CONFIG_MESH_MODEL_TRANS  //transation api
u16_t TRANS_TIMES[] = {1, 10, 100, 6000};

static uint8_t _get_transition_byte(u32_t time)
{
    //BT_DBG("time(%d)", time);

    time /= 100;

    if(time > TRANS_TIMES[3] * 62) {
        return 0;
    }
    else if(time > TRANS_TIMES[2] * 62) {
        return (time/TRANS_TIMES[3]) | 0xC0;
    }
    else if(time > TRANS_TIMES[1] * 62) {
        return (time/TRANS_TIMES[2]) | 0x80;
    }
    else if(time > TRANS_TIMES[0] * 62) {
        return (time/TRANS_TIMES[1]) | 0x40;
    }
    else
        return (time/TRANS_TIMES[0]);
}

//unit is 1ms
u32_t get_transition_time(uint8_t byte)
{
    if((byte & 0x3F) == 0x3F)
    {
        MODEL_E("%s ERROR, invalid 0x%02X!!!\n", __func__, byte);
        return 0xFFFFFFFF;
    }
    return (byte & 0x3F) * TRANS_TIMES[byte>>6] * 100;
}

uint8_t get_remain_byte(S_MODEL_STATE *p_state, bool is_ack)
{
    uint8_t remain_byte = p_state->trans;
    u32_t cur_time = k_uptime_get();

    if (!is_ack && p_state->trans_start_time < cur_time) {
        cur_time -= p_state->trans_start_time;
        u32_t l_trans = get_transition_time(p_state->trans);
        if(l_trans == 0xFFFFFFFF) {
            remain_byte = 0x3F;
        }
        else if(l_trans > cur_time) {
            remain_byte = _get_transition_byte(l_trans - cur_time);
        }
        else {
            remain_byte = 0;
        }

    }

    //BT_DBG("remain_byte(0x%02x)", remain_byte);

    return remain_byte;
}
#endif

#define RECV_MSG_TID_QUEUE_SIZE 5

#define TMALL_GENIE_UADDR_START 0x0001
#define TMALL_GENIE_UADDR_END   0x0010

typedef struct {
    uint8_t tid;
    u16_t addr;
    u32_t time;
} _TID_QUEUE_S;

_TID_QUEUE_S tid_queue[RECV_MSG_TID_QUEUE_SIZE];

E_MESH_ERROR_TYPE mesh_check_tid(u16_t src_addr, uint8_t tid)
{
    static uint8_t cur_index = 0;
    uint8_t i = cur_index;
    uint8_t ri = 0;
    u32_t cur_time = k_uptime_get();
    u32_t end_time = 0;

    if(src_addr >= TMALL_GENIE_UADDR_START && src_addr <= TMALL_GENIE_UADDR_END) {
        src_addr = TMALL_GENIE_UADDR_START;
    }

    while(i < cur_index + RECV_MSG_TID_QUEUE_SIZE) {
        ri = i % RECV_MSG_TID_QUEUE_SIZE;
        if(tid_queue[ri].tid == tid && tid_queue[ri].addr == src_addr) {
            end_time = tid_queue[ri].time + 6000;
            if(cur_time < end_time) {
                break;
            }
            //BT_DBG("---- cur(%lld) last(%lld)", cur_time, end_time);
        }
        i++;
    }
    if(i < cur_index + RECV_MSG_TID_QUEUE_SIZE) {
        return MESH_TID_REPEAT;
    }
    else {
        tid_queue[cur_index].tid = tid;
        tid_queue[cur_index].addr = src_addr;
        tid_queue[cur_index].time = cur_time;
        cur_index++;
        cur_index %= RECV_MSG_TID_QUEUE_SIZE;
        return MESH_SUCCESS;
    }
}

#if 1   //init
static void _prov_complete(u16_t net_idx, u16_t addr)
{
    printk("Provisioning completed!\n");
    printk("Net ID: %u\n", net_idx);
    printk("Unicast addr: 0x%04x\n", addr);

    user_prov_complete(net_idx, addr);
}

static void _prov_reset(void)
{
    printk("reset provisioning\n");
    user_prov_reset();
}

static void _genie_mesh_ready(int err)
{
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk(">>>Mesh initialized<<<\n");
    err = bt_mesh_init(&prov, &comp, NULL);
    if (err) {
        printk("Initializing mesh failed (err %d)\n", err);
        return;
    }
    printk(">>>ali_dfu_init %s<<<\n", __DATE__","__TIME__);
    ali_dfu_init();

    //send event
    genie_event(GENIE_EVT_SDK_MESH_INIT, NULL);
}

void genie_mesh_init(void)
{
    int ret;

    printk("Initializing genie mesh...\n");

    // genie_tri_tuple_load();

    prov.uuid = genie_tri_tuple_get_uuid();
    prov.static_val = NULL;//genie_tri_tuple_get_auth();
    prov.static_val_len = 0;//STATIC_OOB_LENGTH;
    prov.complete = _prov_complete;
    prov.reset = _prov_reset;

    comp.cid = CONFIG_CID_TUYA;
    comp.pid = 0;
    comp.vid = 1; // firmware version fir ota
    comp.elem = elements;
    comp.elem_count = get_vendor_element_num();
    hci_driver_init();

    ret = bt_enable(_genie_mesh_ready);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
    }
}
#endif
