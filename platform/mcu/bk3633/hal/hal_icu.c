/**
****************************************************************************************
*
* @file hal_icu.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "hal_icu.h"

void set_sys_sleep_mode(MCU_SLEEP_MODE mode)
{
    printf("%s %d\r\n", __func__, mode);

    MCU_SLEEP_MODE sleep_mode;
    sleep_mode = mode;

    sddev_control(ICU_DEV_NAME, CMD_SET_SLEEP_MODE, &sleep_mode);
}

MCU_SLEEP_MODE get_sys_sleep_mode(void)
{
    MCU_SLEEP_MODE sleep_mode;
    sddev_control(ICU_DEV_NAME, CMD_GET_SLEEP_MODE, &sleep_mode);
    return sleep_mode;
}

void sys_module_power_set(icu_clk_pwr_dev module, uint8_t onoff)
{
    if(onoff)
    {
        sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&module);
    }
    else
    {
        sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&module);
    }
}