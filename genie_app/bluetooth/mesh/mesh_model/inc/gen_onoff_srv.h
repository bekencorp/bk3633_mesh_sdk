/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ALI_MODEL_GEN_ONOFF_SRV_H_
#define _ALI_MODEL_GEN_ONOFF_SRV_H_

#include "model_bind_ops.h"

#define GEN_ONOFF_OPC_NUM 4

#define GEN_ONOFF_DEFAULT 1

#define MESH_MODEL_GEN_ONOFF_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, \
                                                g_gen_onoff_op, &g_gen_onoff_pub, _user_data)

extern struct bt_mesh_model_pub g_gen_onoff_pub;
extern const struct bt_mesh_model_op g_gen_onoff_op[GEN_ONOFF_OPC_NUM];

extern BUILD_MODEL_STATE_BIND_HANDLER(B_GEN_ONOFF_ID);

void gen_onoff_publication(struct bt_mesh_model *p_model);

u16_t gen_onoff_bound_states_op(S_ELEM_STATE *p_elem);

#endif // _ALI_MODEL_GEN_ONOFF_SRV_H_
