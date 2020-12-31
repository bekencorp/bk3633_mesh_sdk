/* main.c - provisioner demo */

/*
 * Copyright (C) 2020-2021 Beken Corp Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include "log.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif
#include "application.h"


int application_start(int argc, char **argv)
{
    /* provisioner initilize */
    provisioner_init();

    BT_INFO("PROVISIONER_BUILD_TIME:%s", __DATE__","__TIME__);

    //aos_loop_run();

    return 0;
}


