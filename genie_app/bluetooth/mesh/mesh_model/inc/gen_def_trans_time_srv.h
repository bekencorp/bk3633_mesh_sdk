#ifndef _GEN_DEF_TRANS_TIME_SRV_H_
#define _GEN_DEF_TRANS_TIME_SRC_H_


#define MESH_MODEL_GEN_DEF_TRANS_TIME_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV, \
                                                gen_def_trans_time_srv_op, &gen_def_trans_time_srv_pub, _user_data)

extern struct bt_mesh_model_pub gen_def_trans_time_srv_pub;
extern const struct bt_mesh_model_op gen_def_trans_time_srv_op[4];

#endif //_GEN_DEF_TRANS_TIME_SRV_H_