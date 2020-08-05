/**
****************************************************************************************
*
* @file rwip_int.h
*
* @brief RW IP internal SW main module
*
* Copyright (C) RivieraWaves 2009-2015
*
*
****************************************************************************************
*/
#ifndef _RWIP_INT_H_
#define _RWIP_INT_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @brief Entry points of the RW IP stacks/modules
 *
 * This module contains the primitives that allow an application accessing and running the
 * RW IP protocol stacks / modules.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"          // stack configuration

#include <stdint.h>               // standard integer definitions
#include <stdbool.h>              // standard boolean definitions


/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// RWIP Environment structure
struct rwip_env_tag
{
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /// Half slot target timer (in half slots)
    uint32_t timer_hs_target;
    /// Half us target timer (in half us)
    uint32_t timer_hus_target;
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /// 10 ms target timer (in half slots)
    uint32_t timer_10ms_target;
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /// Contains sleep duration accumulated timing error (32kHz: 1/2 half us | 32.768kHz: 1/256 half-us)
    uint32_t sleep_acc_error;
    /// Power_up delay (in LP clock cycle unit, depends on Low power clock frequency)
    uint32_t lp_cycle_wakeup_delay;
    /// Duration of sleep and wake-up algorithm (depends on CPU speed) expressed in half us.
    uint16_t sleep_algo_dur;
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /// Prevent sleep bit field
    uint16_t prevent_sleep;
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /// External wake-up support
    bool     ext_wakeup_enable;
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
};


/*
 * GLOBAL DEFINITIONS
 ****************************************************************************************
 */

/// RW SW environment
extern struct rwip_env_tag rwip_env;


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 * Initialization of the RW IP Common core driver
 *
 * @param reset true if reset requested, false for a boot
 */
void rwip_driver_init(bool reset);


///@} ROOT

#endif // _RWIP_INT_H_
