// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#include <errno.h>
//#include <string.h>
#ifdef CONFIG_BT_MESH_PROVISIONER

#include <zephyr.h>
#include <errno.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <net/buf.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_PROVISIONER_BEACON)
#include "common/log.h"

#include "adv.h"
#include "mesh.h"
#include "net.h"
#include "prov.h"
//#include "crypto.h"
#include "beacon.h"
#include "foundation.h"

#include "provisioner_prov.h"

#include "bt_mesh_custom_log.h"

#define BEACON_TYPE_UNPROVISIONED  0x00
#define BEACON_TYPE_SECURE         0x01



static void provisioner_secure_beacon_recv(struct net_buf_simple *buf)
{
    // TODO: Provisioner receive and handle Secure Beacon
}

void provisioner_beacon_recv(struct net_buf_simple *buf)
{
    u8_t type;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (buf->len < 1) {
        BT_ERR("Too short beacon");
        return;
    }

    type = net_buf_simple_pull_u8(buf);
    switch (type) {
    case BEACON_TYPE_UNPROVISIONED:
        BT_DBG("Unprovisioned device beacon received");
        provisioner_unprov_beacon_recv(buf);
        break;
    case BEACON_TYPE_SECURE:
        provisioner_secure_beacon_recv(buf);
        break;
    default:
        BT_WARN("Unknown beacon type 0x%02x", type);
        break;
    }
}

#endif /* CONFIG_BT_MESH_PROVISIONER */
