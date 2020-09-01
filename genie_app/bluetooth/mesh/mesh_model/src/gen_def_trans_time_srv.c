
#include <api/mesh.h>
#include <mesh.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "genie_app.h"
#include "common/log.h"

struct bt_mesh_model_pub gen_def_trans_time_srv_pub = {
    .msg = NET_BUF_SIMPLE(2 + 1),
};

static void _gen_def_trans_time_prepare_buf(struct bt_mesh_model *p_model, struct net_buf_simple *p_msg, bool is_ack)
{
    S_MODEL_STATE *p_state = &((S_ELEM_STATE *)p_model->user_data)->state;
    u8_t remain_byte = 0;

    BT_DBG("tt(0x%04x)", p_state->tt);
#ifdef CONFIG_MESH_MODEL_TRANS
    remain_byte = get_remain_byte(p_state, is_ack);

    BT_DBG("remain(0x%02x)", remain_byte);
#endif
    //prepear buff
    bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_GEN_DEF_TRANS_TIME_STATUS);

    net_buf_simple_add_u8(p_msg, p_state->tt);

}

static void _gen_def_trans_time_status(struct bt_mesh_model *p_model,
                       struct bt_mesh_msg_ctx *p_ctx, bool is_ack)
{
    struct net_buf_simple *p_msg = NET_BUF_SIMPLE(2 + 1 + 4);

    BT_DBG("addr(0x%04x)", p_model->elem->addr);

    _gen_def_trans_time_prepare_buf(p_model, p_msg, is_ack);

    if (bt_mesh_model_send(p_model, p_ctx, p_msg, NULL, NULL)) {
        BT_ERR("Unable to send ctl Status");
    }
    BT_DBG("Success!!!");
}

/* Generic Default Transition Time Server message handlers */
static void _gen_def_trans_time_get(struct bt_mesh_model *p_model,
				   struct bt_mesh_msg_ctx *p_ctx,
				   struct net_buf_simple *p_buf)
{
    BT_DBG("");

    _gen_def_trans_time_status(p_model, p_ctx, 0);
}

static u8_t _gen_def_trans_time_analyze(struct bt_mesh_model *p_model,
                            u16_t src_addr, struct net_buf_simple *p_buf)
{
    u8_t tt = 0;
    E_MESH_ERROR_TYPE ret = MESH_SUCCESS;
    S_ELEM_STATE *p_elem = NULL;

    p_elem = p_model->user_data;

    if(p_buf->len != 1) {
        BT_ERR("MESH_ANALYZE_SIZE_ERROR p_buf->len(%d)", p_buf->len);
        return MESH_ANALYZE_SIZE_ERROR;
    }

    tt = net_buf_simple_pull_u8(p_buf);

    if ((tt & 0x3F) == 0x3F) {
		return MESH_ANALYZE_ARGS_ERROR;
	}
    p_elem->state.tt = tt;

    return MESH_SUCCESS;
}

static void _gen_def_trans_time_set(struct bt_mesh_model *p_model,
				   struct bt_mesh_msg_ctx *p_ctx,
				   struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = _gen_def_trans_time_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if(ret == MESH_SUCCESS || ret == MESH_TID_REPEAT) {
        _gen_def_trans_time_status(p_model, p_ctx, 1);
        if(ret == MESH_SUCCESS) {
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
        }
    }

	//gen_def_trans_time_publish(model);

}


static void _gen_def_trans_time_set_unack(struct bt_mesh_model *p_model,
					 struct bt_mesh_msg_ctx *p_ctx,
					 struct net_buf_simple *p_buf)
{
    E_MESH_ERROR_TYPE ret = _gen_def_trans_time_analyze(p_model, p_ctx->addr, p_buf);

    BT_DBG("ret %d", ret);

    if(ret == MESH_SUCCESS) {
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, (S_ELEM_STATE *)p_model->user_data);
    }
}



/* Mapping of message handlers for Generic Default TT Server (0x1004) */
const struct bt_mesh_model_op gen_def_trans_time_srv_op[] = {
	{ BT_MESH_MODEL_GEN_DEF_TRANS_TIME_GET,         0, _gen_def_trans_time_get },
	{ BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET,         1, _gen_def_trans_time_set },
	{ BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET_UNACK,   1, _gen_def_trans_time_set_unack },
	BT_MESH_MODEL_OP_END,
};

