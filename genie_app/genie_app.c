/* main.c - Application main entry point */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <hal/hal.h>

#include "genie_app.h"

extern void ota_mutex_init();
void genie_init(void)
{
    printf("genie start\r\n");
    ota_mutex_init();
//
#ifdef CONFIG_GENIE_CLI
    genie_cmds_register();
#endif

#ifdef CONFIG_GENIE_RESET_BY_REPEAT
//    genie_reset_by_repeat_init();
#endif

    genie_flash_init();
    led_startup();

#ifdef CONFIG_BT_MESH
    genie_mesh_init();
#endif
}

