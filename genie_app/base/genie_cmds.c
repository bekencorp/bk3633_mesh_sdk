/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

#include <stdlib.h> //for atol
#include "flash.h"
#include "common/log.h"
#include "mesh/access.h"
#include "net.h"
#include "access.h"

#include "genie_app.h"
#include "tri_tuple_default.h"

static void _get_tri_tuple(char *pwbuf, int blen, int argc, char **argv)
{
    genie_tri_tuple_load();
    genie_tri_tuple_show();
}

static void _set_tri_tuple(char *pwbuf, int blen, int argc, char **argv)
{
    uint32_t pid;
    uint8_t mac[6];
    uint8_t key[16];
    uint8_t ret;

    if (argc != 4)
    {
        BT_INFO("para err");
        return;
    }

    //pro_id
    pid = atol(argv[1]);

    //key
    ret = stringtohex(argv[2], key, 16);

    //addr
    ret = stringtohex(argv[3], mac, 6);

    genie_flash_write_trituple(&pid, mac, key);

    _get_tri_tuple(pwbuf, blen, argc, argv);

}

static void _reboot_handle(char *pwbuf, int blen, int argc, char **argv)
{
    aos_reboot();
}

#if defined(CONFIG_GENIE_DEBUG_CMD)
void print_sw_info(void)
{

    BT_INFO("DEVICE:%s", CONFIG_BT_DEVICE_NAME);
    BT_INFO("SW VER:%08x", PROJECT_SW_VERSION);
    BT_INFO("SDK:v%s", APP_SDK_VERSION);
    BT_INFO("PROUDUCT:%s", SYSINFO_PRODUCT_MODEL);

}

static void _get_sw_info(char *pwbuf, int blen, int argc, char **argv)
{
    print_sw_info();
}

extern uint32_t dump_mm_info_used(void);
static void _get_mm_info(char *pwbuf, int blen, int argc, char **argv)
{
#if RHINO_CONFIG_MM_DEBUG
    dump_mm_info_used();
#endif
}

extern struct bt_mesh_elem elements[];
static void _send_msg(char *pwbuf, int blen, int argc, char **argv)
{
    uint8_t count;
    uint8_t msg_b[32];
    uint8_t ret;
    uint8_t i = 0;
    struct bt_mesh_model *p_model = NULL;
    struct bt_mesh_msg_ctx ctx;
    struct net_buf_simple *msg;

    ctx.app_idx = 0;
    ctx.net_idx = 0;
    ctx.addr = 0xF000;
    ctx.send_ttl = 3;
    ctx.send_rel = 0;

    count = strlen(argv[1])>>1;
    ret = stringtohex(argv[1], msg_b, count);

    if(ret == 0) {
        return;
    }

    msg = NET_BUF_SIMPLE(32);

    if(msg == NULL) {
        BT_INFO("no buff");
        return;
    }

    net_buf_simple_init(msg, 0);
    while(i < count) {
        net_buf_simple_add_u8(msg, msg_b[i]);
        i++;
    }
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    p_model = bt_mesh_model_find_vnd(elements, BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV);
#endif
    if(p_model == NULL) {
        BT_INFO("no model");
        return;
    }

    if (bt_mesh_model_send(p_model, &ctx, msg, NULL, NULL)) {
        BT_INFO("Unable to send");
    }
}
#endif

static const struct cli_command genie_cmds[] = {
    {"get_tt", "get tri truple", _get_tri_tuple},
    {"set_tt", "set_tt pid mac key", _set_tri_tuple},
    {"reboot", "reboot", _reboot_handle},

#if defined(CONFIG_GENIE_DEBUG_CMD)
    {"seq", "seq test", cmd_handle_flash_seq},
    {"sys", "sys flash", cmd_handle_flash_sys},
    {"ud", "userdata teest", cmd_handle_flash_ud},

    {"get_info", "get sw info", _get_sw_info},
    {"mm_info", "get mm info", _get_mm_info},
    {"msg", "send mesh msg", _send_msg},
#endif
};

#ifdef CONFIG_BT_MESH_SHELL
static void handle_bt_mesh_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    struct mesh_shell_cmd *mesh_cmds = NULL, *p;
    char *cmd_str, no_match = 1;

    if (strcmp(argv[0], "bt-mesh") != 0) {
        return;
    }

    if (argc <= 1) {
        cmd_str = "help";
    } else {
        cmd_str = argv[1];
    }

    mesh_cmds = bt_mesh_get_shell_cmd_list();
    if (mesh_cmds) {
        p = mesh_cmds;
        while (p->cmd_name != NULL) {
            if (strcmp(p->cmd_name, cmd_str) != 0) {
                p++;
                continue;
            }
            if (p->cb) {
                no_match = 0;
                p->cb(argc - 1, &(argv[1]));
                return;
            }
        }
        printf("cmd error\n");
    }
}

static uint8_t char2u8(char *c)
{
    uint8_t ret = 0;

    if (isdigit(*c)) {
        ret = *c - '0';
    } else if (*c >= 'A' && *c <= 'F') {
        ret = *c - 'A' + 10;
    } else if (*c >= 'a' && *c <= 'f') {
        ret = *c - 'a' + 10;
    }

    return ret;
}

static void handle_set_mac(char *pwbuf, int blen, int argc, char **argv)
{
    char *p;
    uint8_t mac[6] = {0}, i;

    if (argc < 2) {
        printf("Invalid argument.\r\n");
        printf("Usage:\n");
        return;
    }

    for (p = argv[1], i = 0; *p != '\0'; p += 2, i += 2) {
        if (!isxdigit(*p) || !isxdigit(*(p+1))) {
            printf("Invalid format, MAC not set!!!\r\n");
            return;
        }

        mac[i / 2] = ((char2u8(p) & 0x0f) << 4) | (char2u8(p+1) & 0x0f);
    }

    //ais_set_mac(mac);
}

static struct cli_command ncmd[] = {
                                    {
                                        .name     = "set_mac",
                                        .help     = "set_mac <MAC address in xxxxxxxxxxxx format>",
                                        .function = handle_set_mac
                                    },
                                    {   .name     = "bt-mesh",
                                        .help     = "bt-mesh [cmd] [options]",
                                        .function = handle_bt_mesh_cmd 
                                    },
                                 }

;
#endif

void genie_cmds_register(void)
{
#ifdef CONFIG_AOS_CLI
    aos_cli_register_commands(&genie_cmds[0], sizeof(genie_cmds) / sizeof(genie_cmds[0]));
    aos_cli_register_commands(&ncmd[0], sizeof(ncmd) / sizeof(ncmd[0]));
#endif
}

