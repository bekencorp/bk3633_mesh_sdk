/*
 *
 */

#ifndef _MODEL_LIGHT_CTL_SRV_H_
#define _MODEL_LIGHT_CTL_SRV_H_

#define CTL_TEMP_MIN		800//0x0320
#define CTL_TEMP_MAX		20000//0x4E20
#define CTL_TEMP_DEFAULT	CTL_TEMP_MAX


#define CTL_OPC_NUM 6
#define CTL_SETUP_OPC_NUM 5
#define CTL_TEMP_OPC_NUM 4


#define MESH_MODEL_CTL_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SRV, \
                                            g_ctl_srv_op, &g_ctl_srv_pub, _user_data)


#define MESH_MODEL_CTL_SETUP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, \
                                                g_ctl_setup_srv_op, NULL, _user_data)

#ifdef CONFIG_MESH_MODEL_CTL_TEMPERATURE_SRV
#define MESH_MODEL_CTL_TEMP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV, \
                                                g_ctl_temp_srv_op, &g_ctl_srv_pub, _user_data)
#endif

extern BUILD_MODEL_STATE_BIND_HANDLER(B_LIGHT_CTL_LN_ID);

extern struct bt_mesh_model_pub g_ctl_srv_pub;
extern const struct bt_mesh_model_op g_ctl_srv_op[CTL_OPC_NUM];
extern const struct bt_mesh_model_op g_ctl_setup_srv_op[CTL_SETUP_OPC_NUM];
#ifdef CONFIG_MESH_MODEL_CTL_TEMPERATURE_SRV
extern const struct bt_mesh_model_op g_ctl_temp_srv_op[CTL_TEMP_OPC_NUM];

extern BUILD_MODEL_STATE_BIND_HANDLER(B_LIGHT_CTL_TEMP_ID);
#endif


/**
 * @brief
 * @param[in] model
 */
void ctl_publication(struct bt_mesh_model *model);

/**
 * @brief
 * @param[in] model
 */
void ctl_temp_publication(struct bt_mesh_model *model);

u16_t ctl_lightness_bound_states_op(S_ELEM_STATE *p_elem);
u16_t ctl_temp_bound_states_op(S_ELEM_STATE *p_elem);

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
void bind_ctl_lightness_with_ln_actual(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct);
#endif

#endif // _MODEL_LIGHT_CTL_SRV_H_
