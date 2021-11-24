/**
****************************************************************************************
*
* @file aon_wdt_pub.h
*
* @brief aon_wdt initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifndef _AON_WDT_PUB_H_
#define _AON_WDT_PUB_H_

#include "drv_model_pub.h"

#include "generic.h"

#define AON_WDT_DEV_NAME                "aon_wdt"

enum
{
    CMD_AON_WDT_PERIOD_SET                        = AON_WDT_CMD_MAGIC + 0,
    CMD_AON_WDT_FEED                              = AON_WDT_CMD_MAGIC + 1,
    CMD_AON_WDT_MODE                              = AON_WDT_CMD_MAGIC + 2,
};

typedef enum
{
    AON_WDT_NORMAL = 0,
    AON_WDT_DEEP_SLEEP,
    AON_WDT_IDLE_SLEEP,

    AON_WDT_MODE_MAX
} Aon_wdt_mode;


__inline static void bk_aon_wdt_period(uint32_t wdt_period)
{
    DRIVER_CTRL_RES ret;
    UINT32 para = wdt_period;
    
    ret = sddev_control(AON_WDT_DEV_NAME, CMD_AON_WDT_PERIOD_SET, &para);
    //ASSERT(DRIV_SUCCESS == ret);
}

__inline static void bk_aon_wdt_mode(Aon_wdt_mode mode)
{
    DRIVER_CTRL_RES ret;
    UINT32 para = mode;

    ret = sddev_control(AON_WDT_DEV_NAME, CMD_AON_WDT_MODE, &para);
    //ASSERT(DRIV_SUCCESS == ret);
}

__inline static void bk_aon_wdt_feed(void)
{
    DRIVER_CTRL_RES ret;
    
    ret = sddev_control(AON_WDT_DEV_NAME, CMD_AON_WDT_FEED, (void *)0);
    //ASSERT(DRIV_SUCCESS == ret);
}

void aon_wdt_init(void);
void aon_wdt_exit(void);


#endif  //_AON_WDT_PUB_H_