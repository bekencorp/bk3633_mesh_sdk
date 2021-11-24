
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

u8 JX_prov_flag =0;
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
JX_delay_struct JX_delay_ota;

JX_delay_struct JX_delay_pin[3];

switch_para_struct switch_para;
switch_para_struct switch_para_temp ={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


//struct k_delayed_work JX_app_10ms;
ktimer_t JX_app_10ms;

led_ctrl_struct led_ctrl[3];

void JX_para_init(void)
{
	JX_prov_flag =0;
	JX_adv_prov_silence_flag =1;


	JX_pin_check_flag =0;
	JX_sleep_check_flag =1;

	memset((u8 *)&JX_delay_flash_write, 0, sizeof(JX_delay_flash_write));
	memset((u8 *)&JX_delay_auto_publish, 0, sizeof(JX_delay_auto_publish));
	memset((u8 *)&JX_delay_pin, 0, sizeof(JX_delay_pin));
	memset((u8 *)&JX_delay_ota, 0, sizeof(JX_delay_ota));

//	JX_delay_auto_publish.flag =1;
//	JX_delay_auto_publish.delay_time =aos_now_ms();
}

void JX_auto_publish_loop(void)
{
	if(!JX_delay_auto_publish.flag)
	{
		return;
	}

	if(JX_delay_auto_publish.delay_time + 120 *1000 <=aos_now_ms())
	{//��ʱ120s���ͺ���
//		JX_delay_auto_publish.flag =1;
//		JX_delay_auto_publish.delay_time =aos_now_ms();

//		JX_set_auto_publish(5);
	}
}

void JX_auto_publish_status(void)
{
	if(!bt_mesh_is_provisioned())
	{//xxx:�������ɹ�
//        auto_publish_flag = 0;//�������ϱ�
		return;
	}

	if(auto_publish_flag)
	{
		if(aos_now_ms() > auto_publish_time_1ms +auto_publish_rand_time_100ms || auto_publish_now_flag)
		{
			u8 i;
			u8 pos =0;
			u8 pBuffer[10];

			if(JX_vendor_flag.send_flag)
			{//���ڷ�������
				return;
			}

			for(i=0; i<SWITCH_NUM; i++)
			{
				if(switch_para_temp.single_switch[i] != switch_para.single_switch[i])
				{//·¢ËÍgen_onoff publish
					LIGHT_DBG("i:%d switch_para_temp:%d switch_para:%d",
							i, switch_para_temp.single_switch[i], switch_para.single_switch[i]);
					switch_para_temp.single_switch[i] = switch_para.single_switch[i];
					JX_onoff_send(i+1, switch_para.single_switch[i]);
				}

				if(switch_para_temp.dymic_switch[i] != switch_para.dymic_switch[i])
				{//·¢ËÍÁé¶¯¿ª¹Ø vendor ×´Ì¬
					LIGHT_DBG("i:%d dymic_switch_temp:%d dymic_switch:%d",
							i, switch_para_temp.dymic_switch[i], switch_para.dymic_switch[i]);
					switch_para_temp.dymic_switch[i] = switch_para.dymic_switch[i];
					pBuffer[pos++] =(u8)DYMIC_SWITCH_ONOFF;
					pBuffer[pos++] =(u8)(DYMIC_SWITCH_ONOFF>>8);
					pBuffer[pos++] =switch_para.dymic_switch[i];
					JX_vendor_auto_send(i+1, VD_HW_ATTR_NOTIFY, pBuffer, pos, 4);

					return;
				}
			}

			if(switch_para_temp.back_light != switch_para.back_light)
			{//·¢ËÍ±³¹â¿ª¹Ø vendor ×´Ì¬
				LIGHT_DBG("back_light_temp:%d back_light:%d", switch_para_temp.back_light, switch_para.back_light);
				switch_para_temp.back_light = switch_para.back_light;
				pBuffer[pos++] =(u8)BACKLIGHT_ONOFF;
				pBuffer[pos++] =(u8)(BACKLIGHT_ONOFF>>8);
				pBuffer[pos++] =switch_para.back_light;
				JX_vendor_auto_send(0, VD_HW_ATTR_NOTIFY, pBuffer, pos, 4);
			}

			auto_publish_flag =0;
			auto_publish_now_flag =0;

			JX_delay_auto_publish.delay_time =aos_now_ms();
		}
	}
}

//�����ϱ���������
//������priority ���ȼ���0-��ʱ�ϱ�  1-�����ϱ�
void JX_set_auto_publish(unsigned char priority)
{
	if(!bt_mesh_is_provisioned())
	{//xxx:�������ɹ�
//		  auto_publish_flag = 0;//�������ϱ�
		return;
	}

	if(priority <=1)
	{
	    auto_publish_flag = 1;
	    auto_publish_now_flag = priority;//��ʱ�ϱ����������ϱ�
	    auto_publish_time_1ms = aos_now_ms();
	    auto_publish_rand_time_100ms = (3 + rand()%5) *100;//0.3~0.8s
	}
	else if(priority ==2)
	{
	    auto_publish_flag = 1;
	    auto_publish_now_flag = 0;//��ʱ�ϱ����������ϱ�
	    auto_publish_time_1ms = aos_now_ms() +2000;
	    auto_publish_rand_time_100ms = (3 + rand()%5) *100;//0.3~0.8s
	}
	else if(priority ==5)
	{
	    auto_publish_flag = 1;
	    auto_publish_now_flag = 0;//��ʱ�ϱ����������ϱ�
	    auto_publish_time_1ms = aos_now_ms() +3000;
	    auto_publish_rand_time_100ms = (3 + rand()%5) *100;//0.3~0.8s

		switch_para_temp.single_switch[0] =0xFF;
		switch_para_temp.single_switch[1] =0xFF;
		switch_para_temp.single_switch[2] =0xFF;

		switch_para_temp.back_light =0xFF;

		switch_para_temp.dymic_switch[0] =0xFF;
		switch_para_temp.dymic_switch[1] =0xFF;
		switch_para_temp.dymic_switch[2] =0xFF;
	}
}

void JX_switch_turn(u8 num)
{
	if(switch_para.dymic_switch[num])
	{//�鶯���أ�����
		return;
	}

	if(switch_para.single_switch[num] ==SWITCH_ON)
	{

		JX_switch_light_set(num, SWITCH_OFF);
	}
	else
	{
		JX_switch_light_set(num, SWITCH_ON);
	}

	if(bt_mesh_is_provisioned())
	{//xxx:ÅäÍø²»³É¹¦
		JX_set_auto_publish(1);//ÉÏ±¨×´Ì¬¸øÆ½Ì¨
	}
}


void JX_switch_light_set(u8 num, u8 value)
{
	if(switch_para.dymic_switch[num])
	{//�鶯���أ�����
		return;
	}

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
	u32 current_time =aos_now_ms();

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

	switch(num)
	{
	case 0:
	{
		if(switch_para.dymic_switch[num] ==0)
		{//�鶯���ؽ�ֹ
			store_flash_flag =1;

			JX_delay_pin[num].flag =1;
			JX_delay_pin[num].delay_time =aos_now_ms();
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
		if(switch_para.dymic_switch[num] ==0)
		{//�鶯���ؽ�ֹ
			store_flash_flag =1;

			JX_delay_pin[num].flag =1;
			JX_delay_pin[num].delay_time =aos_now_ms();
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
		if(switch_para.dymic_switch[num] ==0)
		{//�鶯���ؽ�ֹ
			store_flash_flag =1;
			JX_delay_pin[num].flag =1;
			JX_delay_pin[num].delay_time =aos_now_ms();

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

	if(store_flash_flag)
	{
		JX_set_auto_publish(0);
		JX_delay_flash_write.flag =1;
		JX_delay_flash_write.delay_time =aos_now_ms();
	}
}

///*
void JX_dymic_switch_set(u8 num, u8 value)
{
	if(value ==SWITCH_ON)
	{
		switch_para.single_switch_last[num] =switch_para.single_switch[num];
//		BT_DBG("on num:%d single_switch_last:%d", num, switch_para.single_switch_last[num]);
		JX_switch_light_set(num, SWITCH_ON);
		switch_para.dymic_switch[num] =1;
	}
	else
	{//�ָ��鶯������ǰ��״̬
//		BT_DBG("off num:%d single_switch_last:%d", num, switch_para.single_switch_last[num]);
		switch_para.dymic_switch[num] =0;//������1�������޷���������
		if(switch_para.single_switch_last[num] ==SWITCH_ON)
		{
			JX_switch_light_set(num, SWITCH_ON);
		}
		else
		{
			JX_switch_light_set(num, SWITCH_OFF);
		}
	}

	JX_set_auto_publish(0);
	JX_delay_flash_write.flag =1;
	JX_delay_flash_write.delay_time =aos_now_ms();
}
//*/

//����led ����Ƶ�ʣ��Լ�����ʱ��
void JX_led_ctrl_set(u8 led_num, u16 freq, u32 keep_time_ms)
{
	led_ctrl_struct *p_led_ctrl =&led_ctrl[led_num];

	p_led_ctrl->ctrl_flag =1;
	p_led_ctrl->flash_freq =freq;
	p_led_ctrl->flash_time_cur =aos_now_ms();
	p_led_ctrl->flash_time_dst =aos_now_ms() + keep_time_ms;

	JX_led_set(led_num, LED_ON);
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
	JX_delay_flash_write.delay_time =aos_now_ms();
}
//*/

//����led ��ӦIO��
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

//����led ��ӦIO��
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

//ȡ��led  ����������״̬
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
	static u16 flash_freq[3] ={0, 0, 0};

//	LIGHT_DBG("ctrl_flag0:%d ctrl_flag1:%d ctrl_flag2:%d",
//		led_ctrl[0].ctrl_flag, led_ctrl[1].ctrl_flag, led_ctrl[2].ctrl_flag);

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(led_ctrl[i].ctrl_flag)
		{
	//		LIGHT_DBG("flash_time_cur:%d aos_now_ms:%d flash_time_dst:%d",
	//			led_ctrl[0].flash_time_cur, aos_now_ms(), led_ctrl[0].flash_time_dst);

			led_ctrl[i].flash_time_cur =aos_now_ms();
			if(led_ctrl[i].flash_time_cur >= led_ctrl[i].flash_time_dst)
			{
				led_ctrl[i].ctrl_flag =0;
				led_ctrl[i].flash_time_cur =0;
				led_ctrl[i].flash_time_dst =0;

				flash_freq[i] =0;

				JX_led_set_by_switch(i);
			}
			else
			{
				if(flash_freq[i] >= led_ctrl[i].flash_freq)
				{
					flash_freq[i] =0;

					if(led_ctrl[i].led_status ==LED_ON)
					{
						JX_led_set(i, LED_OFF);
					}
					else
					{
						JX_led_set(i, LED_ON);
					}
				}
				else
				{
					flash_freq[i] ++;
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

	JX_flash_write_switch_para();
}

void JX_flash_write_loop(void)
{
	if(!JX_delay_flash_write.flag)
	{
		return;
	}

	if(JX_delay_flash_write.delay_time +1000 <=aos_now_ms())
	{//��ʱ1s�������
		JX_delay_flash_write.flag =0;
		JX_delay_flash_write.delay_time =aos_now_ms();

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
//    return ret;
}

gpio_irq_handler_t JX_irp_func(void)
{
    os_printf("~~~~~~~gpio wake up!!!!!\r\n\r\n");
}

void JX_gpio_init1(void)
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
 	hal_gpio_output_high(&gpio_temp);

    gpio_temp.port =  SWITCH2_2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);

    gpio_temp.port =  SWITCH3_2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);


    gpio_temp.port =  LED1;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);

    gpio_temp.port =  LED2;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  LED3;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);


	gpio_temp.port =  KEY1;
    hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_FALLING_EDGE, NULL, NULL);

	gpio_temp.port =  KEY2;
    hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_FALLING_EDGE, NULL, NULL);

	gpio_temp.port =  KEY3;
	hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_FALLING_EDGE, NULL, NULL);

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
 	hal_gpio_output_low(&gpio_temp);

    gpio_temp.port =  LED3;
 	gpio_temp.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_temp);
 	hal_gpio_output_high(&gpio_temp);


	gpio_temp.port =  KEY1;
    hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_FALLING_EDGE, NULL, NULL);

	gpio_temp.port =  KEY2;
    hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_FALLING_EDGE, NULL, NULL);

	gpio_temp.port =  KEY3;
	hal_gpio_enable_irq(&gpio_temp, IRQ_TRIGGER_FALLING_EDGE, NULL, NULL);

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

void JX_key_func(void)
{
	u8 i, j;
	u32 key_value[3];
	static u8 key_value_last[3] ={0, 0, 0};

	static u32 key_low_cnt[3] ={0, 0, 0};

	static u8 device_status_short_cnt[3] ={0, 0, 0};
	static u8 device_status_short_time[3] ={0, 0, 0};

	u8 pin_check_flag =0;

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

//	LIGHT_DBG("key_value:%d key_low_cnt:%d device_status_short_cnt:%d device_status_short_time:%d ",
//		key_value[0], key_low_cnt[0], device_status_short_cnt[0], device_status_short_time[0]);

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(key_value[i] && key_value[i] !=key_value_last[i])
		{
			if(key_low_cnt[i] >=4 && key_low_cnt[i] <40)
			{//40ms~400ms �̰�
				device_status_short_cnt[i] ++;

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
			else if(key_low_cnt[i] >=500 && key_low_cnt[i] <1000 && switch_para.single_switch[i] == SWITCH_ON)
			{//5s~10s
				if(device_status_short_cnt[i] ==1 || device_status_short_cnt[i] ==2)
				{//ËæÒâÌùÈëÍø
					JX_led_ctrl_set(i, 17, 60000);

					JX_ble_pair_set(i, 1);
				}
				else if((device_status_short_cnt[i] ==3 || device_status_short_cnt[i] ==4) && (i+1) ==SWITCH_NUM)
				{//Ó²¼þ¸´Î»app,ËæÒâÌù²»¸´Î»
					for(j=0; j<SWITCH_NUM; j++)
					{
						if(switch_para.single_switch[j] ==SWITCH_OFF)
						{
							break;
						}
					}
					LIGHT_DBG("key hard value %d", j);

					if(j==SWITCH_NUM)
					{
						LIGHT_DBG("key hard reset");

						switch_para.back_light =1;
						for(j=0; j<SWITCH_NUM; j++)
						{
							switch_para.dymic_switch[j] =0;
						}

						icu_set_reset_reason(REG_HARD_RESET);
						JX_led_ctrl_set(i, 50, 3000);

						JX_vendor_hard_reset(5);

						genie_event(GENIE_EVT_REPEAT_RESET_START, NULL);
					}
				}

				device_status_short_cnt[i] =0;
			}
			else if(key_low_cnt[i] >=1000)
			{//10sÒÔÉÏ
				if(device_status_short_cnt[i] ==1 || device_status_short_cnt[i] ==2)
				{//Ó²¼þ¸´Î»ËæÒâÌù
					JX_ble_mac_one_key_init(i);
					JX_led_ctrl_set(i, 50, 5000);
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

			device_status_short_time[0] =0;

			key_low_cnt[i] =0;
		}
		else if(key_value[i] == 1)
		{//�����ް���״̬
			if(device_status_short_cnt[i])
			{//��֮ǰ�̰��������ʱ1s������̰�״̬
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
		{//����һֱ�����£��ۼư���ʱ��
			pin_check_flag =1;

			if(key_low_cnt[i] < 1200)
			{//ֻ�ۼӵ�12s
				key_low_cnt[i] ++;
			}

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
	icu_set_reset_reason(REG_OTA);

	JX_delay_ota.flag =1;
	JX_delay_ota.delay_time =aos_now_ms();

	u8 i;
	for(i=0; i<SWITCH_NUM; i++)
	{
		JX_led_ctrl_set(i, 100, 8000);
	}

//	JX_led_ctrl_set(0, 100, 8000);
//	JX_led_ctrl_set(1, 100, 8000);
//	JX_led_ctrl_set(2, 100, 8000);
}

void JX_ota_loop(void)
{
	if(JX_delay_ota.flag)
	{
		u32 current_time =aos_now_ms();
		if(current_time >= JX_delay_ota.delay_time +8100)
		{
			JX_delay_ota.flag =1;
			JX_delay_ota.delay_time =current_time;
			aos_reboot();
		}
	}
}

void JX_sleep_loop(void)
{
	u8 i;
	static u8 JX_10ms_cnt =0;

//	printf("%s, %d \r\n", __func__, __LINE__);
//   while(get_rw_sleep_flag())
//    {
//        ;
//    }
	if(get_rw_sleep_flag())
	{
//		printf("%s, %d \r\n", __func__, __LINE__);
		return;
	}

	if(get_sleep_flag() ==1)
	{
//		printf("%s, %d \r\n", __func__, __LINE__);
		return;
	}
	else if(JX_sleep_check_flag ==0)
	{
		printf("%s, %d \r\n", __func__, __LINE__);
		JX_sleep_check_flag =1;
		bt_mesh_scan_enable();
		bt_mesh_proxy_adv_enabled();

		if(JX_adv_prov_silence_flag)
		{
			genie_pbadv_timer_stop();
		}
		else
		{
			bt_mesh_beacon_enable();
		}

        JX_10ms_cnt = 0;
	}


//	LIGHT_DBG("%s %d \r\n", __func__, __LINE__);
//	LIGHT_DBG("%s %d pin_flag£º%d sleep_flag:%d\r\n",
//				__func__, __LINE__, JX_pin_check_flag, get_sleep_flag());

//	LIGHT_DBG("%s %d JX_10ms_cnt:%d ", __func__, __LINE__, JX_10ms_cnt);

	if(JX_vendor_flag.send_flag || auto_publish_flag)
	{
//		LIGHT_DBG("%s %d \r\n", __func__, __LINE__);
		return;
	}

	if(JX_prov_flag ==0)
	{
		for(i=0; i<SWITCH_NUM; i++)
		{
			if(switch_para.single_switch[i] ==SWITCH_OFF)
			{
				break;
			}
		}

		if(i == SWITCH_NUM)
		{
			return;
		}
	}

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(JX_delay_pin[i].flag)
		{//Òý½Å²¨ÐÎ
			return;
		}

		if(JX_ble_pair_flag[i].flag)
		{//ËæÒâÌùÅä¶Ô
			return;
		}
	}

	if(JX_pin_check_flag ==1 || i != SWITCH_NUM)
	{
//		printf("%s, %d \r\n", __func__, __LINE__);
		return;
	}

	JX_10ms_cnt++;
//	LIGHT_DBG("%s %d JX_10ms_cnt:%d ", __func__, __LINE__, JX_10ms_cnt);
	if(JX_10ms_cnt >6)
	{
		if(i== SWITCH_NUM && JX_pin_check_flag ==0)
//		if(JX_pin_check_flag ==0)
		{
			LIGHT_DBG("%s %d JX_10ms_cnt:%d ", __func__, __LINE__, JX_10ms_cnt);
//			LIGHT_DBG("%s %d delay_time:%d",
//			__func__, __LINE__, aos_now_ms());
			JX_sleep_check_flag =0;

//			bt_mesh_adv_stop();
			bt_mesh_scan_stop();
			bt_mesh_proxy_adv_disabled();
			bt_mesh_beacon_disable();
			sleep_mode_enable(1);
		}

		JX_10ms_cnt =0;
	}
//	printf("%s %d \r\n", __func__, __LINE__);
}

static void JX_app_timer_cb(void *p_timer)
{
	//k_delayed_work_submit(&JX_app_10ms, 10);

	JX_key_func();

	JX_led_ctrl_func();

	JX_auto_publish_status();

	JX_vendor_loop();

	JX_ble_remote_func();

	JX_auto_publish_loop();

	JX_pin_loop();

	JX_flash_write_loop();

	JX_ota_loop();

	JX_sleep_loop();
//	printf("%s %d \r\n", __func__, __LINE__);
}

void JX_app_start(void)
{
	static u16 reg_temp =0;

	reg_temp =icu_get_reset_reason();

	icu_set_reset_reason(REG_NONE);

	LIGHT_DBG("%s %d JX_version:%x %X",
	__func__, __LINE__, PROJECT_SW_VERSION, reg_temp);
	JX_para_init();
	JX_gpio_init();

//	gpio_wakeup_test_init();
///*
	JX_flash_read_switch_para();
	JX_vendor_init();

	JX_ble_init();

	//k_delayed_work_init(&JX_app_10ms, JX_app_timer_cb);
	krhino_timer_create(&JX_app_10ms, "JX_APP_TIMER", JX_app_timer_cb,
	                    1, 1, NULL, true);
	//k_delayed_work_submit(&JX_app_10ms, 10);
     //   krhino_timer_start(&JX_app_10ms);
	// icu_set_reset_reason(0);

	if(bt_mesh_is_provisioned())
	{
		JX_adv_prov_silence_flag =0;
		JX_set_auto_publish(2);

//		if(icu_get_reset_reason() ==REG_OTA)
		if(reg_temp ==REG_OTA)
		{
			JX_vendor_auto_version(5);
		}
	}
	else
	{
		if(reg_temp !=REG_HARD_RESET)
		{
			JX_adv_prov_silence_flag =1;
			genie_pbadv_timer_stop();
			bt_mesh_proxy_adv_disabled();
			bt_mesh_beacon_disable();
		}
		else
		{
			u8 i;

			for(i=0; i<SWITCH_NUM; i++)
			{
				JX_led_ctrl_set(i, 20, 60000);
			}

//			JX_led_ctrl_set(0, 20, 60000);
//			JX_led_ctrl_set(1, 20, 60000);
//			JX_led_ctrl_set(2, 20, 60000);
		}
	}

	if(reg_temp !=REG_OTA
	&& reg_temp !=REG_HARD_RESET
	&& reg_temp !=REG_SOFT_RESET
	)
	{
		switch_para.back_light =1;
	}
//	*/
}


