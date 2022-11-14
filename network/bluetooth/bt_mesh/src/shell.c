/** @file
 *  @brief Bluetooth Mesh shell
 *
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_BT_MESH_SHELL

#include <stdlib.h>
#include <ctype.h>
#include <zephyr.h>
#include <misc/printk.h>
#include "errno.h"

#include <bluetooth.h>
#include <api/mesh.h>

/* Private includes for raw Network & Transport layer access */
#include "mesh.h"
#include "net.h"
#include "transport.h"
#include "foundation.h"
#include "gen_onoff_cli.h"
#include "lightness_cli.h"
#include "lightness_internal.h"
#include "gen_lvl_cli.h"
#include "light_ctl_cli.h"

#include "genie_mesh_flash.h"	//for mesh_appkey_para_t & mesh_netkey_para_t used in shell.c.

#define CID_NVAL   0xffff
#define CID_LOCAL  0x01a8

#define CUR_FAULTS_MAX 4

#define NET_PRESSURE_TEST_STRING "helloworld"

/* Default net, app & dev key values, unless otherwise specified */
static const u8_t default_key[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

static struct {
	u16_t local;
	u16_t dst;
	u16_t net_idx;
	u16_t app_idx;
} net = {
	.local = BT_MESH_ADDR_UNASSIGNED,
	.dst = BT_MESH_ADDR_UNASSIGNED,
};



static u8_t cur_faults[CUR_FAULTS_MAX];
static u8_t reg_faults[CUR_FAULTS_MAX * 2];

extern struct k_sem prov_input_sem;
extern u8_t   prov_input[8];
extern u8_t   prov_input_size;

struct mesh_shell_cmd *bt_mesh_get_shell_cmd_list();

#ifdef CONFIG_BT_MESH_PROV
static bt_mesh_prov_bearer_t prov_bear;
static void cmd_pb2(bt_mesh_prov_bearer_t bearer, const char *s);
#endif

#ifdef CONFIG_BT_MESH_CFG_CLI
static struct bt_mesh_cfg_cli cfg_cli = {
};
#endif

#ifdef CONFIG_BT_MESH_HEALTH_CLI
void show_faults(u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count)
{
	size_t i;

	if (!fault_count) {
		printk("Health Test ID 0x%02x Company ID 0x%04x: no faults\n",
		       test_id, cid);
		return;
	}

	printk("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\n",
	       test_id, cid, fault_count);

	for (i = 0; i < fault_count; i++) {
		printk("\t0x%02x\n", faults[i]);
	}
}

static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
				  u8_t test_id, u16_t cid, u8_t *faults,
				  size_t fault_count)
{
	printk("Health Current Status from 0x%04x\n", addr);
	show_faults(test_id, cid, faults, fault_count);
}

static struct bt_mesh_health_cli health_cli = {
	.current_status = health_current_status,
};
#endif

static u8_t dev_uuid[16] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
struct bt_mesh_onoff_cli onoff_cli = BT_MESH_ONOFF_CLI_INIT(NULL);
struct bt_mesh_lightness_cli lightness_cli = BT_MESH_LIGHTNESS_CLI_INIT(NULL);
struct bt_mesh_light_ctl_cli light_ctl_cli = BT_MESH_LIGHT_CTL_CLI_INIT(NULL);
struct bt_mesh_lvl_cli lvl_cli = BT_MESH_LVL_CLI_INIT(NULL);

static struct bt_mesh_model root_models[] = {
#ifdef CONFIG_BT_MESH_CFG_SRV
    BT_MESH_MODEL_CFG_SRV(),
#endif
#ifdef CONFIG_BT_MESH_CFG_CLI
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
#endif
#ifdef CONFIG_BT_MESH_HEALTH_SRV
    BT_MESH_MODEL_HEALTH_SRV(),
#endif
#ifdef CONFIG_BT_MESH_HEALTH_CLI
    BT_MESH_MODEL_HEALTH_CLI(&health_cli),
#endif 
#ifdef CONFIG_BT_MESH_GEN_ONOFF_CLI
    BT_MESH_MODEL_ONOFF_CLI(&onoff_cli),
#endif //CONFIG_BT_MESH_GEN_ONOFF_CLI
#ifdef CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI
    BT_MESH_MODEL_LIGHTNESS_CLI(&lightness_cli),
#endif //CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI
#ifdef CONFIG_BT_MESH_LIGHT_CTL_CLI
    BT_MESH_MODEL_LIGHT_CTL_CLI(&light_ctl_cli),
#endif //CONFIG_BT_MESH_LIGHT_CTL_CLI
#ifdef CONFIG_BT_MESH_GEN_LEVEL_CLI
    BT_MESH_MODEL_LVL_CLI(&lvl_cli),
#endif //CONFIG_BT_MESH_GEN_LEVEL_CLI
};
#if 0
static void vendor_model_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
	return;
}


static const struct bt_mesh_model_op vendor_model_op[] = {
	{ BT_MESH_MODEL_OP_3(0xD0, 0x01A8), 0, vendor_model_get },
};

static struct bt_mesh_model_pub vendor_model_pub = {
    .msg = NET_BUF_SIMPLE(2 + 2),
};
static struct bt_mesh_model vnd_models[] = {
	BT_MESH_MODEL_VND(0x01A8, 0x0000, vendor_model_op, &vendor_model_pub, NULL)
};
#endif

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE,0),
};

static const struct bt_mesh_comp comp = {
	.cid = 0x01A8,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

static u8_t hex2val(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	} else {
		return 0;
	}
}

static size_t hex2bin(const char *hex, u8_t *bin, size_t bin_len)
{
	size_t len = 0;

	while (*hex && len < bin_len) {
		bin[len] = hex2val(*hex++) << 4;

		if (!*hex) {
			len++;
			break;
		}

		bin[len++] |= hex2val(*hex++);
	}

	return len;
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
	printk("Local node provisioned, net_idx 0x%04x address 0x%04x\n",
	       net_idx, addr);
	net.net_idx = net_idx,
	net.local = addr;
	net.dst = addr;
}

static void prov_reset(void)
{
	printk("The local node has been reset and needs reprovisioning\n");
}

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
	printk("OOB Number: %u\n", number);
	return 0;
}

static int output_string(const char *str)
{
	printk("OOB String: %s\n", str);
	return 0;
}

static bt_mesh_input_action_t input_act;
static u8_t input_size;

static int cmd_input_num(int argc, char *argv[])
{
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	if (input_act != BT_MESH_ENTER_NUMBER) {
		printk("A number hasn't been requested!\n");
		return 0;
	}

	if (strlen(argv[1]) < input_size) {
		printk("Too short input (%u digits required)\n",
		       input_size);
		return 0;
	}

	err = bt_mesh_input_number(strtoul(argv[1], NULL, 10));
	if (err) {
		printk("Numeric input failed (err %d)\n", err);
		return 0;
	}

	input_act = BT_MESH_NO_INPUT;
	return 0;
}

static int cmd_input_str(int argc, char *argv[])
{
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	if (input_act != BT_MESH_ENTER_STRING) {
		printk("A string hasn't been requested!\n");
		return 0;
	}

	if (strlen(argv[1]) < input_size) {
		printk("Too short input (%u characters required)\n",
		       input_size);
		return 0;
	}

	err = bt_mesh_input_string(argv[1]);
	if (err) {
		printk("String input failed (err %d)\n", err);
		return 0;
	}

	input_act = BT_MESH_NO_INPUT;
	return 0;
}

static int input(bt_mesh_input_action_t act, u8_t size)
{
	switch (act) {
	case BT_MESH_ENTER_NUMBER:
		printk("Enter a number (max %u digits) with: input-num <num>\n",
		       size);
		break;
	case BT_MESH_ENTER_STRING:
		printk("Enter a string (max %u chars) with: input-str <str>\n",
		       size);
		break;
	default:
		printk("Unknown input action %u (size %u) requested!\n",
		       act, size);
		return -EINVAL;
	}

	input_act = act;
	input_size = size;
	return 0;
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

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	printk("Provisioning link opened on %s\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	printk("Provisioning link closed on %s\n", bearer2str(bearer));
}

static u8_t static_val[16];

static struct bt_mesh_prov prov = {
	.uuid = dev_uuid,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
	.reset = prov_reset,
	//.static_val = NULL,
	//.static_val_len = 0,
	//.output_size = 6,
	//.output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
	//.output_number = output_number,
	//.output_string = output_string,
	//.input_size = 6,
	//.input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
	//.input = input,
};

#if CONFIG_BT_MESH_PROVISIONER
static void provisioner_link_open(bt_mesh_prov_bearer_t bearer)
{
	printk("Provisioner link opened on %s\n", bearer2str(bearer));
}

static void provisioner_link_close(bt_mesh_prov_bearer_t bearer)
{
	printk("Provisioner link closed on %s\n", bearer2str(bearer));
}

static void provisioner_complete(int node_idx, const u8_t device_uuid[16],
                                 u16_t unicast_addr, u8_t element_num,
                                 u16_t netkey_idx, bool gatt_flag)
{
	printk("provisioner_complete\r\n");
	printk("node_idx: %d\r\n", node_idx);
	printk("device_uuid: %02x, %02x, %02x, %02x, %02x, %02x\r\n",
				device_uuid[0], device_uuid[1], device_uuid[2], 
				device_uuid[3], device_uuid[4], device_uuid[5]);
	printk("unicast_addr: %u\r\n", unicast_addr);
	printk("element_num: %u\r\n", element_num);
	printk("netkey_idx: %u\r\n", netkey_idx);
	printk("gatt_flag: %u\r\n", gatt_flag);
}

static int provisioner_input_num(bt_mesh_output_action_t act, u8_t size)
{
    bool    input_num_flag;

	if (BT_MESH_DISPLAY_NUMBER == act) {
		input_num_flag = true;
	} else if (BT_MESH_DISPLAY_STRING == act) {
		input_num_flag = false;
	}

	k_sem_take(&prov_input_sem, K_FOREVER);

	printk("xxxxxxxxxxxx: %s", prov_input);
	printk("get input ");
	printk("xxxxxxxxxxxx: %u", prov_input_size);

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

		printk("===================");
		printk("input number %06u in the device", num);
		printk("===================");
	
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
	
		printk("===================");
		printk("input string %s in the device", temp);
		printk("===================");
	}
	
	bt_mesh_prov_output_data(temp, size, output_num_flag);

	return 0;

}

static struct bt_mesh_provisioner provisioner = {
	.prov_uuid              = 0,
	.prov_unicast_addr      = 0,
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
#endif

static int cmd_static_oob(int argc, char *argv[])
{
	if (argc < 2) {
		prov.static_val = NULL;
		prov.static_val_len = 0;
	} else {
		prov.static_val_len = hex2bin(argv[1], static_val, 16);
		if (prov.static_val_len) {
			prov.static_val = static_val;
		} else {
			prov.static_val = NULL;
		}
	}

	if (prov.static_val) {
		printk("Static OOB value set (length %u)\n",
		       prov.static_val_len);
	} else {
		printk("Static OOB value cleared\n");
	}

	return 0;
}

static int cmd_uuid(int argc, char *argv[])
{
	u8_t uuid[16];
	size_t len;

	if (argc < 2) {
		return -EINVAL;
	}

	len = hex2bin(argv[1], uuid, sizeof(uuid));
	if (len < 1) {
		return -EINVAL;
	}

	memcpy(dev_uuid, uuid, len);
	memset(dev_uuid + len, 0, sizeof(dev_uuid) - len);

	printk("Device UUID set\n");

	return 0;
}

static int cmd_reset(int argc, char *argv[])
{
	bt_mesh_reset();
	printk("Local node reset complete\n");
	return 0;
}

static u8_t str2u8(const char *str)
{
	if (isdigit(str[0])) {
		return strtoul(str, NULL, 0);
	}

	return (!strcmp(str, "on") || !strcmp(str, "enable"));
}

static bool str2bool(const char *str)
{
	return str2u8(str);
}

#if defined(CONFIG_BT_MESH_LOW_POWER)
static int cmd_lpn(int argc, char *argv[])
{
	static bool enabled;
	int err;

	if (argc < 2) {
		printk("%s\n", enabled ? "enabled" : "disabled");
		return 0;
	}

	if (str2bool(argv[1])) {
		if (enabled) {
			printk("LPN already enabled\n");
			return 0;
		}

		err = bt_mesh_lpn_set(true);
		if (err) {
			printk("Enabling LPN failed (err %d)\n", err);
		} else {
			enabled = true;
		}
	} else {
		if (!enabled) {
			printk("LPN already disabled\n");
			return 0;
		}

		err = bt_mesh_lpn_set(false);
		if (err) {
			printk("Enabling LPN failed (err %d)\n", err);
		} else {
			enabled = false;
		}
	}

	return 0;
}

static int cmd_poll(int argc, char *argv[])
{
	int err;

	err = bt_mesh_lpn_poll();
	if (err) {
		printk("Friend Poll failed (err %d)\n", err);
	}

	return 0;
}
static int cmd_lpn_add_sub(int argc, char *argv[])
{
	int err;

	err = bt_mesh_lpn_group_add(0xCFFF);
	if (err) {
		printk("add LPN sub failed (err %d)\n", err);
	}

	return 0;
}
u16_t shell_sub_group[2] = {
	0xC000,
	0xCFFF
};
static int cmd_lpn_del_sub(int argc, char *argv[])
{
	int err;

	err = bt_mesh_lpn_group_del(shell_sub_group,2);
	if (err) {
		printk("delete LPN sub failed (err %d)\n", err);
	}

	return 0;
}

static void lpn_cb(u16_t friend_addr, bool established)
{
	if (established) {
		printk("Friendship (as LPN) established to Friend 0x%04x, at %d\n",
		       friend_addr, k_uptime_get_32());
	} else {
		printk("Friendship (as LPN) lost with Friend 0x%04x, at %d\n",
		       friend_addr, k_uptime_get_32());
	}
}

#endif /* MESH_LOW_POWER */

static void bt_ready(int err)
{
        int ret;

        if (err) {
            printk("Bluetooth init failed (err %d)\n", err);
            return;
        }

        printk("Bluetooth initialized\n");

#if CONFIG_BT_MESH_PROVISIONER
        ret = bt_mesh_init(&prov, &comp, &provisioner);
#else
        ret = bt_mesh_init(&prov, &comp, NULL);
#endif

        if (ret) {
            printk("Mesh initialization failed (err %d)\n", ret);
            return;
        }

        bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
        printk("Mesh initialized\n");
        printk("Use \"pb-adv on\" or \"pb-gatt on\" to enable advertising\n");

#if IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)
        bt_mesh_lpn_set_cb(lpn_cb);
#endif

#ifdef CONFIG_BT_MESH_PROV
	// cmd_pb2(prov_bear, "on");
#endif
}

static int cmd_init(int argc, char *argv[])
{
	int err = 0;
	hci_driver_init();
	// ais_ota_bt_storage_init();

#ifndef CONFIG_MESH_STACK_ALONE
	err = bt_enable(bt_ready);
#endif
        return err;
}

#if defined(CONFIG_BT_MESH_GATT_PROXY)
static int cmd_ident(int argc, char *argv[])
{
	int err;

	err = bt_mesh_proxy_identity_enable();
	if (err) {
		printk("Failed advertise using Node Identity (err %d)\n", err);
	}

	return 0;
}
#endif /* MESH_GATT_PROXY */



static int cmd_dst(int argc, char *argv[])
{
	if (argc < 2) {
		printk("Destination address: 0x%04x%s\n", net.dst,
		       net.dst == net.local ? " (local)" : "");
		return 0;
	}

	if (!strcmp(argv[1], "local")) {
		net.dst = net.local;
	} else {
		net.dst = strtoul(argv[1], NULL, 0);
	}

	printk("Destination address set to 0x%04x%s\n", net.dst,
	       net.dst == net.local ? " (local)" : "");
	return 0;
}

static int cmd_netidx(int argc, char *argv[])
{
	if (argc < 2) {
		printk("NetIdx: 0x%04x\n", net.net_idx);
		return 0;
	}

	net.net_idx = strtoul(argv[1], NULL, 0);
	printk("NetIdx set to 0x%04x\n", net.net_idx);
	return 0;
}

static int cmd_appidx(int argc, char *argv[])
{
	if (argc < 2) {
		printk("AppIdx: 0x%04x\n", net.app_idx);
		return 0;
	}

	net.app_idx = strtoul(argv[1], NULL, 0);
	printk("AppIdx set to 0x%04x\n", net.app_idx);
	return 0;
}

static int cmd_net_send(int argc, char *argv[])
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(32);
	struct bt_mesh_msg_ctx ctx = {
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.net_idx = net.net_idx,
		.addr = net.dst,
		.app_idx = net.app_idx,

	};
	struct bt_mesh_net_tx tx = {
		.ctx = &ctx,
		.src = net.local,
		.xmit = bt_mesh_net_transmit_get(),
		.sub = bt_mesh_subnet_get(net.net_idx),
	};
	size_t len;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	if (!tx.sub) {
		printk("No matching subnet for NetKey Index 0x%04x\n",
		       net.net_idx);
		return 0;
	}

	net_buf_simple_init(msg, 0);
	len = hex2bin(argv[1], msg->data, net_buf_simple_tailroom(msg) - 4);
	net_buf_simple_add(msg, len);

	err = bt_mesh_trans_send(&tx, msg, NULL, NULL);
	if (err) {
		printk("Failed to send (err %d)\n", err);
	}

	return 0;
}

static bool str_is_digit(char *s)
{
	while (*s != '\0') {
		if (!isdigit(*s)) return false;
		else s++;
	}

	return true;
}

static bool str_is_xdigit(char *s)
{
	if (strncmp(s, "0x", strlen("0x")) == 0) {
		s += strlen("0x");
	}

	while (*s != '\0') {
		if (!isxdigit(*s)) return false;
		else s++;
	}

	return true;
}

static int prepare_test_msg(struct net_buf_simple *msg)
{
	net_buf_simple_init(msg, 0);
	memcpy(msg->data, (const void *)NET_PRESSURE_TEST_STRING,
	       sizeof(NET_PRESSURE_TEST_STRING) - 1);
	net_buf_simple_add(msg, sizeof(NET_PRESSURE_TEST_STRING) - 1);
	return 0;
}

#define DEFAULT_PKT_INTERVAL 100
#define DEFAULT_PKT_CNT 3
extern long long k_now_ms();
extern void k_sleep(s32_t ms);
/* cmd: net-pressure-test <dst> <window> <packets-per-window> <duration> [cnt] */
static int cmd_net_pressure_test(int argc, char *argv[])
{
	struct net_buf_simple *msg = NET_BUF_SIMPLE(32);
	struct bt_mesh_msg_ctx ctx = {
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.net_idx = net.net_idx,
		.app_idx = net.app_idx,

	};
	struct bt_mesh_net_tx tx = {
		.ctx = &ctx,
		.src = net.local,
		.xmit = BT_MESH_TRANSMIT(DEFAULT_PKT_CNT, DEFAULT_PKT_INTERVAL),
		.sub = bt_mesh_subnet_get(net.net_idx),
	};
	size_t len;
	int err, window = 0, pkts = 0, dur = 0, i = 0, cnt = DEFAULT_PKT_CNT;
	long long start_time;

	if (argc < 5) {
		printk("%s failed, invalid argument number.\r\n", __func__);
		return -EINVAL;
	}

	if (!str_is_xdigit(argv[1]) || !str_is_digit(argv[2]) ||
	    !str_is_digit(argv[3]) || !str_is_digit(argv[4])) {
		printk("%s failed, invalid argument.\r\n", __func__);
		return -EINVAL;
	} else {
		ctx.addr = strtoul(argv[1], NULL, 16);
		window = strtoul(argv[2], NULL, 0);
		pkts = strtoul(argv[3], NULL, 0);
		dur = strtoul(argv[4], NULL, 0);
	}

	if ((argc == 6) && str_is_digit(argv[5])) {
		cnt = strtoul(argv[5], NULL, 0);
		tx.xmit = BT_MESH_TRANSMIT(cnt, DEFAULT_PKT_INTERVAL);
	}

	if (((window * 1000) / pkts) < (cnt * DEFAULT_PKT_INTERVAL + 100)) {
		printk("Cannot start the test, since the test "
                       "pkt interval is not set properly.\r\n");
		return;
	}

	printk("%s started\r\n", argv[0]);

	start_time = k_now_ms();
	while ((k_now_ms() - start_time) < (dur * 1000)) {
		prepare_test_msg(msg);
		err = bt_mesh_trans_send(&tx, msg, NULL, NULL);
		k_sleep((window * 1000) / pkts);
		if (err) {
			printk("%s failed to send (err %d)\r\n", __func__, err);
		} else {
			i++;
			printk("%s test packet sent (No. %d)\r\n", __func__, i);
		}
	}

	printk("%s ended.\r\n", argv[0]);
}

//#ifdef CONFIG_BT_MESH_BQB
static uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    else if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    else if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    else return 0;
}

static void str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) return;

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s+1))) & 0x0f);
    }
}

int cmd_net_send2(int argc, char *argv[])
{
	u8_t pts_key_val[16] = {0};

        struct net_buf_simple *msg = NET_BUF_SIMPLE(32);
        struct bt_mesh_msg_ctx ctx = {
                .send_ttl = BT_MESH_TTL_DEFAULT,
                //.net_idx = net.net_idx,
                //.addr = net.dst,
                //.app_idx = net.app_idx,

        };
        struct bt_mesh_net_tx tx = {
                .ctx = &ctx,
                //.src = net.local,
                //.xmit = bt_mesh_net_transmit_get(),
                //.sub = bt_mesh_subnet_get(net.net_idx),
        };
        size_t len;
        int err;
        uint8_t aid = 0;

        ctx.send_ttl = (uint8_t)atol(argv[1]);
        ctx.net_idx = (uint16_t)atol(argv[2]);
		ctx.app_idx = (uint16_t)atol(argv[3]);
		ctx.addr = (uint16_t)atol(argv[4]);
        tx.src = (uint16_t)atol(argv[5]);
        tx.xmit = bt_mesh_net_transmit_get();
        tx.sub = bt_mesh_subnet_get(ctx.net_idx);
        aid = (uint8_t)atol(argv[6]);
		//str2hex(key_val, argv[7], 16);
		printk("ctx.send_ttl:            0x%04x\n"
	       	"\tctx.net_idx:              0x%04x\n"
	       	"\tctx.app_idx:              0x%04x\n"
	       	"\tctx.addr:                 0x%04x\n"
	       	"\ttx.src :                  0x%04x\n"
	       	"\taid:                      0x%04x\n"
	       	"\key_val:                   %s\n",

	       ctx.send_ttl, ctx.net_idx, ctx.app_idx, ctx.addr, tx.src, aid,
	       bt_hex(pts_key_val, 16));
		
        if (!tx.sub) {
                printk("No matching subnet for NetKey Index 0x%04x\n",
                       net.net_idx);
                return;
        }

        //str2hex(key_val, argv[6], 16);
        
        bt_mesh_app_key_set_manual(ctx.net_idx, ctx.app_idx, pts_key_val, aid);
        
        net_buf_simple_init(msg, 0);
        len = hex2bin(argv[7], msg->data, net_buf_simple_tailroom(msg) - 4);
        net_buf_simple_add(msg, len);

        err = bt_mesh_trans_send(&tx, msg, NULL, NULL);
        if (err) {
                printk("Failed to send (err %d)\n", err);
        }
}
//#endif

#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
static int cmd_iv_update(int argc, char *argv[])
{
	if (bt_mesh_iv_update()) {
		printk("Transitioned to IV Update In Progress state\n");
	} else {
		printk("Transitioned to IV Update Normal state\n");
	}

	printk("IV Index is 0x%08x\n", bt_mesh.iv_index);

	return 0;
}

static int cmd_iv_update_test(int argc, char *argv[])
{
	bool enable;

	if (argc < 2) {
		return -EINVAL;
	}

	enable = str2bool(argv[1]);
	if (enable) {
		printk("Enabling IV Update test mode\n");
	} else {
		printk("Disabling IV Update test mode\n");
	}

	bt_mesh_iv_update_test(enable);

	return 0;
}
#endif

static int cmd_rpl_clear(int argc, char *argv[])
{
	bt_mesh_rpl_clear();
	return 0;
}

static uint16_t cmd_mesh_prim_addr = 0;
static uint8_t cmd_mesh_devkey[16] = {0};
static uint32_t cmd_mesh_seq = 0;

static mesh_appkey_para_t cmd_app_keys[CONFIG_BT_MESH_APP_KEY_COUNT] = {
        [0 ... (CONFIG_BT_MESH_APP_KEY_COUNT - 1)] = {
            .net_index = BT_MESH_KEY_UNUSED,
			.key_index = BT_MESH_KEY_UNUSED,
        }
};

static mesh_netkey_para_t cmd_subnet[CONFIG_BT_MESH_SUBNET_COUNT]= {
        [0 ... (CONFIG_BT_MESH_SUBNET_COUNT - 1)] = {
            .net_index = BT_MESH_KEY_UNUSED,
        }
};

int cmd_netkey_info_sync(mesh_appkey_para_t *subnet, uint16_t count)
{
	if (subnet) {
	    memcpy(cmd_subnet, subnet, sizeof(struct bt_mesh_subnet) * count);
	}

	return 0;
}

int cmd_appkey_info_sync(mesh_appkey_para_t *appkeys, uint16_t count)
{
	if (appkeys) {
	    memcpy(cmd_app_keys, appkeys, sizeof(mesh_appkey_para_t) * count);
	}

	return 0;
}

static int cmd_net_key_add_local(int argc, char *argv[])
{
	u8_t key_val[16] = {0};
	u16_t key_net_idx;
	u8_t status;
	mesh_netkey_para_t *sub = NULL;
	int err;
    int i;

	if (argc < 3) {
		printf("The parameter number should be more then 2.\n");
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);

	if (argc > 2) {
		size_t len;

		len = hex2bin(argv[2], key_val, sizeof(key_val));
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	for (i = 0; i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
		if (cmd_subnet[i].net_index == BT_MESH_KEY_UNUSED) {
            sub = &cmd_subnet[i];
			break;
		}
	}

	if (sub) {
		sub->net_index = key_net_idx;
		memcpy(sub->key, key_val, sizeof(key_val));
		printf("i: %d, sub->net_idx: %d, sub->keys[0].net: %s\n", i, sub->net_index, bt_hex(sub->key, sizeof(sub->key)));
	} else {
		printf("WARNING: The subnet buffer is full.\n");
	}
#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE
    genie_flash_write_netkey(cmd_subnet, CONFIG_BT_MESH_APP_KEY_COUNT);
#endif
	printf("key_net_idx %d\n", key_net_idx);
	printf("Net key val:%s\n", bt_hex(key_val, sizeof(key_val)));
	return 0;
}

static int cmd_net_key_del_local(int argc, char *argv[])
{
	u16_t key_net_idx;
	u8_t status;
	mesh_netkey_para_t *sub = NULL;
	int err;
    int i;

	if (argc < 2) {
		printf("The parameter number should be more then 1.\n");
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);

	if (key_net_idx == BT_MESH_KEY_UNUSED) {
		printf("Invalid key_net_idx(0x%x) value.\n", key_net_idx);
		return -EINVAL;
	}

	for (i = 0; i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
		if (cmd_subnet[i].net_index == key_net_idx) {
			cmd_subnet[i].net_index = BT_MESH_KEY_UNUSED;
			memset(cmd_subnet[i].key, 0, sizeof(cmd_subnet[i].key));
			printf("Find the netkey and have delete it already.\n");
			break;
		}
	}

	if (i == CONFIG_BT_MESH_SUBNET_COUNT) {
		printf("Didn't find the netkey in the queue for the given net_idx(%d)\n", key_net_idx);
	}

	return 0;
}

static int cmd_net_key_find_local(int argc, char *argv[])
{
    for (int i = 0; i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
		if (cmd_subnet[i].net_index != BT_MESH_KEY_UNUSED) {
			printf("i: %d, net_idx: %d, netkey: %s.\n", i, cmd_subnet[i].net_index, bt_hex(cmd_subnet[i].key, sizeof(cmd_subnet[i].key)));
			break;
		}
	}

	return 0;
}

static int cmd_app_key_add_local(int argc, char *argv[])
{
	u8_t key_val[16] = {0};
	u16_t key_net_idx;
	u16_t key_app_idx;
	u8_t status;
	mesh_appkey_para_t *app_key = NULL;
	int err;
    int i;

	if (argc < 4) {
		printf("The parameter number should be more then 3.\n");
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);
    key_app_idx = strtoul(argv[2], NULL, 0);
	if (argc > 2) {
		size_t len;

		len = hex2bin(argv[3], key_val, sizeof(key_val));
		//memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	for (i = 0; i < CONFIG_BT_MESH_APP_KEY_COUNT; i++) {
		if (cmd_app_keys[i].net_index == BT_MESH_KEY_UNUSED &&
		    cmd_app_keys[i].key_index == BT_MESH_KEY_UNUSED) {
            app_key = &cmd_app_keys[i];
			break;
		}
	}

	if (app_key) {
		app_key->net_index = key_net_idx;
		app_key->key_index = key_app_idx;
		memcpy(app_key->key, key_val, sizeof(key_val));
		printf("i: %d, app_key->net_idx: %d, app_key->app_idx: %d, sub->keys[0].net: %s\n", 
		         i, app_key->net_index, app_key->key_index, bt_hex(app_key->key, sizeof(app_key->key)));
	} else {
		printf("WARNING: The appkey buffer is full.\n");
	}
#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE
    genie_flash_write_appkey(cmd_app_keys, CONFIG_BT_MESH_APP_KEY_COUNT);
#endif
	printf("key_net_idx %d\n", key_net_idx);
	printf("key_app_idx %d\n", key_app_idx);
	printf("APP key val:%s\n", bt_hex(key_val, sizeof(key_val)));
	return 0;
}

static int cmd_app_key_del_local(int argc, char *argv[])
{
	u8_t key_val[16] = {0};
	u16_t key_net_idx;
	u16_t key_app_idx;
	u8_t status;
	mesh_appkey_para_t *app_key = NULL;
	int err;
    int i;

	if (argc < 3) {
		printf("The parameter number should be more then 2.\n");
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);
    key_app_idx = strtoul(argv[2], NULL, 0);

	if (key_net_idx == BT_MESH_KEY_UNUSED ||
	    key_app_idx == BT_MESH_KEY_UNUSED) {
		printf("Invalid key_net_idx(0x%x) or key_app_idx(0x%x) value.\n", key_net_idx, key_app_idx);
		return -EINVAL;
	}

	for (i = 0; i < CONFIG_BT_MESH_APP_KEY_COUNT; i++) {
		if (cmd_app_keys[i].net_index == key_net_idx &&
		    cmd_app_keys[i].key_index == key_app_idx) {
            app_key = &cmd_app_keys[i];
		    app_key->net_index = BT_MESH_KEY_UNUSED;
			app_key->key_index = BT_MESH_KEY_UNUSED;
			memcpy(app_key->key, 0, sizeof(app_key->key));
			printf("Find the netkey and have delete it already.\n");
			break;
		}
	}

	if (i == CONFIG_BT_MESH_APP_KEY_COUNT) {
		printf("Didn't find the appkey in the queue for the given net_idx(%d) and app_idx(%d)\n", key_net_idx, key_app_idx);
		return -EINVAL;
	}

	return 0;
}

static int cmd_app_key_find_local(int argc, char *argv[])
{
    for (int i = 0; i < CONFIG_BT_MESH_APP_KEY_COUNT; i++) {
		if (cmd_app_keys[i].net_index != BT_MESH_KEY_UNUSED &&
		    cmd_app_keys[i].key_index != BT_MESH_KEY_UNUSED) {
			printf("i: %d, net_idx: %d, app_idx: %d, netkey: %s.\n", i, cmd_app_keys[i].net_index, 
			       cmd_app_keys[i].key_index, bt_hex(cmd_app_keys[i].key, sizeof(cmd_app_keys[i].key)));
		}
	}

	return 0;
}

static int cmd_dev_key_add_local(int argc, char *argv[])
{
	u8_t key_val[16] = {0};
	u8_t status;
	int err;

	if (argc > 1) {
		size_t len;

		len = hex2bin(argv[1], key_val, sizeof(key_val));
		//memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

    memcpy(cmd_mesh_devkey, key_val, sizeof(key_val));
#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE	
	genie_flash_write_devkey(cmd_mesh_devkey);
#endif
	printf("Dev key val:%s\n", bt_hex(key_val, sizeof(key_val)));
	return 0;
}

static int cmd_mesh_seq_add(int argc, char *argv[])
{
    u32_t seq;
	u8_t status;
	int err;

	seq = strtoul(argv[1], NULL, 0);

    cmd_mesh_seq = seq;
#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE	
	E_GENIE_FLASH_ERRCODE errcode = genie_flash_write_seq(&seq);
	printf("seq: 0x%x, err: %d\n", seq, errcode);
#endif
	return 0;
}

static int cmd_mesh_prim_addr_add(int argc, char *argv[])
{
	cmd_mesh_prim_addr = strtoul(argv[1], NULL, 0);
	printf("addr:0x%x\n", cmd_mesh_prim_addr);
#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE
	genie_flash_write_addr(&cmd_mesh_prim_addr);
#endif
	return 0;
}

static int cmd_mesh_prov_done(int argc, char *argv[])
{
	bool net_created = false;
        for (int i = 0; i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
            if (cmd_subnet[i].net_index != BT_MESH_KEY_UNUSED) {
                if (!net_created) {
                    net_created = true;
                    bt_mesh_provision(cmd_subnet[i].key, cmd_subnet[i].net_index, cmd_subnet[i].flag, 0, 
					                  cmd_mesh_seq, cmd_mesh_prim_addr, cmd_mesh_devkey);
				    printf("%s, bt_mesh_provision\n", __func__);
                } else {
                    bt_mesh_net_key_add(cmd_subnet[i].net_index, cmd_subnet[i].key);
					printf("%s, bt_mesh_net_key_add\n", __func__);
                }
            }
        }
        extern void genie_appkey_register(u16_t net_idx, u16_t app_idx, const u8_t val[16], bool update);

        for (int j = 0; j < CONFIG_BT_MESH_APP_KEY_COUNT; j++) {
            if (cmd_app_keys[j].net_index != BT_MESH_KEY_UNUSED &&
			    cmd_app_keys[j].key_index != BT_MESH_KEY_UNUSED) {
                genie_appkey_register(cmd_app_keys[j].net_index, cmd_app_keys[j].key_index, cmd_app_keys[j].key, 0);
				printf("%s, genie_appkey_register\n", __func__);
            }
        }

	return 0;
}

#ifdef CONFIG_BT_MESH_CFG_CLI

static int cmd_get_comp(int argc, char *argv[])
{
	struct net_buf_simple *comp = NET_BUF_SIMPLE(32);
	u8_t status, page = 0x00;
	int err;

	if (argc > 1) {
		page = strtol(argv[1], NULL, 0);
	}

	net_buf_simple_init(comp, 0);
	err = bt_mesh_cfg_comp_data_get(net.net_idx, net.dst, page,
					&status, comp);
	if (err) {
		printk("Getting composition failed (err %d)\n", err);
		return 0;
	}

	if (status != 0x00) {
		printk("Got non-success status 0x%02x\n", status);
		return 0;
	}

	printk("Got Composition Data for 0x%04x:\n", net.dst);
	printk("\tCID      0x%04x\n", net_buf_simple_pull_le16(comp));
	printk("\tPID      0x%04x\n", net_buf_simple_pull_le16(comp));
	printk("\tVID      0x%04x\n", net_buf_simple_pull_le16(comp));
	printk("\tCRPL     0x%04x\n", net_buf_simple_pull_le16(comp));
	printk("\tFeatures 0x%04x\n", net_buf_simple_pull_le16(comp));

	while (comp->len > 4) {
		u8_t sig, vnd;
		u16_t loc;
		int i;

		loc = net_buf_simple_pull_le16(comp);
		sig = net_buf_simple_pull_u8(comp);
		vnd = net_buf_simple_pull_u8(comp);

		printk("\n\tElement @ 0x%04x:\n", loc);

		if (comp->len < ((sig * 2) + (vnd * 4))) {
			printk("\t\t...truncated data!\n");
			break;
		}

		if (sig) {
			printk("\t\tSIG Models:\n");
		} else {
			printk("\t\tNo SIG Models\n");
		}

		for (i = 0; i < sig; i++) {
			u16_t mod_id = net_buf_simple_pull_le16(comp);

			printk("\t\t\t0x%04x\n", mod_id);
		}

		if (vnd) {
			printk("\t\tVendor Models:\n");
		} else {
			printk("\t\tNo Vendor Models\n");
		}

		for (i = 0; i < vnd; i++) {
			u16_t cid = net_buf_simple_pull_le16(comp);
			u16_t mod_id = net_buf_simple_pull_le16(comp);

			printk("\t\t\tCompany 0x%04x: 0x%04x\n", cid, mod_id);
		}
	}

	return 0;
}

static int cmd_beacon(int argc, char *argv[])
{
	u8_t status;
	int err;

	if (argc < 2) {
		err = bt_mesh_cfg_beacon_get(net.net_idx, net.dst, &status);
	} else {
		u8_t val = str2u8(argv[1]);

		err = bt_mesh_cfg_beacon_set(net.net_idx, net.dst, val,
					     &status);
	}

	if (err) {
		printk("Unable to send Beacon Get/Set message (err %d)\n", err);
		return 0;
	}

	printk("Beacon state is 0x%02x\n", status);

	return 0;
}

static int cmd_ttl(int argc, char *argv[])
{
	u8_t ttl;
	int err;

	if (argc < 2) {
		err = bt_mesh_cfg_ttl_get(net.net_idx, net.dst, &ttl);
	} else {
		u8_t val = strtoul(argv[1], NULL, 0);

		err = bt_mesh_cfg_ttl_set(net.net_idx, net.dst, val, &ttl);
	}

	if (err) {
		printk("Unable to send Default TTL Get/Set (err %d)\n", err);
		return 0;
	}

	printk("Default TTL is 0x%02x\n", ttl);

	return 0;
}

static int cmd_friend(int argc, char *argv[])
{
	u8_t frnd;
	int err;

	if (argc < 2) {
		err = bt_mesh_cfg_friend_get(net.net_idx, net.dst, &frnd);
	} else {
		u8_t val = str2u8(argv[1]);

		err = bt_mesh_cfg_friend_set(net.net_idx, net.dst, val, &frnd);
	}

	if (err) {
		printk("Unable to send Friend Get/Set (err %d)\n", err);
		return 0;
	}

	printk("Friend is set to 0x%02x\n", frnd);

	return 0;
}

static int cmd_gatt_proxy(int argc, char *argv[])
{
	u8_t proxy;
	int err;

	if (argc < 2) {
		err = bt_mesh_cfg_gatt_proxy_get(net.net_idx, net.dst, &proxy);
	} else {
		u8_t val = str2u8(argv[1]);

		err = bt_mesh_cfg_gatt_proxy_set(net.net_idx, net.dst, val,
						 &proxy);
	}

	if (err) {
		printk("Unable to send GATT Proxy Get/Set (err %d)\n", err);
		return 0;
	}

	printk("GATT Proxy is set to 0x%02x\n", proxy);

	return 0;
}

static int cmd_relay(int argc, char *argv[])
{
	u8_t relay, transmit;
	int err;

	if (argc < 2) {
		err = bt_mesh_cfg_relay_get(net.net_idx, net.dst, &relay,
					    &transmit);
	} else {
		u8_t val = str2u8(argv[1]);
		u8_t count, interval, new_transmit;

		if (val) {
			if (argc > 2) {
				count = strtoul(argv[2], NULL, 0);
			} else {
				count = 2;
			}

			if (argc > 3) {
				interval = strtoul(argv[3], NULL, 0);
			} else {
				interval = 20;
			}

			new_transmit = BT_MESH_TRANSMIT(count, interval);
		} else {
			new_transmit = 0;
		}

		err = bt_mesh_cfg_relay_set(net.net_idx, net.dst, val,
					    new_transmit, &relay, &transmit);
	}

	if (err) {
		printk("Unable to send Relay Get/Set (err %d)\n", err);
		return 0;
	}

	printk("Relay is 0x%02x, Transmit 0x%02x (count %u interval %ums)\n",
	       relay, transmit, BT_MESH_TRANSMIT_COUNT(transmit),
	       BT_MESH_TRANSMIT_INT(transmit));

	return 0;
}

static int cmd_net_key_add(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx;
	u8_t status;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);

	if (argc > 2) {
		size_t len;

		len = hex2bin(argv[3], key_val, sizeof(key_val));
		memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_net_key_add(net.net_idx, net.dst, key_net_idx,
				      key_val, &status);
	if (err) {
		printk("Unable to send NetKey Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("NetKeyAdd failed with status 0x%02x\n", status);
	} else {
		printk("NetKey added with NetKey Index 0x%03x\n", key_net_idx);
	}

	return 0;
}

static int cmd_net_key_get(int argc, char *argv[])
{
	u8_t status;
	int err;

	err = bt_mesh_cfg_net_key_get(net.net_idx, net.dst, &status);

	if (err) {
		printk("Unable to send NetKey Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("NetKeyGet failed with status 0x%02x\n", status);
	}

}

static int cmd_net_key_update(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx;
	u8_t status;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);

	if (argc > 2) {
		size_t len;

		len = hex2bin(argv[3], key_val, sizeof(key_val));
		memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_net_key_update(net.net_idx, net.dst, key_net_idx,
				      key_val, &status);
	if (err) {
		printk("Unable to send NetKey update (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("NetKeyUpdate failed with status 0x%02x\n", status);
	} else {
		printk("NetKey updated with NetKey Index 0x%03x\n", key_net_idx);
	}

	return 0;
}

static int cmd_net_key_del(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx;
	u8_t status;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);


	err = bt_mesh_cfg_net_key_del(net.net_idx, net.dst, key_net_idx, &status);
	if (err) {
		printk("Unable to send NetKey delete (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("NetKey delete failed with status 0x%02x\n", status);
	} else {
		printk("NetKey deleted with NetKey Index 0x%03x\n", key_net_idx);
	}

	return 0;
}

static int cmd_app_key_add(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx, key_app_idx;
	u8_t status;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);
	key_app_idx = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		size_t len;

		len = hex2bin(argv[3], key_val, sizeof(key_val));
		memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_app_key_add(net.net_idx, net.dst, key_net_idx,
				      key_app_idx, key_val, &status);
	if (err) {
		printk("Unable to send App Key Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("AppKeyAdd failed with status 0x%02x\n", status);
	} else {
		printk("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n",
		       key_net_idx, key_app_idx);
	}

	return 0;
}

static int cmd_app_key_del(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx, key_app_idx;
	u8_t status;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);
	key_app_idx = strtoul(argv[2], NULL, 0);


	err = bt_mesh_cfg_app_key_del(net.net_idx, net.dst, key_net_idx, key_app_idx, &status);
	if (err) {
		printk("Unable to send App Key Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("AppKeyAdd failed with status 0x%02x\n", status);
	} else {
		printk("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n",
		       key_net_idx, key_app_idx);
	}

	return 0;
}

static int cmd_app_key_update(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx, key_app_idx;
	u8_t status;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);
	key_app_idx = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		size_t len;

		len = hex2bin(argv[3], key_val, sizeof(key_val));
		memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_app_key_update(net.net_idx, net.dst, key_net_idx,
				      key_app_idx, key_val, &status);
	if (err) {
		printk("Unable to send App Key Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("AppKeyAdd failed with status 0x%02x\n", status);
	} else {
		printk("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n",
		       key_net_idx, key_app_idx);
	}

	return 0;
}

static int cmd_app_key_get(int argc, char *argv[])
{
	u8_t key_val[16];
	u16_t key_net_idx, key_app_idx;
	u8_t status;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	key_net_idx = strtoul(argv[1], NULL, 0);


	err = bt_mesh_cfg_app_key_get(net.net_idx, net.dst, key_net_idx, &status);
	if (err) {
		printk("Unable to send App Key Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("AppKeyAdd failed with status 0x%02x\n", status);
	} else {
		printk("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n",
		       key_net_idx, key_app_idx);
	}

	return 0;
}

static int cmd_mod_app_bind(int argc, char *argv[])
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	mod_app_idx = strtoul(argv[2], NULL, 0);
	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_app_bind_vnd(net.net_idx, net.dst,
						   elem_addr, mod_app_idx,
						   mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_bind(net.net_idx, net.dst, elem_addr,
					       mod_app_idx, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model App Bind (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Model App Bind failed with status 0x%02x\n", status);
	} else {
		printk("AppKey successfully bound\n");
	}

	return 0;
}

static int cmd_mod_app_unbind(int argc, char *argv[])
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	mod_app_idx = strtoul(argv[2], NULL, 0);
	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_app_unbind_vnd(net.net_idx, net.dst,
						   elem_addr, mod_app_idx,
						   mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_unbind(net.net_idx, net.dst, elem_addr,
					       mod_app_idx, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model App Unbind (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Model App Unbind failed with status 0x%02x\n", status);
	} else {
		printk("AppKey successfully Unbound\n");
	}

	return 0;
}

static int cmd_mod_app_get(int argc, char *argv[])
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	mod_id = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		cid = strtoul(argv[3], NULL, 0);
		err = bt_mesh_cfg_mod_app_get_vnd(net.net_idx, net.dst,
						   elem_addr, mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_get(net.net_idx, net.dst, elem_addr, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model App Get (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Model App Get failed with status 0x%02x\n", status);
	} else {
		printk("AppKey successfully Unbound\n");
	}

	return 0;
}

static int cmd_mod_sub_add(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	sub_addr = strtoul(argv[2], NULL, 0);
	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_sub_add_vnd(net.net_idx, net.dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_add(net.net_idx, net.dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Add failed with status 0x%02x\n",
		       status);
	} else {
		printk("Model subscription was successful\n");
	}

	return 0;
}

static int cmd_mod_sub_del(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	sub_addr = strtoul(argv[2], NULL, 0);
	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_sub_del_vnd(net.net_idx, net.dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_del(net.net_idx, net.dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription Delete (err %d)\n",
		       err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Delete failed with status 0x%02x\n",
		       status);
	} else {
		printk("Model subscription deltion was successful\n");
	}

	return 0;
}

static int cmd_mod_sub_del_all(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	mod_id = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		cid = strtoul(argv[3], NULL, 0);
		err = bt_mesh_cfg_mod_sub_del_all_vnd(net.net_idx, net.dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_del_all(net.net_idx, net.dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription Delete All (err %d)\n",
		       err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Delete All failed with status 0x%02x\n",
		       status);
	} else {
		printk("Model subscription all deltion was successful\n");
	}

	return 0;
}


static int cmd_mod_sub_overwrite(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	sub_addr = strtoul(argv[2], NULL, 0);
	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_sub_overwrite_vnd(net.net_idx, net.dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_overwrite(net.net_idx, net.dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription Overwrite (err %d)\n",
		       err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Overwrite failed with status 0x%02x\n",
		       status);
	} else {
		printk("Model subscription Overwrite was successful\n");
	}

	return 0;
}
static int cmd_mod_sub_get(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);
	mod_id = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		cid = strtoul(argv[3], NULL, 0);
		err = bt_mesh_cfg_mod_sub_get_vnd(net.net_idx, net.dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_get(net.net_idx, net.dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription Get (err %d)\n",
		       err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Get failed with status 0x%02x\n",
		       status);
	} else {
		printk("Model subscription get was successful\n");
	}

	return 0;
}
static int cmd_mod_sub_add_va(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);

	len = hex2bin(argv[2], label, sizeof(label));
	memset(label + len, 0, sizeof(label) - len);

	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_sub_va_add_vnd(net.net_idx, net.dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_add(net.net_idx, net.dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		printk("Unable to send Mod Sub VA Add (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Mod Sub VA Add failed with status 0x%02x\n",
		       status);
	} else {
		printk("0x%04x subscribed to Label UUID %s (va 0x%04x)\n",
		       elem_addr, argv[2], sub_addr);
	}

	return 0;
}

static int cmd_mod_sub_del_va(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);

	len = hex2bin(argv[2], label, sizeof(label));
	memset(label + len, 0, sizeof(label) - len);

	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_sub_va_del_vnd(net.net_idx, net.dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_del(net.net_idx, net.dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription Delete (err %d)\n",
		       err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Delete failed with status 0x%02x\n",
		       status);
	} else {
		printk("0x%04x unsubscribed from Label UUID %s (va 0x%04x)\n",
		       elem_addr, argv[2], sub_addr);
	}

	return 0;
}

static int cmd_mod_sub_overwrite_va(int argc, char *argv[])
{
	u16_t elem_addr, sub_addr, mod_id, cid;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 4) {
		return -EINVAL;
	}

	elem_addr = strtoul(argv[1], NULL, 0);

	len = hex2bin(argv[2], label, sizeof(label));
	memset(label + len, 0, sizeof(label) - len);

	mod_id = strtoul(argv[3], NULL, 0);

	if (argc > 4) {
		cid = strtoul(argv[4], NULL, 0);
		err = bt_mesh_cfg_mod_sub_va_overwrite_vnd(net.net_idx, net.dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_overwrite(net.net_idx, net.dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		printk("Unable to send Model Subscription VA overwrite (err %d)\n",
		       err);
		return 0;
	}

	if (status) {
		printk("Model Subscription Overwrite failed with status 0x%02x\n",
		       status);
	} else {
		printk("0x%04x unsubscribed from Label UUID %s (va 0x%04x)\n",
		       elem_addr, argv[2], sub_addr);
	}

	return 0;
}

static int mod_pub_get(u16_t addr, u16_t mod_id, u16_t cid)
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status;
	int err;

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_get(net.net_idx, net.dst, addr,
					      mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_get_vnd(net.net_idx, net.dst, addr,
						  mod_id, cid, &pub, &status);
	}

	if (err) {
		printk("Model Publication Get failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Model Publication Get failed (status 0x%02x)\n",
		       status);
		return 0;
	}

	printk("Model Publication for Element 0x%04x, Model 0x%04x:\n"
	       "\tPublish Address:                0x%04x\n"
	       "\tAppKeyIndex:                    0x%04x\n"
	       "\tCredential Flag:                %u\n"
	       "\tPublishTTL:                     %u\n"
	       "\tPublishPeriod:                  0x%02x\n"
	       "\tPublishRetransmitCount:         %u\n"
	       "\tPublishRetransmitInterval:      %ums\n",
	       addr, mod_id, pub.addr, pub.app_idx, pub.cred_flag, pub.ttl,
	       pub.period, BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit),
	       BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

	return 0;
}

static int mod_pub_set(u16_t addr, u16_t mod_id, u16_t cid, char *argv[])
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status, count;
	u16_t interval;
	int err;
	u8_t uuid[16];

	memset(&pub, 0, sizeof(pub));

	if (strlen(argv[0]) > 20) {
		hex2bin(argv[0], uuid, sizeof(uuid));
		pub.uuid = uuid;
		printf("uuid %s\r\n", bt_hex(pub.uuid, sizeof(pub.uuid)));
	} else {
		pub.addr = strtoul(argv[0], NULL, 0);
	}

	pub.app_idx = strtoul(argv[1], NULL, 0);
	pub.cred_flag = str2bool(argv[2]);
	pub.ttl = strtoul(argv[3], NULL, 0);
	pub.period = strtoul(argv[4], NULL, 0);

	count = strtoul(argv[5], NULL, 0);
	if (count > 7) {
		printk("Invalid retransmit count\n");
		return -EINVAL;
	}

	interval = strtoul(argv[6], NULL, 0);
	if (interval > (31 * 50) || (interval % 50)) {
		printk("Invalid retransmit interval %u\n", interval);
		return -EINVAL;
	}

	pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_set(net.net_idx, net.dst, addr,
					      mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_set_vnd(net.net_idx, net.dst, addr,
						  mod_id, cid, &pub, &status);
	}

	if (err) {
		printk("Model Publication Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Model Publication Set failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Model Publication successfully set\n");
	}

	return 0;
}

static int cmd_mod_pub(int argc, char *argv[])
{
	u16_t addr, mod_id, cid;

	if (argc < 3) {
		return -EINVAL;
	}

	addr = strtoul(argv[1], NULL, 0);
	mod_id = strtoul(argv[2], NULL, 0);

	argc -= 3;
	argv += 3;

	if (argc == 1 || argc == 9) {
		cid = strtoul(argv[0], NULL, 0);
		argc--;
		argv++;
	} else {
		cid = CID_NVAL;
	}

	if (argc > 0) {
		if (argc < 7) {
			return -EINVAL;
		}

		return mod_pub_set(addr, mod_id, cid, argv);
	} else {
		return mod_pub_get(addr, mod_id, cid);
	}
}

#ifdef CONFIG_BT_MESH_LOW_POWER
static int cmd_lpn_timeout_get(int argc, char *argv[])
{
	u8_t status;
	int err;
	u16_t lpn_addr;

	lpn_addr = strtoul(argv[1], NULL, 0);

	err = bt_mesh_cfg_lpn_timeout_get(net.net_idx, net.dst, lpn_addr, &status);

	if (err) {
		printk("send Network transmit Get/Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Network transmit Get/Set failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Network transmit Get/Set successfully\n");
	}
	return 0;
}
#endif //CONFIG_BT_MESH_LOW_POWER

static void hb_sub_print(struct bt_mesh_cfg_hb_sub *sub)
{
	printk("Heartbeat Subscription:\n"
	       "\tSource:      0x%04x\n"
	       "\tDestination: 0x%04x\n"
	       "\tPeriodLog:   0x%02x\n"
	       "\tCountLog:    0x%02x\n"
	       "\tMinHops:     %u\n"
	       "\tMaxHops:     %u\n",
	       sub->src, sub->dst, sub->period, sub->count,
	       sub->min, sub->max);
}

static int hb_sub_get(int argc, char *argv[])
{
	struct bt_mesh_cfg_hb_sub sub;
	u8_t status;
	int err;

	err = bt_mesh_cfg_hb_sub_get(net.net_idx, net.dst, &sub, &status);
	if (err) {
		printk("Heartbeat Subscription Get failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Heartbeat Subscription Get failed (status 0x%02x)\n",
		       status);
	} else {
		hb_sub_print(&sub);
	}

	return 0;
}

static int hb_sub_set(int argc, char *argv[])
{
	struct bt_mesh_cfg_hb_sub sub;
	u8_t status;
	int err;

	sub.src = strtoul(argv[1], NULL, 0);
	sub.dst = strtoul(argv[2], NULL, 0);
	sub.period = strtoul(argv[3], NULL, 0);

	err = bt_mesh_cfg_hb_sub_set(net.net_idx, net.dst, &sub, &status);
	if (err) {
		printk("Heartbeat Subscription Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Heartbeat Subscription Set failed (status 0x%02x)\n",
		       status);
	} else {
		hb_sub_print(&sub);
	}

	return 0;
}

static int cmd_hb_sub(int argc, char *argv[])
{
	if (argc > 1) {
		if (argc < 4) {
			return -EINVAL;
		}

		return hb_sub_set(argc, argv);
	} else {
		return hb_sub_get(argc, argv);
	}
}

static int hb_pub_get(int argc, char *argv[])
{
	struct bt_mesh_cfg_hb_pub pub;
	u8_t status;
	int err;

	err = bt_mesh_cfg_hb_pub_get(net.net_idx, net.dst, &pub, &status);
	if (err) {
		printk("Heartbeat Publication Get failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Heartbeat Publication Get failed (status 0x%02x)\n",
		       status);
		return 0;
	}

	printk("Heartbeat publication:\n");
	printk("\tdst 0x%04x count 0x%02x period 0x%02x\n",
	       pub.dst, pub.count, pub.period);
	printk("\tttl 0x%02x feat 0x%04x net_idx 0x%04x\n",
	       pub.ttl, pub.feat, pub.net_idx);

	return 0;
}

static int hb_pub_set(int argc, char *argv[])
{
	struct bt_mesh_cfg_hb_pub pub;
	u8_t status;
	int err;

	pub.dst = strtoul(argv[1], NULL, 0);
	pub.count = strtoul(argv[2], NULL, 0);
	pub.period = strtoul(argv[3], NULL, 0);
	pub.ttl = strtoul(argv[4], NULL, 0);
	pub.feat = strtoul(argv[5], NULL, 0);
	pub.net_idx = strtoul(argv[5], NULL, 0);

	err = bt_mesh_cfg_hb_pub_set(net.net_idx, net.dst, &pub, &status);
	if (err) {
		printk("Heartbeat Publication Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Heartbeat Publication Set failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Heartbeat publication successfully set\n");
	}

	return 0;
}

static int cmd_hb_pub(int argc, char *argv[])
{
	if (argc > 1) {
		if (argc < 7) {
			return -EINVAL;
		}

		return hb_pub_set(argc, argv);
	} else {
		return hb_pub_get(argc, argv);
	}
}
#endif /*CONFIG_BT_MESH_CFG_CLI*/

static int cmd_node_ident(int argc, char *argv[])
{
	u16_t netkey_idx;
	u8_t ident_state;
	u8_t status;
	int err;

	netkey_idx = strtoul(argv[1], NULL, 0);
	if (argc == 2) {
		 err = bt_mesh_cfg_node_ident_get(netkey_idx, net.dst, net.dst, &status);
	} else {
		ident_state = strtoul(argv[2], NULL, 0);
		err = bt_mesh_cfg_node_ident_set(netkey_idx, net.dst, net.dst, ident_state, &status);
	}

	if (err) {
		printk("Node Identify Set/Get failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Node Identify Set/Get failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Node Identify Set/Get successfully\n");
	}

	return 0;
}

static int cmd_node_reset(int argc, char *argv[])
{
	u8_t status;
	int err;

	err = bt_mesh_cfg_node_reset(net.net_idx, net.dst, &status);

	if (err) {
		printk("send Node Reset failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Node Reset failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Node Reset successfully\n");
	}
	return 0;
}

static int cmd_network_transmit(int argc, char *argv[])
{
	u8_t status;
	int err;
	u8_t count, interval;
	u8_t transmit;

	if (argc < 3) {
		err = bt_mesh_cfg_network_transmit_get(net.net_idx, net.dst, &status);
	} else {
		count = strtoul(argv[1], NULL, 0);
		if (count > 7) {
			printk("Invalid retransmit count\n");
			return -EINVAL;
		}

		interval = strtoul(argv[2], NULL, 0);
		if (interval > (31 * 10) || (interval % 10)) {
			printk("Invalid retransmit interval %u\n", interval);
			return -EINVAL;
		}

		transmit = BT_MESH_TRANSMIT(count, interval);
		err = bt_mesh_cfg_network_transmit_set(net.net_idx, net.dst, transmit, &status);
	}

	if (err) {
		printk("send Network transmit Get/Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Network transmit Get/Set failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Network transmit Get/Set successfully\n");
	}
	return 0;
}

static int cmd_key_refresh(int argc, char *argv[])
{
	u8_t status;
	int err;
	u8_t phase;

	if (argc < 2) {
		err = bt_mesh_cfg_krp_get(net.net_idx, net.dst, net.net_idx, &status);
	} else {
		phase = strtoul(argv[1], NULL, 0);
		err = bt_mesh_cfg_krp_set(net.net_idx, net.dst, net.net_idx, phase, &status);
	}

	if (err) {
		printk("send Network transmit Get/Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printk("Network transmit Get/Set failed (status 0x%02x)\n",
		       status);
	} else {
		printk("Network transmit Get/Set successfully\n");
	}
	return 0;
}

static int cmd_beacon_kr(int argc, char *argv[])
{
	u8_t status;
	int err;
	u8_t state;

	if (argc < 2) {
		printk("cmd_beacon_kr Wrong parameter\r\n");
		return;
	}
	state = str2u8(argv[1]);

	bt_mesh.sub[0].kr_flag = state;
	bt_mesh_kr_update(&bt_mesh.sub[0], state, false);
	//bt_mesh_net_sec_update(&bt_mesh.sub[0]);
	bt_mesh_net_beacon_update(&bt_mesh.sub[0]);

	printf("cmd_beacon_kr kr_flag = %d\r\n", bt_mesh.sub[0].kr_flag);
	return 0;
}

#if defined(CONFIG_BT_MESH_PROV)
static int cmd_pb(bt_mesh_prov_bearer_t bearer, int argc, char *argv[])
{
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	if (str2bool(argv[1])) {
		err = bt_mesh_prov_enable(bearer);
		if (err) {
			printk("Failed to enable %s (err %d)\n",
			       bearer2str(bearer), err);
		} else {
			printk("%s enabled\n", bearer2str(bearer));
		}
	} else {
		err = bt_mesh_prov_disable(bearer);
		if (err) {
			printk("Failed to disable %s (err %d)\n",
			       bearer2str(bearer), err);
		} else {
			printk("%s disabled\n", bearer2str(bearer));
		}
	}

	return 0;
}
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
static int cmd_pb_adv(int argc, char *argv[])
{
#if defined(CONFIG_BT_MESH_PROV)
	return cmd_pb(BT_MESH_PROV_ADV, argc, argv);
#endif
	return 0;
}
#endif /* CONFIG_BT_MESH_PB_ADV */

#if defined(CONFIG_BT_MESH_PB_GATT)
static int cmd_pb_gatt(int argc, char *argv[])
{
#if defined(CONFIG_BT_MESH_PROV)
	return cmd_pb(BT_MESH_PROV_GATT, argc, argv);
#endif
	return 0;
}
#endif /* CONFIG_BT_MESH_PB_GATT */

static int cmd_provision(int argc, char *argv[])
{
	u16_t net_idx, addr;
	u32_t iv_index;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	net_idx = strtoul(argv[1], NULL, 0);
	addr = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		iv_index = strtoul(argv[1], NULL, 0);
	} else {
		iv_index = 0;
	}

	err = bt_mesh_provision(default_key, net_idx, 0, iv_index, 0, addr,
				default_key);
	if (err) {
		printk("Provisioning failed (err %d)\n", err);
	}

	return 0;
}

#ifdef CONFIG_BT_MESH_CFG_CLI
int cmd_timeout(int argc, char *argv[])
{
	s32_t timeout;

	if (argc < 2) {
		timeout = bt_mesh_cfg_cli_timeout_get();
		if (timeout == K_FOREVER) {
			printk("Message timeout: forever\n");
		} else {
			printk("Message timeout: %u seconds\n",
			       timeout / 1000);
		}

		return 0;
	}

	timeout = strtol(argv[1], NULL, 0);
	if (timeout < 0 || timeout > (INT32_MAX / 1000)) {
		timeout = K_FOREVER;
	} else {
		timeout = timeout * 1000;
	}

	bt_mesh_cfg_cli_timeout_set(timeout);
	if (timeout == K_FOREVER) {
		printk("Message timeout: forever\n");
	} else {
		printk("Message timeout: %u seconds\n",
		       timeout / 1000);
	}

	return 0;
}
#endif //CONFIG_BT_MESH_CFG_CLI

static int cmd_fault_get(int argc, char *argv[])
{
	u8_t faults[32];
	size_t fault_count;
	u8_t test_id;
	u16_t cid;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	cid = strtoul(argv[1], NULL, 0);
	fault_count = sizeof(faults);

	err = bt_mesh_health_fault_get(net.net_idx, net.dst, net.app_idx, cid,
				       &test_id, faults, &fault_count);
	if (err) {
		printk("Failed to send Health Fault Get (err %d)\n", err);
	} else {
		show_faults(test_id, cid, faults, fault_count);
	}

	return 0;
}

static int cmd_fault_clear(int argc, char *argv[])
{
	u8_t faults[32];
	size_t fault_count;
	u8_t test_id;
	u16_t cid;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	cid = strtoul(argv[1], NULL, 0);
	fault_count = sizeof(faults);

	err = bt_mesh_health_fault_clear(net.net_idx, net.dst, net.app_idx,
					 cid, &test_id, faults, &fault_count);
	if (err) {
		printk("Failed to send Health Fault Clear (err %d)\n", err);
	} else {
		show_faults(test_id, cid, faults, fault_count);
	}

	return 0;
}

static int cmd_fault_clear_unack(int argc, char *argv[])
{
	u16_t cid;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	cid = strtoul(argv[1], NULL, 0);

	err = bt_mesh_health_fault_clear(net.net_idx, net.dst, net.app_idx,
					 cid, NULL, NULL, NULL);
	if (err) {
		printk("Health Fault Clear Unacknowledged failed (err %d)\n",
		       err);
	}

	return 0;
}

static int cmd_fault_test(int argc, char *argv[])
{
	u8_t faults[32];
	size_t fault_count;
	u8_t test_id;
	u16_t cid;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	cid = strtoul(argv[1], NULL, 0);
	test_id = strtoul(argv[2], NULL, 0);
	fault_count = sizeof(faults);

	err = bt_mesh_health_fault_test(net.net_idx, net.dst, net.app_idx,
					cid, test_id, faults, &fault_count);
	if (err) {
		printk("Failed to send Health Fault Test (err %d)\n", err);
	} else {
		show_faults(test_id, cid, faults, fault_count);
	}

	return 0;
}

static int cmd_fault_test_unack(int argc, char *argv[])
{
	u16_t cid;
	u8_t test_id;
	int err;

	if (argc < 3) {
		return -EINVAL;
	}

	cid = strtoul(argv[1], NULL, 0);
	test_id = strtoul(argv[2], NULL, 0);

	err = bt_mesh_health_fault_test(net.net_idx, net.dst, net.app_idx,
					cid, test_id, NULL, NULL);
	if (err) {
		printk("Health Fault Test Unacknowledged failed (err %d)\n",
		       err);
	}

	return 0;
}

static int cmd_period_get(int argc, char *argv[])
{
	u8_t divisor;
	int err;

	err = bt_mesh_health_period_get(net.net_idx, net.dst, net.app_idx,
					&divisor);
	if (err) {
		printk("Failed to send Health Period Get (err %d)\n", err);
	} else {
		printk("Health FastPeriodDivisor: %u\n", divisor);
	}

	return 0;
}

static int cmd_period_set(int argc, char *argv[])
{
	u8_t divisor, updated_divisor;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	divisor = strtoul(argv[1], NULL, 0);

	err = bt_mesh_health_period_set(net.net_idx, net.dst, net.app_idx,
					divisor, &updated_divisor);
	if (err) {
		printk("Failed to send Health Period Set (err %d)\n", err);
	} else {
		printk("Health FastPeriodDivisor: %u\n", updated_divisor);
	}

	return 0;
}

static int cmd_period_set_unack(int argc, char *argv[])
{
	u8_t divisor;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	divisor = strtoul(argv[1], NULL, 0);

	err = bt_mesh_health_period_set(net.net_idx, net.dst, net.app_idx,
					divisor, NULL);
	if (err) {
		printk("Failed to send Health Period Set (err %d)\n", err);
	}

	return 0;
}

static int cmd_attention_get(int argc, char *argv[])
{
	u8_t attention;
	int err;

	err = bt_mesh_health_attention_get(net.net_idx, net.dst, net.app_idx,
					   &attention);
	if (err) {
		printk("Failed to send Health Attention Get (err %d)\n", err);
	} else {
		printk("Health Attention Timer: %u\n", attention);
	}

	return 0;
}

static int cmd_attention_set(int argc, char *argv[])
{
	u8_t attention, updated_attention;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	attention = strtoul(argv[1], NULL, 0);

	err = bt_mesh_health_attention_set(net.net_idx, net.dst, net.app_idx,
					   attention, &updated_attention);
	if (err) {
		printk("Failed to send Health Attention Set (err %d)\n", err);
	} else {
		printk("Health Attention Timer: %u\n", updated_attention);
	}

	return 0;
}

static int cmd_attention_set_unack(int argc, char *argv[])
{
	u8_t attention;
	int err;

	if (argc < 2) {
		return -EINVAL;
	}

	attention = strtoul(argv[1], NULL, 0);

	err = bt_mesh_health_attention_set(net.net_idx, net.dst, net.app_idx,
					   attention, NULL);
	if (err) {
		printk("Failed to send Health Attention Set (err %d)\n", err);
	}

	return 0;
}

static int cmd_add_fault(int argc, char *argv[])
{
	u8_t fault_id;
	u8_t i;

	if (argc < 2) {
		return -EINVAL;
	}

	fault_id = strtoul(argv[1], NULL, 0);
	if (!fault_id) {
		printk("The Fault ID must be non-zero!\n");
		return -EINVAL;
	}

	for (i = 0; i < sizeof(cur_faults); i++) {
		if (!cur_faults[i]) {
			cur_faults[i] = fault_id;
			break;
		}
	}

	if (i == sizeof(cur_faults)) {
		printk("Fault array is full. Use \"del-fault\" to clear it\n");
		return 0;
	}

	for (i = 0; i < sizeof(reg_faults); i++) {
		if (!reg_faults[i]) {
			reg_faults[i] = fault_id;
			break;
		}
	}

	if (i == sizeof(reg_faults)) {
		printk("No space to store more registered faults\n");
	}

	bt_mesh_fault_update(&elements[0]);

	return 0;
}

static int cmd_del_fault(int argc, char *argv[])
{
	u8_t fault_id;
	u8_t i;

	if (argc < 2) {
		memset(cur_faults, 0, sizeof(cur_faults));
		printk("All current faults cleared\n");
		bt_mesh_fault_update(&elements[0]);
		return 0;
	}

	fault_id = strtoul(argv[1], NULL, 0);
	if (!fault_id) {
		printk("The Fault ID must be non-zero!\n");
		return -EINVAL;
	}

	for (i = 0; i < sizeof(cur_faults); i++) {
		if (cur_faults[i] == fault_id) {
			cur_faults[i] = 0;
			printk("Fault cleared\n");
		}
	}

	bt_mesh_fault_update(&elements[0]);

	return 0;
}
static int cmd_health_pub(int argc, char *argv[])
{

}

static void print_all_help(struct mesh_shell_cmd *cmds)
{
	while (cmds->cmd_name != NULL) {
		printk("%s", cmds->cmd_name);
		if (cmds->help != NULL) {
			printk(": %s\r\n", cmds->help);
		} else {
			printk("\r\n");
		}
		cmds++;
	}
}

static int cmd_display_help(int argc, char *argv[])
{
	char *cmd = NULL, *help_str = NULL;
	struct mesh_shell_cmd *cmd_entry;

	cmd_entry = bt_mesh_get_shell_cmd_list();

	if ((argc == 0) || (argc == 1 && strcmp(argv[0], "help") == 0)) {
		print_all_help(cmd_entry);
		return 0;
	}

	cmd = argv[0];
	if (!cmd_entry) {
		printk("No command supported.\n");
		return 0;
	}

	while (cmd_entry->cmd_name != NULL) {
		if (strcmp(cmd_entry->cmd_name, cmd) != 0) {
			cmd_entry++;
			continue;
		}
		help_str = cmd_entry->help;
		break;
	}

	if (help_str != NULL) {
		printk("%s: %s\n", cmd_entry->cmd_name, help_str);
	}

	return 0;
}

#ifdef CONFIG_BT_MESH_PROV
static void cmd_pb2(bt_mesh_prov_bearer_t bearer, const char *s)
{
	int err;

        if (str2bool(s)) {
                err = bt_mesh_prov_enable(bearer);
                if (err) {
                        printk("Failed to enable %s (err %d)\n",
                               bearer2str(bearer), err);
                } else {
                        printk("%s enabled\n", bearer2str(bearer));
                }
        } else {
                err = bt_mesh_prov_disable(bearer);
                if (err) {
                        printk("Failed to disable %s (err %d)\n",
                               bearer2str(bearer), err);
                } else {
                        printk("%s disabled\n", bearer2str(bearer));
                }
        }
}

static int cmd_bunch_pb_adv(int argc, char *argv[])
{
	cmd_uuid(argc, argv);
    prov_bear = BT_MESH_PROV_ADV;
	cmd_init(0, NULL);
	return 0;
}

static int cmd_bunch_pb_gatt(int argc, char *argv[])
{
	cmd_uuid(argc, argv);
    prov_bear = BT_MESH_PROV_GATT;
	cmd_init(0, NULL);
	return 0;
}
#endif

#if defined (CONFIG_BT_MESH_PROVISIONER)
static int cmd_provisioner(int argc, char *argv[])
{
	printk("start provisioner");
	bt_mesh_provisioner_enable(BT_MESH_PROV_ADV);
}

static int cmd_provisioner_input_str(int argc, char *argv[])
{
	prov_input_size = strlen(argv[1]);

	memcpy(prov_input, argv[1], prov_input_size);

	k_sem_give(&prov_input_sem);
}

static int cmd_provisioner_input_num(int argc, char *argv[])
{
	u32_t   num = 0;
	u8_t    i;

	num = strtoul(argv[1], NULL, 0);

	memset(prov_input, 0, sizeof(prov_input));

	for (i = prov_input_size; i > 0; i--) {
		prov_input[i - 1] = num & 0xFF;
		num >>= 8;
	}

	k_sem_give(&prov_input_sem);
}

extern void send_white_list(u8_t index);
static int cmd_white_list(int argc, char *argv[])
{
    u8_t index = strtoul(argv[1], NULL, 0);
    send_white_list(index);
    return 0;
}

static int cmd_conn(int argc, char *argv[])
{
    my_test_func();
    return 0;
}
#endif


#ifdef CONFIG_BT_MESH_GEN_ONOFF_CLI
void gen_onoff_get(int argc, char **argv)
{
	struct bt_mesh_onoff_status status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	err = bt_mesh_onoff_cli_get(&onoff_cli, &ctx, &status);
	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d %d %d\n", status.present_on_off, status.target_on_off, status.remaining_time);
	return;
}

void gen_onoff_set(int argc, char **argv)
{
	struct bt_mesh_onoff_set set;
	struct bt_mesh_onoff_status status = {0};
	struct bt_mesh_model_transition transition;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	set.on_off = !!(strtoul(argv[2], NULL, 0));
	bool ack = strtoul(argv[3], NULL, 0);
	if(argc > 4){
		transition.time =
			model_transition_decode(strtoul(argv[4], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[5], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	printk("tt=%u delay=%u", transition.time, transition.delay);

	if (ack) {
		err = bt_mesh_onoff_cli_set(&onoff_cli, &ctx, &set, &status);
		printk("status: %d %d %d\n", status.present_on_off, status.target_on_off, status.remaining_time);
	} else {
		err = bt_mesh_onoff_cli_set_unack(&onoff_cli, &ctx, &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}
#endif //CONFIG_BT_MESH_GEN_ONOFF_CLI

#ifdef CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI

void light_lightness_get(int argc, char **argv)
{
	struct bt_mesh_lightness_status status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	err = bt_mesh_lightness_cli_light_get(&lightness_cli, &ctx, &status);
	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d %d %d\n", status.current, status.target, status.remaining_time);
}


void light_lightness_set(int argc, char **argv)
{
	struct bt_mesh_lightness_set set;
	struct bt_mesh_lightness_status status = {0};
	struct bt_mesh_model_transition transition;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	set.lvl = strtoul(argv[2], NULL, 0);
	bool ack = strtoul(argv[3], NULL, 0);
	if(argc > 4){
		transition.time =
			model_transition_decode(strtoul(argv[4], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[5], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	if (ack) {
		err = bt_mesh_lightness_cli_light_set(&lightness_cli, &ctx,
						      &set, &status);
		printk("status: %d %d %d\n", status.current, status.target, status.remaining_time);
	} else {
		err = bt_mesh_lightness_cli_light_set_unack(&lightness_cli,
							    &ctx, &set);
	}
	if (err) {
		printk("err=%d", err);
	}
	
}

void light_lightness_linear_get(int argc, char **argv)
{
	struct bt_mesh_lightness_status status = {0};
	uint16_t current, target;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	err = lightness_cli_light_get(&lightness_cli, &ctx, LINEAR, &status);
	if (err) {
		printk("err=%d", err);
	}

	current = light_to_repr(status.current, LINEAR);
	target = light_to_repr(status.target, LINEAR);
	printk("status: %d %d %d %d\n", status.current, status.target, current, target);
}


void light_lightness_linear_set(int argc, char **argv)
{
	uint16_t current, target;
	struct bt_mesh_lightness_set set;
	struct bt_mesh_lightness_status status={0};
	struct bt_mesh_model_transition transition;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	set.lvl = strtoul(argv[2], NULL, 0);
	bool ack = strtoul(argv[3], NULL, 0);
	if(argc > 4){
		transition.time =
			model_transition_decode(strtoul(argv[4], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[5], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	if (ack) {
		err = lightness_cli_light_set(&lightness_cli, &ctx, LINEAR,
					      &set, &status);

		current = light_to_repr(status.current, LINEAR);
		target = light_to_repr(status.target, LINEAR);

		printk("status: %d %d %d %d %d\n", status.current, status.target, current, target, status.remaining_time);
	} else {
		err = lightness_cli_light_set_unack(&lightness_cli, &ctx,
						    LINEAR, &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}

void light_lightness_last_get(int argc, char **argv)
{
	uint16_t status;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	err = bt_mesh_lightness_cli_last_get(&lightness_cli, &ctx, &status);
	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d \n", status);
}

void light_lightness_default_get(int argc, char **argv)
{
	uint16_t status;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	err = bt_mesh_lightness_cli_default_get(&lightness_cli, &ctx, &status);
	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d \n", status);
}

void light_lightness_default_set(int argc, char **argv)
{
	uint16_t status;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	uint16_t default_light = strtoul(argv[2], NULL, 0);
	if (strtoul(argv[3], NULL, 0)) {
		err = bt_mesh_lightness_cli_default_set(&lightness_cli, &ctx,
							default_light, &status);
		printk("status: %d \n", status);
	} else {
		err = bt_mesh_lightness_cli_default_set_unack(&lightness_cli,
							      &ctx, default_light); 
	}
	if (err) {
		printk("err=%d", err);
	}
}

void light_lightness_range_get(int argc, char **argv)
{
	struct bt_mesh_lightness_range_status status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	err = bt_mesh_lightness_cli_range_get(&lightness_cli, &ctx, &status);
	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d %d %d\n", status.status, status.range.min, status.range.max);
}

void light_lightness_range_set(int argc, char **argv)
{
	struct bt_mesh_lightness_range_status status = {0};
	struct bt_mesh_lightness_range set;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	set.min = strtoul(argv[2], NULL, 0); ;
	set.max = strtoul(argv[3], NULL, 0); 

	if (strtoul(argv[4], NULL, 0)) {
		err = bt_mesh_lightness_cli_range_set(&lightness_cli, &ctx,
						      &set, &status);
		printk("status: %d %d %d\n", status.status, status.range.min, status.range.max);
	} else {
		err = bt_mesh_lightness_cli_range_set_unack(&lightness_cli,
							    &ctx, &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}

void light_ctl_states_get(int argc, char **argv)
{
	struct bt_mesh_light_ctl_status status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	err = bt_mesh_light_ctl_get(&light_ctl_cli, &ctx, &status);

	if (err) {
		printk("err=%d", err);
	}

	printk("status: %d %d %d %d %d\n", status.current_light, status.current_temp,\
	 								 status.target_light, status.target_temp, status.remaining_time);
}


void light_ctl_states_set(int argc, char **argv)
{
	struct bt_mesh_light_ctl_status status;
	struct bt_mesh_model_transition transition;
	struct bt_mesh_light_ctl_set set;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	set.params.light =  strtoul(argv[2], NULL, 0);
	set.params.temp =  strtoul(argv[3], NULL, 0);
	set.params.delta_uv =  strtoul(argv[4], NULL, 0);
	bool ack = strtoul(argv[5], NULL, 0);
	if(argc > 5){
		transition.time =
			model_transition_decode(strtoul(argv[6], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[7], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	printk("tt=%u delay=%u", transition.time, transition.delay);

	if (ack) {
		err = bt_mesh_light_ctl_set(&light_ctl_cli, &ctx, &set,
					    &status);
		printk("status: %d %d %d %d %d\n", status.current_light, status.current_temp,\
	 								 status.target_light, status.target_temp, status.remaining_time);

	} else {
		err = bt_mesh_light_ctl_set_unack(&light_ctl_cli, &ctx, &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}

void light_ctl_temperature_get(int argc, char **argv)
{
	struct bt_mesh_light_temp_status status= {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	err = bt_mesh_light_temp_get(&light_ctl_cli, &ctx, &status);

	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d %d %d %d %d\n", status.current.temp, status.current.delta_uv,\
	 								 status.target.temp, status.target.delta_uv, status.remaining_time);

	return;
}

void light_ctl_temperature_set(int argc, char **argv)
{
	struct bt_mesh_light_temp_status status = {0};
	struct bt_mesh_model_transition transition;
	struct bt_mesh_light_temp_set set;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	set.params.temp = strtoul(argv[2], NULL, 0);
	set.params.delta_uv = strtoul(argv[3], NULL, 0);
	bool ack = strtoul(argv[4], NULL, 0);
	if(argc > 4){
		transition.time =
			model_transition_decode(strtoul(argv[5], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[6], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	printk("tt=%u delay=%u", transition.time, transition.delay);

	if (argc) {   //argv[4] :cmd->ack 
		err = bt_mesh_light_temp_set(&light_ctl_cli, &ctx, &set,
					     &status);
		printk("status: %d %d %d %d %d\n", status.current.temp, status.current.delta_uv,\
	 								 status.target.temp, status.target.delta_uv, status.remaining_time);
	} else {
		err = bt_mesh_light_temp_set_unack(&light_ctl_cli, &ctx, &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}

void light_ctl_default_get(int argc, char **argv)
{
	struct bt_mesh_light_ctl status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	err = bt_mesh_light_ctl_default_get(&light_ctl_cli, &ctx, &status);
	printk("status: %d %d %d \n", status.light, status.temp, status.delta_uv);
	if (err) {
		printk("err=%d", err);
	}
}

void light_ctl_default_set(int argc, char **argv)
{
	struct bt_mesh_light_ctl status;
	struct bt_mesh_light_ctl set;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	set.light = strtoul(argv[2], NULL, 0);
	set.temp = strtoul(argv[3], NULL, 0);
	set.delta_uv = strtoul(argv[4], NULL, 0);

	if (strtoul(argv[5], NULL, 0)) {
		err = bt_mesh_light_ctl_default_set(&light_ctl_cli, &ctx, &set,
						    &status);
		printk("status: %d %d %d\n", status.light, status.temp, status.delta_uv);
	} else {
		err = bt_mesh_light_ctl_default_set_unack(&light_ctl_cli, &ctx,
							  &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}

void light_ctl_temp_range_get(int argc, char **argv)
{
	struct bt_mesh_light_temp_range_status status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	err = bt_mesh_light_temp_range_get(&light_ctl_cli, &ctx, &status);

	if (err) {
		printk("err=%d", err);
	}
	printk("status: %d %d %d\n", status.status, status.range.min, status.range.max);
}

void light_ctl_temp_range_set(int argc, char **argv)
{
	struct bt_mesh_light_temp_range_status status;
	struct bt_mesh_light_temp_range set;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	set.min = strtoul(argv[2], NULL, 0);
	set.max = strtoul(argv[3], NULL, 0);

	if (strtoul(argv[4], NULL, 0)) {  
		err = bt_mesh_light_temp_range_set(&light_ctl_cli, &ctx, &set,
						   &status);
		printk("status: %d %d %d\n", status.status, status.range.min, status.range.max);
	} else {
		err = bt_mesh_light_temp_range_set_unack(&light_ctl_cli, &ctx,
							 &set);
	}
	if (err) {
		printk("err=%d", err);
	}
}
#endif //CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI

#ifdef CONFIG_BT_MESH_GEN_LEVEL_CLI
void gen_lvl_get(int argc, char **argv)
{
	struct bt_mesh_lvl_status status = {0};
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;

	err = bt_mesh_lvl_cli_get(&lvl_cli, &ctx, &status);
	if (err) {
		printk("err=%d", err);
	}

	printk("status: %d %d %d\n", status.current, status.target, status.remaining_time);
}

void gen_lvl_set(int argc, char **argv)
{
	struct bt_mesh_lvl_set set;
	struct bt_mesh_lvl_status status = {0};
	struct bt_mesh_model_transition transition;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	set.lvl = strtoul(argv[2], NULL, 0);
	bool ack = strtoul(argv[3], NULL, 0);
	if(argc > 3){
		transition.time =
			model_transition_decode(strtoul(argv[4], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[5], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	printk("tt=%u delay=%u", transition.time, transition.delay);

	if (ack) {
		err = bt_mesh_lvl_cli_set(&lvl_cli, &ctx, &set, &status);
		printk("status: %d %d %d\n", status.current, status.target, status.remaining_time);
	} else {
		err = bt_mesh_lvl_cli_set_unack(&lvl_cli, &ctx, &set);
	}

	if (err) {
		printk("err=%d", err);
	}
}

void gen_lvl_delta_set(int argc, char **argv)
{
	struct bt_mesh_lvl_delta_set set;
	struct bt_mesh_lvl_status status = {0};
	struct bt_mesh_model_transition transition;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	set.delta = strtoul(argv[2], NULL, 0);
	bool ack = strtoul(argv[3], NULL, 0);
	if(argc > 3){
		transition.time =
			model_transition_decode(strtoul(argv[4], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[5], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	printk("tt=%u delay=%u", transition.time, transition.delay);

	if (ack) {
		err = bt_mesh_lvl_cli_delta_set(&lvl_cli, &ctx, &set, &status);
		printk("status: %d %d %d\n", status.current, status.target, status.remaining_time);
	} else {
		err = bt_mesh_lvl_cli_delta_set_unack(&lvl_cli, &ctx, &set);
	}

	if (err) {
		printk("err=%d", err);
	}
}

void gen_lvl_move_set(int argc, char **argv)
{
	struct bt_mesh_lvl_move_set set;
	struct bt_mesh_lvl_status status;
	struct bt_mesh_model_transition transition;
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = 0x0,
		.app_idx = 0x0,
		.send_ttl = 0x2,
		.addr    = strtoul(argv[1], NULL, 0),
	};
	int err;
	set.delta = strtoul(argv[2], NULL, 0);
	bool ack = strtoul(argv[3], NULL, 0);
	if(argc > 3){
		transition.time =
			model_transition_decode(strtoul(argv[4], NULL, 0));      
		transition.delay = model_delay_decode(strtoul(argv[5], NULL, 0));
		set.transition = &transition;
	} else {
		set.transition = NULL;
	}

	printk("tt=%u delay=%u", transition.time, transition.delay);

	if (ack) {
		err = bt_mesh_lvl_cli_move_set(&lvl_cli, &ctx, &set, &status);
		printk("status: %d %d %d\n", status.current, status.target, status.remaining_time);

	} else {
		err = bt_mesh_lvl_cli_move_set_unack(&lvl_cli, &ctx, &set);
	}

	if (err) {
		printk("err=%d", err);
	}
}

#endif //CONFIG_BT_MESH_GEN_LEVEL_CLI

static const struct mesh_shell_cmd mesh_commands[] = {
	{ "init", cmd_init, NULL },
#if defined (CONFIG_BT_MESH_CFG_CLI)
	{ "timeout", cmd_timeout, "[timeout in seconds]" },
#endif //CONFIG_BT_MESH_CFG_CLI
#if defined(CONFIG_BT_MESH_PB_ADV)
	{ "pb-adv", cmd_pb_adv, "<val: off, on>" },
#endif
#if defined(CONFIG_BT_MESH_PB_GATT)
	{ "pb-gatt", cmd_pb_gatt, "<val: off, on>" },
#endif
	{ "reset", cmd_reset, NULL },
	{ "uuid", cmd_uuid, "<UUID: 1-16 hex values>" },
	{ "input-num", cmd_input_num, "<number>" },
	{ "input-str", cmd_input_str, "<string>" },
	{ "static-oob", cmd_static_oob, "[val: 1-16 hex values]" },

	{ "provision", cmd_provision, "<NetKeyIndex> <addr> [IVIndex]" },
#if defined(CONFIG_BT_MESH_LOW_POWER)
	{ "lpn", cmd_lpn, "<value: off, on>" },
	{ "poll", cmd_poll, NULL },
	{ "add-sub",cmd_lpn_add_sub, NULL},
	{ "del-sub",cmd_lpn_del_sub, NULL},
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
	{ "ident", cmd_ident, NULL },
#endif
	{ "dst", cmd_dst, "[destination address]" },
	{ "netidx", cmd_netidx, "[NetIdx]" },
	{ "appidx", cmd_appidx, "[AppIdx]" },
    {"netkey-add-local", cmd_net_key_add_local, "[NetIdx] [NetKeyVal[16]]"},
	{"netkey-del-local", cmd_net_key_del_local, "[NetIdx]"},
	{"netkey-find-local", cmd_net_key_find_local, "[None]"},
	{"appkey-add-local", cmd_app_key_add_local, "[NetIdx] [AppIdx] [APPKeyVal[16]]"},
	{"appkey-del-local", cmd_app_key_del_local, "[NetIdx] [AppIdx]"},
    {"appkey-find-local", cmd_app_key_find_local, "[None]"},
	{"devkey-add-local", cmd_dev_key_add_local, "DevKeyVal[16]"},
	{"seqnum-add", cmd_mesh_seq_add, "[seq number val]"},
	{"primary-addr-add", cmd_mesh_prim_addr_add, "[primary addr val]"},
	{"prov-done", cmd_mesh_prov_done, "[None]"},
	/* Commands which access internal APIs, for testing only */
	{ "net-send", cmd_net_send, "<hex string>" },
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
	{ "iv-update", cmd_iv_update, NULL },
	{ "iv-update-test", cmd_iv_update_test, "<value: off, on>" },
#endif
	{ "rpl-clear", cmd_rpl_clear, NULL },

#ifdef CONFIG_BT_MESH_CFG_CLI
	/* Configuration Client Model operations */
	{ "get-comp", cmd_get_comp, "[page]" },
	{ "beacon", cmd_beacon, "[val: off, on]" },
	{ "ttl", cmd_ttl, "[ttl: 0x00, 0x02-0x7f]" },
	{ "friend", cmd_friend, "[val: off, on]" },
	{ "gatt-proxy", cmd_gatt_proxy, "[val: off, on]" },
	{ "relay", cmd_relay, "[val: off, on] [count: 0-7] [interval: 0-32]" },
	{ "net-key-add", cmd_net_key_add, "<NetKeyIndex> [val]" },
	{ "net-key-get", cmd_net_key_get, "NULL" },
	{ "net-key-update", cmd_net_key_update, "<NetKeyIndex> [val]" },
	{ "net-key-del", cmd_net_key_del, "<NetKeyIndex> " },
	{ "app-key-add", cmd_app_key_add, "<NetKeyIndex> <AppKeyIndex> [val]" },
	{ "app-key-del", cmd_app_key_del, "<NetKeyIndex> <AppKeyIndex>" },
	{ "app-key-get", cmd_app_key_get, "<NetKeyIndex>" },
	{ "app-key-update", cmd_app_key_update, "<NetKeyIndex> <AppKeyIndex> [val]" },
	{ "mod-app-bind", cmd_mod_app_bind,
		"<addr> <AppIndex> <Model ID> [Company ID]" },
	{ "mod-app-unbind", cmd_mod_app_unbind,
		"<addr> <AppIndex> <Model ID> [Company ID]" },
	{ "mod-app-get", cmd_mod_app_get, "<addr> <Model ID> [Company ID]" },
	{ "mod-pub", cmd_mod_pub, "<addr> <mod id> [cid] [< [PubAddr] [uuid]> "
		"<AppKeyIndex> <cred> <ttl> <period> <count> <interval>]" },
	{ "mod-sub-add", cmd_mod_sub_add,
		"<elem addr> <sub addr> <Model ID> [Company ID]" },
	{ "mod-sub-del", cmd_mod_sub_del,
		"<elem addr> <sub addr> <Model ID> [Company ID]" },
	{ "mod-sub-del-all", cmd_mod_sub_del_all,
		"<elem addr>  <Model ID> [Company ID]" },
	{ "mod-sub-overwrite", cmd_mod_sub_overwrite,
		"<elem addr> <sub addr> <Model ID> [Company ID]" },
	{ "mod-sub-get", cmd_mod_sub_get,
		"<elem addr> <Model ID> [Company ID]" },
	{ "mod-sub-add-va", cmd_mod_sub_add_va,
		"<elem addr> <Label UUID> <Model ID> [Company ID]" },
	{ "mod-sub-del-va", cmd_mod_sub_del_va,
		"<elem addr> <Label UUID> <Model ID> [Company ID]" },
	{ "mod-sub-overwrite-va", cmd_mod_sub_overwrite_va,
		"<elem addr> <Label UUID> <Model ID> [Company ID]" },
	{ "hb-sub", cmd_hb_sub, "[<src> <dst> <period>]" },
	{ "hb-pub", cmd_hb_pub,
		"[<dst> <count> <period> <ttl> <features> <NetKeyIndex>]" },
	{ "node-ident", cmd_node_ident, "<netkey-idx>, [identify state]" },
	{ "node-reset", cmd_node_reset, "NULL" },
	{ "net-transmit", cmd_network_transmit, "<transmit count>, <transmit interval steps>" },
#ifdef CONFIG_BT_MESH_LOW_POWER
	{ "lpn-timeout-get", cmd_lpn_timeout_get, "<lpn_addr>" },
#endif //CONFIG_BT_MESH_LOW_POWER
	{ "key-refresh", cmd_key_refresh, "[KRP Transition]" },
	{ "beacon-kr", cmd_beacon_kr, "<state>" },
#endif

#ifdef CONFIG_BT_MESH_HEALTH_CLI
	/* Health Client Model Operations */
	{ "fault-get", cmd_fault_get, "<Company ID>" },
	{ "fault-clear", cmd_fault_clear, "<Company ID>" },
	{ "fault-clear-unack", cmd_fault_clear_unack, "<Company ID>" },
	{ "fault-test", cmd_fault_test, "<Company ID> <Test ID>" },
	{ "fault-test-unack", cmd_fault_test_unack, "<Company ID> <Test ID>" },
	{ "period-get", cmd_period_get, NULL },
	{ "period-set", cmd_period_set, "<divisor>" },
	{ "period-set-unack", cmd_period_set_unack, "<divisor>" },
	{ "attention-get", cmd_attention_get, NULL },
	{ "attention-set", cmd_attention_set, "<timer>" },
	{ "attention-set-unack", cmd_attention_set_unack, "<timer>" },
#endif

#ifdef CONFIG_BT_MESH_HEALTH_SRV
	/* Health Server Model Operations */
	{ "add-fault", cmd_add_fault, "<Fault ID>" },
	{ "del-fault", cmd_del_fault, "[Fault ID]" },
#endif
	{ "help", cmd_display_help, "[help]"},

#ifdef CONFIG_BT_MESH_PROV
	{ "hk0", cmd_bunch_pb_adv, "<UUID: 1-16 hex values>"},
	{ "hk1", cmd_bunch_pb_gatt, "<UUID: 1-16 hex values>"},
#endif

	{ "net-pressure-test", cmd_net_pressure_test, "<dst> <window(s)> <pkt-per-window> <test duration(s)> [cnt]"},

#if defined (CONFIG_BT_MESH_PROVISIONER)
	{ "provisioner", cmd_provisioner, NULL },
	{ "pinput-str", cmd_provisioner_input_str, "<string>" },
	{ "pinput-num", cmd_provisioner_input_num, "<number>" },
	{ "wl", cmd_white_list, "cmd_white_list" },
	{ "conn", cmd_conn, "cmd_conn" },
#endif
#if defined (CONFIG_BT_MESH_GEN_ONOFF_CLI)
	{ "gen_onoff_get", gen_onoff_get,  "<addr>" },
	{ "gen_onoff_set", gen_onoff_set,  "<addr> ..." },
#endif //CONFIG_BT_MESH_GEN_ONOFF_CLI
#if defined (CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI)
	{ "light_lightness_get", light_lightness_get,  "<addr>" },
	{ "light_lightness_set", light_lightness_set,  "<addr>" },
#endif //CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI
#if defined (CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI)
	{ "light_lightness_linear_get", light_lightness_linear_get,  "<addr>" },
	{ "light_lightness_linear_set", light_lightness_linear_set,  "<addr>" },
	{ "light_lightness_last_get", light_lightness_last_get,  "<addr>" },
	{ "light_lightness_default_get", light_lightness_default_get,  "<addr>" },
	{ "light_lightness_default_set", light_lightness_default_set,  "<addr>" },
	{ "light_lightness_range_get", light_lightness_range_get,  "<addr>" },
	{ "light_lightness_range_set", light_lightness_range_set,  "<addr>" },
	{ "light_ctl_states_get", light_ctl_states_get,  "<addr>" },
	{ "light_ctl_states_set", light_ctl_states_set,  "<addr>" },
	{ "light_ctl_temperature_get", light_ctl_temperature_get,  "<addr>" },
	{ "light_ctl_temperature_set", light_ctl_temperature_set,  "<addr>" },
	{ "light_ctl_default_get", light_ctl_default_get,  "<addr>" },
	{ "light_ctl_default_set", light_ctl_default_set,  "<addr>" },
	{ "light_ctl_temp_range_get", light_ctl_temp_range_get,  "<addr>" },
	{ "light_ctl_temp_range_set", light_ctl_temp_range_set,  "<addr>" },
#endif //CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI
#if defined (CONFIG_BT_MESH_GEN_LEVEL_CLI)
	{ "gen_lvl_get", gen_lvl_get,  "<addr>" },
	{ "gen_lvl_set", gen_lvl_set,  "<addr>" },
	{ "gen_lvl_delta_set", gen_lvl_delta_set,  "<addr>" },
	{ "gen_lvl_move_set", gen_lvl_move_set,  "<addr>" },
#endif //CONFIG_BT_MESH_GEN_LEVEL_CLI
	{ NULL, NULL, NULL}
};

struct mesh_shell_cmd *bt_mesh_get_shell_cmd_list()
{
	return mesh_commands;
}

#endif // CONFIG_BT_MESH_SHELL
