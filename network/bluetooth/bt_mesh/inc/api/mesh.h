/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_H
#define __BT_MESH_H

#include <stddef.h>
#include <net/buf.h>

#include <mesh/access.h>
#include <mesh/main.h>
#include <mesh/cfg_srv.h>
#include <mesh/health_srv.h>

#if defined(CONFIG_BT_MESH_CFG_CLI)
#include <mesh/cfg_cli.h>
#endif

#if defined(CONFIG_BT_MESH_HEALTH_CLI)
#include <mesh/health_cli.h>
#endif

#if defined(CONFIG_BT_MESH_GATT_PROXY)
#include <mesh/proxy.h>
#endif

#if defined(CONFIG_BT_MESH_SHELL)
#include <mesh/mesh_shell.h>
#endif

#ifdef CONFIG_BT_MESH_ALI_TMALL_GENIE
#include "genie_app.h"
#endif /* CONFIG_BT_MESH_ALI_TMALL_GENIE */

#endif /* __BT_MESH_H */
