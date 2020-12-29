/*  Bluetooth Mesh provisioner */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <hal/hal.h>
#include <bluetooth.h>
//#include <api/mesh.h>
#include "application.h"
#include "net.h"
#include "transport.h"
#include "main.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"


uint8_t g_mac[PROVISIONER_SIZE_MAC];

static struct k_thread provisioner_config_thread_data;
static BT_STACK_NOINIT(provisioner_config_thread_stack, 384);        //512
static void provisioner_config_thread(void *p1, void *p2, void *p3);

extern struct bt_mesh_elem elements[];
extern uint8_t provisioner_get_vendor_element_num(void);

static const u8_t default_app_key[16] = {
	0x88, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

char temp_uuid[16] = {0x66, 0x01, 0x71, 0x33, 0x02, 0x00, 0x00, 
                      0x01, 0x02, 0x03, 0x04, 0x05, 0x3B, 
                      0x00, 0xAA, 0xAA};

static const char *bearer2str(bt_mesh_prov_bearer_t bearer)
{
	switch (bearer) {
	case BT_MESH_PROV_ADV:
		return "PB-ADV";
	case BT_MESH_PROV_GATT:
		return "PB-GATT";
	case BT_MESH_PROV_ADV | BT_MESH_PROV_GATT:
		return "PB-ADV and PB-GATT";
	default:
		return "unknown";
	}
}

static void provisioner_link_open(bt_mesh_prov_bearer_t bearer)
{
	BT_INFO("Provisioner link opened on %s\n", bearer2str(bearer));
}

static void provisioner_link_close(bt_mesh_prov_bearer_t bearer, u8_t reason)
{
	BT_INFO("Provisioner link closed on %s\n", bearer2str(bearer));
    if (LINK_CLOSE_SUCCESS == reason) {
        BT_INFO("The provisioner config thread start to run.\n");
        k_sem_give(&g_sem);     //give sem to provisioner_config_thread.
    }
}

static void provisioner_complete(int node_idx, const u8_t device_uuid[16],
                                 u16_t unicast_addr, u8_t element_num,
                                 u16_t netkey_idx, bool gatt_flag)
{
	BT_INFO("provisioner_complete\r\n");
	BT_INFO("node_idx: %d\r\n", node_idx);
	BT_INFO("device_uuid: %02x, %02x, %02x, %02x, %02x, %02x\r\n",
				device_uuid[0], device_uuid[1], device_uuid[2], 
				device_uuid[3], device_uuid[4], device_uuid[5]);
	BT_INFO("unicast_addr: %u\r\n", unicast_addr);
	BT_INFO("element_num: %u\r\n", element_num);
	BT_INFO("netkey_idx: %u\r\n", netkey_idx);
	BT_INFO("gatt_flag: %u\r\n", gatt_flag);
    
    prov_msg.msg_type = PROVISIONER_COMP_DATA_GET;
    prov_msg.comp_get_t.netkey_idx = netkey_idx;
    prov_msg.comp_get_t.unicast_addr = unicast_addr;
    // Give the provisioner config thread to run.
    //k_sem_give(&g_sem);   //old state.
}

extern struct k_sem prov_input_sem;
extern u8_t   prov_input[8];
extern u8_t   prov_input_size;
static int provisioner_input_num(bt_mesh_output_action_t act, u8_t size)
{
    bool    input_num_flag;

	if (BT_MESH_DISPLAY_NUMBER == act) {
		input_num_flag = true;
	} else if (BT_MESH_DISPLAY_STRING == act) {
		input_num_flag = false;
	}

	k_sem_take(&prov_input_sem, K_FOREVER);

	BT_INFO("xxxxxxxxxxxx: %s", prov_input);
	BT_INFO("get input ");
	BT_INFO("xxxxxxxxxxxx: %u", prov_input_size);

	bt_mesh_prov_input_data(prov_input, prov_input_size, input_num_flag);

	return 0;
}

static int provisioner_output_num(bt_mesh_input_action_t act, u8_t size)
{
	u32_t div[8]	= { 10, 100, 1000, 10000, 100000,
						1000000, 10000000, 100000000 };
	u32_t num		= 0;
	u8_t  temp[8];
	u8_t  i;
    bool  output_num_flag;

	if (BT_MESH_ENTER_NUMBER == act) {

		output_num_flag = true;

		bt_mesh_rand(&num, size);

		num %= div[size - 1];

		BT_INFO("===================");
		BT_INFO("input number %06u in the device", num);
		BT_INFO("===================");
	
		memset(temp, 0, sizeof(temp));
	
		for (i = size; i > 0; i--) {
			temp[i - 1] = num & 0xFF;
			num >>= 8;
	
			if (num == 0) {
				break;
			}
		}
	} else if (BT_MESH_ENTER_STRING == act) {
		output_num_flag = false;
	
		bt_mesh_rand(temp, size);
	
		/* Normalize to '0' .. '9' & 'A' .. 'Z' */
		for (i = 0; i < size; i++) {
			temp[i] %= 36;
			if (temp[i] < 10) {
				temp[i] += '0';
			} else {
				temp[i] += 'A' - 10;
			}
		}
		temp[size] = '\0';
	
		BT_INFO("===================");
		BT_INFO("input string %s in the device", temp);
		BT_INFO("===================");
	}
	
	bt_mesh_prov_output_data(temp, size, output_num_flag);
	
	return 0;

}

static struct bt_mesh_provisioner provisioner = {
	.prov_uuid              = 0,
	.prov_unicast_addr      = 1,  ///CHANG 200814
	.prov_start_address     = 2,
	.prov_attention         = 0,
	.prov_algorithm         = 0,
	.prov_pub_key_oob       = 0,
	.prov_pub_key_oob_cb    = 0,
	.prov_static_oob_val    = 0,
	.prov_static_oob_len    = 0,
	.prov_input_num         = provisioner_input_num,
	.prov_output_num        = provisioner_output_num,
	.flags                  = 0,
	.iv_index               = 0,
	.prov_link_open         = provisioner_link_open,
	.prov_link_close        = provisioner_link_close,
	.prov_complete          = provisioner_complete,
};


static void _provisioner_ready(int err)
{
    if (err) {
        BT_INFO("BT init err %d", err);
        return;
    }
    
	err = bt_mesh_init(NULL, &comp, &provisioner);
	BT_INFO(">>>Mesh provisioner initialized<<<");
    if (err) {
        BT_INFO("mesh init err %d", err);
        return;
    }

    bt_mesh_provisioner_set_dev_uuid_match(0x00,16,temp_uuid,1);
    bt_mesh_provisioner_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
	BT_INFO(">>> provisioner enable <<<");
}

static void provisioner_mac_init(void)
{
    uint32_t off_set = 0x000;
    uint8_t addr[6] = {0};
    uint8_t dummy_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if((hal_flash_read(HAL_PARTITION_MAC, &off_set, addr, sizeof(addr)) == 0) &&
       memcmp(addr, dummy_addr, sizeof(addr)) != 0) {
        memcpy(g_mac, addr, 6);
    }

    if ((memcmp(addr, dummy_addr, 6) == 0)) {
        memcpy(addr, PROVISIONER_DEFAULT_MAC, 6);
        memcpy(g_mac, PROVISIONER_DEFAULT_MAC, 6);
        printf("-----------------\n");
    }

    printf("%s, addr:%02x : %02x: %02x : %02x : %02x: %02x\n",
           __func__, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

void provisioner_init(void)
{
    int ret;

    BT_INFO(">>>init provisioner<<<");
    provisioner_mac_init();

    comp.cid = CONFIG_CID;
    comp.pid = 0;
    comp.vid = 1; // firmware version fir ota
    comp.elem = elements;
    comp.elem_count = provisioner_get_vendor_element_num();
    hci_driver_init();

    ret = bt_enable(_provisioner_ready);
    if (ret) {
        BT_INFO("init err %d", ret);
    }
    
	//create provisioner config thread.
    k_thread_create(&provisioner_config_thread_data, provisioner_config_thread_stack,
                    K_THREAD_STACK_SIZEOF(provisioner_config_thread_stack), provisioner_config_thread,
                    NULL, NULL, NULL, CONFIG_BT_MESH_PROVISIONER_CONFIG_PRIO, 0, K_NO_WAIT);

}

static void provisioner_config_thread(void *p1, void *p2, void *p3)
{
   u16_t app_index = 0x01;
   u16_t net_index = 0x00;
   u8_t status = 0;
   
   k_sem_init(&g_sem, 0, 1);

    while(1) {
        k_sem_take(&g_sem, -1);
        switch (prov_msg.msg_type) {
            case PROVISIONER_COMP_DATA_GET:
                BT_INFO("provisioning config start!\r\n");
				BT_INFO("Set app key index end!\r\n");
                bt_mesh_temp_prov_app_idx_set(default_app_key, net_index, &app_index, &status);

				BT_INFO("\r\nBind local model app key index!\r\n");                
                bt_mesh_provisioner_bind_local_model_app_idx(provisioner.prov_unicast_addr, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, 
																0xffff, app_index);
                
				BT_INFO("\r\nGet node's composition data!\r\n");
                user_comp_data_get(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
				BT_INFO("\r\nLet node to add app key!\r\n");
                user_app_key_add(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
				BT_INFO("\r\nLet node to bind model app key!\r\n");
                user_mod_app_bind(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
                BT_INFO("\r\nLet node to subscrip group addr!\r\n");
                user_mod_sub_add(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
				BT_INFO("\r\nSet node's onoff model!\r\n");
                gen_onoff_cli_set(net_index, prov_msg.comp_get_t.unicast_addr);
				
                extern char temp_uuid[16];
                temp_uuid[0x0c]++;
                bt_mesh_provisioner_set_dev_uuid_match(0x00,16,temp_uuid,1); ///for provisioning test.
                BT_INFO("provisioning config end, temp_uuid[0x0c]: 0x%2x !! \r\n", temp_uuid[0x0c]);
                break;
                
            default:
                break;
        }
    }
}
