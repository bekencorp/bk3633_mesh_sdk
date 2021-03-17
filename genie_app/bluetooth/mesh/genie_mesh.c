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

#include "net.h"
#include "transport.h"
#include "genie_app.h"
#include "vendor_model_srv.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

extern u32_t get_mesh_pbadv_time(void);


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
void mesh_timer_stop(S_ELEM_STATE *p_elem)
{
    k_timer_stop(&p_elem->state.delay_timer);
    k_timer_stop(&p_elem->state.trans_timer);
}

static void _mesh_delay_timer_cb(void *p_timer, void *p_arg)
{
    S_ELEM_STATE *p_elem = (S_ELEM_STATE *)p_arg;

    mesh_timer_stop(p_elem);
    genie_event(GENIE_EVT_SDK_DELAY_END, p_arg);
}

void clear_trans_para(S_ELEM_STATE *p_elem)
{
    p_elem->state.trans = 0;
    p_elem->state.trans_start_time = 0;
    p_elem->state.trans_end_time = 0;
}

static void _mesh_trans_timer_cycle(void *p_timer, void *p_arg)
{
    S_ELEM_STATE *p_elem = (S_ELEM_STATE *)p_arg;
    S_MODEL_STATE *p_state = &p_elem->state;

    mesh_timer_stop(p_elem);

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

#define DELTA_ACTUAL_MIN 655
#define DELTA_TEMP_MIN 192

uint8_t calc_cur_state(S_ELEM_STATE * p_elem)
{
    S_MODEL_STATE *p_state = &p_elem->state;
    u32_t cur_time = k_uptime_get();
    uint8_t cycle = 0;
    uint16_t delta;

    //stop cycle when timeout
    if(cur_time <= p_state->trans_end_time - MESH_TRNSATION_CYCLE) {
        uint16_t step = (p_state->trans_end_time - cur_time)/MESH_TRNSATION_CYCLE;
    
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if(p_state->onoff[T_CUR] == 0 && p_state->onoff[T_TAR] == 1) {
            p_state->onoff[T_CUR] = 1;
        } else if(p_state->onoff[T_CUR] == 1 && p_state->onoff[T_TAR] == 0) {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            //turn off when lightness is zero
            if(p_state->light_ln_actual[T_CUR] == 0)
#endif
                p_state->onoff[T_CUR] = 0;
        }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if(p_state->light_ln_actual[T_CUR] != p_state->light_ln_actual[T_TAR]) {
            if(p_state->light_ln_actual[T_TAR] > p_state->light_ln_actual[T_CUR]) {
                delta = (p_state->light_ln_actual[T_TAR]-p_state->light_ln_actual[T_CUR])/step;
                delta = delta>DELTA_ACTUAL_MIN ? delta : DELTA_ACTUAL_MIN;
                if(0xFFFF - p_state->light_ln_actual[T_CUR] < delta ||
                  p_state->light_ln_actual[T_TAR] - p_state->light_ln_actual[T_CUR] < delta) {
                    p_state->light_ln_actual[T_CUR] = p_state->light_ln_actual[T_TAR];
                } else {
                    p_state->light_ln_actual[T_CUR] += delta;
                }
            } else {
                delta = (p_state->light_ln_actual[T_CUR]-p_state->light_ln_actual[T_TAR])/step;
                delta = delta>DELTA_ACTUAL_MIN ? delta : DELTA_ACTUAL_MIN;
                if(p_state->light_ln_actual[T_CUR] < delta ||
                  p_state->light_ln_actual[T_CUR] - delta < p_state->light_ln_actual[T_TAR]) {
                    p_state->light_ln_actual[T_CUR] = p_state->light_ln_actual[T_TAR];
                } else {
                    p_state->light_ln_actual[T_CUR] -= delta;
                }
            }
            //model_bind_operation(B_LIGHTNESS_ACTUAL_ID, p_elem, T_CUR);
            cycle = 1;
        }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if(p_state->ctl_temp[T_CUR] != p_state->ctl_temp[T_TAR]) {
            if(p_state->ctl_temp[T_TAR] > p_state->ctl_temp[T_CUR]) {
                delta = (p_state->ctl_temp[T_TAR]-p_state->ctl_temp[T_CUR])/step;
                delta = delta>DELTA_TEMP_MIN ? delta : DELTA_TEMP_MIN;
                if(CTL_TEMP_MAX - p_state->ctl_temp[T_CUR] < delta ||
                  p_state->ctl_temp[T_TAR] - p_state->ctl_temp[T_CUR] < delta) {
                    p_state->ctl_temp[T_CUR] = p_state->ctl_temp[T_TAR];
                } else {
                    p_state->ctl_temp[T_CUR] += delta;
                }
                delta += p_state->ctl_temp[T_CUR];
                p_state->ctl_temp[T_CUR] = delta>p_state->ctl_temp[T_TAR] ? p_state->ctl_temp[T_TAR] : delta;
            } else {
                delta = (p_state->ctl_temp[T_CUR]-p_state->ctl_temp[T_TAR])/step;
                delta = delta>DELTA_TEMP_MIN ? delta : DELTA_TEMP_MIN;
                if(p_state->ctl_temp[T_CUR] < delta+CTL_TEMP_MIN ||
                  p_state->ctl_temp[T_CUR] - delta < p_state->ctl_temp[T_TAR]) {
                    p_state->ctl_temp[T_CUR] = p_state->ctl_temp[T_TAR];
                } else {
                    p_state->ctl_temp[T_CUR] -= delta;
                }
            }
            cycle = 1;
        }
#endif
    }
#if 0
    BT_DBG("next: %d->%d|%02x->%02x|%02x->%02x", p_state->onoff[T_CUR], p_state->onoff[T_TAR],
        p_state->light_ln_actual[T_CUR], p_state->light_ln_actual[T_TAR], p_state->temp[T_CUR], p_state->temp[T_TAR]);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if(p_state->onoff[T_CUR] == p_state->onoff[T_TAR])
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if(p_state->light_ln_actual[T_CUR] == p_state->light_ln_actual[T_TAR])
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if(p_state->ctl_temp[T_CUR] == p_state->ctl_temp[T_TAR])
#endif
        cycle = 0;

    //BT_DBG("cycle %d", cycle);
    if(cycle == 0) {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        //BT_DBG("onoff %d->%d", p_state->onoff[T_CUR], p_state->onoff[T_TAR]);
        p_state->onoff[T_CUR] = p_state->onoff[T_TAR];
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        //BT_DBG("actual %02x->%02x", p_state->light_ln_actual[T_CUR], p_state->light_ln_actual[T_TAR]);
        p_state->light_ln_actual[T_CUR] = p_state->light_ln_actual[T_TAR];
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        //BT_DBG("temp %02x->%02x", p_state->temp[T_CUR], p_state->temp[T_TAR]);
        p_state->ctl_temp[T_CUR] = p_state->ctl_temp[T_TAR];
#endif
        p_state->trans = 0;
    }
    return cycle;
}
#endif

static void _genie_pbadv_timer_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_SDK_MESH_PBADV_TIMEOUT, NULL);
}

void genie_pbadv_timer_start(void)
{
    static uint8_t inited = 0;
    u32_t timeout = get_mesh_pbadv_time();

    if(!inited) {
        k_timer_init(&g_pbadv_timer, _genie_pbadv_timer_cb, NULL);
        inited = 1;
    }
    k_timer_start(&g_pbadv_timer, timeout);
}

void genie_pbadv_timer_stop(void)
{
    k_timer_stop(&g_pbadv_timer);
}

void genie_pbadv_start_silent_adv(void)
{
    genie_tri_tuple_set_silent_adv();
    //bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    unprov_beacon_interval_set(K_SECONDS(60));
    bt_mesh_prov_enable(BT_MESH_PROV_ADV);
}

static void _genie_prov_timer_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_SDK_MESH_PROV_TIMEOUT, NULL);
}

void genie_prov_timer_start(void)
{
    static uint8_t inited = 0;

    if(!inited) {
        k_timer_init(&g_prov_timer, _genie_prov_timer_cb, NULL);
        inited = 1;
    }
    k_timer_start(&g_prov_timer, MESH_PROVISIONING_TIMEOUT);
}

void genie_prov_timer_stop(void)
{
    k_timer_stop(&g_prov_timer);
}


static void _genie_indicate_cb(void *p_timer, void *args)
{
    struct k_timer *p_cb_timer = p_timer;
    S_ELEM_STATE *p_elem = p_cb_timer->args;
    BT_DBG("p_cb_timer %p", p_cb_timer);
    //genie_prov_timer_stop();
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
#ifdef STATUS_INDICATION_IN_ONE_PACKET
    if (g_indication_flag & INDICATION_FLAG_POWERON) {
        genie_event(GENIE_EVT_SDK_MESH_PWRON_INDC, p_elem);
    } else {
        genie_event(GENIE_EVT_SDK_INDICATE, p_elem);
    }
#else
    uint8_t cur_indication_flag = g_indication_flag;

    if (cur_indication_flag & INDICATION_FLAG_POWERON) {
        g_indication_flag &= ~INDICATION_FLAG_POWERON;
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        g_indication_flag |= INDICATION_FLAG_CTL;
#endif
#endif
    }

    /* not report powerup with other status */
    genie_event(GENIE_EVT_SDK_INDICATE, p_elem);

    /* report powerup independently */
    if (cur_indication_flag & INDICATION_FLAG_POWERON) {
        g_indication_flag |= INDICATION_FLAG_POWERON;
        genie_event(GENIE_EVT_SDK_MESH_PWRON_INDC, p_elem);
    }
#endif
}

void genie_indicate_start(uint16_t delay_ms, S_ELEM_STATE *p_elem)
{
    static uint8_t inited = 0;
    uint8_t rand;
    uint16_t random_time;

    if(!inited) {
        k_timer_init(&g_indc_timer, _genie_indicate_cb, NULL);
        inited = 1;
    }

    bt_rand(&rand, 1);

    if(delay_ms == 0) {
#ifdef CONFIG_MESH_MODEL_TRANS
        random_time = 2000 + 8000 * rand / 255;
#else
        random_time = 500 + 9500 * rand / 255;
#endif
    } else {
        random_time = delay_ms;
    }

    BT_DBG("indicate delay(%d)ms", random_time);
    g_indc_timer.args = p_elem;
    k_timer_start(&g_indc_timer, random_time);
}

#ifdef CONFIG_BT_MESH_JINGXUN
void vendor_C7_ack(u8_t tid)
{
	 struct bt_mesh_elem *elem;
	 struct bt_mesh_model *model;

	 u8_t status;
	 u16_t addr = bt_mesh_primary_addr();

	 u8 len =0;
	 u8 payload[10] = {0};

	 elem = bt_mesh_elem_find(addr);
	 LOG_EN_DBG("addr=%x elem=%x", addr, elem);

	 if(elem == NULL)
	 {
		BT_DBG("elem is not exit !");
		return ;
	 }

	model = bt_mesh_model_find_vnd(elem, BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV);

	if(model == NULL)
	{
		BT_DBG("model is not exit !");
		return;
	}
	else
	{
		int err;
		S_MODEL_STATE *p_state = &((S_ELEM_STATE *)model->user_data)->state;

		struct bt_mesh_msg_ctx ctx;

		struct net_buf_simple *p_msg_buf = NET_BUF_SIMPLE(3 + 17 + 4 + 10);//op:3 par ????17

		bt_mesh_model_msg_init(p_msg_buf, BT_MESH_MODEL_OP_3(VENDOR_OP_C7_INDICATE, CONFIG_CID_JX));


//		payload[len++] = 0x01;
//		payload[len++] = tid;
		payload[len++] = 0x07;
		payload[len++] = 0x02;

		payload[len++] = g_elem_state[0].state.onoff[T_CUR];
		payload[len++] = (u8)((u32)(g_elem_state[0].state.light_ln_actual[T_CUR] *100)/65535);
		payload[len++] = (u8)((u32)((g_elem_state[0].state.ctl_temp[T_CUR] -800)*100)/(20000-800));
		payload[len++] = 0x00;
		payload[len++] = 0x00;
		payload[len++] = 0x00;
		LOG_EN_DBG("len %d: %s", len, bt_hex(payload, len));
		net_buf_simple_add_mem(p_msg_buf, payload, len);

		ctx.addr = 0xFFFF;
		ctx.app_idx = 0;
		ctx.net_idx = 0;
		ctx.send_ttl = 5;

        LOG_EN_DBG("+++++%s, before send +++++\n", __func__);
		err = bt_mesh_model_send(model, &ctx, p_msg_buf, NULL, NULL);
        LOG_EN_DBG("+++++%s, after send +++++\n", __func__);
		if (err)
		{
			BT_ERR("bt_mesh_model_publish err %d\n", err);
			return;
		}
	}
}
#endif //CONFIG_BT_MESH_JINGXUN


#ifdef MESH_MODEL_VENDOR_TIMER
static void _vendor_timer_operate_status(uint8_t tid, u16_t attr_type)
{
    vnd_model_msg reply_msg;
    uint8_t payload[10] = {0};
    u16_t payload_len;

    switch (attr_type) {
    case UNIX_TIME_T:
    {
        uint32_t unix_time = vendor_timer_local_unixtime_get();
        payload_len = 2 + 4;
        payload[0] = UNIX_TIME_T & 0xff;
        payload[1] = (UNIX_TIME_T >> 8) & 0xff;
        payload[2] = (uint8_t)(unix_time & 0xFF);
        payload[3] = (uint8_t)((unix_time >> 8) & 0xFF);
        payload[4] = (uint8_t)((unix_time >> 16) & 0xFF);
        payload[5] = (uint8_t)((unix_time >> 24) & 0xFF);
    }
        break;
    case TIMEZONE_SETTING_T:
    {
        int8_t timezone = vendor_timer_timezone_get();
        payload_len = 2 + 1;
        payload[0] = TIMEZONE_SETTING_T & 0xff;
        payload[1] = (TIMEZONE_SETTING_T >> 8) & 0xff;
        payload[2] = timezone;
    }
        break;
    case TIMING_SYNC_T:
    {
        u16_t period_time = 0;
        u8_t  retry_delay = 0;
        u8_t  retry_times = 0;
        vendor_timer_time_sync_get(&period_time, &retry_delay, &retry_times);
        payload_len = 2 + 4;
        payload[0] = TIMING_SYNC_T & 0xff;
        payload[1] = (TIMING_SYNC_T >> 8) & 0xff;
        payload[2] = period_time & 0xff;;
        payload[3] = (period_time >> 8) & 0xff;
        payload[4] = retry_delay;
        payload[5] = retry_times;
    }
        break;

    default:
        return;
    }
    reply_msg.opid = VENDOR_OP_ATTR_STATUS;
    reply_msg.tid = tid;
    reply_msg.data = payload;
    reply_msg.len = payload_len;
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = 1;
    //genie_vendor_model_msg_send(&reply_msg);
}

static void _vendor_timer_unixtime_indicate()
{
    vnd_model_msg request_msg;
    uint8_t payload[2] = {0};

    payload[0] = UNIX_TIME_T & 0xff;
    payload[1] = (UNIX_TIME_T >> 8) & 0xff;

    request_msg.opid = VENDOR_OP_ATTR_INDICATE_TG;
    request_msg.tid = 0;
    request_msg.data = payload;
    request_msg.len = sizeof(payload);
    request_msg.p_elem = &elements[0];
    request_msg.retry_period = 120 + 300;
    request_msg.retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

    //genie_vendor_model_msg_send(&request_msg);
}

static void _vendor_timer_errcode_status(u16_t attr_type, u8_t err_code, u8_t index, uint8_t tid)
{
    vnd_model_msg reply_msg;
    uint8_t payload[2 + 4] = {0};

    payload[0] = ERROR_CODE_T & 0xff;
    payload[1] = (ERROR_CODE_T >> 8) & 0xff;
    payload[2] = attr_type & 0xff;
    payload[3] = (attr_type >> 8) & 0xff;
    payload[4] = err_code;
    payload[5] = index;

    reply_msg.opid = VENDOR_OP_ATTR_STATUS;
    reply_msg.tid = tid;
    reply_msg.data = payload;
    reply_msg.len = sizeof(payload);
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = 1;

    //genie_vendor_model_msg_send(&reply_msg);

}

static void _vendor_timer_timing_settting_event(u8_t op, u8_t *msg, u16_t msg_length, uint8_t tid)
{
    struct {
        uint32_t unix_time;
        uint8_t index;
        vendor_attr_data_t attr_data;
    } timing_setting_attr;

    u8_t *pmsg = msg;
    uint16_t msg_len = msg_length;

    u16_t attr_type = *pmsg++;
    attr_type += (*pmsg++ << 8);
    msg_len -= 2;

    if (attr_type != TIMING_SETTING_T) {
        return;
    }

    if (op != VENDOR_OP_ATTR_SET_ACK && op != VENDOR_OP_ATTR_GET_STATUS) {
        return;
    }

    if (op == VENDOR_OP_ATTR_SET_ACK) {
        while (msg_len > 0) {
            if (msg_len < sizeof(timing_setting_attr)) {
                break;
            }

            timing_setting_attr.index = *pmsg++;
            uint32_t unixtime_tmp = (pmsg[0]) | (pmsg[1] << 8) | (pmsg[2] << 16) | (pmsg[3] << 24);
            pmsg += 4;

            timing_setting_attr.unix_time = unixtime_tmp / 60 * 60;

            if (unixtime_tmp % 60 != 1) {
                //return _vendor_timer_errcode_status(TIMING_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                //                                  timing_setting_attr.index, tid);
            }

            timing_setting_attr.attr_data.type = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            if (timing_setting_attr.attr_data.type != ONOFF_T) {
                return _vendor_timer_errcode_status(TIMING_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                                                    timing_setting_attr.index, tid);
            }

            timing_setting_attr.attr_data.para = *pmsg++;

            if (timing_setting_attr.attr_data.para > 1) {
                return _vendor_timer_errcode_status(TIMING_SETTING_T, ERR_CODE_NOTSUP_ATTR_PARAM,
                                                    timing_setting_attr.index, tid);
            }

            int ret = vendor_timer_start(timing_setting_attr.index,
                                         timing_setting_attr.unix_time,
                                         &timing_setting_attr.attr_data);

            if (ret) {
                uint8_t errcode;

                if (ret == -VT_E_INIT || ret == VT_E_LOCALTIME_NOTSET) {
                    errcode = ERR_CODE_UNIXTIME;
                } else if (ret == -VT_E_INDEX) {
                    errcode = ERR_CODE_TIMER_INDEX;
                } else if (ret == -VT_E_NORESOURCE) {
                    errcode = ERR_CODE_TIMER_FULL;
                } else if (ret == -VT_E_PARAM) {
                    errcode = ERR_CODE_TIMER_SETTING;
                } else {
                    errcode = ERR_CODE_UNIXTIME;
                }

                return _vendor_timer_errcode_status(TIMING_SETTING_T, errcode,
                                                    timing_setting_attr.index, tid);
            }

            msg_len -= sizeof(timing_setting_attr);
        }

    }

    vnd_model_msg reply_msg;

    reply_msg.opid = VENDOR_OP_ATTR_STATUS;
    reply_msg.tid = tid;
    reply_msg.data = msg;
    reply_msg.len = msg_length;
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = 1;

    //genie_vendor_model_msg_send(&reply_msg);
}

static void _vendor_timer_priordic_timing_settting_event(u8_t op, u8_t *msg, u16_t msg_length, uint8_t tid)
{
    struct {
        uint8_t index;
        uint16_t prioridc_time;
        uint8_t  schedule;
        vendor_attr_data_t attr_data;
    } priordic_timing_attr;

    u8_t *pmsg = msg;
    uint16_t msg_len = msg_length;

    u16_t attr_type = *pmsg++;
    attr_type += (*pmsg++ << 8);
    msg_len -= 2;

    if (attr_type != TIMING_PERIODIC_SETTING_T) {
        return;
    }

    if (op != VENDOR_OP_ATTR_SET_ACK && op != VENDOR_OP_ATTR_GET_STATUS) {
        return;
    }

    if (op == VENDOR_OP_ATTR_SET_ACK) {
        while (msg_len > 0) {
            if (msg_len < 7) { // sizeof(priordic_timing_attr) by bytes
                break;
            }

            priordic_timing_attr.index = *pmsg++;
            uint32_t priordic_tmp = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            priordic_timing_attr.prioridc_time = priordic_tmp & 0x0FFF;

            if (((priordic_tmp >> 12) & 0x0F) != 1) {
                //return _vendor_timer_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                //                                  priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.schedule = *pmsg++;

            if (priordic_timing_attr.schedule > 0x7F) {
                return _vendor_timer_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_TIMER_PRIORDIC_PARAM,
                                                    priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.attr_data.type = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            if (priordic_timing_attr.attr_data.type != ONOFF_T) {
                return _vendor_timer_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_NOTSUP_ATTR_OP,
                                                    priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.attr_data.para = *pmsg++;

            if (priordic_timing_attr.attr_data.para > 1) {
                return _vendor_timer_errcode_status(TIMING_PERIODIC_SETTING_T, ERR_CODE_NOTSUP_ATTR_PARAM,
                                                    priordic_timing_attr.index, tid);
            }

            int ret = vendor_timer_periodic_start(priordic_timing_attr.index,
                                                  priordic_timing_attr.prioridc_time * 60,
                                                  priordic_timing_attr.schedule,
                                                  &priordic_timing_attr.attr_data);

            if (ret) {
                uint8_t errcode;

                if (ret == -VT_E_INIT || ret == VT_E_LOCALTIME_NOTSET) {
                    errcode = ERR_CODE_UNIXTIME;
                } else if (ret == -VT_E_INDEX) {
                    errcode = ERR_CODE_TIMER_INDEX;
                } else if (ret == -VT_E_NORESOURCE) {
                    errcode = ERR_CODE_TIMER_FULL;
                } else if (ret == -VT_E_PARAM) {
                    errcode = ERR_CODE_TIMER_SETTING;
                } else {
                    errcode = ERR_CODE_TIMER_PRIORDIC_PARAM;
                }

                return _vendor_timer_errcode_status(TIMING_PERIODIC_SETTING_T, errcode,
                                                    priordic_timing_attr.index, tid);
            }

            msg_len -= 7; // sizeof(priordic_timing_attr) by bytes
        }

    }

    vnd_model_msg reply_msg;

    reply_msg.opid = VENDOR_OP_ATTR_STATUS;
    reply_msg.tid = tid;
    reply_msg.data = msg;
    reply_msg.len = msg_length;
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = 1;

    //genie_vendor_model_msg_send(&reply_msg);
}

static void _vendor_timer_timing_remove_event(u8_t op, u8_t *msg, u16_t msg_length, uint8_t tid)
{
    u8_t *pmsg = msg;
    uint16_t msg_len = msg_length;

    u16_t attr_type = *pmsg++;
    attr_type += (*pmsg++ << 8);
    msg_len -= 2;

    if (attr_type != TIMING_DELETE_T) {
        return;
    }

    if (op != VENDOR_OP_ATTR_SET_ACK) {
        return;
    }

    if (op == VENDOR_OP_ATTR_SET_ACK) {
        while (msg_len > 0) {
            uint8_t index = *pmsg++;
            msg_len--;

            int ret = vendor_timer_remove(index);

            if (ret) {
                //return _vendor_timer_errcode_status(TIMING_DELETE_T, ERR_CODE_TIMER_INDEX, index, tid);
            }
        }
    }

    vnd_model_msg reply_msg;

    reply_msg.opid = VENDOR_OP_ATTR_STATUS;
    reply_msg.tid = tid;
    reply_msg.data = msg;
    reply_msg.len = msg_length;
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = 1;

    //genie_vendor_model_msg_send(&reply_msg);
}

static void _vendor_timer_timeout_indicate(u8_t index)
{
    vnd_model_msg request_msg;
    uint8_t payload[2 + 2] = {0};

    payload[0] = TIMING_TIMEOUT_T & 0xff;
    payload[1] = (TIMING_TIMEOUT_T >> 8) & 0xff;
    payload[2] = EL_TIMING_TIMEOUT_T;
    payload[3] = index;

    request_msg.opid = VENDOR_OP_ATTR_INDICATE;
    request_msg.tid = 0;
    request_msg.data = payload;
    request_msg.len = sizeof(payload);
    request_msg.p_elem = &elements[0];
    request_msg.retry_period = 120 + 300;
    request_msg.retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

    //genie_vendor_model_msg_send(&request_msg);
}
#endif
u16_t genie_indicate_hw_reset_event (void)
{
    vnd_model_msg reply_msg;
    uint8_t payload[3] = {0};

    payload[0] = DEVICE_EVENT_T & 0xff;
    payload[1] = (DEVICE_EVENT_T >> 8) & 0xff;
    payload[2] = (uint8_t)EL_HW_RESET_T;

    reply_msg.opid = VENDOR_OP_ATTR_INDICATE;
    reply_msg.tid = 0;
    reply_msg.data = payload;
    reply_msg.len = 3;
    reply_msg.p_elem = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

    // genie_vendor_model_msg_send(&reply_msg);

    return 0;
}

u16_t genie_vnd_msg_handle(vnd_model_msg *p_msg){
    uint8_t *p_data = NULL;
    BT_DBG("vendor model message received");
    if (!p_msg || !p_msg->data)
        return -1;
    p_data = p_msg->data;
    BT_DBG("opcode:0x%x, tid:%d, len:%d", p_msg->opid, p_msg->tid, p_msg->len);
    if (p_data && p_msg->len)
        BT_DBG("payload: %s", bt_hex(p_data, p_msg->len));

    switch (p_msg->opid) {
        case VENDOR_OP_ATTR_INDICATE:
        {
            u16_t attr_type = *p_data++;
            attr_type += (*p_data++ << 8);
            if (attr_type == DEVICE_EVENT_T) {
                uint8_t event_id = *p_data;
                switch (event_id) {
                    case EL_DEV_UP_T:
                        genie_indicate_start(0, &g_elem_state[0]); // When receiving genie's device up status, indication device's state and device up event in sequence
                        break;
                    default:
                        break;
                }
            }

            break;
        }
#ifdef CONFIG_BT_MESH_JINGXUN
        case VENDOR_OP_C7_INDICATE:
        {
			if(p_data[0] ==0x01 && p_data[1] ==0x01)
			{
				vendor_C7_ack(p_msg->tid);
			}

			break;
        }
#endif //CONFIG_BT_MESH_JINGXUN
        case VENDOR_OP_ATTR_GET_STATUS: {
#ifdef MESH_MODEL_VENDOR_TIMER
            u16_t attr_type = *p_data++;
            attr_type += (*p_data++ << 8);
            if (attr_type == UNIX_TIME_T ||
                attr_type == TIMEZONE_SETTING_T ||
                attr_type == TIMING_SYNC_T) {
                _vendor_timer_operate_status(p_msg->tid, attr_type);
            } else if (attr_type == TIMING_SETTING_T) {
                _vendor_timer_timing_settting_event(VENDOR_OP_ATTR_GET_STATUS, p_msg->data, p_msg->len, p_msg->tid);
            } else if (attr_type == TIMING_PERIODIC_SETTING_T) {
                _vendor_timer_priordic_timing_settting_event(VENDOR_OP_ATTR_GET_STATUS, p_msg->data, p_msg->len, p_msg->tid);
            }
#endif
            break;
        }

        case VENDOR_OP_ATTR_SET_ACK: {
#ifdef MESH_MODEL_VENDOR_TIMER
            u16_t attr_type = *p_data++;
            attr_type += (*p_data++ << 8);
            if (attr_type == UNIX_TIME_T) {
                uint32_t unix_time = (p_data[0]) | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24);
                p_data += 4;
                vendor_timer_local_time_update(unix_time);
                _vendor_timer_operate_status(p_msg->tid, attr_type);
            } else if (attr_type == TIMEZONE_SETTING_T) {
                int8_t timezone = *p_data++;
                vendor_timer_timezone_update(timezone);
                _vendor_timer_operate_status(p_msg->tid, attr_type);
            } else if (attr_type == TIMING_SYNC_T) {
                u16_t period_time = (p_data[0]) | (p_data[1] << 8);
                p_data += 2;
                u8_t  retry_delay = *p_data++;
                u8_t  retry_times = *p_data++;
                vendor_timer_time_sync_set(period_time, retry_delay, retry_times);
                _vendor_timer_operate_status(p_msg->tid, attr_type);
            } else if (attr_type == TIMING_SETTING_T) {
                _vendor_timer_timing_settting_event(VENDOR_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
            } else if (attr_type == TIMING_PERIODIC_SETTING_T) {
                _vendor_timer_priordic_timing_settting_event(VENDOR_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
            } else if (attr_type == TIMING_DELETE_T) {
                _vendor_timer_timing_remove_event(VENDOR_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
            }
#endif
            break;
        }

        case VENDOR_OP_ATTR_CONFIME_TG: {
#ifdef MESH_MODEL_VENDOR_TIMER
            u16_t attr_type = *p_data++;
            attr_type += (*p_data++ << 8);
            if (attr_type == UNIX_TIME_T) {
                uint32_t unix_time = (p_data[0]) | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24);
                p_data += 4;
                vendor_timer_local_time_update(unix_time);
            }
#endif
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

    // genie_vendor_model_msg_send(&reply_msg);
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
        buff[i++] = p_elem->state.light_ln_actual[T_CUR] & 0xff;
        buff[i++] = (p_elem->state.light_ln_actual[T_CUR] >> 8) & 0xff;
    }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if(cur_indication_flag & INDICATION_FLAG_CTL) {
        buff[i++] = TEMPERATURE_T & 0xff;
        buff[i++] = (TEMPERATURE_T >> 8) & 0xff;
        buff[i++] = p_elem->state.ctl_temp[T_CUR] & 0xff;
        buff[i++] = (p_elem->state.ctl_temp[T_CUR] >> 8) & 0xff;
    }
#endif
    if(cur_indication_flag & INDICATION_FLAG_POWERON) {
        buff[i++] = DEVICE_EVENT_T & 0xff;
        buff[i++] = (DEVICE_EVENT_T >> 8) & 0xff;
        buff[i++] = EL_DEV_UP_T;
    }
    BT_DBG("end flag %02x", g_indication_flag);

    if(i) {
        vnd_model_msg reply_msg;
        seg_count = get_seg_count(i + 4);

        reply_msg.opid = VENDOR_OP_ATTR_INDICATE;

        reply_msg.data = buff;
        reply_msg.len = i;
        reply_msg.p_elem = &elements[p_elem->elem_index];
        //reply_msg.retry_period = GENIE_DEFAULT_DURATION * seg_count + SEG_RETRANSMIT_TIMEOUT;
        reply_msg.retry_period = 125 * seg_count + 400;
        if(seg_count > 1) {
            reply_msg.retry_period *= 2;
        }
        reply_msg.retry = VENDOR_MODEL_MSG_DFT_RETRY_TIMES;

        //genie_vendor_model_msg_send(&reply_msg);
    }

    return 0;
}
#endif


void genie_sub_list_init(void)
{
    if(genie_flash_read_sub(g_sub_list) != GENIE_FLASH_SUCCESS) {
        mesh_sub_init(g_sub_list);
        genie_flash_write_sub(g_sub_list);
    }
    BT_INFO("sb %s", bt_hex(g_sub_list, sizeof(g_sub_list)));
}

s16_t genie_vendor_model_msg_send(vnd_model_msg *p_vendor_msg) {
    s16_t r = -1;
    uint8_t opid = 0;

    if (!p_vendor_msg)
        return r;

    opid = p_vendor_msg->opid;
    BT_DBG("VND msg send: opcode:0x%x, tid:%d, len:%d", opid, p_vendor_msg->tid, p_vendor_msg->len);

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
#ifdef CONFIG_BT_MESH_JINGXUN
		case VENDOR_OP_C7_INDICATE:
#endif //CONFIG_BT_MESH_JINGXUN
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

uint8_t genie_elem_state_init(uint8_t state_count, S_ELEM_STATE *p_elem)
{
    uint8_t i = 0;

    while(i < state_count) {
        p_elem[i].elem_index = i;
        p_elem[i].message_index = MM_INDEX_NONE;
#ifdef CONFIG_MESH_MODEL_TRANS
        //memcpy(&elem[i].powerup, &f_power_up[i], sizeof(S_MESH_POWERUP));
        k_timer_init(&p_elem[i].state.delay_timer, _mesh_delay_timer_cb, &p_elem[i]);
        k_timer_init(&p_elem[i].state.trans_timer, _mesh_trans_timer_cycle, &p_elem[i]);

        p_elem[i].state.trans = 0x41;
        p_elem[i].state.delay = 100;
        if(p_elem[i].state.trans) {
            p_elem[i].state.trans_start_time = k_uptime_get() + p_elem[i].state.delay * 5;
            p_elem[i].state.trans_end_time = p_elem[i].state.trans_start_time + get_transition_time(p_elem[i].state.trans);
        }
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        p_elem[i].state.onoff[T_CUR] = p_elem[i].state.onoff[T_TAR] \
                         = p_elem[i].powerup.last_onoff = GEN_ONOFF_DEFAULT;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        p_elem[i].state.light_ln_actual[T_CUR] = p_elem[i].state.light_ln_actual[T_TAR] \
                         = p_elem[i].powerup.light_ln_last = g_elem_state[i].powerup.default_light_ln = LIGHTNESS_DEFAULT;
        p_elem[i].powerup.light_ln_range_max = LIGHTNESS_MAX;
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
        p_elem[i].state.ctl_temp[T_CUR] = p_elem[i].state.ctl_temp[T_TAR] \
                         = p_elem[i].powerup.ctl_temp_last = p_elem[i].powerup.ctl_temp_def = CTL_TEMP_DEFAULT;
        p_elem[i].powerup.ctl_temp_range_max = CTL_TEMP_MAX;
        p_elem[i].powerup.ctl_temp_range_min = CTL_TEMP_MIN;
        p_elem[i].powerup.ctl_ln_last = p_elem[i].state.ctl_lightness[T_CUR] = p_elem[i].state.ctl_lightness[T_TAR] = LIGHTNESS_DEFAULT;
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
        p_elem[i].state.hsl_lightness[T_CUR] = p_elem[i].state.hsl_lightness[T_TAR] \
                         = p_elem[i].powerup.last_hsl_lightness = LIGHTNESS_DEFAULT;
        p_elem[i].state.hsl_hue[T_CUR] = p_elem[i].state.hsl_hue[T_TAR] = p_elem[i].powerup.last_hsl_hue = 0;
        p_elem[i].state.hsl_sat[T_CUR] = p_elem[i].state.hsl_sat[T_TAR] = p_elem[i].powerup.last_hsl_sat = 0;

        p_elem[i].powerup.default_sat = 0;
#endif

        i++;
    }
    BT_DBG("+ done");
    return 0;
}

uint8_t genie_restore_user_state(uint8_t state_count,S_ELEM_STATE *p_elem, S_MODEL_POWERUP *p_pup)
{
    uint8_t i = 0;
    
    while(i < state_count) 
    {
#if defined(CONFIG_BEKEN_OTA) || defined(CONFIG_GENIE_OTA)
        // if the device reboot by ota, it must be off.
        if(p_pup[i].last_onoff == 0)
        {
            p_elem[i].powerup.last_onoff = p_pup[i].last_onoff;
            p_elem[i].state.onoff[T_TAR] = 0;
            // load lightness
            if(p_pup[i].light_ln_last)
            {
                p_elem[i].state.light_ln_actual[T_TAR] = p_pup[i].light_ln_last;
                p_elem[i].powerup.light_ln_last = p_pup[i].light_ln_last;
            }
            // load temperature
            if(p_pup[i].ctl_temp_last)
            {
                p_elem[i].state.ctl_temp[T_TAR] = p_pup[i].ctl_temp_last;
                p_elem[i].powerup.ctl_temp_last = p_pup[i].ctl_temp_last;
            }
            clear_trans_para(&p_elem[i]);
        } else
#endif
        {
            memcpy(&p_elem[i].powerup, &p_pup[i], sizeof(S_MODEL_POWERUP));
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
            p_elem[i].state.onoff[T_CUR] = p_elem[i].state.onoff[T_TAR] = p_pup[i].last_onoff;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            p_elem[i].state.light_ln_actual[T_CUR] = p_elem[i].state.light_ln_actual[T_TAR] \
                            = p_pup[i].light_ln_last;
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
            p_elem[i].state.ctl_temp[T_CUR] = p_elem[i].state.ctl_temp[T_TAR] = p_pup[i].ctl_temp_last;
            p_elem[i].state.ctl_lightness[T_CUR] = p_elem[i].state.ctl_lightness[T_TAR] = p_pup[i].ctl_ln_last;
            p_elem[i].state.ctl_UV[T_CUR] = p_elem[i].state.ctl_UV[T_TAR] = p_pup[i].ctl_uv_last;
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
            p_elem[i].state.hsl_lightness[T_CUR] = p_elem[i].state.hsl_lightness[T_TAR] = p_pup[i].last_hsl_lightness;
            p_elem[i].state.hsl_hue[T_CUR] = p_elem[i].state.hsl_hue[T_TAR] = p_pup[i].last_hsl_hue;
            p_elem[i].state.hsl_sat[T_CUR] = p_elem[i].state.hsl_sat[T_TAR] = p_pup[i].last_hsl_sat;
#endif
        }
        i++;
    }
}

#ifdef CONFIG_MESH_MODEL_TRANS  //transation api
u16_t TRANS_TIMES[] = {1, 10, 100, 6000};

static uint8_t _get_transition_byte(u32_t time)
{
    //BT_DBG("time(%d)", time);

    time /= 100;

    if(time > TRANS_TIMES[3] * DEFAULT_TRANS_TIME_RANGE) {
        return 0;
    }
    else if(time > TRANS_TIMES[2] * DEFAULT_TRANS_TIME_RANGE) {
        return (time/TRANS_TIMES[3]) | 0xC0;
    }
    else if(time > TRANS_TIMES[1] * DEFAULT_TRANS_TIME_RANGE) {
        return (time/TRANS_TIMES[2]) | 0x80;
    }
    else if(time > TRANS_TIMES[0] * DEFAULT_TRANS_TIME_RANGE) {
        return (time/TRANS_TIMES[1]) | 0x40;
    }
    else
        return (time/TRANS_TIMES[0]);
}

//unit is 1ms
u32_t get_transition_time(uint8_t byte)
{
    if((byte & DEFAULT_TRANS_NUM_STEPS_MASK) == 0x3F)
    {
        //Shoule be 0x00~0x3E
        MODEL_E("%s ERROR, invalid 0x%02X!!!\n", __func__, byte);
        return 0xFFFFFFFF;
    }
    return (byte & DEFAULT_TRANS_NUM_STEPS_MASK) * TRANS_TIMES[DEFAULT_TRANS_SEP_RESO(byte)] * 100;
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

#ifdef MESH_MODEL_VENDOR_TIMER
int _vendor_timer_event(uint8_t event, uint8_t index, vendor_attr_data_t *data)
{
    if (event == VT_TIMEOUT) {
        vendor_timer_local_time_show();
        printf("timer index %d timeout\n", index);
        genie_event(GENIE_EVT_TIME_OUT, data);
        _vendor_timer_timeout_indicate(index);
    } else if (event == VT_TIMING_SYNC) {
        _vendor_timer_unixtime_indicate();
    }

    return 0;
}
#endif

#if 1   //init
static void _prov_complete(u16_t net_idx, u16_t addr)
{
    BT_DBG("Provisioning completed");
#ifdef MESH_MODEL_VENDOR_TIMER
    vendor_timer_init(_vendor_timer_event);
#endif

    //BT_DBG("Net ID: %u", net_idx);
    //BT_DBG("ua %04x", addr);
}


static void _prov_reset(void)
{
    BT_INFO("reset prov");
    //user_prov_reset();
#ifdef MESH_MODEL_VENDOR_TIMER
    vendor_timer_finalize();
#endif

}

// bool bt_ready_count = false;
static void _genie_mesh_ready(int err)
{
    if (err) {
        BT_INFO("BT init err %d", err);
        return;
    }

    BT_INFO(">>>Mesh initialized<<<");
    err = bt_mesh_init(&prov, &comp, NULL);
    if (err) {
        BT_INFO("mesh init err %d", err);
        return;
    }

    // bt_ready_count = true;
#ifdef MESH_MODEL_VENDOR_TIMER
    vendor_timer_init(_vendor_timer_event);
#endif

    //send event
    // genie_event(GENIE_EVT_SDK_MESH_INIT, NULL);
}

void genie_mesh_init(void)
{
    int ret;

    BT_INFO(">>>init genie<<<");

#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE
    genie_tri_tuple_load();
    prov.static_val = genie_tri_tuple_get_auth();
    prov.static_val_len = STATIC_OOB_LENGTH;
#endif /*CONFIG_BT_MESH_ALI_TMALL_GENIE*/
    prov.uuid = genie_tri_tuple_get_uuid();
    prov.complete = _prov_complete;
    prov.reset = _prov_reset;

#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE
    comp.cid = CONFIG_CID_TAOBAO;
#else /*CONFIG_BT_MESH_ALI_TMALL_GENIE*/
    comp.cid = CONFIG_CID_JX;
#endif
    comp.pid = 0;
    comp.vid = 1; // firmware version fir ota
    comp.elem = elements;
    comp.elem_count = get_vendor_element_num();
    hci_driver_init();

    ret = bt_enable(_genie_mesh_ready);
    if (ret) {
        BT_INFO("init err %d", ret);
    }
}
#endif
