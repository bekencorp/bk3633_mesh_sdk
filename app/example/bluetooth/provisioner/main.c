/* main.c - provisioner demo */

/*
 * Copyright (C) 2020-2021 Beken Corp Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include <hal/soc/gpio.h>
#include <hal/soc/pwm.h>

#include "mesh.h"
#include "log.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif
#include "application.h"


#ifdef CONFIG_BT_MESH_CFG_CLI
static struct bt_mesh_cfg_cli cfg_cli = {

};
struct bt_mesh_cfg_cli onoff_cli = {

};
#endif

static struct bt_mesh_model root_models[] = {

    //BT_MESH_MODEL_CFG_SRV(),
    //BT_MESH_MODEL_HEALTH_SRV(),
#ifdef CONFIG_BT_MESH_CFG_CLI
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, NULL, NULL, &onoff_cli),
#endif
};

static struct bt_mesh_model vnd_models[] = {
//     BT_MESH_MODEL_VND(_company, _id, _op, _pub, _user_data), //example.
// #ifdef CONFIG_MESH_MODEL_VENDOR_SRV
//     MESH_MODEL_VENDOR_SRV(&g_elem_state[0]),
// #endif
};

struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models, 0),
};

uint8_t provisioner_get_vendor_element_num(void)
{
    return ARRAY_SIZE(elements);
}


int application_start(int argc, char **argv)
{
    /* provisioner initilize */
    provisioner_init();

// #ifdef CONFIG_UART_TEST_CMD
//     uart_test_init();
// #endif

    BT_INFO("PROVISIONER_BUILD_TIME:%s", __DATE__","__TIME__);

    //aos_loop_run();

    return 0;
}


