/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _GENIE_MESH_H_
#define _GENIE_MESH_H_

//#include <zephyr/types.h>
#include <stddef.h>
#include <net/buf.h>

#include <mesh/access.h>
#include <mesh/main.h>
#include <mesh/cfg_srv.h>
#include <mesh/health_srv.h>
#include "mesh_model/inc/vendor_model.h"
#include "bluetooth/mesh/genie_mesh_flash.h"

#include "mesh_model/inc/ble_mesh.h"

#define CONFIG_CID_TAOBAO            0x01A8
#define CONFIG_CID_TUYA              0x07d0
#define CONFIG_CID_JX                0x0211

#define CONFIG_MESH_VENDOR_COMPANY_ID CONFIG_CID_TAOBAO
#define CONFIG_MESH_VENDOR_MODEL_SRV  0x0000
#define CONFIG_MESH_VENDOR_MODEL_CLI  0x0001

#define STATIC_OOB_LENGTH            16

#define MESH_TRNSATION_CYCLE 10

#define DEFAULT_TRANS_NUM_STEPS_MASK    0x3F
#define DEFAULT_TRANS_SEP_RESO(x)       (((x)>>6) & 0x03)
#define DEFAULT_TRANS_TIME_RANGE        62

#define UNPROV_ADV_FEATURE_AUTO_BIND_MODEL_SUB     0x02
#define UNPROV_ADV_FEATURE_SILENT_ADV              0x01
#define UNPROV_ADV_FLAG_GENIE_MESH_STACK           0x10     //bit4-7
#define UNPROV_ADV_FEATURE_ULTRA_PROV              0x03     //bit0-1

typedef enum {
    T_CUR = 0,
    T_TAR,
    TYPE_NUM,
} E_VALUE_TYPE;

typedef struct{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t onoff[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    s16_t gen_level[TYPE_NUM];

    u8_t trans_id;
    u8_t trans_src;
    s16_t trans_level;
#endif

#ifdef CONFIG_MESH_MODEL_GEN_DEF_TRANS_TIME_SRV
    u8_t tt;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t light_ln_actual[TYPE_NUM];
    u16_t light_ln_linear[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    u16_t ctl_lightness[TYPE_NUM];
    u16_t ctl_temp[TYPE_NUM];
    u16_t ctl_UV[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t delay;    //unit:5ms
    u8_t trans;    //unit:100ms

    u32_t trans_start_time;
    u32_t trans_last_time;
    u32_t trans_end_time;
    s16_t actual_trans_step;
    s16_t temp_trans_step;

    struct k_timer delay_timer;
    struct k_timer trans_timer;
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    u16_t hsl_lightness[TYPE_NUM];
    u16_t hsl_hue[TYPE_NUM];
    u16_t hsl_sat[TYPE_NUM];
#endif //CONFIG_MESH_MODEL_HSL_SRV
}  S_MODEL_STATE;

typedef struct{

#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t def_trans;
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t default_onoff;
    u8_t last_onoff;
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
    u8_t onpowerup;
#endif

    u8_t range_status;

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    s16_t default_level;
    s16_t last_level;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t default_light_ln;
    u16_t light_ln_last;
    u16_t light_ln_range_min;
    u16_t light_ln_range_max;
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    u16_t ctl_temp_def;
    u16_t ctl_temp_range_min;
    u16_t ctl_temp_range_max;
    u16_t ctl_temp_last;

    u16_t ctl_uv_def;
    u16_t ctl_ln_last;
    u16_t ctl_uv_last;
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    u16_t default_hue;
    u16_t min_hue;
    u16_t max_hue;

    u16_t default_sat;
    u16_t min_sat;
    u16_t max_sat;

    u16_t last_hsl_lightness;
    u16_t last_hsl_hue;
    u16_t last_hsl_sat;

#endif //CONFIG_MESH_MODEL_HSL_SRV
////#endif
}  S_MODEL_POWERUP;

typedef struct{
    u8_t elem_index;
    model_message_index_e message_index;
    S_MODEL_STATE state;
    S_MODEL_POWERUP powerup;
    void *user_data;
} S_ELEM_STATE;

#define GENIE_MAX_ELEMENT_COUNT 10

#define ALI_MODEL_TAG "\t[ALI_MODEL]"

#define MODEL_D(f, ...) printf("%d "ALI_MODEL_TAG"[D] %s "f"\n", (u32_t)aos_now_ms(), __func__, ##__VA_ARGS__)
#define MODEL_I(f, ...) printf(ALI_MODEL_TAG"[I] %s "f"\n", __func__, ##__VA_ARGS__)
#define MODEL_E(f, ...) printf(ALI_MODEL_TAG"[E] %s "f"\n", __func__, ##__VA_ARGS__)

#define GENIE_MESH_EVENT_PRINT(id) BT_DBG(F_YELLOW "%s" F_END, genie_event_str[id])

typedef enum{
    MESH_SUCCESS = 0,
    MESH_TID_REPEAT,
    MESH_ANALYZE_SIZE_ERROR,
    MESH_ANALYZE_ARGS_ERROR,
    MESH_SET_TRANSTION_ERROR,
} E_MESH_ERROR_TYPE;

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
#define INDICATION_FLAG_POWERON 0x80
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#define INDICATION_FLAG_ONOFF 0x01
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
#define INDICATION_FLAG_LIGHTNESS 0x02
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
#define INDICATION_FLAG_CTL 0x04
#endif
#ifdef CONFIG_GENIE_OTA
#define INDICATION_FLAG_VERSION 0x08
#endif
#endif

extern uint8_t g_indication_flag;

E_MESH_ERROR_TYPE mesh_check_tid(u16_t src_addr, u8_t tid);
//u8_t get_remain_byte(S_MESH_STATE *state, bool is_ack);
#ifdef CONFIG_MESH_MODEL_TRANS
u32_t get_transition_time(u8_t byte);
#endif
void genie_event(E_GENIE_EVENT event, void *args);
s16_t genie_vendor_model_msg_send(vnd_model_msg *p_vendor_msg);
s16_t genie_light_action_notify(S_ELEM_STATE *p_elem);

/**
 * @brief stop the delay_timer and trans_timer for element.
 * @param[in] p_elem refers to the element.
 */
void mesh_timer_stop(S_ELEM_STATE *p_elem);

/**
 * @brief handle the vendor message
 * @param[in] p_msg refers to the message to be handled
 * @return 0 for successed, -1 for failed.
 */
u16_t genie_vnd_msg_handle(vnd_model_msg *p_msg);

/**
 * @brief
 */
void genie_indicate_start(uint16_t delay_ms, S_ELEM_STATE *p_elem);

/**
 * @brief
 */
void genie_pbadv_timer_start(void);

/**
 * @brief
 */
void genie_pbadv_timer_stop(void);

/**
 * @brief
 */
void genie_prov_timer_start(void);

/**
 * @brief
 */
void genie_prov_timer_stop(void);

/**
 * @brief
 * @param[in] p_elem
 */
void clear_trans_para(S_ELEM_STATE *p_elem);

/**
 * @brief
 * @param[in] p_elem
 * @return
 */
uint8_t calc_cur_state(S_ELEM_STATE * p_elem);

/**
 * @brief
 */
void genie_pbadv_start_silent_adv(void);

/**
 * @brief
 * @return
 */
u16_t genie_indicate_hw_reset_event (void);
uint8_t genie_elem_state_init(uint8_t state_count, S_ELEM_STATE *p_elem);
uint8_t genie_restore_user_state(uint8_t state_count,S_ELEM_STATE *p_elem, S_MODEL_POWERUP *p_pup);
void standart_indication(S_ELEM_STATE *p_elem);
void genie_sub_list_init(void);

void vendor_C7_ack(u8_t tid);


#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#include "bluetooth/mesh/mesh_model/inc/gen_onoff_srv.h"
#include "bluetooth/mesh/mesh_model/inc/model_bind_ops.h"
#endif
#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
#include "bluetooth/mesh/mesh_model/inc/gen_level_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
#include "bluetooth/mesh/mesh_model/inc/lightness_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
#include "bluetooth/mesh/mesh_model/inc/light_ctl_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
#include "bluetooth/mesh/mesh_model/inc/vendor_model_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_GEN_DEF_TRANS_TIME_SRV
#include "bluetooth/mesh/mesh_model/inc/gen_def_trans_time_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
#include "bluetooth/mesh/mesh_model/inc/gen_onpowerup_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
#include "bluetooth/mesh/mesh_model/inc/light_hsl_srv.h"
#endif


void genie_mesh_init(void);

#endif /* __BT_MESH_H */