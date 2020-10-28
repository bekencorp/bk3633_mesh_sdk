/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */
//


#include <api/mesh.h>
#include "inc/model_bind_ops.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

const struct bind_handler_tab _bind_handler_t[] = {

#if CONFIG_MESH_MODEL_GEN_ONOFF_SRV
/* Generic OnOff */
    { B_GEN_ONOFF_ID,              B_BIND_HANDLER(B_GEN_ONOFF_ID) },
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
/* Generic Level */
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
/* Light Lightness */
    { B_LIGHTNESS_ACTUAL_ID,        B_BIND_HANDLER(B_LIGHTNESS_ACTUAL_ID) },
    { B_LIGHTNESS_LINEAR_ID,        B_BIND_HANDLER(B_LIGHTNESS_LINEAR_ID) },
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
/* Light CTL */
    { B_LIGHT_CTL_LN_ID,            B_BIND_HANDLER(B_LIGHT_CTL_LN_ID) },
    { B_LIGHT_CTL_TEMP_ID,          B_BIND_HANDLER(B_LIGHT_CTL_TEMP_ID) },
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    { B_HSL_HUE_ID,               B_BIND_HANDLER(B_HSL_HUE_ID) },
    { B_HSL_SAT_ID,               B_BIND_HANDLER(B_HSL_SAT_ID) },
    { B_HSL_LIGHTNESS_ID,         B_BIND_HANDLER(B_HSL_LIGHTNESS_ID) },
#endif

};


u16_t model_bind_operation(E_BIND_OPERATION_ID id, E_BIND_OPERATION_ID id_except, S_ELEM_STATE *p_elem) 
{
    struct state_bind_handler p_handle;
    u16_t i;

    struct state_bind_handler* bind_h = NULL;
    u16_t loop_size = ARRAY_SIZE(_bind_handler_t);

    for(i = 0; i < loop_size; i++)
    {
        if(_bind_handler_t[i].bind_id == id)
        {
            bind_h = _bind_handler_t[i].bind_h;
            break;
        }
    }

    BT_DBG("bind ops - id: %d, id_except: %d", id, id_except);

    if(bind_h == NULL)
    {
        BT_DBG("BOUND ALL");
        return -1;
    }

    for(i = 0; i < B_OPS_END_ID; i++)
    {
        p_handle = bind_h[i];

        if(p_handle.bind_id == B_OPS_END_ID)
        {
            break;
        }
        else if(p_handle.bind_id == id_except)
        {
            continue;
        }

        if(p_handle.BIND_OPS_HANDLER != NULL)
        {
            p_handle.BIND_OPS_HANDLER(p_elem, p_handle.direct);
        }

        model_bind_operation(p_handle.bind_id, id, p_elem);
    }

}

