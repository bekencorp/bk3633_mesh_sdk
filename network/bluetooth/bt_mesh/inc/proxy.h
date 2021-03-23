/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MESH_PROXY_H
#define __MESH_PROXY_H

#include <mesh_def.h>

#define BT_MESH_PROXY_NET_PDU   0x00
#define BT_MESH_PROXY_BEACON    0x01
#define BT_MESH_PROXY_CONFIG    0x02
#define BT_MESH_PROXY_PROV      0x03

struct proxy_multi_adv {
    u8_t state;
    int pb_gatt_intant_id;
    int gatt_proxy_intant_id;
};

int bt_mesh_proxy_send(bt_mesh_conn_t conn, u8_t type,
		       struct net_buf_simple *msg);

int bt_mesh_proxy_prov_enable(void);
int bt_mesh_proxy_prov_disable(void);

int bt_mesh_proxy_gatt_enable(void);
int bt_mesh_proxy_gatt_disable(void);
void bt_mesh_proxy_gatt_disconnect(void);

#if (defined CONFIG_BT_MESH_TELINK) || (defined CONFIG_BT_MESH_JINGXUN)
int bt_mesh_proved_reset_flag_set(u8_t flag);
#endif /* CONFIG_BT_MESH_TELINK||CONFIG_BT_MESH_JINGXUN*/

void bt_mesh_proxy_beacon_send(struct bt_mesh_subnet *sub);

struct net_buf_simple *bt_mesh_proxy_get_buf(void);

s32_t bt_mesh_proxy_adv_start(void);
void bt_mesh_proxy_adv_stop(void);

void bt_mesh_proxy_identity_start(struct bt_mesh_subnet *sub);
void bt_mesh_proxy_identity_stop(struct bt_mesh_subnet *sub);

bool bt_mesh_proxy_relay(struct net_buf_simple *buf, u16_t dst);
void bt_mesh_proxy_addr_add(struct net_buf_simple *buf, u16_t addr);

int bt_mesh_proxy_init(void);

#ifdef CONFIG_BT_MESH_CUSTOM_ADV
void bt_mesh_custom_adv_start(void);
void bt_mesh_custom_adv_stop(void);
#endif

void bt_mesh_proxy_adv_enabled(void);
void bt_mesh_proxy_adv_disabled(void);

/**
 * @brief Enable advertising with Node Identity.
 *
 * This API requires that GATT Proxy support has been enabled. Once called
 * each subnet will start advertising using Node Identity for the next
 * 60 seconds.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_proxy_identity_enable(void);

#endif
