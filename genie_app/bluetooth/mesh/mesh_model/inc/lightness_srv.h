/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __MODEL_LIGHTNESS_SRV__H_
#define __MODEL_LIGHTNESS_SRV_H_


#define LIGHTNESS_MAX                 (0xFFFF)
#define LIGHTNESS_DEFAULT             (round(LIGHTNESS_MAX * 0.8))      //80%
#define LIGHTNESS_OPC_NUM 10
#define LIGHTNESS_SETUP_OPC_NUM 5


#define MESH_MODEL_LIGHTNESS_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, \
                                                g_lightness_op, &g_lightness_pub, _user_data)

extern struct bt_mesh_model_pub g_lightness_pub;
extern const struct bt_mesh_model_op g_lightness_op[LIGHTNESS_OPC_NUM];

#define MESH_MODEL_LIGHTNESS_SETUP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, \
                                                g_lightness_setup_op, &g_lightness_setup_pub, _user_data)

extern struct bt_mesh_model_pub g_lightness_setup_pub;
extern const struct bt_mesh_model_op g_lightness_setup_op[LIGHTNESS_SETUP_OPC_NUM];

extern BUILD_MODEL_STATE_BIND_HANDLER(B_LIGHTNESS_ACTUAL_ID);

void lightness_publication(struct bt_mesh_model *p_model);

 u16_t light_ln_bound_states_op(S_ELEM_STATE *p_elem);

#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
void bind_ln_actual_with_gen_onpowerup(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
void bind_ln_actual_with_gen_onoff(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
void bind_ln_actual_with_gen_level(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct);
#endif

#endif // __MODEL_LIGHTNESS_SRV_H_
