/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MESH_MODEL_H_
#define _MESH_MODEL_H_

#include "api/mesh.h"
#include "inc/ble_mesh.h"

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#include "inc/gen_onoff_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
#include "inc/gen_level_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
#include "inc/lightness_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
#include "inc/light_ctl_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
#include "inc/light_hsl_srv.h"
#endif

#if defined(CONFIG_BT_MESH_GEN_ONOFF_CLI)
#include "mod_inc/gen_onoff_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_BATTERY_CLI)
#include "mod_inc/gen_battery_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_LEVEL_CLI)
#include "mod_inc/gen_lvl_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_DEF_TRANS_TIME_CLI)
#include "mod_inc/gen_dtt_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_POWER_ONOFF_CLI)
#include "mod_inc/gen_ponoff_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_POWER_LEVEL_CLI)
#include "mod_inc/gen_plvl_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_LOCATION_CLI)
#include "mod_inc/gen_loc_cli.h"
#endif
#if defined(CONFIG_BT_MESH_GEN_PROP_CLI)
#include "mod_inc/gen_prop_cli.h"
#endif
#if defined(CONFIG_BT_MESH_SENSOR_CLI)
#include "mod_inc/sensor_cli.h"
#endif
#if defined(CONFIG_BT_MESH_TIME_CLI)
#include "mod_inc/time_cli.h"
#endif
#if defined(CONFIG_BT_MESH_SCENE_CLI)
#include "mod_inc/scene_cli.h"
#endif
#if defined(CONFIG_BT_MESH_SCHEDULER_CLI)
#include "mod_inc/scheduler_cli.h"
#endif
#if defined(CONFIG_BT_MESH_LIGHT_LIGHTNESS_CLI)
#include "mod_inc/light_lightness_cli.h"
#endif
#if defined(CONFIG_BT_MESH_LIGHT_CTL_CLI)
#include "mod_inc/light_ctl_cli.h"
#endif
#if defined(CONFIG_BT_MESH_LIGHT_HSL_CLI)
#include "mod_inc/light_hsl_cli.h"
#endif
#if defined(CONFIG_BT_MESH_LIGHT_XYL_CLI)
#include "mod_inc/light_xyl_cli.h"
#endif
#if defined(CONFIG_BT_MESH_LIGHT_LC_CLI)
#include "mod_inc/light_lc_cli.h"
#endif

#if defined(CONFIG_MESH_MODEL_VENDOR_SRV)
#include "inc/vendor_model_srv.h"
#endif

#include "inc/model_bind_ops.h"

#define TMM_TRANS_DEFAULT   0x41    //1000ms
#define TMM_ACTUAL_DEFAULT  0xFFFF  //max
#define TMM_TEMP_DEFAULT    0x4E20  //20000

typedef struct {
    long long trans_end_time;
    long long trans_start_time;
} _TMM_STATE_WORK_S;
extern _TMM_STATE_WORK_S g_tmm_state_work;

#if 0
#define ALI_MODEL_TAG "\t[ALI_MODEL]"

#define MODEL_D(f, ...) printf("%d "ALI_MODEL_TAG"[D] %s "f"\n", (u32_t)aos_now_ms(), __func__, ##__VA_ARGS__)
#define MODEL_I(f, ...) printf(ALI_MODEL_TAG"[I] %s "f"\n", __func__, ##__VA_ARGS__)
#define MODEL_E(f, ...) printf(ALI_MODEL_TAG"[E] %s "f"\n", __func__, ##__VA_ARGS__)

//#define MODEL_D
//#define MODEL_I
//#define MODEL_E
#endif

//void light_publication(void);
void mesh_state_save(void);

#endif // _MESH_MODEL_H_
