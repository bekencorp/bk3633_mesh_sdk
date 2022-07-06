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
#include "sys_version.h"

#define AOS_START_STACK 300

static ktask_t aos_app_task_obj;

extern void board_init(void);

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

static void sys_init(void)
{
    int i = 0;
    // os_printf("%s \r\n", __func__);

    soc_system_init();

#ifdef BOOTLOADER
    main();
#else

    board_init();

#if 1
	// os_printf("The APP code build at %s, %s\r\n\n", __TIME__, __DATE__);
#endif

    aos_mutex_lock_init();    //add mutex lock, add_provisioner_supported.

    aos_components_init(&kinit);

    aos_kernel_init(&kinit);

#endif
    // krhino_task_del(krhino_cur_task_get());
    // aos_free(aos_app_task_obj);
}

struct reset_register
{
    uint32_t cpsr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10; // sl stack limit
    uint32_t r11; // fp frame pointer
    uint32_t r12; // ip Intra-Procedure-call scratch register
    uint32_t r13; // sp Stack Pointer.
    uint32_t r14; // lr Link Register.
};

void reset_register_dump(void)
{
    struct reset_register *reg = (struct reset_register *)0x400020;
    printf("\r\nR0: %x\n", (reg->r0));
    printf("R1: %x\n", (reg->r1));
    printf("R2: %x\n", (reg->r2));
    printf("R3: %x\n", (reg->r3));
    printf("R4: %x\n", (reg->r4));
    printf("R5: %x\n", (reg->r5));
    printf("R6: %x\n", (reg->r6));
    printf("R7: %x\n", (reg->r7));
    printf("R8: %x\n", (reg->r8));
    printf("R9: %x\n", (reg->r9));
    printf("R10: %x\n", (reg->r10));
    printf("R11: %x\n", (reg->r11));
    printf("R12: %x\n", (reg->r12));
    printf("R13: %x\n", reg->r13);
    printf("R14(LR): %x\n", (reg->r14));
}
static cpu_stack_t aos_app_stack[AOS_START_STACK];

void sys_start(void)
{
    reset_register_dump();
    printf("%s, reset reason 0x%x\r\n", __func__, icu_get_reset_reason());
    aos_init();
    //printf("aos init ok \r\n\r\n");

    soc_driver_init();
    printf("soc_driver init ok \r\n\r\n");

    printf("\r\n\r\n/*Version Information**************");
    printf("\r\n *        release_version:%s", RELEASE_VERSION);
    printf("\r\n *        release_time:%s", RELEASE_TIME);
    //printf("\r\n *        full_mac_version:%s", FMALL_VERSION);
    //printf("\r\n *        mac_lib_version:%s", FMAC_LIB_VERSON);
    printf("\r\n *        bulid date:%s, time:%s", __DATE__, __TIME__);
    printf("\r\n *Version Over**********************/\r\n\r\n");

    fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);
    hal_flash_secure_sector(FLASH_PROTECT_ALL);

    // printf("start sys_init \r\n\r\n");
    // aos_app_task_obj = aos_zalloc(sizeof(ktask_t));
    // krhino_task_dyn_create(aos_app_task_obj, "aos-app", 0, AOS_DEFAULT_APP_PRI, 0,
    //                        AOS_START_STACK, sys_init, 1);
    krhino_task_create(&aos_app_task_obj, "aos-app", NULL, AOS_DEFAULT_APP_PRI, 0,
                       aos_app_stack, sizeof(aos_app_stack) / sizeof(cpu_stack_t),
                       (task_entry_t)sys_init, 1);

    aos_start();
}

