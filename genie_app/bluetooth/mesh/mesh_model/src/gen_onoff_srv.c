/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

struct bt_mesh_model_pub g_gen_onoff_pub = {
    .msg = NET_BUF_SIMPLE(2 + 3),
    .update = NULL,
};

static void _gen_onoff_prepear_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_model->user_data)->state;
    u8_t remain_byte = 0;
    
    BT_DBG("onoff cur(%d) tar(%d)", p_state->onoff[T_CUR], p_state->onoff[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_byte = get_remain_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_byte);
#endif

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);

    if(is_ack && remain_byte == 0) {
        net_buf_simple_add_u8(p_msg, p_state->onoff[T_TAR]);
    } else {
        net_buf_simple_add_u8(p_msg, p_state->onoff[T_CUR]);
#ifdef CONFIG_MESH_MODEL_TRANS
        if(remain_byte) {
            net_buf_simple_add_u8(p_msg, p_state->onoff[T_TAR]);
            net_buf_simple_add_u8(p_msg, remain_byte);
        }
#endif
    }
}

static void _gen_onoff_status(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 3 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _gen_onoff_prepear_buf(p_model, p_msg, is_ack);

    if(bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send OnOff Status");
    }
    BT_DBG("Success!!!");
}

struct net_buf_simple *p_ack_buff;

static E_MESH_ERROR_TYPE _gen_onoff_analyze(struct bt_mesh_model *p_model,
                                            u16_t src_addr, struct net_buf_simple *p_buf)
{
    u8_t onoff = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    S_ELEM_STATE *p_elem = p_model->user_data;

    if (!p_model || !p_buf) return MESH_ANALYZE_ARGS_ERROR;


    if(p_buf->len != 2 && p_buf->len != 4) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    p_elem->message_index = MM_INDEX_SRV_ONOFF;

    //get message info
    onoff = net_buf_simple_pull_u8(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);
#ifdef CONFIG_MESH_MODEL_TRANS
    if(p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    } else {
        trans = p_elem->powerup.def_trans;
        delay = 0;
    }
#endif

    if(onoff >> 1) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR onoff(0x%02x)", onoff);
        return MESH_ANALYZE_ARGS_ERROR;
    }

#ifdef CONFIG_MESH_MODEL_TRANS
    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }
#endif

    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS) {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }
    
    p_elem->state.onoff[T_TAR] = onoff;
    BT_DBG("onoff cur(%d) tar(%d)", p_elem->state.onoff[T_CUR], p_elem->state.onoff[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = k_uptime_get() + p_elem->state.delay*5;
        p_elem->state.trans_end_time = p_elem->state.trans_start_time + get_transition_time(p_elem->state.trans);
    }
    BT_DBG("trans(0x%02x) delay(0x%02x)", p_elem->state.trans, p_elem->state.delay);
#endif

    if(p_elem->state.onoff[T_CUR] != p_elem->state.onoff[T_TAR])
    {
        model_bind_operation(B_GEN_ONOFF_ID, B_OPS_END_ID, p_elem);
    } 

    return MESH_SUCCESS;
}

void gen_onoff_publication(struct bt_mesh_model *p_model)
{
    struct net_buf_simple *msg = p_model->pub->msg;
    int err;

    if(!p_model) return;

    BT_DBG("addr(0x%04x)", p_model->pub->addr);

    /*
     * If a server has a publish address, it is required to
     * publish status on a state change
     *
     * See Mesh Profile Specification 3.7.6.1.2
     *
     * Only publish if there is an assigned address
     */
    if (p_model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _gen_onoff_prepear_buf(p_model, msg, 0);
        
        err = bt_mesh_model_publish(p_model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d\n", err);
        }
        BT_DBG("Success!!!");
    }
}

static void _gen_onoff_get(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _gen_onoff_status(p_model, p_ctx, 0);
}

static void _gen_onoff_set(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = _gen_onoff_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);
    
    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        // gen_onoff_publication(p_model);
        _gen_onoff_status(p_model, p_ctx, 1);
    }
}

static void _gen_onoff_set_unack(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    BT_DBG("");
    
    E_MESH_ERROR_TYPE ret = _gen_onoff_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        // gen_onoff_publication(p_model);
    }
}

const struct bt_mesh_model_op g_gen_onoff_op[] = {
    { BT_MESH_MODEL_OP_GEN_ONOFF_GET,       0, _gen_onoff_get },
    { BT_MESH_MODEL_OP_GEN_ONOFF_SET,       2, _gen_onoff_set },
    { BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, _gen_onoff_set_unack },
    BT_MESH_MODEL_OP_END,
};


u16_t gen_onoff_bound_states_op(S_ELEM_STATE *p_elem)
{
    BT_DBG("");

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    bind_ln_actual_with_gen_onoff(p_elem, BIND_FORWARD);
#endif
}

BUILD_MODEL_STATE_BIND_HANDLER(B_GEN_ONOFF_ID)= {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    //{B_LIGHTNESS_ACTUAL_ID,         bind_ln_actual_with_gen_onoff,       BIND_FORWARD},
#endif
    {B_OPS_END_ID,           NULL,                                BIND_NULL},
};

