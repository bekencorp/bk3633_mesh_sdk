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
#include <api/mesh.h>
#include "application.h"
#include "net.h"
#include "transport.h"
#include "main.h"
#include "foundation.h"
#include "static_partition.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_PROVISIONER)
#include "common/log.h"

#ifdef CONFIG_BT_MESH_CFG_CLI
static struct bt_mesh_cfg_cli cfg_cli = {

};
struct bt_mesh_cfg_cli onoff_cli = {

};
#endif

static struct bt_mesh_model root_models[] = {

    //BT_MESH_MODEL_CFG_SRV(),
    //BT_MESH_MODEL_HEALTH_SRV(),
#ifdef CONFIG_BT_MESH_CFG_CLI
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, NULL, NULL, &onoff_cli),
#endif
};

static struct bt_mesh_model vnd_models[] = {
//     BT_MESH_MODEL_VND(_company, _id, _op, _pub, _user_data), //example.
// #ifdef CONFIG_MESH_MODEL_VENDOR_SRV
//     MESH_MODEL_VENDOR_SRV(&g_elem_state[0]),
// #endif
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models, 0),
};

static struct k_thread provisioner_config_thread_data;
static BT_STACK_NOINIT(provisioner_config_thread_stack, PROVISIONER_CONFIG_THREAD_STACK_SIZE);
static void provisioner_config_thread(void *p1, void *p2, void *p3);

static const u8_t default_app_key[16] = {
	0x88, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

uint8_t g_mac[PROVISIONER_SIZE_MAC];
// char temp_uuid[16] = {0x66, 0x01, 0x71, 0x33, 0x02, 0x00, 0x00, 
//                       0x01, 0x02, 0x03, 0x04, 0x05, 0x3B, 
//                       0x00, 0xAA, 0xAA};
char temp_uuid[16] = {0x22, 0x11, 0x78, 0x56, 0x34, 0x12,
                      0x02, 0x15, 0x64, 0x66, 0x76, 0x6F,
                      0x61, 0x72, 0x63, 0x61};

uint8_t provisioner_get_vendor_element_num(void)
{
    return ARRAY_SIZE(elements);
}

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
	BT_DBG("Provisioner link opened on %s\n", bearer2str(bearer));
}

static void provisioner_link_close(bt_mesh_prov_bearer_t bearer, u8_t reason)
{
	BT_DBG("Provisioner link closed on %s.", bearer2str(bearer));
    if (LINK_CLOSE_SUCCESS == reason) {
        BT_DBG("The provisioner config thread start to run.");
        k_sem_give(&provisioner_config_sem);     //give sem to provisioner_config_thread.
    }
}

static void provisioner_complete(int node_idx, const u8_t device_uuid[16],
                                 u16_t unicast_addr, u8_t element_num,
                                 u16_t netkey_idx, bool gatt_flag)
{
	BT_DBG("provisioner_complete!!");
	BT_DBG("node_idx: %d", node_idx);
	BT_DBG("device_uuid: %02x, %02x, %02x, %02x, %02x, %02x",
				device_uuid[0], device_uuid[1], device_uuid[2], 
				device_uuid[3], device_uuid[4], device_uuid[5]);
	BT_DBG("unicast_addr: %u", unicast_addr);
	BT_DBG("element_num: %u", element_num);
	BT_DBG("netkey_idx: %u", netkey_idx);
	BT_DBG("gatt_flag: %u", gatt_flag);
    
    prov_msg.msg_type = PROVISIONER_COMP_DATA_GET;
    prov_msg.comp_get_t.netkey_idx = netkey_idx;
    prov_msg.comp_get_t.unicast_addr = unicast_addr;
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

	BT_DBG("xxxxxxxxxxxx: %s", prov_input);
	BT_DBG("get input ");
	BT_DBG("xxxxxxxxxxxx: %u", prov_input_size);

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

		BT_DBG("===================");
		BT_DBG("input number %06u in the device", num);
		BT_DBG("===================");
	
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
	
		BT_DBG("===================");
		BT_DBG("input string %s in the device", temp);
		BT_DBG("===================");
	}
	
	bt_mesh_prov_output_data(temp, size, output_num_flag);
	
	return 0;

}

static struct bt_mesh_provisioner provisioner = {
	.prov_uuid              = 0,
	.prov_unicast_addr      = 1,
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

void provisioner_config_comp_data_get(u16_t net_idx, u16_t dst)
{
	u8_t status, page = 0x00;
	int err;
	struct net_buf_simple *comp = NET_BUF_SIMPLE(47); //NET_BUF_SIMPLE(_size),change the size

	net_buf_simple_init(comp, 0);

	while(1) {
		err = bt_mesh_cfg_comp_data_get(net_idx, dst, page, &status, comp);
		if(!err) {
			BT_DBG("Getting composition data successful");
			break;
		}
	}

	if (err) {
		BT_ERR("Getting composition failed (err %d)", err);
		return 0;
	}

	if (status != 0x00) {
		BT_ERR("Got non-success status 0x%02x", status);
		return 0;
	}

}

void provisioner_config_app_key_add(u16_t net_idx, u16_t dst)
{
    u8_t key_val[16];
	u16_t key_net_idx = 0;
	u16_t key_app_idx = 1;
	u8_t status;
	int err;

	memcpy(key_val, default_app_key, sizeof(key_val));

	while(1) {
		err = bt_mesh_cfg_app_key_add(net_idx, dst, key_net_idx, key_app_idx, key_val, &status);
		if(!err) {
			break;
		}
	}

	if (err) {
		BT_ERR("Unable to send App Key Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		BT_ERR("AppKeyAdd failed with status 0x%02x\n", status);
	} else {
		BT_DBG("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n",
		       key_net_idx, key_app_idx);
	}
}

int provisioner_config_mod_app_bind(u16_t net_idx, u16_t dst, u16_t mod_app_idx)
{
    u16_t elem_addr, mod_id, cid;
	u8_t status;
	int err;

	while(1) {
		err = bt_mesh_cfg_mod_app_bind(net_idx, dst, /*elem_addr*/0x02, mod_app_idx, 0x1000, &status);
		if(!err) {
			break;
		}
	}

	if (err) {
		BT_ERR("Unable to send Model App Bind (err %d)\n", err);
		return 0;
	}

	if (status) {
		BT_ERR("Model App Bind failed with status 0x%02x\n", status);
	} else {
		BT_DBG("AppKey successfully bound\n");
	}
       
}

int provisioner_config_mod_sub_add(u16_t net_idx, u16_t dst)
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t status;
	int err;

	while(1) {
		err = bt_mesh_cfg_mod_sub_add(net_idx, dst, /*elem_addr*/0x02, /*sub_addr*/0xc000, /*mod_id*/0x1000, &status);
		if(!err) {
			break;
		}
	}

	if (err) {
		BT_ERR("Unable to send Model Subscription Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		BT_ERR("Model Subscription Add failed with status 0x%02x\n",
		       status);
	} else {
		BT_DBG("Model subscription was successful\n");
	}
	return 0;
}

static void _provisioner_ready(int err)
{
    if (err) {
        BT_ERR("BT init err %d", err);
        return;
    }
    
	err = bt_mesh_init(NULL, &comp, &provisioner);
    if (err) {
        BT_ERR("mesh provisioner init err %d", err);
        return;
    }

    bt_mesh_provisioner_set_dev_uuid_match(0x00,16,temp_uuid,1);
    bt_mesh_provisioner_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
	BT_DBG(">>> provisioner enable <<<");   
	//create provisioner config thread.
    k_thread_create(&provisioner_config_thread_data, "Mesh provisioner", provisioner_config_thread_stack,
                    K_THREAD_STACK_SIZEOF(provisioner_config_thread_stack), provisioner_config_thread,
                    NULL, NULL, NULL, CONFIG_BT_MESH_PROVISIONER_CONFIG_PRIO, 0, K_NO_WAIT);
}

static void provisioner_mac_init(void)
{
    uint32_t off_set = 0x000;
    uint8_t addr[6] = {0};
    uint8_t dummy_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if((hal_flash_read(STATIC_SECTION_MAC, &off_set, addr, sizeof(addr)) == 0) &&
       memcmp(addr, dummy_addr, sizeof(addr)) != 0) {
        memcpy(g_mac, addr, 6);
    }

    if ((memcmp(addr, dummy_addr, 6) == 0)) {
        memcpy(addr, PROVISIONER_DEFAULT_MAC, 6);
        memcpy(g_mac, PROVISIONER_DEFAULT_MAC, 6);
        BT_DBG("-----------------\n");
    }

    BT_DBG("%s, addr:%02x : %02x: %02x : %02x : %02x: %02x\n",
           __func__, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

void provisioner_init(void)
{
    int ret;

    BT_DBG(">>>init provisioner<<<");
    provisioner_mac_init();

    comp.cid = CONFIG_CID;
    comp.pid = 0;
    comp.vid = 1;
    comp.elem = elements;
    comp.elem_count = provisioner_get_vendor_element_num();

    hci_driver_init();

    ret = bt_enable(_provisioner_ready);
    if (ret) {
        BT_ERR("init err %d", ret);
    }
}

static void provisioner_config_thread(void *p1, void *p2, void *p3)
{
    u16_t app_index = 0x0001;
    u16_t net_index = 0x0000;
    u8_t status = 0;
	//u16_t temp_onoff_val = 0x0001;
   
    k_sem_init(&provisioner_config_sem, 0, 1);

	BT_DBG("Set app key index end!");
	bt_mesh_temp_prov_app_idx_set(default_app_key, net_index, &app_index, &status);
	BT_DBG("Bind local model app key index!");           
	bt_mesh_provisioner_bind_local_model_app_idx(provisioner.prov_unicast_addr, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, 
													0xffff, app_index);
    while(1) {
        k_sem_take(&provisioner_config_sem, -1);
        switch (prov_msg.msg_type) {
            case PROVISIONER_COMP_DATA_GET:

                BT_DBG("provisioning config start!");
				BT_DBG("Get composition data");
                provisioner_config_comp_data_get(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
				BT_DBG("Add app key");
                provisioner_config_app_key_add(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
				BT_DBG("Bind model app key");
                provisioner_config_mod_app_bind(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr, 
													app_index);
                
                BT_DBG("Subscrip group addr");
                provisioner_config_mod_sub_add(prov_msg.comp_get_t.netkey_idx, prov_msg.comp_get_t.unicast_addr);
                
				// BT_DBG("Set node's onoff model!");
                // bt_mesh_cfg_cli_gen_onoff_set(net_index, provisioner.prov_unicast_addr, prov_msg.comp_get_t.unicast_addr, temp_onoff_val);
				
                extern char temp_uuid[16];
                temp_uuid[0x0c]++;
                bt_mesh_provisioner_set_dev_uuid_match(0x00,16,temp_uuid,1); ///for provisioning test.
                BT_DBG("provisioning config end, temp_uuid[0x0c]: 0x%2x !!", temp_uuid[0x0c]);
                break;
                
            default:
                break;
        }
    }
}
