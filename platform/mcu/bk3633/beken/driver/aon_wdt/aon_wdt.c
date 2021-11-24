/**
****************************************************************************************
*
* @file aon_wdt.c
*
* @brief aon_wdt initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include <stddef.h>     // standard definition
#include "BK3633_RegList.h"
#include "aon_wdt.h"
#include "aon_wdt_pub.h"
#include "drv_model_pub.h"
#include "ll.h"

static UINT32 aon_wdt_ctrl(UINT32 cmd, void *param);

static SDD_OPERATIONS aon_wdt_op =
{ 
    aon_wdt_ctrl
};

void aon_wdt_init(void)
{
	sddev_register_dev(AON_WDT_DEV_NAME, &aon_wdt_op);
}

void aon_wdt_exit(void)
{
	sddev_unregister_dev(AON_WDT_DEV_NAME);
}

static void aon_wdt_clk(UINT32 wdt_clk)
{
    UINT32 value;

    value = REG_READ(REG_AON_WDT_PERIOD);
    value = wdt_clk;

    REG_WRITE(REG_AON_WDT_PERIOD, value);
}

static void aon_wdt_set(Aon_wdt_mode mode)
{
    UINT32 value;

    if(mode == AON_WDT_NORMAL)
    {
        value = REG_READ(REG_AON_WDT_MODE);
        value = 0;

        REG_WRITE(REG_AON_WDT_PERIOD, value);
    }
    else if(mode == AON_WDT_DEEP_SLEEP)
    {
        AON_WDT_DEEP_SLEEP_ON;
    }
    else if(mode == AON_WDT_IDLE_SLEEP)
    {
        AON_WDT_IDLE_SLEEP_ON;
    }
    
}

static void Delay_us(int num)
{
    volatile int x, y;
    for(y = 0; y < num; y ++ )
    {
        for(x = 0; x < 10; x++);
    }
}

static void aon_wdt_feed(void)
{
    UINT32 value;
    GLOBAL_INT_DISABLE();
    value = REG_READ(REG_AON_WDT_FEED);
    value = WDT_FEED_BYTE1;

    REG_WRITE(REG_AON_WDT_FEED, value);
    GLOBAL_INT_RESTORE();
    //Delay_us(3);
    GLOBAL_INT_DISABLE();
    value = WDT_FEED_BYTE2;

    REG_WRITE(REG_AON_WDT_FEED, value);
    GLOBAL_INT_RESTORE();
}

static UINT32 aon_wdt_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = DRIV_SUCCESS;

    switch(cmd)
    {
        case CMD_AON_WDT_PERIOD_SET:
            aon_wdt_clk(*(UINT32 *)param);
            break;

        case CMD_AON_WDT_FEED:
            aon_wdt_feed();
            break;

        case CMD_AON_WDT_MODE:
            aon_wdt_set(*(Aon_wdt_mode *)param);
            break;

        default:
            break;
    }


    return ret;
}