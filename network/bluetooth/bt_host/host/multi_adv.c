/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifdef CONFIG_BT_MESH_MULTIADV

#include <zephyr.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/slist.h>
#include <misc/byteorder.h>
#include <misc/stack.h>
#include <misc/__assert.h>

#include <bluetooth.h>
#include <conn.h>
#include <l2cap.h>
#include <hci.h>
#include <hci_vs.h>
#include <hci_driver.h>
#include <storage.h>

#define BT_DBG_ENABLED (IS_ENABLED(CONFIG_BT_DEBUG_MULTI_ADV))
#include "common/log.h"

#include "hci_core.h"
#include "hci_ecc.h"
#include "ecc.h"
#include "multi_adv.h"

static struct multi_adv_instant g_multi_adv_list[MAX_MULTI_ADV_INSTANT];
static struct multi_adv_scheduler g_multi_adv_scheduler;
static struct k_delayed_work g_mesh_multi_adv_timer;

kevent_t g_multi_adv_events;

void multi_adv_schedule_instant(struct multi_adv_scheduler *adv_scheduler);
int multi_adv_schedule_timer_stop(void);

int multi_adv_get_instant_num(void)
{
    int i, num = 0;
    struct multi_adv_instant *inst = &(g_multi_adv_list[0]);
    
    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        if (inst[i].inuse_flag)
            num++;
    }
    return num;
}

struct multi_adv_instant *multi_adv_alloc_unused_instant(void)
{
    int i;
    struct multi_adv_instant *inst = &(g_multi_adv_list[0]);

    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        if (inst[i].inuse_flag == 0) {
            inst[i].inuse_flag = 1;
            inst[i].instant_id = i+1;
            return &(inst[i]);
        }
    }
    return 0;
}

int multi_adv_delete_instant(struct multi_adv_instant *instant)
{
    int i;
    struct multi_adv_instant *inst = &(g_multi_adv_list[0]);
    
    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        if ((inst[i].inuse_flag) && (instant == &(inst[i]))) {
            inst[i].inuse_flag = 0;
            return 0;
        }
    return -1;
    }
    
}

int multi_adv_delete_instant_by_id(int instant_id)
{
    int i;
    struct multi_adv_instant *inst = &(g_multi_adv_list[0]);
    
    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        if ((inst[i].inuse_flag) && (instant_id == (inst[i].instant_id))) {
            inst[i].inuse_flag = 0;
            return 0;
        }
    }
    return -1;       
}

struct multi_adv_instant *multi_adv_find_instant_by_id(int instant_id)
{
    int i;
    struct multi_adv_instant *inst = &(g_multi_adv_list[0]);
    
    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        if ((inst[i].inuse_flag) && (instant_id == (inst[i].instant_id))) {
            return &(inst[i]);
        }
    }
    return 0;
}

struct multi_adv_instant *multi_adv_find_instant_by_order(int order)
{

    struct multi_adv_instant *inst = &(g_multi_adv_list[0]);
    
    if (inst[order].inuse_flag) {
        return &(inst[order]);
    }
    return 0;
}

int multi_adv_set_ad_data(uint8_t * ad_data, const struct bt_data *ad, size_t ad_len)
{
    int i, len;

    memset(ad_data, 0, MAX_AD_DATA_LEN);
    len = 0;
    for (i = 0; i < ad_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (len + ad[i].data_len + 2 > MAX_AD_DATA_LEN) {
            break;
        }

        ad_data[len++] = ad[i].data_len + 1;
        ad_data[len++] = ad[i].type;

        memcpy(&ad_data[len], ad[i].data, ad[i].data_len);
        len += ad[i].data_len;
    }

    return len;
}

int change_to_tick(int min_interval, int max_interval)
{
    int interval;
    int tick;

    if (max_interval/SLOT_PER_PERIOD != min_interval/SLOT_PER_PERIOD) {
        tick = min_interval/SLOT_PER_PERIOD;
        if (min_interval%SLOT_PER_PERIOD)
            tick++;
    } else {
        tick = min_interval/SLOT_PER_PERIOD;
    }
    if (tick <= 1)
        tick = 1;

    return tick;
}

int calculate_min_multi(int a, int b)
{
    int x = a, y = b, z;

    while (y != 0) {
        z = x%y;
        x = y;
        y = z;
    }

    return a*b/x; 
}

void multi_adv_reorder(int inst_num, uint16_t inst_interval[], uint8_t inst_order[])
{
    int i, j;
    
    for (i = 0; i < inst_num; i++) {
        int max = inst_interval[0];
        int max_idx = 0;
        int temp;
        
        for (j = 1; j < inst_num-i ; j++) {
            if (max < inst_interval[j]) {
                max = inst_interval[j];
                max_idx = j;
            }
        }
            
        temp = inst_interval[inst_num-i-1];
        inst_interval[inst_num-i-1] = inst_interval[max_idx];
        inst_interval[max_idx] = temp;
    
        temp = inst_order[inst_num-i-1];
        inst_order[inst_num-i-1] = inst_order[max_idx];
        inst_order[max_idx] = temp;        
    }
}

int calculate_offset(uint16_t interval[], uint16_t offset[], int num, int duration)
{
    int i, j, k, curr_offset = 0;
    int curr_max_instants, min_max_instants, instants;
    int offset_range;

    offset_range = interval[num];
    if (offset_range > duration)
        offset_range = duration;

    if (num == 0)
        return 0;

    min_max_instants = 0x7fffffff;
    /* using 0-interval-1 as offset */
    for (i = 0; i < offset_range; i++) {

        curr_max_instants = 0;
        /* search slot form 0 - duration to get the max instants number */
        for (j = 0; j < duration; j++) {

            /* get instant number in each slot */
            instants = 0;
            for (k = 0; k < num; k++) {
                if (j%interval[k] == offset[k]) {
                    instants++;
                }
            }
            if (j%interval[num] == i)
                instants++;
            if (curr_max_instants < instants) {
                curr_max_instants = instants;
            }
        }

        /* check if min max instants */
        if (min_max_instants > curr_max_instants) {
            min_max_instants = curr_max_instants;
            curr_offset = i;
        }
    }
    return curr_offset;

}

void multi_adv_schedule_table(int inst_num, uint16_t inst_interval[], uint16_t inst_offset[])
{
    int i, min_multi, last_min_multi;
    
    /* calculate min multi */
    last_min_multi = min_multi = inst_interval[0];
    for (i = 1; i < inst_num; i++) {
        min_multi = calculate_min_multi(min_multi, inst_interval[i]);
        if (min_multi > MAX_MIN_MULTI) {
            min_multi = last_min_multi;
            break;
        }
        last_min_multi = min_multi;
    }
    BT_DBG("Min Multi = %d, number = %d", min_multi, i);

    /* offset calcute for schedule just for small interval range */
    for (i = 0; i < inst_num; i++) {
        inst_offset[i] = calculate_offset(inst_interval, inst_offset, i, min_multi);
    }
}

int multi_adv_start_adv_instant(struct multi_adv_instant *adv_instant)
{
    int ret;
    
//    BT_DBG("multi_adv_start_adv_instant, inst id = %d", adv_instant->instant_id);
//    BT_DBG("inst interval = %d, inst_offset = %d", adv_instant->instant_interval, adv_instant->instant_offset);
//    BT_DBG("current slot = %d, offset = %d", g_multi_adv_scheduler.slot_clock, g_multi_adv_scheduler.slot_offset);
    if (adv_instant->own_addr_valid) {
        adv_instant->param.own_addr = &(adv_instant->own_addr);
    } else {
        adv_instant->param.own_addr = NULL;
    }

    ret = bt_le_adv_start_instant(&adv_instant->param, 
                                adv_instant->ad, adv_instant->ad_len,
                                adv_instant->sd, adv_instant->sd_len);
    if (ret) {
        BT_ERR("adv start instant failed: inst_id %d, err %d", adv_instant->instant_id, ret);
    }
    return ret;
}

void multi_adv_schedule_timer_callback(void *timer)
{
    kevent_t *event = bt_mesh_multi_adv_get_event();

    k_event_set(event, EVENT_TYPE_MULTI_ADV_TIMER_EVENT);
    return;
}


void multi_adv_schedule_timer_handle(void)
{
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;

    BT_DBG("multi_adv_schedule_timer_handle");
    multi_adv_schedule_timer_stop();
    if (adv_scheduler->schedule_state == SCHEDULE_STOP)
        return;

    adv_scheduler->slot_clock = adv_scheduler->next_slot_clock;
    adv_scheduler->slot_offset = adv_scheduler->next_slot_offset;

    multi_adv_schedule_timeslot(adv_scheduler);
    return;
}


int multi_adv_schedule_timer_start(int timeout)
{
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;

    multi_adv_schedule_timer_stop();

    BT_DBG("multi_adv_schedule_timer_start %d", timeout);
    timeout = timeout;

    k_delayed_work_submit(&g_mesh_multi_adv_timer, timeout);
    adv_scheduler->schedule_timer_active = 1;
    
    return 1;
}

int multi_adv_schedule_timer_stop(void)
{
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;

//    BT_DBG("multi_adv_schedule_timer_stop");
    if (adv_scheduler->schedule_timer_active) {
        k_delayed_work_cancel(&g_mesh_multi_adv_timer);
        adv_scheduler->schedule_timer_active = 0;
    }
    return 0;
}

void multi_adv_schedule_timeslot(struct multi_adv_scheduler *adv_scheduler)
{
    int i, inst_num;
    int inst_clk, inst_off, match, insts, next_slot, min_next_slot;
    struct multi_adv_instant *adv_instant;
    uint16_t inst_interval[MAX_MULTI_ADV_INSTANT];
    uint16_t inst_offset[MAX_MULTI_ADV_INSTANT];
    uint8_t inst_order[MAX_MULTI_ADV_INSTANT];
    uint8_t match_order[MAX_MULTI_ADV_INSTANT];

    /* get instants */
//    BT_DBG("\n\n===********************====");
//    BT_DBG("multi_adv_schedule_timeslot start");
    inst_num = 0;
    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        adv_instant = multi_adv_find_instant_by_order(i);
        if (adv_instant) {
            inst_interval[inst_num] = adv_instant->instant_interval;
            inst_offset[inst_num] = adv_instant->instant_offset;
            inst_order[inst_num] = i;
            inst_num++;
        }
    }

    inst_clk = adv_scheduler->slot_clock;
    inst_off = adv_scheduler->slot_offset;
    match = 0;
    for (i = 0; i < inst_num; i++) {
        if ((inst_clk%inst_interval[i]) == inst_offset[i]) {
            match_order[match] = i;
            match++;
        }
    }

//    BT_DBG("multi_adv_schedule_timeslot, num = %d, match = %d", inst_num, match);
    if (match) {
        int offset_per_instant, diff;
        offset_per_instant = TIME_PRIOD_MS/match;
        diff = inst_off - (inst_off+offset_per_instant/2)/offset_per_instant*offset_per_instant;

        /* means this is the time to start */
        if (diff <= 2) {
            insts = (inst_off+offset_per_instant/2)/offset_per_instant;

            /* start instant */
            adv_instant = multi_adv_find_instant_by_order(inst_order[match_order[insts]]);
            multi_adv_start_adv_instant(adv_instant);
        }

        /* next instant in the same slot */
        if (match-insts > 1) {
            adv_scheduler->next_slot_offset = adv_scheduler->slot_offset+offset_per_instant;
            adv_scheduler->next_slot_clock = adv_scheduler->slot_clock;

            if ((adv_scheduler->next_slot_offset >= (TIME_PRIOD_MS-2))&&(adv_scheduler->slot_offset <= (TIME_PRIOD_MS+2))) {
                adv_scheduler->next_slot_clock++;
                adv_scheduler->next_slot_offset = 0;
            }
            multi_adv_schedule_timer_start(offset_per_instant);
            return;
        }
    }

    /* next instant not in the same slot */
    min_next_slot = 0x7fffffff;
    for (i = 0; i < inst_num; i++) {
        if (inst_clk-inst_offset[i] < 0) {
            match = 0;
        } else {
            match = (inst_clk-inst_offset[i])/inst_interval[i]+1;
        }
        next_slot = match*inst_interval[i]+inst_offset[i];
        if (next_slot < min_next_slot) {
            min_next_slot = next_slot; 
        } 
    }
    adv_scheduler->next_slot_offset = 0;
    adv_scheduler->next_slot_clock = min_next_slot;

    next_slot = (adv_scheduler->next_slot_clock - adv_scheduler->slot_clock)*TIME_PRIOD_MS+(adv_scheduler->next_slot_offset-adv_scheduler->slot_offset);
    multi_adv_schedule_timer_start(next_slot);
    return;
}

int multi_adv_schedule_stop(void)
{
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;

    BT_DBG("multi_adv_schedule_stop");
    
    multi_adv_schedule_timer_stop();
    adv_scheduler->schedule_state = SCHEDULE_STOP;
}

int multi_adv_schedule_start(void)
{
    int i;
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;
    
    BT_DBG("multi_adv_schedule_start");
    /* get all instant and calculate ticks and */
    if (adv_scheduler->schedule_state == SCHEDULE_START) {
        multi_adv_schedule_stop();
    }

    /* reinit scheduler */
    adv_scheduler->slot_clock = 0;
    adv_scheduler->slot_offset = 0;
    adv_scheduler->schedule_state = SCHEDULE_START;
    multi_adv_schedule_timeslot(adv_scheduler);
}

void multi_adv_new_schedule(void)
{
    int i;
    struct multi_adv_instant *adv_instant, *high_duty_instant, *beacon_instant;
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;
    uint16_t inst_offset[MAX_MULTI_ADV_INSTANT];
    uint16_t inst_interval[MAX_MULTI_ADV_INSTANT];
    uint8_t inst_order[MAX_MULTI_ADV_INSTANT];
    int inst_num = 0;
    int min_multi;

    if (adv_scheduler->schedule_state == SCHEDULE_START) {
        multi_adv_schedule_stop();
    }
    /* get all instant and calculate ticks and */
    high_duty_instant = 0;
    beacon_instant = 0;
    for (i = 0; i < MAX_MULTI_ADV_INSTANT; i++) {
        adv_instant = multi_adv_find_instant_by_order(i);
        if (adv_instant) {
            /* if high duty cycle adv found */
            if (adv_instant->param.interval_min <= HIGH_DUTY_CYCLE_INTERVAL) {
                high_duty_instant = adv_instant;
                break;
            }
            if(!(adv_instant->param.options & BT_LE_ADV_OPT_CONNECTABLE)) {
            	beacon_instant = adv_instant;
                break;
            }

            inst_interval[inst_num] = change_to_tick(adv_instant->param.interval_min, adv_instant->param.interval_max);
            inst_order[inst_num] = i;
            inst_num++;
        }
    }

    if (high_duty_instant) {
        BT_DBG("High Duty Cycle Instants, id = %d, interval = %d", adv_instant->instant_id, adv_instant->param.interval_min);
        multi_adv_start_adv_instant(adv_instant);
        return;
    }

    if (beacon_instant) {
        //printk("%s beacon instant\r\n", __func__);
        multi_adv_start_adv_instant(adv_instant);
        return;
    }

    /* instant number equal 0 and 1 */
    if (inst_num == 0) {
        bt_le_adv_stop();
        return;
    }
    if (inst_num == 1) {
        adv_instant = multi_adv_find_instant_by_order(inst_order[0]);
        multi_adv_start_adv_instant(adv_instant);
        return;
    }

    /* reorder by inst_interval */
    multi_adv_reorder(inst_num, inst_interval, inst_order);

    /* calcuate schedule table */
    multi_adv_schedule_table(inst_num, inst_interval, inst_offset);

    /* set interval and offset to instant */
    for (i = 0; i < inst_num; i++) {
        adv_instant = multi_adv_find_instant_by_order(inst_order[i]);
        adv_instant->instant_interval = inst_interval[i];
        adv_instant->instant_offset = inst_offset[i];

        BT_DBG("adv_instant id = %d, interval = %d, offset = %d", adv_instant->instant_id, adv_instant->instant_interval, adv_instant->instant_offset);
    }

    multi_adv_schedule_start();
}

void multi_adv_event_handle(void)
{
    /* UNDO: nothing to do, reserved */
    

}

kevent_t *bt_mesh_multi_adv_get_event(void)
{
    return &g_multi_adv_events;
}

int bt_mesh_multi_adv_thread_init(void)
{
    struct multi_adv_scheduler *adv_scheduler = &g_multi_adv_scheduler;
    kevent_t *event = bt_mesh_multi_adv_get_event();

    /* timer and event init */
    k_delayed_work_init(&g_mesh_multi_adv_timer, multi_adv_schedule_timer_callback);
    k_event_create(event, "multi_adv", 0);

    /* proxy init */
    bt_mesh_proxy_adv_init();

    /* mesh init */
    bt_mesh_adv_event_init();

    return 0;
}

int bt_mesh_multi_adv_thread_run(void)
{    
    kevent_t *event = bt_mesh_multi_adv_get_event();
    uint32_t flag, actual_flag;
    int ret;

    flag = EVENT_TYPE_ALL;
    actual_flag = 0;
    /* event get */
    ret = k_event_get(event, flag, &actual_flag, -1);
    if (ret != 0) {
        return ret;
    }

    /* proxy event need process */
    if (actual_flag&EVENT_TYPE_PROXY_EVENT) {
        bt_mesh_proxy_adv_process();
    }

    /* mesh event need process */
    if (actual_flag&EVENT_TYPE_MESH_EVENT) {
        bt_mesh_adv_event_process();
    }

    /* mesh timer event need process */
    if (actual_flag&EVENT_TYPE_MESH_TIMER_EVENT) {
        bt_mesh_adv_timer_event_process();
        bt_mesh_adv_event_process();
    }

    /* multi adv event need process */
    if (actual_flag&EVENT_TYPE_MULTI_ADV_EVENT) {
        multi_adv_event_handle();
    }

    /* multi adv timer event need process */
    if (actual_flag&EVENT_TYPE_MULTI_ADV_TIMER_EVENT) {
        multi_adv_schedule_timer_handle();
    }

    return 0;
}

int bt_le_multi_adv_start(const struct bt_le_adv_param *param,
                    const struct bt_data *ad, size_t ad_len,
                    const struct bt_data *sd, size_t sd_len, int *instant_id)
{
    int instant_num;
    struct multi_adv_instant *adv_instant;

    BT_DBG("\n\n--------------------------------");
    BT_DBG("      MULTI ADV START    ");

    instant_num = multi_adv_get_instant_num();
    if (instant_num >= MAX_MULTI_ADV_INSTANT)
        return -ENOMEM;

    adv_instant = multi_adv_alloc_unused_instant();
    if (adv_instant == 0)
        return -ENOMEM;
    
    memcpy(&(adv_instant->param), param, sizeof(struct bt_le_adv_param));
    if (param->own_addr) {
        memcpy(&(adv_instant->own_addr), param->own_addr, sizeof(bt_addr_t));
        adv_instant->own_addr_valid = 1;
    } else {
        adv_instant->own_addr_valid = 0;
    }

    adv_instant->ad_len = multi_adv_set_ad_data(adv_instant->ad, ad, ad_len);
    adv_instant->sd_len = multi_adv_set_ad_data(adv_instant->sd, sd, sd_len);

    multi_adv_new_schedule();
    
    *instant_id = adv_instant->instant_id;
    return 0;
}
                    
int bt_le_multi_adv_stop(int instant_id)
{
    BT_DBG("\n\n--------------------------------");
    BT_DBG("      MULTI ADV STOP    ");

    if (multi_adv_find_instant_by_id(instant_id) == 0)
        return -ESRCH;

    multi_adv_delete_instant_by_id(instant_id);
    multi_adv_new_schedule();

    return 0;
}

#endif

