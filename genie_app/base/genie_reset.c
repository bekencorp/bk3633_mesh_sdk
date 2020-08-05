/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "genie_flash.h"
#include "genie_reset.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

struct k_timer g_genie_reset_timer;
uint8_t g_genie_reset_by_repeat_flag = 0;
uint8_t hal_led_ctrl(bool onoff);

uint8_t genie_reset_get_flag(void)
{
    return g_genie_reset_by_repeat_flag;
}

void genie_reset_set_flag(uint8_t flag)
{
    g_genie_reset_by_repeat_flag = flag;
}

void genie_reset_done_cb(void *p_timer, void *args)
{
    genie_event(GENIE_EVT_HW_RESET_DONE, NULL);
}

void genie_reset_timer(void)
{
    k_timer_init(&g_genie_reset_timer, genie_reset_done_cb, NULL);
    k_timer_start(&g_genie_reset_timer, GENIE_RESET_WAIT_TIMEOUT);
}

E_GENIE_FLASH_ERRCODE genie_reset_by_repeat_write_reset_cnt(uint16_t cnt)
{
    uint16_t data = cnt;

    BT_DBG("%s 0x%04x", __func__, cnt);

    return genie_flash_write_userdata(GFI_MESH_RESET_CNT, &data, 2);
}

E_GENIE_FLASH_ERRCODE genie_reset_by_repeat_read_reset_cnt(uint16_t *cnt)
{
    E_GENIE_FLASH_ERRCODE ret;
    uint16_t size = 2;
    uint16_t data;

    BT_DBG("%s", __func__);

    ret = genie_flash_read_userdata(GFI_MESH_RESET_CNT, &data, &size);

    if(ret == GENIE_FLASH_SUCCESS) {
        if(size == 2) {
            *cnt = data;
        } else {
            BT_ERR("read size error");
            ret = GENIE_FLASH_READ_FAIL;
        }
    }

    return ret;
}

static void _genie_reset_by_repeat_timer_cb(void *p_timer, void *args)
{
    uint16_t number = 0;

    BT_DBG("%s", __func__);
    genie_reset_by_repeat_write_reset_cnt(number);
}

void genie_reset_clear_reset_cnt(void)
{
    genie_reset_by_repeat_write_reset_cnt(0);
}

void genie_reset_by_repeat_init(void)
{
    uint8_t by_boot = 1;
    uint16_t number;
    E_GENIE_FLASH_ERRCODE flash_err;

    /* we should check flash flag first */
    flash_err = genie_reset_by_repeat_read_reset_cnt(&number);
    if (flash_err != GENIE_FLASH_SUCCESS) {
        number = 0;
    }

#if 0
    if (number == GENIE_RESET_BY_REPEAT_COUNTER) {
        BT_DBG("Genie Event Reset By Repeat Notify");
        genie_event(GENIE_EVT_RESET_BY_REPEAT_NOTIFY, &by_boot);
        number = 0;
    }
#endif

    /* update number and check if exceeded */
    if (number > GENIE_RESET_BY_REPEAT_COUNTER) {
        number = 0;
    }
    if (number < GENIE_RESET_BY_REPEAT_COUNTER) {
        number++;
        BT_DBG("%s, number = %d", __func__, number);
        genie_reset_by_repeat_write_reset_cnt(number);
        k_timer_init(&g_genie_reset_timer, _genie_reset_by_repeat_timer_cb, NULL);
        k_timer_start(&g_genie_reset_timer, GENIE_RESET_BY_REPEAT_TIMEOUT);
    } else {
        //genie_event(GENIE_EVT_HW_RESET, NULL);
        genie_reset_set_flag(1);
    }
}

