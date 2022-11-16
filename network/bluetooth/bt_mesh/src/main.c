/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdbool.h>
#include <errno.h>

#include <net/buf.h>
#include <bluetooth.h>
#include <conn.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG)
#include "common/log.h"

#include "test.h"
#include "adv.h"
#include "prov.h"
#include "net.h"
#include "beacon.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "mesh.h"

#include "bt_mesh_custom_log.h"

static bool provisioned;

static bool user_provisioned;

static volatile bool provisioner_en;

void bt_mesh_setup(u32_t seq, u16_t addr)
{
    bt_mesh.seq = seq;

    bt_mesh_comp_provision(addr);
    provisioned = true;

#ifdef CONFIG_NETWORK_CHANGE
	user_provisioned = true;
#endif
    if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    } else {
        bt_mesh_beacon_disable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
        bt_mesh_gatt_proxy_get() != BT_MESH_GATT_PROXY_NOT_SUPPORTED) {
        
		printf("[%s, %d]bt_mesh_setup!\n", __func__, __LINE__);
        bt_mesh_proxy_gatt_enable();
        bt_mesh_adv_update();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_init();
    } else {
    
		printf("[%s, %d]bt_mesh_scan_enable!\n", __func__, __LINE__);
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_init();
    }
}

#ifdef CONFIG_NETWORK_CHANGE
void bt_mesh_def_net_setup(u32_t seq, u16_t addr)
{
    bt_mesh.seq = seq;

    bt_mesh_comp_provision(addr);
    provisioned = true;

}

int bt_mesh_user_provision(const u8_t net_key[16], u16_t net_idx,
              u8_t flags, u32_t iv_index, u32_t seq,
              u16_t addr, const u8_t dev_key[16])
{
    int err;

    // printf("ua(%04x)\n", addr);
    // printf("dk %s\n", bt_hex(dev_key, 16));
    // printf("nk %s\n", bt_hex(net_key, 16));
    // printf("net_idx 0x%04x flags 0x%02x iv_index 0x%04x\n",
    //        net_idx, flags, iv_index);

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        bt_mesh_proxy_prov_disable();
    }
	err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
	{
		printf("[%s, %d] bt_mesh_is_user_provisioned = 1\n", __func__, __LINE__);
	    if (err) {
			
			printf("[%s, %d] error = %d\n", __func__, __LINE__, err);
	        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
				
				printf("[%s, %d] CONFIG_BT_MESH_PB_GATT\n", __func__, __LINE__);
#if (!defined CONFIG_BT_MESH_TELINK) && (!defined CONFIG_BT_MESH_JINGXUN)
				printf("[%s, %d] call bt_mesh_proxy_prov_enable", __func__, __LINE__);

	            bt_mesh_proxy_prov_enable();  
#endif /* !CONFIG_BT_MESH_TELINK && !CONFIG_BT_MESH_JINGXUN */
	        }

	        return err;
	    }
	}

	memcpy(bt_mesh.dev_key, dev_key, 16);

	if(!bt_mesh_is_user_provisioned()) //default network
	{
    	bt_mesh_def_net_setup(seq, addr);
	}
	else//has config network by rc
	{
	
		bt_mesh_setup(seq, addr);
	}

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_complete(net_idx, addr);
    }

    return 0;
}

int bt_mesh_rc_net_comp(u16_t addr, u32_t seq)
{
	int err;

	if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) 
	{
		bt_mesh_proxy_prov_disable();
	}


	bt_mesh_setup(seq, addr);

	
	if (IS_ENABLED(CONFIG_BT_MESH_PROV)) 
	{
		bt_mesh_prov_complete(0, addr);
	}

	return 0;
}

int bt_mesh_default_net_comp(u16_t addr, u32_t seq)
{
	int err;

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY))
	{
        bt_mesh_proxy_gatt_disable();
    }
    bt_mesh_beacon_disable();

	bt_mesh_set_user_provisioned(false);
	bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
	
	if (IS_ENABLED(CONFIG_BT_MESH_PROV)) 
	{
		bt_mesh_prov_complete(0, addr);
	}
	return 0;
}
#endif

int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
              u8_t flags, u32_t iv_index, u32_t seq,
              u16_t addr, const u8_t dev_key[16])
{
    int err;

    // printf("ua(%04x)\n", addr);
    // printf("dk %s\n", bt_hex(dev_key, 16));
    // printf("nk %s\n", bt_hex(net_key, 16));
    // printf("net_idx 0x%04x flags 0x%02x iv_index 0x%04x\n",
    //        net_idx, flags, iv_index);

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        bt_mesh_proxy_prov_disable();
    }

    err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
    if (err) {
		
		printf("[%s, %d] create net error !\n", __func__, __LINE__);
        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
#if (!defined CONFIG_BT_MESH_TELINK) && (!defined CONFIG_BT_MESH_JINGXUN)            
            bt_mesh_proxy_prov_enable();
#endif /* !CONFIG_BT_MESH_TELINK && !CONFIG_BT_MESH_JINGXUN */
        }

        return err;
    }


    memcpy(bt_mesh.dev_key, dev_key, 16);

    bt_mesh_setup(seq, addr);

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_complete(net_idx, addr);
    }

    return 0;
}
			  
void bt_mesh_reset_config(void)
{
    if (!provisioned) {
        return;
    }

    bt_mesh_comp_unprovision();

    memset(g_sub_list, 0, sizeof(g_sub_list));

    bt_mesh.iv_index = 0;
    bt_mesh.seq = 0;
    bt_mesh.iv_update = 0;
    bt_mesh.pending_update = 0;
    bt_mesh.valid = 0;
    bt_mesh.last_update = 0;
    bt_mesh.ivu_initiator = 0;

    k_delayed_work_cancel(&bt_mesh.ivu_complete);

    bt_mesh_cfg_reset();

    bt_mesh_rx_reset();
    bt_mesh_tx_reset();

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_disable(true);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(BT_MESH_KEY_ANY);
    }

    memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));

    provisioned = false;

}

void bt_mesh_reset(void)
{
    if (!provisioned) {
        return;
    }

    bt_mesh_comp_unprovision();

    memset(g_sub_list, 0, sizeof(g_sub_list));

    bt_mesh.iv_index = 0;
    bt_mesh.seq = 0;
    bt_mesh.iv_update = 0;
    bt_mesh.pending_update = 0;
    bt_mesh.valid = 0;
    bt_mesh.last_update = 0;
    bt_mesh.ivu_initiator = 0;

    k_delayed_work_cancel(&bt_mesh.ivu_complete);

    bt_mesh_cfg_reset();

    bt_mesh_rx_reset();
    bt_mesh_tx_reset();

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_disable(true);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(BT_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        bt_mesh_proxy_gatt_disable();
    }

    memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));

    provisioned = false;

    bt_mesh_scan_disable();
    bt_mesh_beacon_disable();

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_reset();
    }
}

bool bt_mesh_is_provisioned(void)
{
    return provisioned;
}

#ifdef CONFIG_NETWORK_CHANGE
void bt_mesh_set_user_provisioned(bool prov_flg)
{
    user_provisioned = prov_flg;
}

bool bt_mesh_is_user_provisioned(void)
{
    return user_provisioned;
}
#endif 

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
#ifdef CONFIG_NETWORK_CHANGE
    if (bt_mesh_is_provisioned() && bt_mesh_is_user_provisioned()) 
#else
	if (bt_mesh_is_provisioned() )
#endif
	{
		printf("%s, %d\r\n",__func__,__LINE__);
        return -EALREADY;
    }

    BT_DBG("~~~~~~~~~~~%s, %d\r\n\r\n", __func__, __LINE__);

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
        (bearers & BT_MESH_PROV_ADV)) {
        /* Make sure we're scanning for provisioning inviations */
        bt_mesh_scan_enable();
        /* Enable unprovisioned beacon sending */
        bt_mesh_beacon_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
        (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_enable();
        bt_mesh_adv_update();
    }

    return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
#ifdef CONFIG_NETWORK_CHANGE
    if (bt_mesh_is_provisioned() &&bt_mesh_is_user_provisioned())
#else
	if(bt_mesh_is_provisioned())
#endif
	{
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
        (bearers & BT_MESH_PROV_ADV)) {
        bt_mesh_beacon_disable();
        bt_mesh_scan_disable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
        (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_disable();
        bt_mesh_adv_update();
    }

    return 0;
}

#ifdef CONFIG_BT_MESH_PROVISIONER

bool bt_mesh_is_provisioner_en(void)
{
    return provisioner_en;
}

int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers)
{
    int err;

    if (bt_mesh_is_provisioner_en()) {
        BT_ERR("Provisioner already enabled");
        return -EALREADY;
    }

    err = provisioner_upper_init();
    if (err) {
        BT_ERR("%s: provisioner_upper_init fail", __func__);
        return err;
    }

    if ((IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
            (bearers & BT_MESH_PROV_ADV)) ||
            (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT))) {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT)) {
        provisioner_pb_gatt_enable();
    }

    provisioner_en = true;

    return 0;
}

int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers)
{
    if (!bt_mesh_is_provisioner_en()) {
        BT_ERR("Provisioner already disabled");
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT)) {
        provisioner_pb_gatt_disable();
    }

    if ((IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
            (bearers & BT_MESH_PROV_ADV)) &&
            (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT))) {
        bt_mesh_scan_disable();
    }

    provisioner_en = false;

    return 0;
}

#endif /* CONFIG_BT_MESH_PROVISIONER */

#ifdef CONFIG_MESH_STACK_ALONE
static void bt_mesh_prepare()
{
        k_work_q_start();
}
#endif

int bt_mesh_init(const struct bt_mesh_prov *prov,
                    const struct bt_mesh_comp *comp,
                    const struct bt_mesh_provisioner *provisioner)
{
    int err;

#ifdef CONFIG_MESH_STACK_ALONE
        bt_mesh_prepare();
#endif

    err = bt_mesh_test();
    if (err) {
        return err;
    }

    err = bt_mesh_comp_register(comp);
    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        err = bt_mesh_prov_init(prov);
        if (err) {
            return err;
        }
    }

    #ifdef CONFIG_BT_MESH_PROVISIONER
        err = provisioner_prov_init(provisioner);
        if (err) {
            return err;
        }
    #endif

    bt_mesh_net_init();
    bt_mesh_trans_init();
    bt_mesh_beacon_init();
    bt_mesh_adv_init();

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
#if CONFIG_BT_MESH_PROVISIONER
        provisioner_proxy_init();
#else
        bt_mesh_proxy_init();
#endif
    }

    return 0;
}

