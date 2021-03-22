/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>
#include "icu_pub.h"
#include "gpio.h"
#include "gpio_pub.h"
#include "func_pub.h"
#include "flash_pub.h"
#include "fake_clock_pub.h"
#include "BK3633_RegList.h"

#define AOS_START_STACK 300

static ktask_t *aos_app_task_obj;

extern void board_init(void);

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

static void sys_init(void)
{
    int i = 0;
    os_printf("%s \r\n", __func__);

    soc_system_init();

#ifdef BOOTLOADER
    main();
#else

    board_init();

#if 0
    ///initial LED gpio
    static gpio_dev_t led_pin = {GPIO_P10, OUTPUT_PUSH_PULL, NULL};
    hal_gpio_init(&led_pin);
    hal_gpio_output_high(&led_pin); /// LED off
#endif

#if 1
	os_printf("The APP code build at %s, %s\r\n\n", __TIME__, __DATE__);
#endif

    aos_mutex_lock_init();    //add mutex lock, add_provisioner_supported.

    aos_components_init(&kinit);

    aos_kernel_init(&kinit);

#endif

    krhino_task_del(krhino_cur_task_get());
    aos_free(aos_app_task_obj);
}

//static cpu_stack_t aos_app_stack[AOS_START_STACK];

void sys_start(void)
{
    //printf("%s \r\n\r\n", __func__);
    aos_init();
    //printf("aos init ok \r\n\r\n");

    soc_driver_init();
    printf("soc_driver init ok \r\n\r\n");

    fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
    hal_flash_secure_sector(FLASH_PROTECT_SEC_120);

    printf("start sys_init \r\n\r\n");
    aos_app_task_obj = aos_zalloc(sizeof(ktask_t));
    krhino_task_dyn_create(aos_app_task_obj, "aos-app", 0, AOS_DEFAULT_APP_PRI, 0, 
                           AOS_START_STACK, sys_init, 1);
    //krhino_task_create(&aos_app_task_obj, "aos-app", NULL, AOS_DEFAULT_APP_PRI, 0,
    //                   aos_app_stack, sizeof(aos_app_stack) / sizeof(cpu_stack_t),
    //                   (task_entry_t)sys_init, 1);

    aos_start();
}

