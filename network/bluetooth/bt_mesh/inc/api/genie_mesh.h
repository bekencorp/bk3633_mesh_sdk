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
#include "inc/light_ctl_srv.h"
#include "inc/light_hsl_srv.h"
#include "vendor_model.h"
#include "inc/vendor_model_srv.h"

#define CONFIG_CID_TAOBAO            0x01A8
#define CONFIG_CID_TUYA              0x07d0
#define CONFIG_MESH_VENDOR_COMPANY_ID CONFIG_CID_TAOBAO
#define CONFIG_MESH_VENDOR_MODEL_SRV  0x0000
#define CONFIG_MESH_VENDOR_MODEL_CLI  0x0001

#define STATIC_OOB_LENGTH            16

#define MESH_TRNSATION_CYCLE 10

#define UNPROV_ADV_FEATURE_AUTO_BIND_MODEL_SUB     0x02
#define UNPROV_ADV_FEATURE_SILENT_ADV              0x01
#define UNPROV_ADV_FLAG_GENIE_MESH_STACK           0x10     //bit4-7
#define UNPROV_ADV_FEATURE_ULTRA_PROV              0x03     //bit0-1

typedef enum {
/* !!!START!!! --- Don't add new ID before this one */
    GENIE_EVT_START = 0,

/* Reset Related Operation */
    GENIE_EVT_SW_RESET = GENIE_EVT_START, /* triggered from cloud */
    GENIE_EVT_HW_RESET_START,  /* triggered from user */
    GENIE_EVT_HW_RESET_DONE,   /*triggered by reset by repeat module */

/* SDK triggered event, with prefix of GENIE_EVT_SDK_MESH_ */
    GENIE_EVT_SDK_START,
    GENIE_EVT_SDK_MESH_INIT = GENIE_EVT_SDK_START,
    GENIE_EVT_SDK_MESH_PBADV_START,
    GENIE_EVT_SDK_MESH_PBADV_TIMEOUT,
    GENIE_EVT_SDK_MESH_SILENT_START,

    GENIE_EVT_SDK_MESH_PROV_START,
    GENIE_EVT_SDK_MESH_PROV_DATA,
    GENIE_EVT_SDK_MESH_PROV_TIMEOUT,
    GENIE_EVT_SDK_MESH_PROV_SUCCESS,
    GENIE_EVT_SDK_MESH_PROV_FAIL,

    GENIE_EVT_SDK_APPKEY_ADD,
    GENIE_EVT_SDK_APPKEY_DEL,
    GENIE_EVT_SDK_APPKEY_UPDATE,
    GENIE_EVT_SDK_NETKEY_ADD,
    GENIE_EVT_SDK_NETKEY_DEL,
    GENIE_EVT_SDK_NETKEY_UPDATE,
    GENIE_EVT_SDK_SUB_ADD,
    GENIE_EVT_SDK_SUB_DEL,
    GENIE_EVT_SDK_SEQ_UPDATE,
    GENIE_EVT_SDK_STATE_SYNC, // Used to sync device's state with cloud
    //GENIE_EVT_SDK_IVI_UPDATE,
    //GENIE_EVENT_SUB_SET,
    //GENIE_EVENT_HB_SET,

    GENIE_EVT_SDK_ANALYZE_MSG,
#ifdef CONFIG_MESH_MODEL_TRANS
    GENIE_EVT_SDK_DELAY_START,
    GENIE_EVT_SDK_DELAY_END,
    GENIE_EVT_SDK_TRANS_START,
    GENIE_EVT_SDK_TRANS_CYCLE,
    GENIE_EVT_SDK_TRANS_END,
#endif
    GENIE_EVT_SDK_ACTION_DONE,
    
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    GENIE_EVT_SDK_MESH_PWRON_INDC,
    GENIE_EVT_SDK_INDICATE,
    GENIE_EVT_SDK_VENDOR_MSG,
#endif

/* APP triggered event, with prefix of GENIE_EVT_APP_ */
    GENIE_EVT_APP_START,
    GENIE_EVT_APP_INDICATE = GENIE_EVT_APP_START,
    GENIE_EVT_APP_VENDOR_MSG,

/* !!!END!!! --- Don't add new ID after this one */
    GENIE_EVT_END

} E_GENIE_EVENT;

static const char *genie_event_str[] = {
    "SW_RESET",
    "HW_RESET_START",
    "HW_RESET_DONE",

    "SDK->MESH_INIT",
    "SDK->PBADV_START",
    "SDK->PBADV_TIMEOUT",
    "SDK->SILENT_START",

    "SDK->PROV_START",
    "SDK->PROV_DATA",
    "SDK->PROV_TIMEOUT",
    "SDK->PROV_SUCCESS",
    "SDK->PROV_FAIL",

    "SDK->APPKEY_ADD",
    "SDK->APPKEY_DEL",
    "SDK->APPKEY_UPDATE",
    "SDK->NETKEY_ADD",
    "SDK->NETKEY_DEL",
    "SDK->NETKEY_UPDATE",
    "SDK->SUB_ADD",
    "SDK->SUB_DEL",
    "SDK->SEQ_UPDATE",
    "SDK->STATE_SYNC",

    "SDK->ANALYZE_MSG",
#ifdef CONFIG_MESH_MODEL_TRANS
    "SDK->DELAY_START",
    "SDK->DELAY_END",
    "SDK->TRANS_START",
    "SDK->TRANS_CYCLE",
    "SDK->TRANS_END",
#endif
    "SDK->ACTION_DONE",
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    "SDK->PWRON_INDC",
    "SDK->INDICATE",
    "SDK->VENDOR_MSG",
#endif

#if 0
    "APP->TURN_ON",
    "APP->TURN_OFF",
#endif
    "APP->VENDOR_MSG",
};

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
    s16_t level[TYPE_NUM];

    u8_t trans_id;
    u8_t trans_src;
    s16_t trans_level;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t actual[TYPE_NUM];
#ifndef CONFIG_ALI_SIMPLE_MODLE
    u16_t linear[TYPE_NUM];
#endif
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    u16_t temp[TYPE_NUM];
    u16_t UV[TYPE_NUM];
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
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        u8_t last_onoff;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t last_actual;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    u16_t last_temp;
#endif

////#ifndef CONFIG_ALI_SIMPLE_MODLE
#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t def_trans;
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t default_onoff;
#endif

    u8_t range_status;

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    s16_t default_level;
    s16_t last_level;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t default_actual;
    u16_t min_actual;
    u16_t max_actual;
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    //temp
    u16_t default_temp;
    u16_t min_temp;
    u16_t max_temp;

    u16_t default_UV;
    u16_t last_UV;
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    u16_t default_lightness;

    u16_t default_hue;
    u16_t min_hue;
    u16_t max_hue;

    u16_t default_sat;
    u16_t min_sat;
    u16_t max_sat;

#endif //CONFIG_MESH_MODEL_HSL_SRV
////#endif
}  S_MODEL_POWERUP;

typedef struct{
    u8_t elem_index;
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

#endif /* __BT_MESH_H */