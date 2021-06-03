/**
****************************************************************************************
*
* @file hal_icu.h
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifndef HAL_ICU_H
#define HAL_ICU_H

#include "icu_pub.h"

void set_sys_sleep_mode(MCU_SLEEP_MODE mode);
MCU_SLEEP_MODE get_sys_sleep_mode(void);

#endif  //HAL_ICU_H