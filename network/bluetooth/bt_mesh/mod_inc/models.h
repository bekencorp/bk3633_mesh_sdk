/*
 * Copyright (c) 2019 - 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#ifndef BT_MESH_MODELS_H__
#define BT_MESH_MODELS_H__

// #include <bluetooth/mesh.h>

#include <model_types.h>

// /* Foundation models */
// #include <cfg_cli.h>
// #include <cfg_srv.h>
// #include <health_cli.h>
// #include <health_srv.h>

/* Generic models */

#include <gen_onoff_cli.h>
#include <gen_lvl_cli.h>
#include <lightness_cli.h>
#include <light_ctl_cli.h>
#include <light_hsl_cli.h>

/** @brief Check whether the model publishes to a unicast address.
 *
 * @param[in] model Model to check
 *
 * @return true if the model publishes to a unicast address, false otherwise.
 */
bool bt_mesh_model_pub_is_unicast(const struct bt_mesh_model *model);

/** Shorthand macro for defining a model list directly in the element. */
#define BT_MESH_MODEL_LIST(...) ((struct bt_mesh_model[]){ __VA_ARGS__ })
#define SYS_FOREVER_MS (-1)
#define CONFIG_BT_MESH_MOD_ACKD_TIMEOUT_BASE     1000
#define CONFIG_BT_MESH_MOD_ACKD_TIMEOUT_PER_HOP  30  


#endif /* BT_MESH_MODELS_H__ */
