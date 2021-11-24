
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

ble_mac_struct JX_ble_mac[SWITCH_NUM];
JX_delay_struct JX_ble_pair_flag[SWITCH_NUM];

ble_adv_struct JX_ble_adv;

JX_delay_struct JX_delay_ble_flash_write;

/*==================================================================================
	 len type data len type	Manufacturer_ID user_ID   MAC               Channel 	KEYCODE 	data1	data2	data3	data4	crc
BYTE: 1   1    1    1   1            2         2       6                   1           1
第一个随意贴：
par:  02  01   1a   11  ff        48 53      43 58  6a 03 dc 98 53 48     01 1c 01 00 00 3c d1
第二个随意贴：
par:  02  01   1a   11  ff        48 53      43 58  05 03 da fb 53 48     01 1d 01 00 00 02 c7
==================================================================================*/

//ble 广播包处理
void JX_ble_adv_event(u8 *pBuffer, u8 length)
{
	u8 i;

	if(length < sizeof(JX_ble_adv))
	{
		LIGHT_DBG("length:%d %d", length, sizeof(JX_ble_adv));
		return;
	}

	memcpy(&JX_ble_adv, pBuffer, sizeof(JX_ble_adv));
	if(JX_ble_tid_check(JX_ble_adv.tid))
	{
		LIGHT_DBG("tid:%d", JX_ble_adv.tid);
		return;
	}

	LIGHT_DBG("man_ID:%x user_ID:%x tid:%d mac:%s",
			JX_ble_adv.man_ID, JX_ble_adv.user_ID, JX_ble_adv.tid, bt_hex(JX_ble_adv.mac, 6));

	if(JX_ble_adv.man_ID == 0x5348 && JX_ble_adv.user_ID == 0x5843)
	{
		for(i=0; i<SWITCH_NUM; i++)
		{
//			LIGHT_DBG("i:%d JX_ble_pair_flag[i].flag:%d", i, JX_ble_pair_flag[i].flag);
			if(JX_ble_pair_flag[i].flag)
			{//需要配对
				if(JX_ble_mac_key_code_check(i, JX_ble_adv.mac, JX_ble_adv.key_code) ==0)
				{
					JX_ble_mac_key_code_add(i, JX_ble_adv.mac, JX_ble_adv.key_code);
				}
			}
			else
			{//无需配对
				if(JX_ble_mac_key_code_check(i, JX_ble_adv.mac, JX_ble_adv.key_code))
				{//若mac 匹配成功，则控制翻转
					JX_switch_turn(i);
				}
			}
		}
	}
}

void JX_ble_init(void)
{
	memset(JX_ble_pair_flag, 0, sizeof(JX_ble_pair_flag));
	memset(&JX_ble_adv, 0, sizeof(JX_ble_adv));

	JX_ble_flash_read();
}

void JX_ble_remote_func(void)
{
	JX_ble_pair();
	JX_ble_flash_write_loop();
}

//校验tid,避免重复
u8 JX_ble_tid_check(u8 tid)
{
	static u8 tid_last =0xD0;

	if(tid_last != tid)
	{
		tid_last =tid;
		return 0;
	}
	else
	{
		return 1;
	}
}

//检测随意贴配对
void JX_ble_pair(void)
{
	u8 i;

	for(i=0; i<SWITCH_NUM; i++)
	{
		if(JX_ble_pair_flag[i].flag)
		{
			u32 current_time =aos_now_ms();
			if(current_time >= JX_ble_pair_flag[i].delay_time + 60*1000)
			{
				LIGHT_DBG("JX_ble_pair_flag =0 %d", i);
				JX_ble_pair_flag[i].flag =0;
				JX_ble_pair_flag[i].delay_time =current_time;
			}
		}
	}
}

//设置进入随意贴配对功能
void JX_ble_pair_set(u8 num, u8 value)
{
	JX_ble_pair_flag[num].flag =value;
	JX_ble_pair_flag[num].delay_time =aos_now_ms();

	LIGHT_DBG("num:%d value:%d", num, value);
}

void JX_ble_mac_one_key_init(u8 num)
{
	memset(&JX_ble_mac[num], 0, sizeof(JX_ble_mac[num]));
	JX_delay_ble_flash_write.flag =1;
	JX_delay_ble_flash_write.delay_time =aos_now_ms();
}

void JX_ble_mac_all_init(void)
{
	memset(JX_ble_mac, 0, sizeof(JX_ble_mac));
	JX_delay_ble_flash_write.flag =1;
	JX_delay_ble_flash_write.delay_time =aos_now_ms();
}

//校验随意贴mac 和keycode
u8 JX_ble_mac_key_code_check(u8 num, u8 *pCmpMac, u8 key_code)
{
	u8 i, j;

	if(num >= SWITCH_NUM)
	{
		return 0xFF;
	}

	for(i=0; i<JX_ble_mac[num].mac_size; i++)
	{
		if(memcmp(JX_ble_mac[num].mac_addr[i], pCmpMac, 6) ==0 &&
			JX_ble_mac[num].key_code[i] ==key_code
		)
		{
			LIGHT_DBG("i:%d ble mac =1", i);
			return 1;
		}
	}

	LIGHT_DBG("ble mac =0");
	return 0;
}

void JX_ble_mac_key_code_add(u8 num, u8 *pCmpMac, u8 key_code)
{
	if(num >= SWITCH_NUM)
	{
		return;
	}

	LIGHT_DBG("num:%d mac_size:%d ", num, JX_ble_mac[num].mac_size);

	JX_ble_pair_set(num, 0);
	JX_led_ctrl_cancel(num);
	JX_led_set_by_switch(num);

	if(JX_ble_mac[num].mac_size >= 5)
	{
		return;
	}

	u8 mac_pos =JX_ble_mac[num].mac_size;

	JX_ble_mac[num].mac_size ++;
	memcpy(JX_ble_mac[num].mac_addr[mac_pos], pCmpMac, 6);
	JX_ble_mac[num].key_code[mac_pos] =key_code;

	JX_delay_ble_flash_write.flag =1;
	JX_delay_ble_flash_write.delay_time =aos_now_ms();
}

void JX_ble_flash_write_loop(void)
{
	if(!JX_delay_ble_flash_write.flag)
	{
		return;
	}

	if(JX_delay_ble_flash_write.delay_time +1000 <=aos_now_ms())
	{//延时1s保存参数
		JX_delay_ble_flash_write.flag =0;
		JX_delay_ble_flash_write.delay_time =aos_now_ms();

		JX_ble_flash_write();
	}
}

void JX_ble_flash_write(void)
{
//    uint8_t data = count;
//
    LIGHT_DBG("");

    genie_flash_write_userdata(GFI_BLE_PARA, JX_ble_mac, sizeof(JX_ble_mac));
}

void JX_ble_flash_read(void)
{
	u8 i;
    E_GENIE_FLASH_ERRCODE ret;

    ret = genie_flash_read_userdata(GFI_BLE_PARA, JX_ble_mac, sizeof(JX_ble_mac));

    if(ret != GENIE_FLASH_SUCCESS)
	{
        LIGHT_DBG("read error %d", ret);
		JX_ble_mac_all_init();
    }
//    return ret;
}



