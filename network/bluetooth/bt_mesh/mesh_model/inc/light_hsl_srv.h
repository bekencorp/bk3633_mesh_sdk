#ifndef _BK_MODEL_LIGHT_HSL_SRV_H_
#define _BK_MODEL_LIGHT_HSL_SRV_H_


#define HSL_OPC_NUM 7
#define HSL_SETUP_OPC_NUM 5
#define HSL_HUE_OPC_NUM 4
#define HSL_SAT_OPC_NUM 4


#ifdef CONFIG_MESH_MODEL_HSL_SRV
#define MESH_MODEL_HSL_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_HSL_SRV, \
                                            g_hsl_srv_op, &g_hsl_srv_pub, _user_data)

extern struct bt_mesh_model_pub g_hsl_srv_pub;
extern const struct bt_mesh_model_op g_hsl_srv_op[HSL_OPC_NUM];
#endif //CONFIG_MESH_MODEL_HSL_SRV

#ifdef CONFIG_MESH_MODEL_HSL_SETUP_SRV
#define MESH_MODEL_HSL_SETUP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV, \
                                                g_hsl_setup_srv_op, &g_hsl_srv_pub, _user_data)

extern const struct bt_mesh_model_op g_hsl_setup_srv_op[HSL_SETUP_OPC_NUM];

#endif //CONFIG_MESH_MODEL_HSL_SETUP_SRV

#ifdef CONFIG_MESH_MODEL_HSL_HUE_SRV
#define MESH_MODEL_HSL_HUE_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV, \
                                                g_hsl_hue_srv_op, &g_hsl_srv_pub, _user_data)

extern const struct bt_mesh_model_op g_hsl_hue_srv_op[HSL_HUE_OPC_NUM];
#endif //CONFIG_MESH_MODEL_HSL_HUE_SRV

#ifdef CONFIG_MESH_MODEL_HSL_SAT_SRV
#define MESH_MODEL_HSL_SAT_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV, \
                                                g_hsl_sat_srv_op, &g_hsl_srv_pub, _user_data)

extern const struct bt_mesh_model_op g_hsl_sat_srv_op[HSL_SAT_OPC_NUM];
#endif //CONFIG_MESH_MODEL_HSL_SAT_SRV

#endif  //_BK_MODEL_LIGHT_HSL_SRV_H_
