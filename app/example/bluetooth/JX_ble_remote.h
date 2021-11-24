#ifndef _JX_BLE_REMOTE_H
#define _JX_BLE_REMOTE_H

#include "common/log.h"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <bluetooth.h>
#include <soc.h>
#include <api/mesh.h>
#include "genie_app.h"
#include "gpio_pub.h"

#include "JX_app.h"

typedef struct{
	u8 mac_size;
	u8 mac_addr[5][6];
	u8 key_code[5];
}ble_mac_struct;

typedef struct{
	u8 len;
	u8 type;
	u16 man_ID;
	u16 user_ID;
	u8 mac[6];
	u8 channel;
	u8 key_code;
	u8 data[3];
	u8 tid;
}ble_adv_struct;

extern JX_delay_struct JX_ble_pair_flag[SWITCH_NUM];

void JX_ble_adv_event(u8 *pBuffer, u8 length);

void JX_ble_init(void);
void JX_ble_remote_func(void);

u8 JX_ble_tid_check(u8 tid);
void JX_ble_pair(void);
void JX_ble_pair_set(u8 num, u8 value);

void JX_ble_mac_one_key_init(u8 num);
void JX_ble_mac_all_init(void);
u8 JX_ble_mac_key_code_check(u8 num, u8 *pCmpMac, u8 key_code);
void JX_ble_mac_key_code_add(u8 num, u8 *pCmpMac, u8 key_code);

void JX_ble_flash_write_loop(void);
void JX_ble_flash_write(void);
void JX_ble_flash_read(void);

#endif

