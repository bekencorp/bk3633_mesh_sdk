#ifndef __JX_APP_H
#define __JX_APP_H

#include "common/log.h"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <bluetooth.h>
#include <soc.h>
#include <api/mesh.h>
#include "genie_app.h"
#include "gpio_pub.h"

#include "light_board.h"

enum
{
	REG_NONE =0,
	REG_OTA =0x33,
	REG_HARD_RESET,
	REG_SOFT_RESET,
};

#define DEV_1_KEY		0x11
#define DEV_2_KEY		0x12
#define DEV_3_KEY		0x13

#define DEV_TYPE		DEV_3_KEY

#if DEV_TYPE == DEV_1_KEY
	#define SWITCH_NUM			1

	#define SWITCH1_1		GPIO_P10
	#define SWITCH2_1		GPIO_P10
	#define SWITCH3_1		GPIO_P10

	#define SWITCH1_2		GPIO_P13
	#define SWITCH2_2		GPIO_P13
	#define SWITCH3_2		GPIO_P13


	#define LED1			GPIO_P07
	#define LED2			GPIO_P07
	#define LED3			GPIO_P07

	#define KEY1			GPIO_P03
	#define KEY2			GPIO_P03
	#define KEY3			GPIO_P03
#elif DEV_TYPE == DEV_2_KEY
	#define SWITCH_NUM			2

	#define SWITCH1_1		GPIO_P11
	#define SWITCH2_1		GPIO_P06
	#define SWITCH3_1		GPIO_P06

	#define SWITCH1_2		GPIO_P12
	#define SWITCH2_2		GPIO_P33
	#define SWITCH3_2		GPIO_P33


	#define LED1			GPIO_P31
	#define LED2			GPIO_P05
	#define LED3			GPIO_P05

	#define KEY1			GPIO_P04
	#define KEY2			GPIO_P02
	#define KEY3			GPIO_P02
#elif DEV_TYPE == DEV_3_KEY
	#define SWITCH_NUM			3

	#define SWITCH1_1		GPIO_P11
	#define SWITCH2_1		GPIO_P10
	#define SWITCH3_1		GPIO_P06

	#define SWITCH1_2		GPIO_P12
	#define SWITCH2_2		GPIO_P13
	#define SWITCH3_2		GPIO_P33


	#define LED1			GPIO_P31
	#define LED2			GPIO_P07
	#define LED3			GPIO_P05

	#define KEY1			GPIO_P04
	#define KEY2			GPIO_P03
	#define KEY3			GPIO_P02
#endif



enum
{
	SWITCH_OFF =0,
	SWITCH_ON,
};

enum
{
	LED_OFF =0,
	LED_ON,
};

typedef struct
{
	u8 flag;
	u32 delay_time;
}JX_delay_struct;

typedef struct
{
	u8 single_switch[3];				//单火开关
	u8 back_light;						//背光开关
	u8 dymic_switch[3];					//灵动开关
	u8 single_switch_last[3];			//单火开关最后状态
}switch_para_struct;

typedef struct
{
	u8 ctrl_flag;
	u8 led_status;
	u16 flash_freq;
	u64 flash_time_cur;
	u64 flash_time_dst;

}led_ctrl_struct;

extern u8 JX_prov_flag;
extern u8 JX_adv_prov_silence_flag;


extern JX_delay_struct JX_delay_ota;

extern u16 JX_dst_dev_addr;
extern switch_para_struct switch_para;
extern led_ctrl_struct led_ctrl[3];


void JX_auto_publish_status(void);
void JX_set_auto_publish(unsigned char priority);

void JX_flash_switch_para_init(void);
void JX_flash_write_loop(void);
void JX_flash_write_switch_para(void);
void JX_flash_read_switch_para(void);

void JX_gpio_init(void);
void JX_app_start(void);
void JX_gpio_init1(void);

void JX_led_ctrl_cancel(u8 led_num);
void JX_led_set_by_switch(u8 num);

void JX_switch_turn(u8 num);
void JX_switch_light_set(u8 num, u8 value);
void JX_pin_loop(void);

void JX_switch_set(u8 num, u8 value);
void JX_dymic_switch_set(u8 num, u8 value);
void JX_backlight_set(u8 value);


#endif

