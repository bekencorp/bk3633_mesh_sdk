/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

#include "genie_app.h"
#include "tri_tuple_default.h"
#include "flash.h"

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
        printk("para error!!!\n");
        return;
    }

    //pro_id
    pid = atol(argv[1]);

    //key
    ret = stringtohex(argv[2], key, 16);

    //addr
    ret = stringtohex(argv[3], mac, 6);

    genie_flash_write_trituple(pid, mac, key);

    _get_tri_tuple(pwbuf, blen, argc, argv);

}

static void _reboot_handle(char *pwbuf, int blen, int argc, char **argv)
{
    aos_reboot();
}

#if defined(CONFIG_GENIE_DEBUG_CMD_FLASH)
static void _seq_handle(char *pwbuf, int blen, int argc, char **argv)
{
    int32_t ret;
    uint32_t seq;

    if(!strcmp(argv[1], "get")) {
        ret = genie_flash_read_seq(&seq);
        printk("get seq (%d), ret(%d)\n", seq, ret);
    } else if(!strcmp(argv[1], "set")) {
        seq = atol(argv[2]);
        printk("set seq (%d), ret(%d)\n", seq, genie_flash_write_seq(seq));
    } else if(!strcmp(argv[1], "del")) {
        printk("del seq, ret(%d)\n", genie_flash_delete_seq());
    } else if(!strcmp(argv[1], "dump")) {
        genie_flash_dump(GENIE_FLASH_PARTITION_SEQ, atol(argv[2]));
    } else {
        printk("seq cmd error\n");
    }
}

static void _sys_handle(char *pwbuf, int blen, int argc, char **argv)
{
    int32_t ret;
    uint32_t tmp;

    if(!strcmp(argv[1], "get")) {
        //ret = genie_print_data(GENIE_FLASH_PARTITION_SYSTEM);
        ret = genie_print_data(GENIE_FLASH_PARTITION_SYSTEM);
    } else if(!strcmp(argv[1], "set")) {
        tmp = atol(argv[2]);
        ret = genie_flash_write_data(GENIE_FLASH_PARTITION_SYSTEM, tmp);
        printk("set count(%d), ret(%d)\n", tmp, ret);
    } else if(!strcmp(argv[1], "del")) {
        tmp = atol(argv[2]);
        ret = genie_flash_delete_reliable(tmp);
        printk("del index(%d), ret(%d)\n", tmp, ret);
    } else if(!strcmp(argv[1], "dump")) {
        genie_flash_dump(GENIE_FLASH_PARTITION_SYSTEM, atol(argv[2]));
    } else if(!strcmp(argv[1], "rst")) {
        printk("delete mesh\n");
        genie_flash_reset_system();
    } else {
        printk("sys cmd error\n");
    }
}

static void _ud_handle(char *pwbuf, int blen, int argc, char **argv)
{
    int32_t ret;
    uint32_t index;
    uint32_t count;

    if(!strcmp(argv[1], "get")) {
        //ret = genie_print_data(GENIE_FLASH_PARTITION_SYSTEM);
        ret = genie_print_data(GENIE_FLASH_PARTITION_USERDATA);
    } else if(!strcmp(argv[1], "set")) {
        count = atol(argv[2]);
        ret = genie_flash_write_data(GENIE_FLASH_PARTITION_USERDATA, count);
        printk("set count(%d), ret(%d)\n", count, ret);
    } else if(!strcmp(argv[1], "del")) {
        index = atol(argv[2]);
        ret = genie_flash_delete_userdata(index);
        printk("del index(%d), ret(%d)\n", index, ret);
    } else if(!strcmp(argv[1], "dump")) {
        genie_flash_dump(GENIE_FLASH_PARTITION_USERDATA, atol(argv[2]));
    } else if(!strcmp(argv[1], "rst")) {
        printk("delete userdata\n");
        genie_flash_reset_userdata();
    } else {
        printk("ud cmd error\n");
    }
}

#endif

static void _get_sdk_version(char *pwbuf, int blen, int argc, char **argv)
{
    uint32_t ver = APP_SDK_VERSION;

    printk("SDK:v%d\nBUILD_TIME:%s\n", ver, SYSINFO_BUILD_TIME);
}

extern uint32_t dump_mm_info_used(void);
static void _get_mm_info(char *pwbuf, int blen, int argc, char **argv)
{
#if RHINO_CONFIG_MM_DEBUG
    dump_mm_info_used();
#endif
}

#if defined(CONFIG_MESH_MODEL_LIGHT_CMD)

extern uint8_t light_turn_onoff(bool on);
extern s8_t light_adjust_lightness(u8_t l);

static void light_ctl_handler(char *pwbuf, int blen, int argc, char **argv) {
    int32_t ret;
    u8_t lightness = 0;

    if(!strcmp(argv[1], "on")) {
        printk("on command detected\n");
        light_turn_onoff(true);
    } else if(!strcmp(argv[1], "off")) {
        printk("off command detected\n");
        light_turn_onoff(false);
    } else {
        u8_t lightness = atoi(argv[1]);

        if (lightness > 100)
            lightness = 100;
        printk("lightness:%d\n", lightness);

        light_adjust_lightness(lightness);
    }

}

#endif

extern struct bt_mesh_elem elements[1];
static void _send_msg(char *pwbuf, int blen, int argc, char **argv)
{
    uint8_t count;
    uint8_t msg_b[32];
    uint8_t ret;
    uint8_t i = 0;
    struct bt_mesh_model *p_model;
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
        printk("no buff\n");
        return;
    }
    
    net_buf_simple_init(msg, 0);
    while(i < count) {
        net_buf_simple_add_u8(msg, msg_b[i]);
        i++;
    }
    p_model = bt_mesh_model_find_vnd(elements, BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV);

    if(p_model == NULL) {
        printk("no model\n");
        return;
    }

    if (bt_mesh_model_send(p_model, &ctx, msg, NULL, NULL)) {
        printk("Unable to send\n");
    }
}

static const struct cli_command genie_cmds[] = {
    {"get_tt", "get tri truple", _get_tri_tuple},
    {"set_tt", "set_tt pid mac key", _set_tri_tuple},
    {"reboot", "reboot", _reboot_handle},

#if defined(CONFIG_MESH_MODEL_LIGHT_CMD)
    {"light", "light  <cmd>\n\t<light on>: turn on light\n\t<light off> turn off device\n", light_ctl_handler},
#endif

#if defined(CONFIG_GENIE_DEBUG_CMD_FLASH)
    {"seq", "seq test", _seq_handle},
    {"sys", "sys flash", _sys_handle},
    {"ud", "userdata teest", _ud_handle},
#endif

    {"get_ver", "get sdk version", _get_sdk_version},
    {"mm_info", "get mm info", _get_mm_info},
    {"msg", "send mesh msg", _send_msg},
};

void genie_cmds_register(void)
{
#ifdef CONFIG_AOS_CLI
    aos_cli_register_commands(&genie_cmds[0], sizeof(genie_cmds) / sizeof(genie_cmds[0]));
#endif
}

