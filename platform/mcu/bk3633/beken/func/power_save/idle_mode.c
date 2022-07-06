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
#include "idle_mode.h"


static uint8_t sleep_flag = 0;
static uint8_t never_sleep = false;

static idle_task_app_hook_t idle_hook_func = NULL;
static idle_task_app_sleep_start_t idle_sleep_start = NULL;
static sys_time_t idle_task_sleep_time = 0;

void idle_task_register_hook(idle_task_app_hook_t app_hook)
{
    idle_hook_func = app_hook;
}

idle_task_app_hook_t idle_task_get_hook(void)
{
    return idle_hook_func;
}

void idle_task_register_sleep_start(idle_task_app_sleep_start_t sleep_start)
{
    idle_sleep_start = sleep_start;
}

void idle_task_set_sleep_start_time(uint32_t start_time)
{
    idle_task_sleep_time = start_time;
}

void sleep_mode_enable(uint8_t enable)
{
    //os_printf("sleep_mode_enable %d\r\n", enable);
    sleep_flag = enable;
}

void idle_mode_never_sleep(uint8_t sleep_state)
{
    never_sleep = sleep_state;
}

uint8_t idle_model_get_never_sleep_state(void)
{
    return never_sleep;
}

uint8_t get_sleep_flag(void)
{
    return sleep_flag;
}

uint8_t get_rw_sleep_flag(void)
{
    return ((rwip_func.rwip_sleep_flag() & (RW_WAKE_UP_ONGOING | RW_BLE_SLEEP_ONGOING)) != 0);
}

#include "reg_ipcore.h"
#define LV_SLEEP_TIME_S_DEFAULT  10
uint32_t curr_sleep_hs = 0; 
static uint32_t sleep_flag_2 = 0;
static uint8_t sleep_start_flag = 0;

volatile uint32_t lv_sleep_time_s = LV_SLEEP_TIME_S_DEFAULT;

volatile uint8_t g_waitup_by_gpio = 0xFF;


// time_s: 1~145,440 (24 hours)
void set_lv_sleep(uint32_t time_s)
{
	lv_sleep_time_s = time_s;
}

void waitup_by_gpio(void *arg)
{
	uint8_t *gpio;
	gpio = arg;
	g_waitup_by_gpio = 1;
}


uint8_t  reduce_voltage_sleep(void)
{
	uint8_t sleep_type = RWIP_ACTIVE;

	gpio_dev_t gpio;
	
	gpio.port = 0x04;

	hal_gpio_enable_irq(&gpio, IRQ_TRIGGER_RISING_EDGE,waitup_by_gpio, &(gpio.port));

	for(int i = 0; i < 1000; i++)
	{
		sleep_type = idle_mode();
		
		if(sleep_type = RWIP_DEEP_SLEEP)
		{
			break;
		}
	}

	printf("+++++reduce_voltage_sleep:  sleep_type = %X, waaitup value: %X +++++++++++\n", sleep_type, g_waitup_by_gpio);
	sleep_type = RWIP_ACTIVE;

	return g_waitup_by_gpio;
}

uint8_t idle_mode(void)
{
    MCU_SLEEP_MODE sleep_mode;
    uint32_t slot_h = 0, tick_com;
    uint32_t sleep_hs, wakeup_hs;
    uint8_t sleep = 0;
	
#ifdef CONFIG_DUT_TEST_CMD
	if(get_dut_flag())//dut mode can not sleep
	{
		//printf("%s return\r\n", __func__);
		return; 
	}
#endif
    core_peri_clk_freq_set(2, 2);
    sddev_control(ICU_DEV_NAME, CMD_GET_SLEEP_MODE, &sleep_mode);

    if(sleep_mode == MCU_NO_SLEEP) {
        return;
    }

    if (idle_model_get_never_sleep_state()) {
        return;
    }

    GLOBAL_INT_DISABLE();
	{
        fclk_disable(FCLK_PWM_ID);  //close PWM1-5 timer.
        sys_module_power_set(CLK_PWR_DEV_PWM1, 0);  //close PWM1 module power.
        hal_aon_wdt_stop(); //close AON_WDT.

        uint32_t sleep_duration = 0;

        sleep_duration = lv_sleep_time_s * 1000 * 1000 / 32;    //rw sleep duration.
        sleep = rwip_func.rwip_sleep(&slot_h, sleep_duration);
    }

    sleep_hs = rwip_func.rwip_time_get().hs;
    switch(sleep)
    {
        case RWIP_DEEP_SLEEP:
            if (idle_sleep_start) {
                idle_sleep_start();
            }
			g_waitup_by_gpio = 0;
			
            // rf_set_ldo_mode();
            cpu_reduce_voltage_sleep();
            //sleep_mode_enable(0);
            cpu_wakeup();
			//sleep_mode_enable(0);
			break;
			
        case RWIP_CPU_SLEEP:	
			//printf("++++++ idle ++++++++\r\n");
            //cpu_idle_sleep();
            //sleep_mode_enable(0);
            WFI();
            break;
			
        case RWIP_ACTIVE:
            break;
			
        default:
			//printf("++++++idle_mode : default++++++++\r\n");
            break;
    }

    sleep_start_flag = sleep_start_flag ? 0 : 1;
    GLOBAL_INT_RESTORE();
    if (sleep == RWIP_DEEP_SLEEP) {
        while(get_rw_sleep_flag())
        {
            sleep_flag_2++;
            if ((sleep_flag_2 % 2000) == 0) {
                sleep_flag_2 = 0;
                // os_printf("sleep_start_flag %d\n", sleep_start_flag);
                // os_printf("rwip_sleep_flag 0x%x\n", get_rw_sleep_flag());
                // os_printf("clknintmsk 0x%x\n", ip_intcntl1_clknintmsk_getf());
            }
        }
        GLOBAL_INT_DISABLE();
        sleep_flag_2 = 0;
        //core_peri_clk_freq_set(1, 1);
        sleep_mode_enable(0);
        wakeup_hs = rwip_func.rwip_time_get().hs;

        tick_com = (((wakeup_hs - sleep_hs) * RHINO_CONFIG_TICKS_PER_SECOND * 5) >> 4) / 1000;
        
        curr_sleep_hs += ((wakeup_hs - sleep_hs) % 32);
        if(curr_sleep_hs >= 32)
        {
            curr_sleep_hs -= 32;
            //At least compensate 1 tick to pend idle thread
            tick_com += 1;
        }

        // rf_set_bulk_mode();
        //fclk_update_tick(tick_com);
        fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);  //open PWM and init fclk.
        sys_module_power_set(CLK_PWR_DEV_PWM1, 1);  //open PWM1 module power.
        hal_aon_wdt_start(0x4ffff); //open AON_WDT.
    	hal_aon_wdt_idle_sleep();   //start AON_WDT idle sleep.
        //tick_count_update(tick_com);
        GLOBAL_INT_RESTORE();
        tick_list_update(tick_com);
        // Give other task a chanse to run.
        idle_task_app_hook_t idle_func = idle_task_get_hook();
        if (idle_func) {
            idle_func();
            //cpu_idle_sleep();
        }
        krhino_task_yield();
    }

}
