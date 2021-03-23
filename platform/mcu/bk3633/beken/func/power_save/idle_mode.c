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

#if _GPIO_WAKEUP_TEST_
gpio_irq_handler_t gpio_test_func(void)
{
    os_printf("~~~~~~~gpio wake up!!!!!\r\n\r\n");
}

void gpio_wakeup_test_init(void)
{
    gpio_dev_t gpio_test;
    gpio_test.port = GPIO_P02;

    hal_gpio_enable_irq(&gpio_test, IRQ_TRIGGER_FALLING_EDGE, gpio_test_func, NULL);
}
#endif

void sleep_mode_enable(uint8_t enable)
{
    os_printf("sleep_mode_enable %d\r\n", enable);
    sleep_flag = enable;
}

uint8_t get_sleep_flag(void)
{
    return sleep_flag;
}

void idle_mode(void)
{
    MCU_SLEEP_MODE sleep_mode;
    uint32_t slot = 0, tick_com;

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

    uint8_t sleep = rwip_func.rwip_sleep(&slot);
    tick_com = slot * RHINO_CONFIG_TICKS_PER_SECOND * 0.625/2000;
    os_printf("sleep %d tick_com %d\r\n", sleep, tick_com);

    switch(sleep)
    {
        case RWIP_DEEP_SLEEP:
            fclk_disable(FCLK_PWM_ID);
            cpu_reduce_voltage_sleep();
            cpu_wakeup();

            while((rwip_func.rwip_sleep_flag() & RW_WAKE_UP_ONGOING) != 0);

            fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
            fclk_update_tick(tick_com);
            tick_list_update(tick_com);
            break;
        case RWIP_CPU_SLEEP:
            cpu_idle_sleep();
            break;
        case RWIP_ACTIVE:
            break;
        default:
            break;
    }

    GLOBAL_INT_RESTORE();
}