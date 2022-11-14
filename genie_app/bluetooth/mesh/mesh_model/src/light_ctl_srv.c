/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include <mesh.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "genie_app.h"

struct bt_mesh_model_pub g_ctl_srv_pub = {
    .msg = NET_BUF_SIMPLE(2 + 9),
};

#ifdef CONFIG_MESH_MODEL_CTL_SRV   //light ctl server
static void _ctl_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_model->user_data)->state;
    u8_t remain_byte = 0;

    BT_DBG("temp cur(0x%04x) tar(0x%04x)", p_state->ctl_temp[T_CUR], p_state->ctl_temp[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_byte = get_remain_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_byte);
#endif

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_CTL_STATUS);

    if(is_ack && remain_byte == 0) {
        net_buf_simple_add_le16(p_msg, p_state->ctl_lightness[T_TAR]);
        net_buf_simple_add_le16(p_msg, p_state->ctl_temp[T_TAR]);
    } else {
        net_buf_simple_add_le16(p_msg, p_state->ctl_lightness[T_CUR]);
        net_buf_simple_add_le16(p_msg, p_state->ctl_temp[T_CUR]);
    
#ifdef CONFIG_MESH_MODEL_TRANS
        if(remain_byte){
            net_buf_simple_add_le16(p_msg, p_state->ctl_lightness[T_TAR]);
            net_buf_simple_add_le16(p_msg, p_state->ctl_temp[T_TAR]);
            net_buf_simple_add_u8(p_msg, remain_byte);
        }
#endif
    }
}

static void _ctl_status(struct bt_mesh_model *p_model,
                       struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 9 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _ctl_prepare_buf(p_model, p_msg, is_ack);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send ctl Status");
    }
    BT_DBG("Success!!!");
}

static u8_t _ctl_analyze(struct bt_mesh_model *p_model,
                            u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t lightness = 0;
    u16_t temp = 0;
    u16_t uv = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    S_ELEM_STATE *p_elem = NULL;

    if (!p_model || !p_buf) return MESH_ANALYZE_ARGS_ERROR;

    p_elem = p_model->user_data;

    if(p_buf->len < 7 || p_buf->len > 9) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    set_light_board_type(LIGHT_TYPE_CTL);
    p_elem->message_index = MM_INDEX_SRV_CTL;

    //get message info
    lightness = net_buf_simple_pull_le16(p_buf);
    temp = net_buf_simple_pull_le16(p_buf);
    uv = net_buf_simple_pull_le16(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);
#ifdef CONFIG_MESH_MODEL_TRANS
    if(p_buf->len) 
    {
        trans = net_buf_simple_pull_u8(p_buf);
        if(p_buf->len)
        {
            delay = net_buf_simple_pull_u8(p_buf);
        }
    } 
    else 
    {
        trans = p_elem->powerup.def_trans;
        delay = 0;
    }

    if((trans & DEFAULT_TRANS_NUM_STEPS_MASK) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }
#endif

    //check temp
    if(temp < CTL_TEMP_MIN || temp > CTL_TEMP_MAX) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR temp(0x%04x)", temp);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }
    
    p_elem->state.ctl_temp[T_TAR] = temp;
    BT_DBG("ctl_temp cur(%04x) tar(%04x)", p_elem->state.ctl_temp[T_CUR], p_elem->state.ctl_temp[T_TAR]);

    p_elem->state.ctl_lightness[T_TAR] = lightness;
    p_elem->state.ctl_UV[T_TAR] = uv;
    BT_DBG("ctl_lightness cur(%04x) tar(%04x)", p_elem->state.ctl_lightness[T_CUR], p_elem->state.ctl_lightness[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = k_uptime_get() + p_elem->state.delay * 5; // delay is in 5 millisecond steps
        p_elem->state.trans_end_time = p_elem->state.trans_start_time + get_transition_time(p_elem->state.trans);
    }
    BT_DBG("trans(0x%02x) delay(0x%02x)", p_elem->state.trans, p_elem->state.delay);
#endif

    if(p_elem->state.ctl_temp[T_CUR] != p_elem->state.ctl_temp[T_TAR])
    {
        model_bind_operation(B_LIGHT_CTL_TEMP_ID, B_OPS_END_ID, p_elem);
    }

    if(p_elem->state.ctl_lightness[T_CUR] != p_elem->state.ctl_lightness[T_TAR])
    {
        model_bind_operation(B_LIGHT_CTL_LN_ID, B_OPS_END_ID, p_elem);
    }

    return MESH_SUCCESS;
}

void ctl_publication(struct bt_mesh_model *p_model)
{
    struct net_buf_simple *p_msg = NULL;
    int err;

    if(!p_model) return;
    p_msg = p_model->pub->msg;
    BT_DBG("addr(0x%04x)", p_model->pub->addr);

    if (p_model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        _ctl_prepare_buf(p_model, p_msg, 0);

        err = bt_mesh_model_publish(p_model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d\n", err);
        }
        BT_DBG("Success!!!");
    }

}

static void _ctl_get(struct bt_mesh_model *p_model,
                    struct bt_mesh_msg_ctx *p_ctx,
                    struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _ctl_status(p_model, p_ctx, 0);
}

static void _ctl_set(struct bt_mesh_model *p_model,
                    struct bt_mesh_msg_ctx *p_ctx,
                    struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = _ctl_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _ctl_status(p_model, p_ctx, 1);
    }
}

static void _ctl_set_unack(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    BT_DBG("");
    
    E_MESH_ERROR_TYPE ret = _ctl_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}


static void _ctl_temp_range_status(struct bt_mesh_model *p_model, struct bt_mesh_msg_ctx *p_ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);
    S_ELEM_STATE *p_elem = p_model->user_data;


    BT_DBG("range_status(0x%02x) ctl_temp_range_min(0x%04x) ctl_temp_range_max(0x%04x)",
            p_elem->powerup.range_status, p_elem->powerup.ctl_temp_range_min,
            p_elem->powerup.ctl_temp_range_max);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS);
    net_buf_simple_add_u8(p_msg, p_elem->powerup.range_status);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.ctl_temp_range_min);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.ctl_temp_range_max);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send temp range Status");
    }
    BT_DBG("Success!!!");
}

static void _ctl_temp_range_get(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _ctl_temp_range_status(p_model, p_ctx);
}

static void _ctl_defatult_status(struct bt_mesh_model *p_model, struct bt_mesh_msg_ctx *p_ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 6 + 4);
    S_ELEM_STATE *p_elem = p_model->user_data;


    BT_DBG("actual(0x%04x) temp(0x%04x) uv(0x%04x)",
            p_elem->powerup.default_light_ln, p_elem->powerup.ctl_temp_def,
            p_elem->powerup.ctl_uv_def);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_CTL_DEFAULT_STATUS);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.default_light_ln);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.ctl_temp_def);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.ctl_uv_def);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send default Status");
    }
    BT_DBG("Success!!!");
}

static void _ctl_default_get(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx,
                            struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _ctl_defatult_status(p_model, p_ctx);
}
#endif

const struct bt_mesh_model_op g_ctl_srv_op[CTL_OPC_NUM] = {
        { BT_MESH_MODEL_LIGHT_CTL_GET,              0, _ctl_get },
        { BT_MESH_MODEL_LIGHT_CTL_SET,              7, _ctl_set },
        { BT_MESH_MODEL_LIGHT_CTL_SET_UNACK,        7, _ctl_set_unack },
        { BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_GET,   0, _ctl_temp_range_get },
        { BT_MESH_MODEL_LIGHT_CTL_DEFAULT_GET,      0, _ctl_default_get },
        BT_MESH_MODEL_OP_END,
};



#if CONFIG_MESH_MODEL_CTL_SETUP_SRV   //light ctl setup server
static E_MESH_ERROR_TYPE _ctl_default_analyze(struct bt_mesh_model *p_model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t actual = 0;
    u16_t temp = 0;
    u16_t uv = 0;
	S_ELEM_STATE *p_elem = p_model->user_data;

    if(p_buf->len != 6) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    actual = net_buf_simple_pull_le16(p_buf);
    temp = net_buf_simple_pull_le16(p_buf);
    uv = net_buf_simple_pull_le16(p_buf);

    if(temp < p_elem->powerup.ctl_temp_range_min || temp > p_elem->powerup.ctl_temp_range_max) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR tar(0x%04x) min(0x%04x) max(0x%04x)",
                temp, p_elem->powerup.ctl_temp_range_min, p_elem->powerup.ctl_temp_range_max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    p_elem->powerup.default_light_ln = actual;
    p_elem->powerup.ctl_temp_def = temp;
    p_elem->powerup.ctl_uv_def = uv;

    BT_DBG("actual(0x%04x) temp(0x%04x) uv(0x%04x)", p_elem->powerup.default_light_ln,
            p_elem->powerup.ctl_temp_def, p_elem->powerup.ctl_uv_def);

    return MESH_SUCCESS;
}

static void _ctl_default_set(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx,
                            struct net_buf_simple *p_buf)
{
    BT_DBG("");

    if(_ctl_default_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _ctl_defatult_status(p_model, p_ctx);
    }
}

static void _ctl_default_set_unack(struct bt_mesh_model *p_model,
                                  struct bt_mesh_msg_ctx *p_ctx,
                                  struct net_buf_simple *p_buf)
{
    BT_DBG("");

    E_MESH_ERROR_TYPE ret = _ctl_default_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}

static E_MESH_ERROR_TYPE _ctl_temp_range_analyze(struct bt_mesh_model *p_model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t min = 0;
    u16_t max = 0;
    S_ELEM_STATE *p_elem = p_model->user_data;

    if(p_buf->len != 4) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    min = net_buf_simple_pull_le16(p_buf);
    max = net_buf_simple_pull_le16(p_buf);

    if(!min || !max || min > max || min < CTL_TEMP_MIN || max > CTL_TEMP_MAX)
    {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR min(0x%04x) max(0x%04x)", min, max);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    p_elem->powerup.ctl_temp_range_min = min;
    p_elem->powerup.ctl_temp_range_max = max;

    BT_DBG("ctl_temp_range_min(0x%04x) ctl_temp_range_max(0x%04x)",
            p_elem->powerup.ctl_temp_range_min, p_elem->powerup.ctl_temp_range_max);

    return MESH_SUCCESS;
}

static void _ctl_temp_range_set(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    BT_DBG("");

    if(_ctl_temp_range_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _ctl_temp_range_status(p_model, p_ctx);
    }
}

static void _ctl_temp_range_set_unack(struct bt_mesh_model *p_model,
                                     struct bt_mesh_msg_ctx *p_ctx,
                                     struct net_buf_simple *p_buf)
{
    BT_DBG("");

    E_MESH_ERROR_TYPE ret = _ctl_temp_range_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}

const struct bt_mesh_model_op g_ctl_setup_srv_op[CTL_SETUP_OPC_NUM] = {
        { BT_MESH_MODEL_LIGHT_CTL_DEFAULT_SET,              6, _ctl_default_set },
        { BT_MESH_MODEL_LIGHT_CTL_DEFAULT_SET_UNACK,        6, _ctl_default_set_unack },
        { BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_SET,           4, _ctl_temp_range_set },
        { BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_SET_UNACK,     4, _ctl_temp_range_set_unack },
        BT_MESH_MODEL_OP_END,
};
#endif

#if CONFIG_MESH_MODEL_CTL_TEMPERATURE_SRV   //ctl temperatur server
static void _ctl_temp_prepear_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_model->user_data)->state;
#if CONFIG_MESH_MODEL_TRANS
    u8_t remain_byte = get_remain_byte(is_ack);
#else
    u8_t remain_byte = 0;
#endif //#if CONFIG_MESH_MODEL_TRANS
    BT_DBG("cur_temp(0x%04x) tar_temp(0x%04x) uv(0x%04x) uv(0x%04x) remain(0x%02x)",
            p_state->ctl_temp[T_CUR], p_state->ctl_temp[T_TAR],
            p_state->ctl_UV[T_CUR], p_state->ctl_UV[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS);

    net_buf_simple_add_le16(p_msg, p_state->ctl_temp[T_CUR]);
    net_buf_simple_add_le16(p_msg, p_state->ctl_UV[T_CUR]);

    if(remain_byte){
        net_buf_simple_add_le16(p_msg, p_state->ctl_temp[T_TAR]);
        net_buf_simple_add_le16(p_msg, p_state->ctl_UV[T_TAR]);
        net_buf_simple_add_u8(p_msg, remain_byte);
    }
}

static void _ctl_temp_status(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 9 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _ctl_temp_prepear_buf(p_model, p_msg, is_ack);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send ctl Status");
    }
    BT_DBG("Success!!!");
}

static u8_t _ctl_temp_analyze(struct bt_mesh_model *p_model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t temp = 0;
    u16_t uv = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    S_ELEM_STATE *p_elem = NULL;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;

    p_elem = p_model->user_data;

    if(p_buf->len != 5 && p_buf->len != 7) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    set_light_board_type(LIGHT_TYPE_CTL);
    p_elem->message_index = MM_INDEX_SRV_CTL;

    temp = net_buf_simple_pull_le16(p_buf);
    if(temp < CTL_TEMP_MIN || temp > CTL_TEMP_MAX) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR temp(0x%04x)", temp);
        return MESH_ANALYZE_ARGS_ERROR;
    }
    if(temp < p_elem->powerup.ctl_temp_range_min){
        temp = p_elem->powerup.ctl_temp_range_min;
    }
    if(temp > p_elem->powerup.ctl_temp_range_max) {
        temp = p_elem->powerup.ctl_temp_range_max;
    }

    uv = net_buf_simple_pull_le16(p_buf);

    tid = net_buf_simple_pull_u8(p_buf);
    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }

    if(p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    }
    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    p_elem->state.ctl_temp[T_TAR] = temp;
    p_elem->state.ctl_UV[T_TAR] = uv;
#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans?trans:p_elem->powerup.def_trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = aos_now_ms() + p_elem->state.delay*5;
    }

    BT_DBG("temp(0x%04x) uv(0x%04x) trans(0x%02x) delay(0x%02x)",
            p_elem->state.ctl_temp[T_TAR], p_elem->state.ctl_UV[T_TAR],
            p_elem->state.trans, p_elem->state.delay);
#endif //CONFIG_MESH_MODEL_TRANS
    if(p_elem->state.ctl_temp[T_CUR] != p_elem->state.ctl_temp[T_TAR])
    {
        model_bind_operation(B_LIGHT_CTL_TEMP_ID, B_OPS_END_ID, p_elem);
    } 

    return MESH_SUCCESS;
}

static void _ctl_temp_gpio(u16_t temp)
{
    BT_DBG("0x%04x", temp);
}

static void _ctl_temp_done(struct bt_mesh_model *p_model)
{
    S_ELEM_STATE *p_elem = NULL;

    p_elem = p_model->user_data;

    BT_DBG("");
    p_elem->state.ctl_temp[T_CUR] = p_elem->state.ctl_temp[T_TAR];
    p_elem->state.ctl_UV[T_CUR] = p_elem->state.ctl_UV[T_TAR];
    //////mesh_state_bound(LIGHT_CTL_TEMP, T_CUR);
    ctl_gpio(p_elem->state.ctl_temp[T_CUR]);
}

static void _ctl_temp_transition(struct bt_mesh_model *p_model)
{
    S_ELEM_STATE *p_elem = NULL;

    p_elem = p_model->user_data;
#if CONFIG_MESH_MODEL_TRANS
    BT_DBG("trans %d", get_transition_time(p_elem->state.trans));
    aos_msleep(get_transition_time(p_elem->state.trans));
    BT_DBG("trans end");

    p_elem->state.trans = 0;
    p_elem->state.trans_start_time = 0;
#endif //CONFIG_MESH_MODEL_TRANS
    _ctl_temp_done(p_model);
    //mesh_publication(p_model->elem, MESH_PUB_CTL_TEMP);
}


static void _ctl_temp_delay(struct bt_mesh_model *p_model)
{
    S_ELEM_STATE *p_elem = NULL;

    p_elem = p_model->user_data;
#if CONFIG_MESH_MODEL_TRANS
    BT_DBG("delay %d", p_elem->state.delay*5);
    aos_msleep(p_elem->state.delay*5);

    BT_DBG("delay end");

    p_elem->state.delay = 0;

    if(p_elem->state.trans == 0) {
        _ctl_temp_done(p_model);
       // mesh_publication(p_model->elem, MESH_PUB_CTL_TEMP);
    } else {
        aos_schedule_call(_ctl_temp_transition, p_model);
    }
#endif // CONFIG_MESH_MODEL_TRANS
}

static bool _ctl_temp_action(struct bt_mesh_model *p_model)
{
    S_ELEM_STATE *p_elem = NULL;

    p_elem = p_model->user_data;
#if CONFIG_MESH_MODEL_TRANS
    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            aos_schedule_call(_ctl_temp_delay, p_model);
        }
        else {
            aos_schedule_call(_ctl_temp_transition, p_model);
        }
        return 0;
    }
    else {
        _ctl_temp_done(p_model);
        return 1;
    }
#endif //CONFIG_MESH_MODEL_TRANS
}

void ctl_temp_publication(struct bt_mesh_model *p_model)
{
    struct net_buf_simple *p_msg = p_model->pub->msg;
    int err;

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
        _ctl_temp_prepear_buf(p_model, p_msg, 0);

        err = bt_mesh_model_publish(p_model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d\n", err);
        }
        BT_DBG("Success!!!");
    }
}

static void _ctl_temp_get(struct bt_mesh_model *p_model,
                         struct bt_mesh_msg_ctx *p_ctx,
                         struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _ctl_temp_status(p_model, p_ctx, 0);
}

static void _ctl_temp_set(struct bt_mesh_model *p_model,
                         struct bt_mesh_msg_ctx *p_ctx,
                         struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    if(_ctl_temp_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _ctl_temp_status(p_model, p_ctx, 1);
    }
}

static void _ctl_temp_set_unack(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    if(_ctl_temp_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}

const struct bt_mesh_model_op g_ctl_temp_srv_op[CTL_TEMP_OPC_NUM] = {
        { BT_MESH_MODEL_LIGHT_CTL_TEMP_GET,         0, _ctl_temp_get },
        { BT_MESH_MODEL_LIGHT_CTL_TEMP_SET,         5, _ctl_temp_set },
        { BT_MESH_MODEL_LIGHT_CTL_TEMP_SET_UNACK,   0, _ctl_temp_set_unack },
        BT_MESH_MODEL_OP_END,
};
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
void bind_ctl_temp_with_gen_level(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;
    u16_t T_MIN = p_elem->powerup.ctl_temp_range_min;
    u16_t T_MAX = p_elem->powerup.ctl_temp_range_max;

    BT_DBG("");

    //FORWARD function performed whenever Generic Level state changes
    if(direct == BIND_FORWARD)
    {
        p_state->ctl_temp[T_TAR] = T_MIN + (p_state->gen_level[T_TAR] + 32768) * (T_MAX - T_MIN) / 65535;
    }
    //REVERSE function performed whenever Light CTL Temperature state changes
    else if(direct == BIND_REVERSE)
    {
        p_state->gen_level[T_TAR] = (p_state->ctl_temp[T_TAR] - T_MIN) * 65535 / (T_MAX - T_MIN) - 32768;
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
//performed during power up sequence
void bind_ctl_temp_with_gen_onpowerup(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    if(direct == BIND_FORWARD)
    {
        switch(p_elem->powerup.onpowerup)
        {
            case ONPOWERUP_OFF:
            case ONPOWERUP_DEF:
                p_state->ctl_temp[T_TAR] = p_elem->powerup.ctl_temp_def;
                break;
            case ONPOWERUP_RES:
                p_state->ctl_temp[T_TAR] = p_elem->powerup.ctl_temp_last;
                break;
        }
    }
}

//performed during power up sequence
void bind_ctl_delta_uv_with_gen_onpowerup(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    if(direct == BIND_FORWARD)
    {
        switch(p_elem->powerup.onpowerup)
        {
            case ONPOWERUP_OFF:
            case ONPOWERUP_DEF:
                p_state->ctl_UV[T_TAR] = p_elem->powerup.ctl_uv_def;
                break;
            case ONPOWERUP_RES:
                p_state->ctl_UV[T_TAR] = p_elem->powerup.ctl_uv_last;
                break;
        }
    }
}
#endif

void bind_ctl_temp_with_ctl_temp_range(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Light CTL Temperature state changes
    if(direct == BIND_REVERSE)
    {
        if(p_state->ctl_temp[T_TAR] < p_elem->powerup.ctl_temp_range_min)
        {
            p_state->ctl_temp[T_TAR] = p_elem->powerup.ctl_temp_range_min;
        }
        else if(p_state->ctl_temp[T_TAR] > p_elem->powerup.ctl_temp_range_max)
        {
            p_state->ctl_temp[T_TAR] > p_elem->powerup.ctl_temp_range_max;
        }
    }

}

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
void bind_ctl_lightness_with_ln_actual(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Light Lightness Actual state changes
    if(direct == BIND_FORWARD)
    {
        p_state->ctl_lightness[T_TAR] = p_state->light_ln_actual[T_TAR];
    }
    //REVERSE function performed whenever Light CTL Temperature state changes
    else if(direct == BIND_REVERSE)
    {
        p_state->light_ln_actual[T_TAR] = p_state->ctl_lightness[T_TAR];
    }
}
#endif

BUILD_MODEL_STATE_BIND_HANDLER(B_LIGHT_CTL_TEMP_ID)= {
#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    {B_GEN_LEVEL_ID,                bind_ctl_temp_with_gen_level,        BIND_REVERSE},
#endif
    {B_LIGHT_CTL_TEMP_RANGE_ID,     bind_ctl_temp_with_ctl_temp_range,   BIND_REVERSE},
    {B_OPS_END_ID,                  NULL,                                BIND_NULL},
};

BUILD_MODEL_STATE_BIND_HANDLER(B_LIGHT_CTL_LN_ID)= {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    {B_LIGHTNESS_ACTUAL_ID,         bind_ctl_lightness_with_ln_actual,   BIND_REVERSE},
#endif
    {B_OPS_END_ID,                  NULL,                                BIND_NULL},
};