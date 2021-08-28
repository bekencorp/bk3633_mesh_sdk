/**
 ****************************************************************************************
 *
 * @file hal_aon_wdt.h
 *
 * @brief aon_wdt function user interface
 *
 * Copyright (C) Beken Leonardo 2021
 *
 *
 ****************************************************************************************
 */
#ifndef _HAL_AON_WDT_H_
#define _HAL_AON_WDT_H_

#include "aon_wdt_pub.h"

DRIVER_CTRL_RES hal_aon_wdt_start(uint32_t wdt_period);
DRIVER_CTRL_RES hal_aon_wdt_stop(void);
DRIVER_CTRL_RES hal_aon_wdt_normal_mode(void);
DRIVER_CTRL_RES hal_aon_wdt_deep_sleep(void);
DRIVER_CTRL_RES hal_aon_wdt_idle_sleep(void);
DRIVER_CTRL_RES hal_aon_wdt_feed(void);

#endif  //_HAL_AON_WDT_H_