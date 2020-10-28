//
// Created by ethan on 2019/7/8.
//

#ifndef _MODEL_BOUND_OPERATION_H
#define _MODEL_BOUND_OPERATION_H

#include "genie_app.h"

#define BUILD_MODEL_STATE_BIND_HANDLER(id)   const struct state_bind_handler id##_BIND_HANDLER[]
#define B_BIND_HANDLER(id)                   id##_BIND_HANDLER

typedef enum _BIND_OPERATION_ID_ {
/* !!!START!!! --- Don't add new ID before this ID */
    B_OPS_START_ID = -1,

#if CONFIG_MESH_MODEL_GEN_ONOFF_SRV
/* Generic OnOff */
    B_GEN_ONOFF_ID = 0,
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONPOWERUP_SRV
    B_GEN_ONPOWERUP_ID,
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
/* Generic Level */
    B_GEN_LEVEL_ID,
    B_GEN_DELTA_ID,
    B_GEN_MOVE_ID,
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
/* Light Lightness */
    B_LIGHTNESS_LINEAR_ID,
    B_LIGHTNESS_ACTUAL_ID,
    B_LIGHTNESS_LAST_ID,
    B_LIGHTNESS_DFT_ID,
    B_LIGHTNESS_RANGE_ID,
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
/* Light CTL */
    B_LIGHT_CTL_LN_ID,
    B_LIGHT_CTL_TEMP_ID,
    B_LIGHT_CTL_DFT_TEMP_ID,
    B_LIGHT_CTL_TEMP_RANGE_ID,
    B_LIGHT_CTL_DELTA_UV_ID,
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    B_HSL_HUE_ID,
    B_HSL_HUE_RANGE_ID,
    B_HSL_SAT_ID,
    B_HSL_SAT_RANGE_ID,
    B_HSL_LIGHTNESS_ID,
#endif

/* !!!END!!! --- Don't add new ID after this ID */
    B_OPS_END_ID

} E_BIND_OPERATION_ID;

typedef enum
{
    BIND_NULL = 0,
    BIND_FORWARD,
    BIND_REVERSE,

} E_BIND_DIRECT;

struct state_bind_handler {

	E_BIND_OPERATION_ID  bind_id;

	/* Message handler for the opcode */
	u16_t (*BIND_OPS_HANDLER)(S_ELEM_STATE *p_elem, E_BIND_DIRECT direct);

    E_BIND_DIRECT direct;
};

struct bind_handler_tab {

	E_BIND_OPERATION_ID  bind_id;

	struct state_bind_handler* bind_h;
};

u16_t model_bind_operation(E_BIND_OPERATION_ID id, E_BIND_OPERATION_ID id_except, S_ELEM_STATE *p_elem) ;


#endif //_MODEL_BOUND_OPERATION_H
