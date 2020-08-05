#include <api/mesh.h>
#include <mesh.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "light_hsl_srv.h"

struct bt_mesh_model_pub g_hsl_srv_pub = {
#ifndef CONFIG_ALI_SIMPLE_MODLE
    .msg = NET_BUF_SIMPLE(2 + 9 + 4),
#endif
};

#if CONFIG_MESH_MODEL_HSL_SRV   //light hsl server

static void _hsl_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack, bool target)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_model->user_data)->state;
    u8_t remain_byte = 0;
    u8_t data_offset = target ? T_TAR : T_CUR;

    BT_DBG("temp cur(0x%04x) tar(0x%04x)", p_state->temp[T_CUR], p_state->temp[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_byte = get_remain_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_byte);
#endif

    u32_t opcode = target ? BT_MESH_MODEL_OP_2(0x82, 0x7a) : BT_MESH_MODEL_OP_2(0x82, 0x78);
    //prepear buff
    bt_mesh_model_msg_init(p_msg, opcode);

    net_buf_simple_add_le16(p_msg, p_state->hsl_lightness[data_offset]);
    net_buf_simple_add_le16(p_msg, p_state->hsl_hue[data_offset]);
    net_buf_simple_add_le16(p_msg, p_state->hsl_sat[data_offset]);

    if(remain_byte)
    {
        net_buf_simple_add_u8(p_msg, remain_byte);
    }

}

static void _hsl_status(struct bt_mesh_model *p_model,
                       struct bt_mesh_msg_ctx *p_ctx, bool is_ack, bool target)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 7 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _hsl_prepare_buf(p_model, p_msg, is_ack, target);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send hsl Status");
    }
    BT_DBG("Success!!!");
}


static u8_t _hsl_analyze(struct bt_mesh_model *p_model,
                            u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t lightness = 0;
    u16_t hue = 0;
    u16_t sat = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *p_elem = NULL;

    if (!p_model || !p_buf) return MESH_ANALYZE_ARGS_ERROR;

    p_elem = p_model->user_data;

    if(p_buf->len != 7 && p_buf->len != 9) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    //get message info
    lightness = net_buf_simple_pull_le16(p_buf);
    hue = net_buf_simple_pull_le16(p_buf);
    sat = net_buf_simple_pull_le16(p_buf);
    tid = net_buf_simple_pull_u8(p_buf);
#ifdef CONFIG_MESH_MODEL_TRANS
    if(p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    } else {
#ifdef CONFIG_ALI_SIMPLE_MODLE
        trans = 0;
#else
        trans = p_elem->powerup.def_trans;
#endif
        delay = 0;
    }
    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }
#endif

    if(mesh_check_tid(src_addr, tid) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    }
    
    p_elem->state.hsl_lightness[T_TAR] = lightness;
    //p_elem->powerup.last_hsl_temp = p_elem->state.lightness;
    BT_DBG("temp cur(%04x) tar(%04x)", p_elem->state.temp[T_CUR], p_elem->state.temp[T_TAR]);

    p_elem->state.hsl_hue[T_TAR] = hue;
    p_elem->state.hsl_sat[T_TAR] = sat;
    BT_DBG("actual cur(%04x) tar(%04x)", p_elem->state.actual[T_CUR], p_elem->state.actual[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = k_uptime_get() + p_elem->state.delay * 5; // delay is in 5 millisecond steps
        p_elem->state.trans_end_time = p_elem->state.trans_start_time + get_transition_time(p_elem->state.trans);
    }
    BT_DBG("trans(0x%02x) delay(0x%02x)", p_elem->state.trans, p_elem->state.delay);
#endif

/* #ifdef CONFIG_ALI_SIMPLE_MODLE
    //only check temp when ali simaple model
    if(p_elem->state.temp[T_CUR] != p_elem->state.temp[T_TAR]) {
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        g_indication_flag |= INDICATION_FLAG_HSL;
#endif
#ifdef CONFIG_ALI_SIMPLE_MODLE
        //only bind temp when ali_simple_model is enable
        model_bind_operation(B_LIGHT_HSL_TEMP_ID, p_elem, T_TAR);
#endif
    }
#endif */

    return MESH_SUCCESS;
}

static void _hsl_get(struct bt_mesh_model *p_model,
                    struct bt_mesh_msg_ctx *p_ctx,
                    struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_status(p_model, p_ctx, 0, 0);
}

static void _hsl_tgt_get(struct bt_mesh_model *p_model,
                    struct bt_mesh_msg_ctx *p_ctx,
                    struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_status(p_model, p_ctx, 0, 1);
}

static void _hsl_set(struct bt_mesh_model *p_model,
                    struct bt_mesh_msg_ctx *p_ctx,
                    struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = _hsl_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _hsl_status(p_model, p_ctx, 1, 0);
        if(ret == MESH_SUCCESS) {
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        }
    }
}

static void _hsl_set_unack(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    BT_DBG("");
    
    E_MESH_ERROR_TYPE ret = _hsl_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}

static void _hsl_range_status(struct bt_mesh_model *p_model, struct bt_mesh_msg_ctx *p_ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 9 + 4);
    S_ELEM_STATE *p_elem = p_model->user_data;


    BT_DBG("range_status(0x%02x) min_hue(0x%04x) max_hue(0x%04x) min_sat(0x%04x) max_sat(0x%04x)",
            p_elem->powerup.range_status, p_elem->powerup.min_hue,
            p_elem->powerup.max_hue, p_elem->powerup.min_sat, p_elem->powerup.max_sat);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x7e));
    net_buf_simple_add_u8(p_msg, p_elem->powerup.range_status);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.min_hue);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.max_hue);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.min_sat);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.max_sat);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send temp range Status");
    }
    BT_DBG("Success!!!");
}

static void _hsl_defatult_status(struct bt_mesh_model *p_model, struct bt_mesh_msg_ctx *p_ctx)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 6 + 4);
    S_ELEM_STATE *p_elem = p_model->user_data;


    BT_DBG("default_lightness(0x%04x) default_hue(0x%04x) default_sat(0x%04x)",
            p_elem->powerup.default_lightness, p_elem->powerup.default_hue,
            p_elem->powerup.default_sat);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x68));
    net_buf_simple_add_le16(p_msg, p_elem->powerup.default_lightness);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.default_hue);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.default_sat);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send default Status");
    }
    BT_DBG("Success!!!");
}

static void _hsl_default_get(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx,
                            struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_defatult_status(p_model, p_ctx);
}

static void _hsl_range_get(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_range_status(p_model, p_ctx);
}

const struct bt_mesh_model_op g_hsl_srv_op[HSL_OPC_NUM] = {
        { BT_MESH_MODEL_OP_2(0x82, 0x6d), 0, _hsl_get },
        { BT_MESH_MODEL_OP_2(0x82, 0x76), 7, _hsl_set },
        { BT_MESH_MODEL_OP_2(0x82, 0x77), 7, _hsl_set_unack },
        { BT_MESH_MODEL_OP_2(0x82, 0x79), 0, _hsl_tgt_get },
#ifndef CONFIG_ALI_SIMPLE_MODLE
        { BT_MESH_MODEL_OP_2(0x82, 0x7d), 0, _hsl_range_get },
        { BT_MESH_MODEL_OP_2(0x82, 0x7b), 0, _hsl_default_get },
#endif
        BT_MESH_MODEL_OP_END,
};
#endif //CONFIG_MESH_MODEL_HSL_SRV






#if CONFIG_MESH_MODEL_HSL_SETUP_SRV   //light hsl setup server

static E_MESH_ERROR_TYPE _hsl_default_analyze(struct bt_mesh_model *p_model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t lightness = 0;
    u16_t hue = 0;
    u16_t sat = 0;
	S_ELEM_STATE *p_elem = p_model->user_data;

    if(p_buf->len != 6) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    lightness = net_buf_simple_pull_le16(p_buf);
    hue = net_buf_simple_pull_le16(p_buf);
    sat = net_buf_simple_pull_le16(p_buf);

    if(hue < p_elem->powerup.min_hue || hue > p_elem->powerup.max_hue) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR hue(0x%04x) min(0x%04x) max(0x%04x)",
                hue, p_elem->powerup.min_hue, p_elem->powerup.max_hue);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    if(hue < p_elem->powerup.min_sat || hue > p_elem->powerup.max_sat) {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR hue(0x%04x) min(0x%04x) max(0x%04x)",
                hue, p_elem->powerup.min_sat, p_elem->powerup.max_sat);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    p_elem->powerup.default_lightness = lightness;
    p_elem->powerup.default_hue = hue;
    p_elem->powerup.default_sat = sat;

    BT_DBG("lightness(0x%04x) hue(0x%04x) sat(0x%04x)", p_elem->powerup.default_lightness,
            p_elem->powerup.default_hue, p_elem->powerup.default_sat);

    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, p_elem);

    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, p_elem);
        }
        else {
            genie_event(GENIE_EVT_SDK_TRANS_START, p_elem);
        }
    }
    else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, p_elem);
    }

    return MESH_SUCCESS;
}

static void _hsl_default_set(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx,
                            struct net_buf_simple *p_buf)
{
    BT_DBG("");

    if(_hsl_default_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        _hsl_defatult_status(p_model, p_ctx);
    }
}

static void _hsl_default_set_unack(struct bt_mesh_model *p_model,
                                  struct bt_mesh_msg_ctx *p_ctx,
                                  struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_default_analyze(p_model, p_ctx->addr, p_buf);
}

static E_MESH_ERROR_TYPE _hsl_range_analyze(struct bt_mesh_model *p_model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t min_hue = 0;
    u16_t max_hue = 0;
    u16_t min_sat = 0;
    u16_t max_sat = 0;
    S_ELEM_STATE *p_elem = p_model->user_data;

    if(p_buf->len != 8) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    min_hue = net_buf_simple_pull_le16(p_buf);
    max_hue = net_buf_simple_pull_le16(p_buf);
    min_sat = net_buf_simple_pull_le16(p_buf);
    max_sat = net_buf_simple_pull_le16(p_buf);

    if(min_hue > max_hue || min_sat > max_sat)
    {
        BT_ERR("MESH_ANALYZE_ARGS_ERROR min_hue(0x%04x) max_hue(0x%04x) min_sat(0x%04x) max_sat(0x%04x)"
                        , min_hue, max_hue, min_sat, max_sat);
        return MESH_ANALYZE_ARGS_ERROR;
    }

    p_elem->powerup.min_hue = min_hue;
    p_elem->powerup.max_hue = max_hue;
    p_elem->powerup.min_sat = min_sat;
    p_elem->powerup.max_sat = max_sat;

    BT_DBG("min_hue(0x%04x) max_hue(0x%04x) min_sat(0x%04x) max_sat(0x%04x)",
            p_elem->powerup.min_hue, p_elem->powerup.max_hue, p_elem->powerup.min_sat, p_elem->powerup.max_sat);

    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, p_elem);

    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, p_elem);
        }
        else {
            genie_event(GENIE_EVT_SDK_TRANS_START, p_elem);
        }
    }
    else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, p_elem);
    }

    return MESH_SUCCESS;
}

static void _hsl_range_set(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    BT_DBG("");

    if(_hsl_range_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        _hsl_range_status(p_model, p_ctx);
    }
}

static void _hsl_range_set_unack(struct bt_mesh_model *p_model,
                                     struct bt_mesh_msg_ctx *p_ctx,
                                     struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_range_analyze(p_model, p_ctx->addr, p_buf);
}


const struct bt_mesh_model_op g_hsl_setup_srv_op[HSL_SETUP_OPC_NUM] = {
        { BT_MESH_MODEL_OP_2(0x82, 0x7f), 6, _hsl_default_set },
        { BT_MESH_MODEL_OP_2(0x82, 0x80), 6, _hsl_default_set_unack },
        { BT_MESH_MODEL_OP_2(0x82, 0x81), 4, _hsl_range_set },
        { BT_MESH_MODEL_OP_2(0x82, 0x82), 4, _hsl_range_set_unack },
        BT_MESH_MODEL_OP_END,
};
#endif //CONFIG_MESH_MODEL_HSL_SETUP_SRV






#if CONFIG_MESH_MODEL_HSL_HUE_SRV   //light hsl hue server

static void _hsl_hue_prepear_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    u8_t remain_byte = get_remain_byte(is_ack);
    S_ELEM_STATE *p_elem = p_model->user_data;

    BT_DBG("hue_cur(0x%04x) hue_tar(0x%04x) remain(0x%02x)",
            p_elem->state.hsl_hue[T_CUR], p_elem->state.hsl_hue[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x71));

    net_buf_simple_add_le16(p_msg, p_elem->state.hsl_hue[T_CUR]);

    if(remain_byte){
        net_buf_simple_add_le16(p_msg, p_elem->state.hsl_hue[T_TAR]);
        net_buf_simple_add_u8(p_msg, remain_byte);
    }
}

static void _hsl_hue_status(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _hsl_hue_prepear_buf(p_model, p_msg, is_ack);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send hue Status");
    }
    BT_DBG("Success!!!");
}


static u8_t _hsl_hue_analyze(struct bt_mesh_model *p_model,
                                u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t hue = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *p_elem = p_model->user_data;

    if(p_buf->len != 3 && p_buf->len != 5) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    hue = net_buf_simple_pull_le16(p_buf);

    if(hue < p_elem->powerup.min_hue){
        hue = p_elem->powerup.min_hue;
    }
    if(hue > p_elem->powerup.max_hue) {
        hue = p_elem->powerup.max_hue;
    }

    tid = net_buf_simple_pull_u8(p_buf);
/*     if(mesh_check_TID(src_addr, tid) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    } */

    if(p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    }
    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    p_elem->state.hsl_hue[T_TAR] = hue;

    //mesh_state_bound(LIGHT_HSL_HUE, T_TAR);

    p_elem->state.trans = trans?trans:p_elem->powerup.def_trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = aos_now_ms() + p_elem->state.delay*5;
    }

    BT_DBG("hue(0x%04x) trans(0x%02x) delay(0x%02x)",
            p_elem->state.hsl_hue[T_TAR], p_elem->state.trans, p_elem->state.delay);

    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (void *)p_elem);

    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, (void *)p_elem);
        } else {
            genie_event(GENIE_EVT_SDK_TRANS_START, (void *)p_elem);
        }
    } else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, (void *)p_elem);
    }

    return MESH_SUCCESS;
}


static void _hsl_hue_get(struct bt_mesh_model *p_model,
                         struct bt_mesh_msg_ctx *p_ctx,
                         struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_hue_status(p_model, p_ctx, 0);
}

static void _hsl_hue_set(struct bt_mesh_model *p_model,
                         struct bt_mesh_msg_ctx *p_ctx,
                         struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    if(_hsl_hue_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        _hsl_hue_status(p_model, p_ctx, 1);
    }
}

static void _hsl_hue_set_unack(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    _hsl_hue_analyze(p_model, p_ctx->addr, p_buf);
}

const struct bt_mesh_model_op g_hsl_hue_srv_op[HSL_HUE_OPC_NUM] = {
        { BT_MESH_MODEL_OP_2(0x82, 0x6e), 0, _hsl_hue_get },
        { BT_MESH_MODEL_OP_2(0x82, 0x6f), 6, _hsl_hue_set },
        { BT_MESH_MODEL_OP_2(0x82, 0x70), 6, _hsl_hue_set_unack },
        BT_MESH_MODEL_OP_END,
};
#endif //CONFIG_MESH_MODEL_HSL_HUE_SRV




#if CONFIG_MESH_MODEL_HSL_SAT_SRV   //light hsl sat server

static void _hsl_sat_prepear_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    u8_t remain_byte = get_remain_byte(is_ack);
    S_ELEM_STATE *p_elem = p_model->user_data;

    BT_DBG("sat_cur(0x%04x) sat_tar(0x%04x) remain(0x%02x)",
            p_elem->state.hsl_sat[T_CUR], p_elem->state.hsl_sat[T_TAR], remain_byte);

    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_2(0x82, 0x75));

    net_buf_simple_add_le16(p_msg, p_elem->state.hsl_sat[T_CUR]);

    if(remain_byte){
        net_buf_simple_add_le16(p_msg, p_elem->state.hsl_sat[T_TAR]);
        net_buf_simple_add_u8(p_msg, remain_byte);
    }
}

static void _hsl_sat_status(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 5 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _hsl_sat_prepear_buf(p_model, p_msg, is_ack);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send hue Status");
    }
    BT_DBG("Success!!!");
}


static u8_t _hsl_sat_analyze(struct bt_mesh_model *p_model, u16_t src_addr, struct net_buf_simple *p_buf)
{
    u16_t sat = 0;
    u8_t tid = 0;
    u8_t trans = 0;
    u8_t delay = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *p_elem = p_model->user_data;

    if(p_buf->len != 3 && p_buf->len != 5) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    sat = net_buf_simple_pull_le16(p_buf);

    if(sat < p_elem->powerup.min_sat){
        sat = p_elem->powerup.min_sat;
    }
    if(sat > p_elem->powerup.max_sat) {
        sat = p_elem->powerup.max_sat;
    }

    tid = net_buf_simple_pull_u8(p_buf);
/*     if(mesh_check_TID(src_addr, tid) != MESH_SUCCESS)
    {
        BT_ERR("MESH_TID_REPEAT src_addr(0x%04x) tid(0x%02x)", src_addr, tid);
        return MESH_TID_REPEAT;
    } */

    if(p_buf->len) {
        trans = net_buf_simple_pull_u8(p_buf);
        delay = net_buf_simple_pull_u8(p_buf);
    }
    if((trans & 0x3F) == 0x3F) {
        BT_ERR("MESH_SET_TRANSTION_ERROR");
        return MESH_SET_TRANSTION_ERROR;
    }

    p_elem->state.hsl_sat[T_TAR] = sat;

    //mesh_state_bound(LIGHT_HSL_HUE, T_TAR);

    p_elem->state.trans = trans?trans:p_elem->powerup.def_trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = aos_now_ms() + p_elem->state.delay*5;
    }

    BT_DBG("sat(0x%04x) trans(0x%02x) delay(0x%02x)",
            p_elem->state.hsl_sat[T_TAR], p_elem->state.trans, p_elem->state.delay);

    genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (void *)p_elem);

    if(p_elem->state.trans || p_elem->state.delay) {
        if(p_elem->state.delay) {
            genie_event(GENIE_EVT_SDK_DELAY_START, (void *)p_elem);
        } else {
            genie_event(GENIE_EVT_SDK_TRANS_START, (void *)p_elem);
        }
    } else {
        genie_event(GENIE_EVT_SDK_ACTION_DONE, (void *)p_elem);
    }

    return MESH_SUCCESS;
}



static void _hsl_sat_get(struct bt_mesh_model *p_model,
                         struct bt_mesh_msg_ctx *p_ctx,
                         struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _hsl_sat_status(p_model, p_ctx, 0);
}

static void _hsl_sat_set(struct bt_mesh_model *p_model,
                         struct bt_mesh_msg_ctx *p_ctx,
                         struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    if(_hsl_sat_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        _hsl_sat_status(p_model, p_ctx, 1);
    }
}

static void _hsl_sat_set_unack(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    _hsl_sat_analyze(p_model, p_ctx->addr, p_buf);
}

const struct bt_mesh_model_op g_hsl_sat_srv_op[HSL_SAT_OPC_NUM] = {
        { BT_MESH_MODEL_OP_2(0x82, 0x72), 0, _hsl_sat_get },
        { BT_MESH_MODEL_OP_2(0x82, 0x73), 6, _hsl_sat_set },
        { BT_MESH_MODEL_OP_2(0x82, 0x74), 6, _hsl_sat_set_unack },
        BT_MESH_MODEL_OP_END,
};
#endif //CONFIG_MESH_MODEL_HSL_SAT_SRV