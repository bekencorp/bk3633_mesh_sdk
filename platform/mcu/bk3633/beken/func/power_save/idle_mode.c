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
	if(enable ==1)
	{

//		bt_mesh_scan_disable();
//		bt_mesh_proxy_adv_disabled();
//		bt_mesh_beacon_disable();

	}
	else
	{
//		JX_sleep_check_flag =1;
	}
    os_printf("sleep_mode_enable %d\r\n", enable);
    sleep_flag = enable;
}

uint8_t get_sleep_flag(void)
{
    return sleep_flag;
}

uint8_t get_rw_sleep_flag(void)
{
    return ((rwip_func.rwip_sleep_flag() & RW_WAKE_UP_ONGOING) != 0);
}

void idle_mode(void)
{
    MCU_SLEEP_MODE sleep_mode;
    uint32_t slot_h = 0, tick_com;

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

    uint8_t sleep ;
	sleep = rwip_func.rwip_sleep(&slot_h, 200);      //no less than 6 slots. if set to 0, sleep forever

    tick_com = slot_h * RHINO_CONFIG_TICKS_PER_SECOND * 0.625/2000;
os_printf("%s %d sleep %d %d\r\n",__func__, __LINE__, sleep, tick_com);
    switch(sleep)
    {
        case RWIP_DEEP_SLEEP:

            os_printf("deep sleep tick_com %d\r\n", tick_com);
            fclk_disable(FCLK_PWM_ID);
            cpu_reduce_voltage_sleep();
            while(get_rw_sleep_flag())
            {
                ;
            }

            sleep_mode_enable(0);
            cpu_wakeup();

            GLOBAL_INT_DISABLE();
            fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
            if(!tick_com)
            {
                //At least compensate 1 tick to pend idle thread
                tick_com = 1;
            }

            fclk_update_tick(tick_com);
            tick_list_update(tick_com);
            GLOBAL_INT_RESTORE();
			//sleep_mode_enable(0);

			break;
        case RWIP_CPU_SLEEP:
            cpu_idle_sleep();
            sleep_mode_enable(0);
            break;
        case RWIP_ACTIVE:
            break;
        default:
            break;
    }

    GLOBAL_INT_RESTORE();
}