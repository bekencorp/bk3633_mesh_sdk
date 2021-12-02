// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SERVER_COMMON_H_
#define _SERVER_COMMON_H_

#include <string.h>
#include <stdint.h>
#include "mesh.h"
#include "access.h"
#include "work.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_ATOMIC_DEFINE(name, num_bits) \
        atomic_t name[1 + ((num_bits) - 1) / ATOMIC_BITS]

#define BLE_MESH_SERVER_RSP_MAX_LEN         384

#define BLE_MESH_SERVER_TRANS_MIC_SIZE      4

#define BLE_MESH_CHECK_SEND_STATUS(_func) do {                      \
        int __status = (_func);                                     \
        if (__status) {                                             \
            BT_ERR("%s, Send failed, err %d", __func__, __status);  \
        }                                                           \
    } while(0);

#define BLE_MESH_STATE_OFF                      0x00
#define BLE_MESH_STATE_ON                       0x01
#define BLE_MESH_STATE_RESTORE                  0x02

/* Following 4 values are as per Mesh Model specification */
#define BLE_MESH_LIGHTNESS_MIN                  0x0001
#define BLE_MESH_LIGHTNESS_MAX                  0xFFFF
#define BLE_MESH_TEMPERATURE_MIN                0x0320
#define BLE_MESH_TEMPERATURE_MAX                0x4E20
#define BLE_MESH_TEMPERATURE_UNKNOWN            0xFFFF

/* Refer 7.2 of Mesh Model Specification */
#define BLE_MESH_RANGE_UPDATE_SUCCESS           0x00
#define BLE_MESH_CANNOT_SET_RANGE_MIN           0x01
#define BLE_MESH_CANNOT_SET_RANGE_MAX           0x02

#define BLE_MESH_UNKNOWN_REMAIN_TIME            0x3F
#define BLE_MESH_DEVICE_SPECIFIC_RESOLUTION     10

typedef enum {
    LC_OFF,
    LC_STANDBY,
    LC_FADE_ON,
    LC_RUN,
    LC_FADE,
    LC_PROLONG,
    LC_FADE_STANDBY_AUTO,
    LC_FADE_STANDBY_MANUAL,
} bt_mesh_lc_state;

enum {
    BLE_MESH_TRANS_TIMER_START,  /* Proper transition timer has been started */
    BLE_MESH_TRANS_FLAG_MAX,
};

struct bt_mesh_state_transition {
    bool just_started;

    uint8_t  trans_time;
    uint8_t  remain_time;
    uint8_t  delay;
    uint32_t quo_tt;
    uint32_t counter;
    uint32_t total_duration;
    int64_t  start_timestamp;

    BLE_MESH_ATOMIC_DEFINE(flag, BLE_MESH_TRANS_FLAG_MAX);
    struct k_delayed_work timer;
};

struct bt_mesh_last_msg_info {
    uint8_t  tid;
    uint16_t src;
    uint16_t dst;
    int64_t  timestamp;
};

#define BLE_MESH_SERVER_RSP_BY_APP  0
#define BLE_MESH_SERVER_AUTO_RSP    1

struct bt_mesh_server_rsp_ctrl {
    /**
     * @brief BLE Mesh Server Response Option
     *        1. If get_auto_rsp is set to BLE_MESH_SERVER_RSP_BY_APP, then the response
     *           of Client Get messages need to be replied by the application;
     *        2. If get_auto_rsp is set to BLE_MESH_SERVER_AUTO_RSP, then the response
     *           of Client Get messages will be replied by the server models;
     *        3. If set_auto_rsp is set to BLE_MESH_SERVER_RSP_BY_APP, then the response
     *           of Client Set messages need to be replied by the application;
     *        4. If set_auto_rsp is set to BLE_MESH_SERVER_AUTO_RSP, then the response
     *           of Client Set messages will be replied by the server models;
     *        5. If status_auto_rsp is set to BLE_MESH_SERVER_RSP_BY_APP, then the response
     *           of Server Status messages need to be replied by the application;
     *        6. If status_auto_rsp is set to BLE_MESH_SERVER_AUTO_RSP, then the response
     *           of Server status messages will be replied by the server models;
     */
    uint8_t get_auto_rsp : 1, /* Response for Client Get messages */
            set_auto_rsp : 1, /* Response for Client Set messages */
            status_auto_rsp : 1; /* Response for Server Status messages */
};

/**
 *  @def NET_BUF_SIMPLE_DEFINE
 *  @brief Define a net_buf_simple stack variable.
 *
 *  This is a helper macro which is used to define a net_buf_simple object
 *  on the stack.
 *
 *  @param _name Name of the net_buf_simple object.
 *  @param _size Maximum data storage for the buffer.
 */
#define NET_BUF_SIMPLE_DEFINE(_name, _size) \
    uint8_t net_buf_data_##_name[_size];    \
    struct net_buf_simple _name = {         \
        .data  = net_buf_data_##_name,      \
        .len   = 0,                         \
        .size  = _size,                     \
        .__buf = net_buf_data_##_name,      \
    }


struct bt_mesh_gen_def_trans_time_state {
    uint8_t trans_time;
};

struct bt_mesh_light_lc_state {
    uint32_t mode : 1,              /* default 0 */
             occupancy_mode : 1,    /* default 1 */
             light_onoff : 1,
             target_light_onoff : 1,
             occupancy : 1,
             ambient_luxlevel : 24; /* 0x000000 ~ 0xFFFFFF */

    uint16_t linear_output;         /* 0x0000 ~ 0xFFFF */
};

struct bt_mesh_light_lc_property_state {
    uint32_t time_occupancy_delay;         /* 0x003A */
    uint32_t time_fade_on;                 /* 0x0037 */
    uint32_t time_run_on;                  /* 0x003C */
    uint32_t time_fade;                    /* 0x0036 */
    uint32_t time_prolong;                 /* 0x003B */
    uint32_t time_fade_standby_auto;       /* 0x0038 */
    uint32_t time_fade_standby_manual;     /* 0x0039 */

    uint16_t lightness_on;                 /* 0x002E */
    uint16_t lightness_prolong;            /* 0x002F */
    uint16_t lightness_standby;            /* 0x0030 */

    uint16_t ambient_luxlevel_on;          /* 0x002B, 0x0000 ~ 0xFFFF */
    uint16_t ambient_luxlevel_prolong;     /* 0x002C, 0x0000 ~ 0xFFFF */
    uint16_t ambient_luxlevel_standby;     /* 0x002D, 0x0000 ~ 0xFFFF */

    float    regulator_kiu;                /* 0x0033, 0.0 ~ 1000.0, default 250.0 */
    float    regulator_kid;                /* 0x0032, 0.0 ~ 1000.0, default 25.0 */
    float    regulator_kpu;                /* 0x0035, 0.0 ~ 1000.0, default 80.0 */
    float    regulator_kpd;                /* 0x0034, 0.0 ~ 1000.0, default 80.0 */
    int8_t   regulator_accuracy;           /* 0x0031, 0.0 ~ 100.0, default 2.0 */

    uint32_t set_occupancy_to_1_delay;
};

struct bt_mesh_light_lc_state_machine {
    struct {
        uint8_t fade_on;
        uint8_t fade;
        uint8_t fade_standby_auto;
        uint8_t fade_standby_manual;
    } trans_time;
    bt_mesh_lc_state state;
    struct k_delayed_work timer;
};

struct bt_mesh_light_control {
    struct bt_mesh_light_lc_state          state;
    struct bt_mesh_light_lc_property_state prop_state;
    struct bt_mesh_light_lc_state_machine  state_machine;
};



struct bt_mesh_light_lc_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_control *lc;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
};

struct bt_mesh_gen_def_trans_time_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_def_trans_time_state state;
};

uint8_t bt_mesh_get_default_trans_time(struct bt_mesh_model *model);

int bt_mesh_get_light_lc_trans_time(struct bt_mesh_model *model, uint8_t *trans_time);

int bt_mesh_server_get_optional(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf,
                                uint8_t *trans_time, uint8_t *delay,
                                bool *optional);

void bt_mesh_server_alloc_ctx(struct k_work *work);
void bt_mesh_server_free_ctx(struct k_work *work);

bool bt_mesh_is_server_recv_last_msg(struct bt_mesh_last_msg_info *last,
                                     uint8_t tid, uint16_t src, uint16_t dst, int64_t *now);

void bt_mesh_server_update_last_msg(struct bt_mesh_last_msg_info *last,
                                    uint8_t tid, uint16_t src, uint16_t dst, int64_t *now);

struct net_buf_simple *bt_mesh_server_get_pub_msg(struct bt_mesh_model *model, uint16_t msg_len);

struct net_buf_simple *bt_mesh_alloc_buf(uint16_t size);

void bt_mesh_free_buf(struct net_buf_simple *buf);

void bt_mesh_server_calc_remain_time(struct bt_mesh_state_transition *transition);

void bt_mesh_server_stop_transition(struct bt_mesh_state_transition *transition);

void transition_time_values(struct bt_mesh_state_transition *transition,
                                   uint8_t trans_time, uint8_t delay);

void tt_values_calculator(struct bt_mesh_state_transition *transition);

void bt_mesh_server_start_transition(struct bt_mesh_state_transition *transition);

void scene_recall_work_handler(struct k_work *work);

#ifdef __cplusplus
}
#endif

#endif /* _SERVER_COMMON_H_ */
