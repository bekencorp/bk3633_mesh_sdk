
/* main.c - light demo */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include <hal/soc/gpio.h>
#include <hal/soc/pwm.h>
#include "inc/time_scene_server.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif

#include "genie_app.h"



#include "light_board.h"
#include "uart_test_cmd.h"

#include "JX_app.h"
#include "JX_vendor.h"
#include "JX_ble_remote.h"

#include "idle_mode.h"
#include "k_sys.h"

u8 JX_power_on_flag =0;
u8 JX_hard_reset_flag =0;
u8 JX_all_on_flag =0;
u8 JX_adv_prov_silence_flag =1;

u8 auto_publish_flag =0;
u8 auto_publish_now_flag =0;
u32 auto_publish_time_1ms =0;
u32 auto_publish_rand_time_100ms =0;

u8 JX_pin_check_flag =0;
u8 JX_sleep_check_flag =1;

u16 JX_dst_dev_addr =0;
JX_delay_struct JX_delay_flash_write;
JX_delay_struct JX_delay_auto_publish;
JX_delay_struct JX_delay_ota_start;
JX_delay_struct JX_delay_ota_finish;
JX_delay_struct JX_delay_prov;
u8 JX_prov_flag =0;
u8 JX_hard_reset_start_flag =0;

JX_delay_struct JX_delay_pin[3];

switch_para_struct switch_para;
switch_para_struct switch_para_temp ={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define JX_APP_START_STACK 350
#define JX_APP_TASK_PRI 32
static ktask_t jx_app_task_obj;
static cpu_stack_t jx_app_stack[JX_APP_START_STACK];
ksem_t jx_app_sem;
//struct k_delayed_work JX_app_10ms;
ktimer_t JX_app_10ms;
static volatile uint32_t jx_app_scan_time = 0;
static volatile uint32_t curr_time = 0;
led_ctrl_struct led_ctrl[3];
kmutex_t jx_app_mutex;

void JX_para_init(void)
{
	JX_prov_flag =0;
	JX_all_on_flag =0;
	JX_adv_prov_silence_flag =1;


	JX_pin_check_flag =0;
	JX_sleep_check_flag =1;

	memset((u8 *)&JX_delay_flash_write, 0, sizeof(JX_delay_flash_write));
	memset((u8 *)&JX_delay_auto_publish, 0, sizeof(JX_delay_auto_publish));
	memset((u8 *)&JX_delay_pin, 0, sizeof(JX_delay_pin));
	memset((u8 *)&JX_delay_prov, 0, sizeof(JX_delay_prov));
	memset((u8 *)&JX_delay_ota_start, 0, sizeof(JX_delay_ota_start));
	memset((u8 *)&JX_delay_ota_finish, 0, sizeof(JX_delay_ota_finish));

//	JX_delay_auto_publish.flag =1;
//	JX_delay_auto_publish.delay_time =aos_now_ms();
}

u32 JX_current_time(void)
{
	return (u32)krhino_ticks_to_ms(krhino_sys_tick_get());
}

void JX_auto_publish_loop(void)
{
	if(!JX_delay_auto_publish.flag)
	{
		return;
	}

	u32 current_time =JX_current_time();
	if(JX_delay_auto_publish.delay_time + 120 *1000 <=current_time)
	{
//		JX_delay_auto_publish.flag =1;
//		JX_delay_auto_publish.delay_time =aos_now_ms();

//		JX_set_auto_publish(5);
	}
}

void JX_auto_publish_status(void)
{
	if(!bt_mesh_is_provisioned())
	{//xxx
//        auto_publish_flag = 0;
		return;
	}

    krhino_mutex_lock(&jx_app_mutex, -1);

	if(auto_publish_flag)
	{
		u32 current_time =JX_current_time();

		if((current_time > auto_publish_time_1ms +auto_publish_rand_time_100ms) || auto_publish_now_flag)
		{
			u8 i;
			u8 pos =0;
			u8 pBuffer[10];

//			LIGHT_DBG("current_time:%d auto_publish_now_flag:%d auto_publish_time_1ms:%d auto_publish_rand_time_100ms:%d",
//					current_time, auto_publish_now_flag, auto_publish_time_1ms, auto_publish_rand_time_100ms);


			LIGHT_DBG("%s %d send_flag:%d", __func__, __LINE__, JX_vendor_flag.send_flag);
			if(JX_vendor_flag.send_flag)
			{
				krhino_mutex_unlock(&jx_app_mutex);
				return;
			}

			for(i=0; i<SWITCH_NUM; i++)
			{
				if(switch_para_temp.single_switch[i] != switch_para.single_switch[i])
				{//gen_onoff publish
					LIGHT_DBG("i:%d switch_para_temp:%d switch_para:%d",
							i, switch_para_temp.single_switch[i], switch_para.single_switch[i]);
					switch_para_temp.single_switch[i] = switch_para.single_switch[i];
					JX_onoff_send(i+1, switch_para.single_switch[i]);
				}

//				if(switch_para_temp.dymic_switch[i] != switch_para.dymic_switch[i])
//				{
//					LIGHT_DBG("i:%d dymic_switch_temp:%d dymic_switch:%d",
//							i, switch_para_temp.dymic_switch[i], switch_para.dymic_switch[i]);
//					switch_para_temp.dymic_switch[i] = switch_para.dymic_switch[i];
//					pBuffer[pos++] =(u8)DYMIC_SWITCH_ONOFF;
//					pBuffer[pos++] =(u8)(DYMIC_SWITCH_ONOFF>>8);
//					pBuffer[pos++] =switch_para.dymic_switch[i];
//					JX_vendor_auto_send(i+1, VD_HW_ATTR_NOTIFY, pBuffer, pos, 4);
//
//					return;
//				}
			}

			if(switch_para_temp.back_light != switch_para.back_light)
			{
				LIGHT_DBG("back_light_temp:%d back_light:%d", switch_para_temp.back_light, switch_para.back_light);
				switch_para_temp.back_light = switch_para.back_light;
				pBuffer[pos++] =(u8)BACKLIGHT_ONOFF;
				pBuffer[pos++] =(u8)(BACKLIGHT_ONOFF>>8);
				pBuffer[pos++] =switch_para.back_light;
				JX_vendor_auto_send(0, VD_HW_ATTR_NOTIFY, pBuffer, pos, 4);
                krhino_mutex_unlock(&jx_app_mutex);
				return;
			}

			if(switch_para_temp.memory_flag != switch_para.memory_flag)
			{
				LIGHT_DBG("memory_flag_temp:%d memory_flag:%d", switch_para_temp.memory_flag, switch_para.memory_flag);
				switch_para_temp.memory_flag = switch_para.memory_flag;
				pBuffer[pos++] =(u8)MEMORY_ONOFF;
				pBuffer[pos++] =(u8)(MEMORY_ONOFF>>8);
				pBuffer[pos++] =switch_para.memory_flag;
				JX_vendor_auto_send(0, VD_HW_ATTR_NOTIFY, pBuffer, pos, 4);
                krhino_mutex_unlock(&jx_app_mutex);
				return;
			}

			auto_publish_flag =0;
			auto_publish_now_flag =0;

			JX_delay_auto_publish.delay_time =current_time;
		}
	}

	krhino_mutex_unlock(&jx_app_mutex);
}

void JX_set_auto_publish(unsigned char priority)
{
	if(!bt_mesh_is_provisioned())
	{
//		  auto_publish_flag = 0;
		return;
	}

	krhino_mutex_lock(&jx_app_mutex, -1);

	u32 current_time =JX_current_time();
	u8 rand_value =0;

	if(bt_rand(&rand_value, 1) !=0)
	{
		rand_value =rand();
	}

	if(priority <=1)
	{
	    auto_publish_flag = 1;
	    auto_publish_now_flag = priority;
	    auto_publish_time_1ms = current_time;
	    auto_publish_rand_time_100ms = (3 + rand_value%5) *100;//0.3~0.8s

	}
	else if(priority ==2)
	{
	    auto_publish_flag = 1;
	    auto_publish_now_flag = 0;
	    auto_publish_time_1ms = current_time +2000;
	    auto_publish_rand_time_100ms = (3 + rand_value%5) *100;//2.1~2.6s
	}
	else if(priority ==5)
	{
	    auto_publish_flag = 1;
	    auto_publish_now_flag = 0;
	    auto_publish_time_1ms = current_time +3000;
	    auto_publish_rand_time_100ms = (3 + rand_value%5) *100;//3.3~3.8s

		switch_para_temp.single_switch[0] =0xFF;
		switch_para_temp.single_switch[1] =0xFF;
		switch_para_temp.single_switch[2] =0xFF;

		switch_para_temp.back_light =0xFF;

		switch_para_temp.dymic_switch[0] =0xFF;
		switch_para_temp.dymic_switch[1] =0xFF;
		switch_para_temp.dymic_switch[2] =0xFF;
	}
//	auto_publish_flag = 0;
//	LIGHT_DBG("current_time:%d auto_publish_now_flag:%d auto_publish_time_1ms:%d auto_publish_rand_time_100ms:%d", current_time, auto_publish_now_flag, auto_publish_time_1ms, auto_publish_rand_time_100ms);

	krhino_mutex_unlock(&jx_app_mutex);
}

void JX_switch_turn(u8 num)
{
//	LIGHT_DBG("num:%d dymic_switch:%d ", num, switch_para.dymic_switch[num]);
//	if(switch_para.dymic_switch[num])
//	{
//		return;
//	}

	if(switch_para.single_switch[num] ==SWITCH_ON)
	{
		JX_switch_light_set(num, SWITCH_OFF);
	}
	else
	{
		JX_switch_light_set(num, SWITCH_ON);
	}

	if(bt_mesh_is_provisioned())
	{
		JX_set_auto_publish(0);
	}
}


void JX_switch_light_set(u8 num, u8 value)
{
//	LIGHT_DBG("num:%d dymic_switch:%d ", num, switch_para.dymic_switch[num]);
//	if(switch_para.dymic_switch[num])
//	{
//		return;
//	}

	JX_switch_set(num, value);

	if(led_ctrl[num].ctrl_flag)
	{
		JX_led_ctrl_cancel(num);
	}

	JX_led_set_by_switch(num);
}

void JX_pin_loop(void)
{
	u8 i;
	gpio_dev_t gpio_temp;

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(JX_delay_pin[i].flag)
		{
			break;
		}
	}

	if(i ==SWITCH_NUM)
	{
		return;
	}

	u32 current_time =JX_current_time();

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(JX_delay_pin[i].flag)
		{
			if(current_time >= JX_delay_pin[i].delay_time +20)
			{
				JX_delay_pin[i].flag =0;
				JX_delay_pin[i].delay_time =current_time;

//				LIGHT_DBG("%s %d i:%d delay_time:%d",
//				__func__, __LINE__, i, current_time);
				switch(i)
				{
				case 0:
					gpio_temp.port =  SWITCH1_1;
					hal_gpio_output_low(&gpio_temp);
					gpio_temp.port =  SWITCH1_2;
					hal_gpio_output_low(&gpio_temp);
					break;
				case 1:
					gpio_temp.port =  SWITCH2_1;
					hal_gpio_output_low(&gpio_temp);
					gpio_temp.port =  SWITCH2_2;
					hal_gpio_output_low(&gpio_temp);
					break;
				case 2:
					gpio_temp.port =  SWITCH3_1;
					hal_gpio_output_low(&gpio_temp);
					gpio_temp.port =  SWITCH3_2;
					hal_gpio_output_low(&gpio_temp);
					break;
				}
			}
		}
	}
}

void JX_switch_set(u8 num, u8 value)
{
	u8 store_flash_flag =0;
	gpio_dev_t gpio_temp;
	u32 current_time =JX_current_time();

	JX_all_on_flag =0;

	switch(num)
	{
	case 0:
	{
//		if(switch_para.dymic_switch[num] ==0)
		{
			store_flash_flag =1;

			JX_delay_pin[num].flag =1;
			JX_delay_pin[num].delay_time =current_time;
//			LIGHT_DBG("%s %d delay_time:%d",
//			__func__, __LINE__, JX_delay_pin[num].delay_time);
			if(value ==SWITCH_ON)
			{
				gpio_temp.port =  SWITCH1_1;
				hal_gpio_output_high(&gpio_temp);
				gpio_temp.port =  SWITCH1_2;
				hal_gpio_output_low(&gpio_temp);

//				gpio_set(SWITCH1_1, 1);
//				gpio_set(SWITCH1_2, 1);
				switch_para.single_switch[num] =1;
			}
			else
			{
				gpio_temp.port =  SWITCH1_1;
				hal_gpio_output_low(&gpio_temp);
				gpio_temp.port =  SWITCH1_2;
				hal_gpio_output_high(&gpio_temp);

//				gpio_set(SWITCH1_1, 0);
//				gpio_set(SWITCH1_2, 0);
				switch_para.single_switch[num] =0;
			}
		}
		break;
	}
	case 1:
	{
//		if(switch_para.dymic_switch[num] ==0)
		{
			store_flash_flag =1;

			JX_delay_pin[num].flag =1;
			JX_delay_pin[num].delay_time =current_time;
//			LIGHT_DBG("%s %d delay_time:%d",
//			__func__, __LINE__, JX_delay_pin[num].delay_time);

			if(value ==SWITCH_ON)
			{
				gpio_temp.port =  SWITCH2_1;
				hal_gpio_output_high(&gpio_temp);
				gpio_temp.port =  SWITCH2_2;
				hal_gpio_output_low(&gpio_temp);

//				gpio_set(SWITCH2_1, 1);
//				gpio_set(SWITCH2_2, 1);
				switch_para.single_switch[num] =1;
			}
			else
			{
				gpio_temp.port =  SWITCH2_1;
				hal_gpio_output_low(&gpio_temp);
				gpio_temp.port =  SWITCH2_2;
				hal_gpio_output_high(&gpio_temp);

//				gpio_set(SWITCH2_1, 0);
//				gpio_set(SWITCH2_2, 0);
				switch_para.single_switch[num] =0;
			}
		}
		break;
	}
	case 2:
	{
//		if(switch_para.dymic_switch[num] ==0)
		{
			store_flash_flag =1;
			JX_delay_pin[num].flag =1;
			JX_delay_pin[num].delay_time =current_time;

			if(value ==SWITCH_ON)
			{
				gpio_temp.port =  SWITCH3_1;
				hal_gpio_output_high(&gpio_temp);
				gpio_temp.port =  SWITCH3_2;
				hal_gpio_output_low(&gpio_temp);

//				gpio_set(SWITCH3_1, 1);
//				gpio_set(SWITCH3_2, 1);
				switch_para.single_switch[num] =1;
			}
			else
			{
				gpio_temp.port =  SWITCH3_1;
				hal_gpio_output_low(&gpio_temp);
				gpio_temp.port =  SWITCH3_2;
				hal_gpio_output_high(&gpio_temp);

//				gpio_set(SWITCH3_1, 0);
//				gpio_set(SWITCH3_2, 0);
				switch_para.single_switch[num] =0;
			}
		}
		break;
	}
	default:
		break;
	}

	if(store_flash_flag && JX_hard_reset_start_flag==0)
	{
		JX_set_auto_publish(0);
		JX_delay_flash_write.flag =1;
		JX_delay_flash_write.delay_time =current_time;
	}
}

///*
void JX_dymic_switch_set(u8 num, u8 value)
{
//	if(value ==SWITCH_ON)
//	{
//		switch_para.single_switch_last[num] =switch_para.single_switch[num];
////		LIGHT_DBG("on num:%d single_switch_last:%d", num, switch_para.single_switch_last[num]);
//		JX_switch_light_set(num, SWITCH_ON);
//		switch_para.dymic_switch[num] =1;
//	}
//	else
//	{
////		LIGHT_DBG("off num:%d single_switch_last:%d", num, switch_para.single_switch_last[num]);
//		switch_para.dymic_switch[num] =0;
//		if(switch_para.single_switch_last[num] ==SWITCH_ON)
//		{
//			JX_switch_light_set(num, SWITCH_ON);
//		}
//		else
//		{
//			JX_switch_light_set(num, SWITCH_OFF);
//		}
//	}
//
//	JX_set_auto_publish(0);
//	JX_delay_flash_write.flag =1;
//	JX_delay_flash_write.delay_time =aos_now_ms();
}
//*/

void JX_proving_switch_set(void)
{
	u8 i;
	gpio_dev_t gpio_temp;
	u32 current_time;
	static u8 hard_reset_flag =0;

	if(JX_hard_reset_flag != hard_reset_flag)
	{
		hard_reset_flag =JX_hard_reset_flag;

		if(JX_hard_reset_flag)
		{
			current_time =JX_current_time();
			for(i=0; i<SWITCH_NUM; i++)
			{
				switch_para.single_switch_last[i] =switch_para.single_switch[i];
				JX_led_ctrl_set(i, 200, 60000/400);
				JX_switch_set(i, SWITCH_ON);

				JX_delay_pin[i].flag =1;
				JX_delay_pin[i].delay_time =current_time;
//				switch(i)
//				{
//					case 0:
//						gpio_temp.port =  SWITCH1_1;
//						hal_gpio_output_high(&gpio_temp);
//						gpio_temp.port =  SWITCH1_2;
//						hal_gpio_output_low(&gpio_temp);
//						break;
//					case 1:
//						gpio_temp.port =  SWITCH2_1;
//						hal_gpio_output_high(&gpio_temp);
//						gpio_temp.port =  SWITCH2_2;
//						hal_gpio_output_low(&gpio_temp);
//						break;
//					case 2:
//						gpio_temp.port =  SWITCH3_1;
//						hal_gpio_output_high(&gpio_temp);
//						gpio_temp.port =  SWITCH3_2;
//						hal_gpio_output_low(&gpio_temp);
//						break;
//					default:
//						break;
//				}
			}
		}
//		else
//		{
//			for(i=0; i<SWITCH_NUM; i++)
//			{
//				switch_para.single_switch[i] =switch_para.single_switch_last[i];
//
//				JX_delay_pin[i].flag =1;
//				JX_delay_pin[i].delay_time =current_time;
//				JX_switch_light_set(i, switch_para.single_switch[i]);
//			}
//		}
	}

}

///*
void JX_backlight_set(u8 value)
{
	u8 i;
	gpio_dev_t gpio_temp;

	LIGHT_DBG("value:%d", value);
	if(value ==0)
	{
		switch_para.back_light =0;

		gpio_temp.port =LED1;
 		hal_gpio_output_high(&gpio_temp);
		gpio_temp.port =LED2;
 		hal_gpio_output_high(&gpio_temp);
		gpio_temp.port =LED3;
 		hal_gpio_output_high(&gpio_temp);

//		gpio_set(LED1, 1);
//		gpio_set(LED2, 1);
//		gpio_set(LED3, 1);
		for(i=0; i<SWITCH_NUM; i++)
		{
			led_ctrl[i].led_status =0;
		}
	}
	else
	{
		switch_para.back_light =1;
		for(i=0; i<SWITCH_NUM; i++)
		{
			if(switch_para.single_switch[i] ==SWITCH_ON)
			{
				JX_switch_light_set(i, SWITCH_ON);
			}
			else
			{
				JX_switch_light_set(i, SWITCH_OFF);
			}
		}

	}

	JX_set_auto_publish(0);
	JX_delay_flash_write.flag =1;
	JX_delay_flash_write.delay_time =(u32)krhino_ticks_to_ms(krhino_sys_tick_get());
}
//*/

void JX_memory_init(u16 reset_value)
{
	u8 i;

	printf("\r\n %s %d reset_value:%d memory_flag", __func__, __LINE__, reset_value, switch_para.memory_flag);
	if(reset_value >=REG_OTA && reset_value <=REG_APP_START)
	{
		for(i=0; i<SWITCH_NUM; i++)
		{
			if(switch_para.single_switch[i] ==SWITCH_ON)
			{
				JX_switch_light_set(i, SWITCH_ON);
			}
			else
			{
				JX_switch_light_set(i, SWITCH_OFF);
			}
		}
	}
	else
	{
		switch_para.back_light =1;

		switch(switch_para.memory_flag)
		{
			case 0:
				for(i=0; i<SWITCH_NUM; i++)
				{
					JX_switch_light_set(i, SWITCH_OFF);
				}
				break;
			case 1:
				for(i=0; i<SWITCH_NUM; i++)
				{
					JX_switch_light_set(i, SWITCH_ON);
				}
				break;
			case 2:
				for(i=0; i<SWITCH_NUM; i++)
				{
					if(switch_para.single_switch[i] ==SWITCH_ON)
					{
						JX_switch_light_set(i, SWITCH_ON);
					}
					else
					{
						JX_switch_light_set(i, SWITCH_OFF);
					}
				}
				break;
		}
	}
}
void JX_memory_set(u8 value)
{
	u8 i;
	gpio_dev_t gpio_temp;

	LIGHT_DBG("value:%d", value);
	switch_para.memory_flag =value;

	JX_set_auto_publish(0);
	JX_delay_flash_write.flag =1;
	JX_delay_flash_write.delay_time =(u32)krhino_ticks_to_ms(krhino_sys_tick_get());
}

void JX_led_set_by_switch(u8 num)
{
	if(switch_para.single_switch[num] ==SWITCH_ON)
	{
		JX_led_set(num, LED_OFF);
	}
	else
	{
		JX_led_set(num, LED_ON);
	}
}

void JX_led_set(u8 num, u8 value)
{
	if(!switch_para.back_light)
	{
		return;
	}

	gpio_dev_t gpio_temp;

	switch(num)
	{
	case 0:
	{
		if(value ==LED_ON)
		{
    		gpio_temp.port =  LED1;
	 		hal_gpio_output_low(&gpio_temp);

//			gpio_set(LED1, 0);
			led_ctrl[num].led_status =1;
		}
		else
		{
    		gpio_temp.port =  LED1;
	 		hal_gpio_output_high(&gpio_temp);

//			gpio_set(LED1, 1);
			led_ctrl[num].led_status =0;
		}
		break;
	}
	case 1:
	{
		if(value ==LED_ON)
		{
    		gpio_temp.port =  LED2;
	 		hal_gpio_output_low(&gpio_temp);

//			gpio_set(LED2, 0);
			led_ctrl[num].led_status =1;
		}
		else
		{
    		gpio_temp.port =  LED2;
	 		hal_gpio_output_high(&gpio_temp);

//			gpio_set(LED2, 1);
			led_ctrl[num].led_status =0;
		}
		break;
	}
	case 2:
	{
		if(value ==LED_ON)
		{
    		gpio_temp.port =  LED3;
	 		hal_gpio_output_low(&gpio_temp);

//			gpio_set(LED3, 0);
			led_ctrl[num].led_status =1;
		}
		else
		{
    		gpio_temp.port =  LED3;
	 		hal_gpio_output_high(&gpio_temp);

//			gpio_set(LED3, 1);
			led_ctrl[num].led_status =0;
		}
		break;
	}
	}
}

void JX_led_ctrl_set(u8 led_num, u32 flash_freq_time, u16 flash_cnt)
{
	u32 keep_time_ms =0;
	u32 current_time =JX_current_time();

	led_ctrl_struct *p_led_ctrl =&led_ctrl[led_num];

	keep_time_ms =flash_freq_time*flash_cnt*2;

	p_led_ctrl->ctrl_flag =1;
	p_led_ctrl->flash_freq_time =flash_freq_time;
	p_led_ctrl->flash_time_cur =current_time;
	p_led_ctrl->flash_time_dst =current_time + keep_time_ms;

	JX_led_set(led_num, LED_ON);
//	LIGHT_DBG("flash_time_cur:%d current_time:%d flash_time_dst:%d ",
//		p_led_ctrl->flash_time_cur, current_time, p_led_ctrl->flash_time_dst);
}

void JX_led_ctrl_cancel(u8 led_num)
{
	led_ctrl_struct *p_led_ctrl =&led_ctrl[led_num];

	p_led_ctrl->ctrl_flag =0;
	p_led_ctrl->flash_time_cur =0;
	p_led_ctrl->flash_time_dst =0;
}

void JX_led_ctrl_func(void)
{
	u8 i;
	static u32 flash_freq_time[3] ={0, 0, 0};

//	LIGHT_DBG("ctrl_flag0:%d ctrl_flag1:%d ctrl_flag2:%d",
//		led_ctrl[0].ctrl_flag, led_ctrl[1].ctrl_flag, led_ctrl[2].ctrl_flag);

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(led_ctrl[i].ctrl_flag)
		{
			break;
		}
	}

	if(i ==SWITCH_NUM)
	{
		return;
	}

	u32 current_time =JX_current_time();

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(led_ctrl[i].ctrl_flag)
		{
			if(flash_freq_time[i] ==0)
			{
				flash_freq_time[i] =current_time;
			}

//			LIGHT_DBG("flash_time_cur:%d current_time:%d flash_time_dst:%d flash_freq_time:%d",
//				led_ctrl[i].flash_time_cur, current_time, led_ctrl[i].flash_time_dst, flash_freq_time[i]);

			led_ctrl[i].flash_time_cur =current_time;//aos_now_ms();
			if(led_ctrl[i].flash_time_cur >= led_ctrl[i].flash_time_dst)
			{
				led_ctrl[i].ctrl_flag =0;
				led_ctrl[i].flash_time_cur =0;
				led_ctrl[i].flash_time_dst =0;

				flash_freq_time[i] =0;

				JX_led_set_by_switch(i);
			}
			else
			{
				if(current_time < flash_freq_time[i])
				{
					flash_freq_time[i] =current_time;
				}

				if((current_time -flash_freq_time[i]) >= led_ctrl[i].flash_freq_time)
				{
					if(led_ctrl[i].led_status == LED_ON)
					{
						JX_led_set(i, LED_OFF);
					}
					else
					{
						JX_led_set(i, LED_ON);
					}

					flash_freq_time[i] =current_time;
				}
			}
		}
	}
}

void JX_flash_switch_para_init(void)
{
	memset((u8 *)&switch_para, 0, sizeof(switch_para));
	switch_para.single_switch[0] =0;
	switch_para.single_switch[1] =0;
	switch_para.single_switch[2] =0;
	switch_para.back_light =1;
	switch_para.dymic_switch[0] =0;
	switch_para.dymic_switch[1] =0;
	switch_para.dymic_switch[2] =0;
	switch_para.memory_flag =0;

	JX_flash_write_switch_para();
}

void JX_flash_write_loop(void)
{
	if(!JX_delay_flash_write.flag)
	{
		return;
	}

	u32 current_time =JX_current_time();

	if(JX_delay_flash_write.delay_time +1000 <=current_time)
	{
		JX_delay_flash_write.flag =0;
		JX_delay_flash_write.delay_time =current_time;

		JX_flash_write_switch_para();
	}
}

void JX_flash_write_switch_para(void)
{
//    uint8_t data = count;
//
    LIGHT_DBG("%d ", __LINE__);

    genie_flash_write_userdata(GFI_SWITCH_PARA, (u8 *)&switch_para, sizeof(switch_para));

//	JX_flash_read_switch_para();
}

void JX_flash_read_switch_para(void)
{
	u8 i;
    E_GENIE_FLASH_ERRCODE ret;

    LIGHT_DBG("%d", __LINE__);
    ret = genie_flash_read_userdata(GFI_SWITCH_PARA, (u8 *)&switch_para, sizeof(switch_para));
    LIGHT_DBG("%d ret:%d", __LINE__, ret);

    if(ret != GENIE_FLASH_SUCCESS)
	{
        LIGHT_DBG("read error %d", ret);
		JX_flash_switch_para_init();
    }

//    return ret;
}

gpio_irq_handler_t JX_irp_func(void)
{
    os_printf("~~%s\r\n\r\n", __func__);
}

void JX_key_func(void)
{
	u8 i, j;
	u32 key_value[3];
	static u8 key_value_last[3] ={0, 0, 0};

	static u32 key_low_cnt[3] ={0, 0, 0};

	static u8 device_status_short_cnt[3] ={0, 0, 0};
	static u8 device_status_short_time[3] ={0, 0, 0};

	static u8 key_led_flag[3];

	u8 pin_check_flag =0;

	u32 cur_time =(u32)krhino_ticks_to_ms(krhino_sys_tick_get())/10;
	static u8 key_low_flag[3] ={0}; //krhino_ticks_to_ms
	static u32 key_low_time[3] ={0}; //krhino_ticks_to_ms

	gpio_dev_t gpio_temp;

#if DEV_TYPE == DEV_1_KEY
	gpio_temp.port =  KEY1;
	hal_gpio_input_get(&gpio_temp, &key_value[0]);
//	key_value[0] =gpio_get_input(KEY1);
#elif DEV_TYPE == DEV_2_KEY
	gpio_temp.port =  KEY1;
	hal_gpio_input_get(&gpio_temp, &key_value[0]);
	gpio_temp.port =  KEY2;
	hal_gpio_input_get(&gpio_temp, &key_value[1]);
#elif DEV_TYPE == DEV_3_KEY
	gpio_temp.port =  KEY1;
	hal_gpio_input_get(&gpio_temp, &key_value[0]);
	gpio_temp.port =  KEY2;
	hal_gpio_input_get(&gpio_temp, &key_value[1]);
	gpio_temp.port =  KEY3;
	hal_gpio_input_get(&gpio_temp, &key_value[2]);

//	key_value[0] =gpio_get_input(KEY1);
//	key_value[1] =gpio_get_input(KEY2);
//	key_value[2] =gpio_get_input(KEY3);
#endif

#define KEY_DEBUG 1
//	LIGHT_DBG("key_value:%d key_low_cnt:%d device_status_short_cnt:%d device_status_short_time:%d ",
//		key_value[KEY_DEBUG], key_low_cnt[KEY_DEBUG], device_status_short_cnt[KEY_DEBUG], device_status_short_time[KEY_DEBUG]);
//	LIGHT_DBG("key_low_flag:%d key_low_time:%d cur_time:%d data:%d ",
//		key_low_flag[KEY_DEBUG], key_low_time[KEY_DEBUG], cur_time, cur_time -key_low_time[KEY_DEBUG]);

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(key_value[i] && key_value[i] !=key_value_last[i])
		{
			if(key_low_flag[i])
			{
				key_low_flag[i] =0;
				if(cur_time > key_low_time[i])
				{
					key_low_cnt[i] =cur_time -key_low_time[i];
					key_low_time[i] =cur_time;
				}
LIGHT_DBG("cur_time:%d key_low_time:%d key_low_cnt:%d ", cur_time, key_low_time[i], key_low_cnt[i]);
			}

			if(key_low_cnt[i] >=2 && key_low_cnt[i] <50)
			{
				device_status_short_cnt[i] ++;

				key_led_flag[i] =0;
				JX_switch_turn(i);

	//			JX_led_ctrl_set(0, 50, 3000);
			}
	//		else if(key1_low_cnt >=100 && key1_low_cnt <300)
	//		{
	//			device_status_short_cnt[num] ++;
	//			if(device_status_short_cnt[num] >=4)
	//			{
	//				device_status_short_cnt[num] =0;
	//			}
	//
	////			JX_led_ctrl_set(0, 50, 5000);
	//		}
//			else if(key_low_cnt[i] >=500 && key_low_cnt[i] <1000 && switch_para.single_switch[i] == SWITCH_ON)
			else if(key_low_cnt[i] >=500 && key_low_cnt[i] <1000)
			{//5s~10s
				if(device_status_short_cnt[i] ==1)
				{
					key_led_flag[i] =0;
					JX_led_ctrl_set(i, 170, 60000/(170*2));

					JX_ble_pair_set(i, 1);
				}
				else if((device_status_short_cnt[i] ==3) && (i+1) ==SWITCH_NUM)
				{
					for(j=0; j<SWITCH_NUM; j++)
					{
						if(switch_para.single_switch[j] ==SWITCH_OFF)
						{
							break;
						}
					}
					LIGHT_DBG("key hard value %d", j);

//					if(j==SWITCH_NUM)
					{
						LIGHT_DBG("key hard reset");
						JX_hard_reset_start_flag =1;

						key_led_flag[i] =0;

						JX_hard_reset_flag =1;
						switch_para.back_light =1;
						for(j=0; j<SWITCH_NUM; j++)
						{
							switch_para.dymic_switch[j] =0;
							JX_switch_set(j, SWITCH_ON);
						}

						icu_set_reset_reason(REG_HARD_RESET);
						JX_led_ctrl_set(i, 50, 3000);

						JX_vendor_hard_reset(5);
						genie_event(GENIE_EVT_REPEAT_RESET_START, NULL);
					}
				}
				else if((device_status_short_cnt[i] ==5) && i ==0)
				{
					for(j=0; j<SWITCH_NUM; j++)
					{
						if(switch_para.single_switch[j] ==SWITCH_OFF)
						{
							break;
						}
					}
					LIGHT_DBG("key OTA bug %d", j);

					if(j==SWITCH_NUM)
					{
						bt_mesh_scan_stop();
                        idle_mode_never_sleep(1);
						JX_delay_ota_start.flag =1;
						JX_delay_ota_start.delay_time =(u32)krhino_ticks_to_ms(krhino_sys_tick_get());

						ota_init(0);
						LIGHT_DBG("key OTA on");
						bt_mesh_proxy_adv_enabled();
					}
				}

				device_status_short_cnt[i] =0;
			}
			else if(key_low_cnt[i] >=1000)
			{
				if(device_status_short_cnt[i] ==1)
				{
					LIGHT_DBG("key ble remote delete pair:%d", i);
					key_led_flag[i] =0;
					JX_ble_mac_one_key_init(i);
					JX_led_ctrl_set(i, 500, 5);
				}
	//			else if(device_status_short_cnt[num] ==3)
	//			{
	//				JX_led_ctrl_set(0, 25, 2500);
	//			}
			}
			else if(key_low_cnt[i])
			{
//				LIGHT_DBG("%d", __LINE__);
				device_status_short_cnt[i] =0;
			}

			if(key_led_flag[i] ==1)
			{
				key_led_flag[i] =0;
				JX_led_set_by_switch(i);
			}

			device_status_short_time[0] =0;

			key_low_cnt[i] =0;
		}
		else if(key_value[i] == 1)
		{
			if(key_low_flag[i])
			{
				key_low_flag[i] =0;
//				if(cur_time > key_low_time[i])
//				{
//					key_low_cnt[i] =(cur_time -key_low_time[i])/10;
//				}
//LIGHT_DBG("cur_time:%d key_low_time:%d key_low_cnt:%d ", cur_time, key_low_time[i], key_low_cnt[i]);
			}

			if(device_status_short_cnt[i])
			{
				device_status_short_time[i] ++;
				if(device_status_short_time[i] >=100)
				{
//					LIGHT_DBG("%d", __LINE__);
					device_status_short_cnt[i] =0;
					device_status_short_time[i] =0;
				}
			}
			else if(device_status_short_time[i])
			{
				device_status_short_time[i] =0;
			}
		}
		else if(key_value[i] == 0)
		{
			pin_check_flag =1;

//			if(key_low_cnt[i]>=500)
			if(key_low_flag[i] && cur_time >=(key_low_time[i]+500))
			{
				if(key_led_flag[i] ==0)
				{
					key_led_flag[i] =1;
					JX_led_set(i, LED_ON);
				}
			}

//			if(key_low_cnt[i]>=1000)
			if(key_low_flag[i] && cur_time >=(key_low_time[i]+1000))
			{
				if(key_led_flag[i] ==1)
				{
					key_led_flag[i] =1;
					JX_led_set(i, LED_OFF);
				}
			}

			if(key_low_flag[i] ==0)
			{
				key_low_time[i] =cur_time;
				key_low_flag[i] =1;
			}

//			if(key_low_flag[i])
//			{
//				if(cur_time > key_low_time[i])
//				{
//					key_low_cnt[i] +=cur_time -key_low_time[i];
//					key_low_time[i] =cur_time;
//				}
//LIGHT_DBG("cur_time:%d key_low_time:%d key_low_cnt:%d ", cur_time, key_low_time[i], key_low_cnt[i]);
//			}

//			if(key_low_cnt[i] < 1200)
//			{
//				key_low_cnt[i] ++;
//			}


			if(device_status_short_time[i])
			{
				device_status_short_time[i] =0;
			}
		}

		if(key_value[i] !=key_value_last[i])
		{
			key_value_last[i] =key_value[i];
		}
	}

	JX_pin_check_flag =pin_check_flag;
}

void JX_ota_reset(void)
{
	u8 i;
	u32 current_time =JX_current_time();

	icu_set_reset_reason(REG_OTA);

	JX_delay_ota_finish.flag =1;
	JX_delay_ota_finish.delay_time =current_time;

	for(i=0; i<SWITCH_NUM; i++)
	{
		JX_led_ctrl_set(i, 1000, 4);
	}

//	JX_led_ctrl_set(0, 100, 8000);
//	JX_led_ctrl_set(1, 100, 8000);
//	JX_led_ctrl_set(2, 100, 8000);
}

void JX_ota_loop(void)
{
	if(!(JX_delay_ota_start.flag || JX_delay_ota_finish.flag))
	{
		return;
	}

	u32 current_time =JX_current_time();

	if(JX_delay_ota_start.flag)
	{
		if(current_time >= JX_delay_ota_start.delay_time +60*1000)
		{
			LIGHT_DBG("OTA Off ");
			JX_delay_ota_start.flag =0;
			JX_delay_ota_start.delay_time =current_time;

//			sleep_mode_enable(0);
//			idle_mode_never_sleep(0);
			aos_reboot();
		}
	}

	if(JX_delay_ota_finish.flag)
	{
		if(current_time >= JX_delay_ota_finish.delay_time +8100)
		{
			JX_delay_ota_finish.flag =0;
			JX_delay_ota_finish.delay_time =current_time;
			aos_reboot();
		}
	}
}

void JX_prov_start_loop(void)
{
	if(JX_delay_prov.flag)
	{
		u32 current_time =JX_current_time();
		if(JX_delay_prov.delay_time + 100 <=current_time)
		{
			JX_delay_prov.flag =0;
			JX_delay_prov.delay_time =current_time;
		}
	}
}

void JX_sleep_loop(void)
{
	u8 i;
	static u8 JX_10ms_cnt =0;

//	printf("%s, %d \r\n", __func__, __LINE__);
// 	if(get_rw_sleep_flag())
// 	{
// //		printf("%s, %d \r\n", __func__, __LINE__);
// 		return;
// 	}

	if(get_sleep_flag() == 1)
	{
    //     //printf("%s, %d \r\n", __func__, __LINE__);
	    return;
	}

	if(JX_sleep_check_flag ==0)
	{
		curr_time = (uint32_t)krhino_sys_time_get();

		//printf("curr_time %d, jx_app_scan_time %d\r\n", curr_time, jx_app_scan_time);
		if ((curr_time - jx_app_scan_time) >= (200 - ((WORK_TIME_10MS + 1) * 10))) {
			if(JX_delay_ota_start.flag == 0) {
//				printf("\r\n %s scan enable \r\n", __func__);
				bt_mesh_scan_enable();
			}

			JX_sleep_check_flag = 1;
			//bt_mesh_proxy_adv_enabled();
			if(JX_adv_prov_silence_flag)
			{
				genie_pbadv_timer_stop();
			}
			else
			{
				bt_mesh_beacon_enable();
			}
		}
        JX_10ms_cnt = 0;
	}

	if(JX_delay_ota_start.flag)
	{
//		LIGHT_DBG("%s %d \r\n", __func__, __LINE__);
		return;
	}

//	LIGHT_DBG("%s %d \r\n", __func__, __LINE__);
//	if(JX_vendor_flag.send_flag || auto_publish_flag)
//	{
////		LIGHT_DBG("%s %d \r\n", __func__, __LINE__);
//		return;
//	}

	if(JX_delay_prov.flag ==0 && JX_hard_reset_flag ==1)
	{
//		for(i=0; i<SWITCH_NUM; i++)
//		{
//			if(switch_para.single_switch[i] ==SWITCH_OFF)
//			{
////				break;
//			}
//		}

//		if(i == SWITCH_NUM && JX_all_on_flag)
//		{
			return;
//		}
	}

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(JX_delay_pin[i].flag)
		{
			return;
		}

//		if(JX_ble_pair_flag[i].flag)
//		{
//			return;
//		}
	}

//	if(JX_pin_check_flag ==1 || i != SWITCH_NUM)
	if(i != SWITCH_NUM)
	{
//		printf("%s, %d \r\n", __func__, __LINE__);
		return;
	}

	JX_10ms_cnt++;
//	LIGHT_DBG("%s %d JX_10ms_cnt:%d ", __func__, __LINE__, JX_10ms_cnt);
	if(JX_10ms_cnt >WORK_TIME_10MS)
	{
//		if(i== SWITCH_NUM && JX_pin_check_flag ==0)
//		if(JX_pin_check_flag ==0)
		{
//			LIGHT_DBG("%s %d JX_10ms_cnt:%d ", __func__, __LINE__, JX_10ms_cnt);
			JX_10ms_cnt =0;
//			LIGHT_DBG("%s %d delay_time:%d",
//			__func__, __LINE__, aos_now_ms());

//			if(JX_all_on_flag ==0)
//			{
//				 ++;
//			}

			//bt_mesh_beacon_disable();
			bt_mesh_proxy_adv_disabled();
			bt_mesh_scan_stop();
			JX_sleep_check_flag =0;
			JX_all_on_flag =1;

			//bt_mesh_adv_stop();
			// printf("%s\n", __func__);
            sleep_mode_enable(1);
			// Record the time when stop the scan.
            jx_app_scan_time = (uint32_t)krhino_sys_time_get();
			idle_task_set_sleep_start_time(jx_app_scan_time);
		}

		JX_10ms_cnt =0;
	}
//	printf("%s %d \r\n", __func__, __LINE__);
}

void JX_gpio_init(void)
{
	gpio_dev_t gpio_temp;

    gpio_temp.port =  SWITCH1_1;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  SWITCH2_1;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  SWITCH3_1;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  SWITCH1_2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  SWITCH2_2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  SWITCH3_2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);


    gpio_temp.port =  LED1;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);

    gpio_temp.port =  LED2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);

    gpio_temp.port =  LED3;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);


	gpio_temp.port =  KEY1;
 	gpio_temp.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_temp);
    hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_BOTH_EDGES/*IRQ_TRIGGER_FALLING_EDGE*/, NULL, NULL);

	gpio_temp.port =  KEY2;
 	gpio_temp.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_temp);
    hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_BOTH_EDGES /*IRQ_TRIGGER_FALLING_EDGE*/, NULL, NULL);

	gpio_temp.port =  KEY3;
 	gpio_temp.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_temp);
	hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_BOTH_EDGES, NULL, NULL);

/*
	gpio_dev_t gpio;

    gpio.port =  GPIO_P07;

 gpio.config = 2;

    hal_gpio_init(&gpio);
 hal_gpio_output_low(&gpio);

    gpio.port =  GPIO_P31;

 gpio.config = OUTPUT_PUSH_PULL;

    hal_gpio_init(&gpio);
 hal_gpio_output_low(&gpio);
*/

/*
	gpio_config	(SWITCH1_1, OUTPUT, PULL_HIGH);
	gpio_set	(SWITCH1_1, 0);
	gpio_config	(SWITCH2_1, OUTPUT, PULL_HIGH);
	gpio_set	(SWITCH2_1, 0);
	gpio_config	(SWITCH3_1, OUTPUT, PULL_HIGH);
	gpio_set	(SWITCH3_1, 0);

	gpio_config	(SWITCH1_2, OUTPUT, PULL_HIGH);
	gpio_set	(SWITCH1_2, 0);
	gpio_config	(SWITCH2_2, OUTPUT, PULL_HIGH);
	gpio_set	(SWITCH2_2, 0);
	gpio_config	(SWITCH3_2, OUTPUT, PULL_HIGH);
	gpio_set	(SWITCH3_2, 0);


	gpio_config	(LED1, OUTPUT, PULL_HIGH);
	gpio_set	(LED1, 1);
	gpio_config	(LED2, OUTPUT, PULL_HIGH);
	gpio_set	(LED2, 1);
	gpio_config	(LED3, OUTPUT, PULL_HIGH);
	gpio_set	(LED3, 1);

	gpio_config	(KEY1, INPUT, PULL_HIGH);
	gpio_config	(KEY2, INPUT, PULL_HIGH);
	gpio_config	(KEY3, INPUT, PULL_HIGH);
*/
}

static bool jx_task_pro_finish = true;
static void JX_app_timer_cb(void *p_timer)
{
    krhino_timer_start(&JX_app_10ms);

	JX_pin_loop();

	if (jx_task_pro_finish) {
	    krhino_sem_give(&jx_app_sem);
	}
}

static void jx_app_task_hdl(void *param)
{
	while(1) {
        krhino_sem_take(&jx_app_sem, -1);
		jx_task_pro_finish = false;
		JX_key_func();

		JX_led_ctrl_func();

		JX_auto_publish_status();

		JX_vendor_loop();

		JX_ble_remote_func();

		JX_auto_publish_loop();

		JX_flash_write_loop();

		JX_prov_start_loop();

		JX_ota_loop();

		JX_sleep_loop();

		jx_task_pro_finish = true;
	}
}

static void jx_app_deep_sleep_wake_up(void)
{
	// Restart the timer
	//krhino_timer_start(&JX_app_10ms);
}

static void jx_app_deep_sleep_start(void)
{
	//Stop the timer before deep sleep, Avoid clock inaccuracy during deep sleep.
	//krhino_timer_stop(&JX_app_10ms);
}

void JX_app_start(void)
{
	krhino_sem_create(&jx_app_sem, "jx-app-sem", 1);
	krhino_mutex_create(&jx_app_mutex, "jx-app-mutex");
    krhino_task_create(&jx_app_task_obj, "jx-app", NULL, JX_APP_TASK_PRI, 0,
                       jx_app_stack, sizeof(jx_app_stack) / sizeof(cpu_stack_t),
                       (task_entry_t)jx_app_task_hdl, 1);
    idle_task_register_hook(jx_app_deep_sleep_wake_up);
    idle_task_register_sleep_start(jx_app_deep_sleep_start);
	printf("%s, jx_app_stack %p\n", __func__, &jx_app_stack[JX_APP_START_STACK]);
	//sleep_mode_enable(1);
	/* because the "init and erase the flash" is a time-consuming operation,
	 * keep sleep to let the cpu go to sleep mode before "init and erase the flash".
	 */
	//k_sleep(50);

	LIGHT_DBG("%s %d ", __func__, __LINE__);
	JX_para_init();
	JX_gpio_init();

//	gpio_wakeup_test_init();
///*
	JX_flash_read_switch_para();

	JX_vendor_init();

	JX_ble_init();

//	k_delayed_work_init(&JX_app_10ms, JX_app_timer_cb);
//	k_delayed_work_submit(&JX_app_10ms, 10);
//	*/
//	krhino_task_create(&JX_app_main_task_obj, "JX_app_main", NULL,
//							  JX_APP_MAIN_TASK_PRIO, 0, JX_app_main_task,
//							  sizeof(JX_app_main_task) / sizeof(cpu_stack_t),
//							  (task_entry_t)JX_app_main, 1);
}

void JX_app_start_timer(void)
{
	if(JX_delay_flash_write.flag)
	{
		JX_delay_flash_write.delay_time =(u32)krhino_ticks_to_ms(krhino_sys_tick_get());
	}

	krhino_timer_create(&JX_app_10ms, "JX_APP_TIMER", JX_app_timer_cb,
	                    1, 0, NULL, true);
}


