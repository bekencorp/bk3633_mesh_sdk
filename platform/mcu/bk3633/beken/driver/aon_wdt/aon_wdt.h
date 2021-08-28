/**
****************************************************************************************
*
* @file aon_wdt.h
*
* @brief aon_wdt initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/
#ifndef _AON_WDT_H_
#define _AON_WDT_H_

#define REG_AON_WDT_MODE                &(addAON_WDT_Reg0x0)
#define REG_AON_WDT_FEED                &(addAON_WDT_Reg0x1)
#define REG_AON_WDT_PERIOD              &(addAON_WDT_Reg0x2)


#define WDT_FEED_BYTE1                  (0x5A)
#define WDT_FEED_BYTE2                  (0xA5)

#define AON_WDT_DEEP_SLEEP_ON           setf_AON_WDT_Reg0x0_deep_enable
#define AON_WDT_IDLE_SLEEP_ON           setf_AON_WDT_Reg0x0_idle_enable

#endif //_AON_WDT_H_