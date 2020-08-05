/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>

#include <misc/printk.h>
#include <misc/byteorder.h>

#include <api/mesh.h>
#include <bluetooth.h>

#include <aos/kernel.h>
#include <misc/slist.h>
#include <hci.h>


//#include ”ais_btmesh_service.h“
extern void ais_port_init();


static u8_t g_ais_btmesh_data[15] =
		{
				0xa8, 0x01,
				0x85, 1,
				0x15,
				0x11,
				0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33,
		};
bt_addr_t addr = {.val = { 0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33 }};

struct bt_data g_ais_ota_ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA_BYTES(BT_DATA_UUID16_SOME, 0xB3, 0xFE),
//	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, (sizeof(CONFIG_BT_DEVICE_NAME) - 1)),
		BT_DATA(BT_DATA_MANUFACTURER_DATA, g_ais_btmesh_data, 15),
};

static const struct bt_data prov_sd[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, (sizeof(CONFIG_BT_DEVICE_NAME) - 1)),
};

struct bt_le_adv_param fast_adv_param = {
		.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),\
    .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,\
    .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,   \
	.own_addr = &addr, \
};
static const struct bt_le_adv_param slow_adv_param = {
		.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
		.interval_min = BT_GAP_ADV_SLOW_INT_MIN,
		.interval_max = BT_GAP_ADV_SLOW_INT_MAX,
		.own_addr = &addr, \
};

static void ota_adv_thread(void *p1, void *p2, void *p3)
{
	int err;

	printf("Starting Advertiser...\n");

	do {
		aos_msleep(200);

		/* Start advertising */
		err = bt_le_adv_start(&fast_adv_param, g_ais_ota_ad, ARRAY_SIZE(g_ais_ota_ad), prov_sd, ARRAY_SIZE(prov_sd));
		if (err) {
			printf("Advertising failed to start (err %d)\n", err);
			return;
		}

		aos_msleep(200);
		err = bt_le_adv_stop();
		if (err) {
			printf("Advertising failed to stop (err %d)\n", err);
			return;
		}
	} while (1);
}
#define GATT_ADV_STACK_SIZE 1024
static struct k_thread gatt_adv_thread_data;
static K_THREAD_STACK_DEFINE(gatt_adv_thread_stack, GATT_ADV_STACK_SIZE);

void bt_gatt_adv_init(void)
{
	printk("%s called\n", __FUNCTION__);

	k_thread_create(&gatt_adv_thread_data, gatt_adv_thread_stack,
	                K_THREAD_STACK_SIZEOF(gatt_adv_thread_stack), ota_adv_thread,
	                NULL, NULL, NULL, CONFIG_BT_MESH_ADV_PRIO, 0, K_NO_WAIT);
}


void ali_dfu_init(void)
{
	printk("%s called\n", __FUNCTION__);
	ais_port_init();
    //bt_gatt_adv_init();
	btmesh_ais_init();
}
