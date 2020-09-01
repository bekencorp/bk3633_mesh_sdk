#ifndef _GEN_ONPOWERUP_SRV_H_
#define _GEN_ONPOWERUP_SRV_H_


#define MESH_MODEL_GEN_ONPOWERUP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV, \
                                                gen_power_onoff_srv_op, &gen_onpowerup_srv_pub, _user_data)

extern struct bt_mesh_model_pub gen_onpowerup_srv_pub;
extern const struct bt_mesh_model_op gen_power_onoff_srv_op[2];

#define MESH_MODEL_GEN_ONPOWERUP_SETUP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV, \
                                                gen_power_onoff_setup_srv_op, &gen_onpowerup_srv_pub, _user_data)

extern struct bt_mesh_model_pub gen_onpowerup_setup_srv_pub;
extern const struct bt_mesh_model_op gen_power_onoff_setup_srv_op[3];

typedef enum {
    ONPOWERUP_OFF = 0,
    ONPOWERUP_DEF,
    ONPOWERUP_RES,
    
} E_GEN_ONPOWERUP_STATE;

#endif //_GEN_ONPOWERUP_SRV_H_