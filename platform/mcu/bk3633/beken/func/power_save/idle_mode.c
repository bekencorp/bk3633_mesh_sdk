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
#include "reg_ipcore.h"

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

#define INTERNAL_CRYSTAL_OSCILLATOR_32K     1
uint64_t lv_sleep_period_get(void)
{
    uint64_t cal_dur_1 = 0;
    uint64_t cal_dur_2 = 0;
    uint64_t cal_dur_3 = 0;
    uint64_t cal_dur_4 = 0;

    uint64_t sleep_period = ip_deepslstat_get();    //get low power clk numbers.

#if INTERNAL_CRYSTAL_OSCILLATOR_32K
    cal_dur_1 = (sleep_period * 2 * 1000) >> 5; //calculate for 32 KHz, transform to half_us. 
    cal_dur_3 = cal_dur_1 / 625;    //calculate for 32 KHz, transform to half_slot.
#else/* external crystal oscillator 32.768 KHz */
    cal_dur_1 = (sleep_period * 2 * 1000) >> 5; //calculate for 32.768 KHz, transform to half_us. 
    cal_dur_2 = (sleep_period * 47) >> 5; //compensate for 32 KHz when use 32.768 KHz.
    cal_dur_3 = (cal_dur_1 - cal_dur_2)  / 625;  //calculate for 32.768 KHz, transform to half_slot.
#endif

    cal_dur_4 = cal_dur_1 - (cal_dur_3 * 625);  //convert half_slot.
    if(cal_dur_4 > (625 >> 1))
    {
        cal_dur_3 += 1; //compensate for half_slot in 32 or 32.768 KHz.
    }

    return cal_dur_3;
}
uint32_t curr_sleep_hs = 0;
static uint32_t sleep_flag_2 = 0;
static uint8_t sleep_start_flag = 0;
void idle_mode(void)
{
    MCU_SLEEP_MODE sleep_mode;
    uint32_t slot_h = 0;
    uint64_t tick_com = 0;
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

    uint32_t time_now_ms = (uint32_t)krhino_sys_time_get();
    GLOBAL_INT_DISABLE();
	{
        uint32_t sleep_time = 0;
        if (time_now_ms > idle_task_sleep_time) {
            sleep_time = ((time_now_ms - idle_task_sleep_time) > TOTAL_TIME_MS - WORK_TIME_MS ) ? (TOTAL_TIME_MS - WORK_TIME_MS) : (TOTAL_TIME_MS - (time_now_ms - idle_task_sleep_time) - WORK_TIME_MS);
        } else {
            sleep_time = TOTAL_TIME_MS - WORK_TIME_MS;
        }
        
        if (sleep_time > 10000) {
            printf("%s, sleep_time(%d) invalid.\n", __func__, sleep_time);
            return;
        }

		sleep = rwip_func.rwip_sleep(&slot_h, ((sleep_time) * 8)/5);      //no less than 6 slots. if set to 0, sleep forever
    }

    switch(sleep)
    {
        case RWIP_DEEP_SLEEP:
            if (idle_sleep_start) {
                idle_sleep_start();
            }
            rf_set_ldo_mode();
            //fclk_disable(FCLK_PWM_ID);
            sys_module_power_set(CLK_PWR_DEV_PWM1, 0);
            cpu_reduce_voltage_sleep();
            //sleep_mode_enable(0);
            cpu_wakeup();
            //hal_wdg_reload(NULL);
            hal_aon_wdt_feed();
			//sleep_mode_enable(0);
			break;
        case RWIP_CPU_SLEEP:
            //cpu_idle_sleep();
            //sleep_mode_enable(0);
            WFI();
            break;
        case RWIP_ACTIVE:
            break;
        default:
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

        tick_com = (((lv_sleep_period_get()) * RHINO_CONFIG_TICKS_PER_SECOND * 5) >> 4) / 1000;
        
        curr_sleep_hs += ((lv_sleep_period_get()) % 32);
        if(curr_sleep_hs >= 32)
        {
            curr_sleep_hs -= 32;
            //At least compensate 1 tick to pend idle thread
            tick_com += 1;
        }

        rf_set_bulk_mode();
        //fclk_update_tick(tick_com);
        //fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
        sys_module_power_set(CLK_PWR_DEV_PWM1, 1);
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
