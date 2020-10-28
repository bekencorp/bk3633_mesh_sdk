#include <api/mesh.h>
#include <mesh.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"
#include "genie_app.h"

struct bt_mesh_model_pub g_hsl_srv_pub = {
    .msg = NET_BUF_SIMPLE(2 + 9),
};

#ifdef CONFIG_MESH_MODEL_HSL_SRV   //light hsl server

static void _hsl_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack, bool target)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_model->user_data)->state;
    u8_t remain_byte = 0;
    u8_t data_offset = target ? T_TAR : T_CUR;

    BT_DBG("temp cur(0x%04x) tar(0x%04x)", p_state->ctl_temp[T_CUR], p_state->ctl_temp[T_TAR]);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_byte = get_remain_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_byte);
#endif

    u32_t opcode = target ? BT_MESH_MODEL_LIGHT_HSL_TARGET_STATUS : BT_MESH_MODEL_LIGHT_HSL_STATUS;
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
    set_light_board_type(LIGHT_TYPE_HSL);
    p_elem->message_index = MM_INDEX_SRV_HSL;

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
        trans = p_elem->powerup.def_trans;
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
    p_elem->state.hsl_hue[T_TAR] = hue;
    p_elem->state.hsl_sat[T_TAR] = sat;

#ifdef CONFIG_MESH_MODEL_TRANS
    p_elem->state.trans = trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = k_uptime_get() + p_elem->state.delay * 5; // delay is in 5 millisecond steps
        p_elem->state.trans_end_time = p_elem->state.trans_start_time + get_transition_time(p_elem->state.trans);
    }
    BT_DBG("trans(0x%02x) delay(0x%02x)", p_elem->state.trans, p_elem->state.delay);
#endif

    if(p_elem->state.hsl_hue[T_CUR] != p_elem->state.hsl_hue[T_TAR])
    {
        model_bind_operation(B_HSL_HUE_ID, B_OPS_END_ID, p_elem);
    }

    if(p_elem->state.hsl_sat[T_CUR] != p_elem->state.hsl_sat[T_TAR])
    {
        model_bind_operation(B_HSL_SAT_ID, B_OPS_END_ID, p_elem);
    }

    if(p_elem->state.hsl_lightness[T_CUR] != p_elem->state.hsl_lightness[T_TAR])
    {
        model_bind_operation(B_HSL_LIGHTNESS_ID, B_OPS_END_ID, p_elem);
    } 

    return MESH_SUCCESS;
}

void light_hsl_publication(struct bt_mesh_model *p_model)
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
        _hsl_prepare_buf(p_model, msg, 0, 0);
        
        err = bt_mesh_model_publish(p_model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d\n", err);
        }
        BT_DBG("Success!!!");
    }
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
        //light_hsl_publication(p_model);
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _hsl_status(p_model, p_ctx, 1, 0);
    }
}

static void _hsl_set_unack(struct bt_mesh_model *p_model,
                          struct bt_mesh_msg_ctx *p_ctx,
                          struct net_buf_simple *p_buf)
{
    BT_DBG("");
    
    E_MESH_ERROR_TYPE ret = _hsl_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        //light_hsl_publication(p_model);
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

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_HSL_RANGE_STATUS);
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


    BT_DBG("default_light_ln(0x%04x) default_hue(0x%04x) default_sat(0x%04x)",
            p_elem->powerup.default_light_ln, p_elem->powerup.default_hue,
            p_elem->powerup.default_sat);

    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_HSL_DEF_STATUS);
    net_buf_simple_add_le16(p_msg, p_elem->powerup.default_light_ln);
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
        { BT_MESH_MODEL_LIGHT_HSL_GET, 0, _hsl_get },
        { BT_MESH_MODEL_LIGHT_HSL_SET, 7, _hsl_set },
        { BT_MESH_MODEL_LIGHT_HSL_SET_UNACK, 7, _hsl_set_unack },
        { BT_MESH_MODEL_LIGHT_HSL_TGT_GET, 0, _hsl_tgt_get },
        { BT_MESH_MODEL_LIGHT_HSL_RANGE_GET, 0, _hsl_range_get },
        { BT_MESH_MODEL_LIGHT_HSL_DEF_GET, 0, _hsl_default_get },
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

    set_light_board_type(LIGHT_TYPE_HSL);
    p_elem->message_index = MM_INDEX_SRV_HSL;

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

    p_elem->powerup.default_light_ln = lightness;
    p_elem->powerup.default_hue = hue;
    p_elem->powerup.default_sat = sat;

    BT_DBG("lightness(0x%04x) hue(0x%04x) sat(0x%04x)", p_elem->powerup.default_light_ln,
            p_elem->powerup.default_hue, p_elem->powerup.default_sat);

    return MESH_SUCCESS;
}

static void _hsl_default_set(struct bt_mesh_model *p_model,
                            struct bt_mesh_msg_ctx *p_ctx,
                            struct net_buf_simple *p_buf)
{
    BT_DBG("");

    if(_hsl_default_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _hsl_defatult_status(p_model, p_ctx);
    }
}

static void _hsl_default_set_unack(struct bt_mesh_model *p_model,
                                  struct bt_mesh_msg_ctx *p_ctx,
                                  struct net_buf_simple *p_buf)
{
    BT_DBG("");

    E_MESH_ERROR_TYPE ret = _hsl_default_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
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

    set_light_board_type(LIGHT_TYPE_HSL);
    p_elem->message_index = MM_INDEX_SRV_HSL;

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

    return MESH_SUCCESS;
}

static void _hsl_range_set(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    BT_DBG("");

    if(_hsl_range_analyze(p_model, p_ctx->addr, p_buf) == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _hsl_range_status(p_model, p_ctx);
    }
}

static void _hsl_range_set_unack(struct bt_mesh_model *p_model,
                                     struct bt_mesh_msg_ctx *p_ctx,
                                     struct net_buf_simple *p_buf)
{
    BT_DBG("");

    E_MESH_ERROR_TYPE ret = _hsl_range_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}


const struct bt_mesh_model_op g_hsl_setup_srv_op[HSL_SETUP_OPC_NUM] = {
        { BT_MESH_MODEL_LIGHT_HSL_DEF_SET,          6, _hsl_default_set },
        { BT_MESH_MODEL_LIGHT_HSL_DEF_SET_UNACK,    6, _hsl_default_set_unack },
        { BT_MESH_MODEL_LIGHT_HSL_RANGE_SET,        4, _hsl_range_set },
        { BT_MESH_MODEL_LIGHT_HSL_RANGE_SET_UNACK,  4, _hsl_range_set_unack },
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
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_HUE_STATUS);

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

    p_elem->state.hsl_hue[T_TAR] = hue;

    if(p_elem->state.hsl_hue[T_CUR] != p_elem->state.hsl_hue[T_TAR])
    {
        model_bind_operation(B_HSL_HUE_ID, B_OPS_END_ID, p_elem);
    }

    p_elem->state.trans = trans?trans:p_elem->powerup.def_trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = aos_now_ms() + p_elem->state.delay*5;
    }

    BT_DBG("hue(0x%04x) trans(0x%02x) delay(0x%02x)",
            p_elem->state.hsl_hue[T_TAR], p_elem->state.trans, p_elem->state.delay);

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
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _hsl_hue_status(p_model, p_ctx, 1);
    }
}

static void _hsl_hue_set_unack(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    E_MESH_ERROR_TYPE ret = _hsl_hue_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}

const struct bt_mesh_model_op g_hsl_hue_srv_op[HSL_HUE_OPC_NUM] = {
        { BT_MESH_MODEL_LIGHT_HUE_GET,          0, _hsl_hue_get },
        { BT_MESH_MODEL_LIGHT_HUE_SET,          6, _hsl_hue_set },
        { BT_MESH_MODEL_LIGHT_HUE_SET_UNACK,    6, _hsl_hue_set_unack },
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
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_LIGHT_SAT_STATUS);

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

    p_elem->state.hsl_sat[T_TAR] = sat;

    if(p_elem->state.hsl_sat[T_CUR] != p_elem->state.hsl_sat[T_TAR])
    {
        model_bind_operation(B_HSL_SAT_ID, B_OPS_END_ID, p_elem);
    }

    p_elem->state.trans = trans?trans:p_elem->powerup.def_trans;
    p_elem->state.delay = delay;
    if(p_elem->state.trans) {
        p_elem->state.trans_start_time = aos_now_ms() + p_elem->state.delay*5;
    }

    BT_DBG("sat(0x%04x) trans(0x%02x) delay(0x%02x)",
            p_elem->state.hsl_sat[T_TAR], p_elem->state.trans, p_elem->state.delay);

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
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        _hsl_sat_status(p_model, p_ctx, 1);
    }
}

static void _hsl_sat_set_unack(struct bt_mesh_model *p_model,
                               struct bt_mesh_msg_ctx *p_ctx,
                               struct net_buf_simple *p_buf)
{
    u8_t pub_need = 0;

    BT_DBG("");

    E_MESH_ERROR_TYPE ret = _hsl_sat_analyze(p_model, p_ctx->addr, p_buf);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}

const struct bt_mesh_model_op g_hsl_sat_srv_op[HSL_SAT_OPC_NUM] = {
        { BT_MESH_MODEL_LIGHT_SAT_GET,          0, _hsl_sat_get },
        { BT_MESH_MODEL_LIGHT_SAT_SET,          6, _hsl_sat_set },
        { BT_MESH_MODEL_LIGHT_SAT_SET_UNACK,    6, _hsl_sat_set_unack },
        BT_MESH_MODEL_OP_END,
};
#endif //CONFIG_MESH_MODEL_HSL_SAT_SRV


#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
void bind_hsl_hue_with_gen_level(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Generic Level state changes
    if(direct == BIND_FORWARD)
    {
        if(p_elem->powerup.max_hue - p_state->gen_level[T_TAR] > 32768)
        {
            p_state->hsl_hue[T_TAR] = p_state->gen_level[T_TAR] + 32768;
        }
        else
        {
            p_state->hsl_hue[T_TAR] = p_elem->powerup.max_hue;
        }
    }
    //REVERSE function performed whenever Light HSL Hue state changes
    else if(direct == BIND_REVERSE)
    {
        if(p_state->hsl_hue[T_TAR] - p_elem->powerup.min_hue > 32768)
        {
            p_state->gen_level[T_TAR] = p_state->hsl_hue[T_TAR] - 32768;
        }
        else
        {
            p_state->gen_level[T_TAR] = p_elem->powerup.min_hue;
        }
    }
}

void bind_hsl_sat_with_gen_level(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Generic Level state changes
    if(direct == BIND_FORWARD)
    {
        if(p_elem->powerup.max_sat - p_state->gen_level[T_TAR] > 32768)
        {
            p_state->hsl_sat[T_TAR] = p_state->gen_level[T_TAR] + 32768;
        }
        else
        {
            p_state->hsl_sat[T_TAR] = p_elem->powerup.max_sat;
        }
    }
    //REVERSE function performed whenever Light HSL Hue state changes
    else if(direct == BIND_REVERSE)
    {
        if(p_state->hsl_sat[T_TAR] - p_elem->powerup.min_sat > 32768)
        {
            p_state->gen_level[T_TAR] = p_state->hsl_sat[T_TAR] - 32768;
        }
        else
        {
            p_state->gen_level[T_TAR] = p_elem->powerup.min_sat;
        }
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
//performed during power up sequence
void bind_hsl_hue_with_gen_onpowerup(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    if(direct == BIND_FORWARD)
    {
        switch(p_elem->powerup.onpowerup)
        {
            case ONPOWERUP_OFF:
            case ONPOWERUP_DEF:
                p_state->hsl_hue[T_TAR] = p_elem->powerup.default_hue;
                break;
            case ONPOWERUP_RES:
                p_state->hsl_hue[T_TAR] = p_elem->powerup.last_hsl_hue;
                break;
        }
    }
}

//performed during power up sequence
void bind_hsl_sat_with_gen_onpowerup(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    if(direct == BIND_FORWARD)
    {
        switch(p_elem->powerup.onpowerup)
        {
            case ONPOWERUP_OFF:
            case ONPOWERUP_DEF:
                p_state->hsl_sat[T_TAR] = p_elem->powerup.default_hue;
                break;
            case ONPOWERUP_RES:
                p_state->hsl_sat[T_TAR] = p_elem->powerup.last_hsl_hue;
                break;
        }
    }
}
#endif

void bind_hsl_hue_with_hue_range(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Light HSL Hue state changes
    if(direct == BIND_REVERSE)
    {
        if(p_state->hsl_hue[T_TAR] < p_elem->powerup.min_hue)
        {
            p_state->hsl_hue[T_TAR] = p_elem->powerup.min_hue;
        }
        else if(p_state->hsl_hue[T_TAR] > p_elem->powerup.max_hue)
        {
            p_state->hsl_hue[T_TAR] > p_elem->powerup.max_hue;
        }
    }
}

void bind_hsl_sat_with_sat_range(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Light HSL Hue state changes
    if(direct == BIND_REVERSE)
    {
        if(p_state->hsl_sat[T_TAR] < p_elem->powerup.min_sat)
        {
            p_state->hsl_sat[T_TAR] = p_elem->powerup.min_sat;
        }
        else if(p_state->hsl_sat[T_TAR] > p_elem->powerup.max_sat)
        {
            p_state->hsl_sat[T_TAR] > p_elem->powerup.max_sat;
        }
    }
}

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
void bind_hsl_lightness_with_ln_actual(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct)
{
    S_MODEL_STATE *p_state = &p_elem->state;

    BT_DBG("");

    //FORWARD function performed whenever Light Lightness Actual state changes
    if(direct == BIND_FORWARD)
    {
        p_state->hsl_lightness[T_TAR] = p_state->light_ln_actual[T_TAR];
    }
    //REVERSE function performed whenever HSL Lightness state changes
    else if(direct == BIND_REVERSE)
    {
        p_state->light_ln_actual[T_TAR] = p_state->hsl_lightness[T_TAR];
    }
}
#endif

BUILD_MODEL_STATE_BIND_HANDLER(B_HSL_HUE_ID)= {
#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    {B_GEN_LEVEL_ID,            bind_hsl_hue_with_gen_level,       BIND_REVERSE},
#endif
    {B_HSL_HUE_RANGE_ID,        bind_hsl_hue_with_hue_range,       BIND_REVERSE},
    {B_OPS_END_ID,              NULL,                              BIND_NULL},
};

BUILD_MODEL_STATE_BIND_HANDLER(B_HSL_SAT_ID)= {
#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    {B_GEN_LEVEL_ID,            bind_hsl_sat_with_gen_level,       BIND_REVERSE},
#endif
    {B_HSL_SAT_RANGE_ID,        bind_hsl_sat_with_sat_range,       BIND_REVERSE},
    {B_OPS_END_ID,              NULL,                              BIND_NULL},
};

BUILD_MODEL_STATE_BIND_HANDLER(B_HSL_LIGHTNESS_ID)= {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    {B_LIGHTNESS_ACTUAL_ID,     bind_hsl_lightness_with_ln_actual, BIND_REVERSE},
#endif

    {B_OPS_END_ID,              NULL,                              BIND_NULL},
};
