/**
****************************************************************************************
*
* @file idle_module.c
*
* @brief CPU low power control in idle tast
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "icu_pub.h"
#include "gpio_pub.h"
#include "rwip.h"
#include "fake_clock_pub.h"

static uint8_t sleep_flag = 0;

void sleep_mode_enable(uint8_t enable)
{
    sleep_flag = enable;
}

uint8_t get_sleep_flag(void)
{
    return sleep_flag;
}

void idle_mode(void)
{
    MCU_SLEEP_MODE sleep_mode;
    uint32_t dur;

    if(!sleep_flag)
    {
        return;
    }

    sddev_control(ICU_DEV_NAME, CMD_GET_SLEEP_MODE, &sleep_mode);

    if(sleep_mode == MCU_NO_SLEEP)
    {
        return;
    }

    GLOBAL_INT_DISABLE();

    uint8_t sleep = rwip_func.rwip_sleep(&dur);
    // printf("sleep %d dur 0x%x!!!!!\r\n\r\n", sleep, dur);

    switch(sleep)
    {
        case RWIP_DEEP_SLEEP:
            fclk_disable(FCLK_PWM_ID);
            cpu_reduce_voltage_sleep();
            cpu_wakeup();
            fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
            fclk_update_tick(dur);
            tick_list_update(dur);
            break;
        case RWIP_CPU_SLEEP:
            fclk_disable(FCLK_PWM_ID);
            cpu_idle_sleep();
            fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
            fclk_update_tick(dur);
            tick_list_update(dur);
            break;
        case RWIP_ACTIVE:
            break;
        default:
            break;
    }

    GLOBAL_INT_RESTORE();
}