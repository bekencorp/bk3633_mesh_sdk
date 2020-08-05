/**
 ****************************************************************************************
 *
 * @file ke_event.c
 *
 * @brief This file contains the event handling primitives.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup EVT
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"  // stack configuration

#include "arch.h"         // architecture
#include "co_math.h"      // maths definitions
#include <stdint.h>       // standard integer definition
#include <stddef.h>       // standard definition
#include <string.h>       // memcpy defintion

#include "ke_event.h"     // kernel event
#include "k_type.h"
#include "rwip.h"
#include "dbg.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Format of an event callback function
typedef void (*p_callback_t)(void);


/*
 * STRUCTURES DEFINTIONS
 ****************************************************************************************
 */

/// KE EVENT environment structure
struct ke_event_env_tag
{
    /// Event field
    uint32_t event_field;

    /// Callback table
    p_callback_t callback[KE_EVENT_MAX];
};


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// KE EVENT environment
static struct ke_event_env_tag ke_event_env;

ksem_t ke_event_sem;

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void ke_event_init(void)
{
    memset(&ke_event_env, 0, sizeof(ke_event_env));
    if(rom_env.krhino_sem_create)
    {
        rom_env.krhino_sem_create(&ke_event_sem, "ke_event", 0);
        //bim_printf("%s\r\n", __func__);
    }

}

uint8_t ke_event_callback_set(uint8_t event_type, void (*p_callback)(void))
{
    uint8_t status = KE_EVENT_CAPA_EXCEEDED;

    ASSERT_INFO((event_type < KE_EVENT_MAX) && (p_callback != NULL), event_type, p_callback);

    if(event_type < KE_EVENT_MAX)
    {
        // Store callback
        ke_event_env.callback[event_type] = p_callback;

        // Status OK
        status = KE_EVENT_OK;
    }

    return (status);
}

void ke_event_set(uint8_t event_type)
{
    ASSERT_INFO((event_type < KE_EVENT_MAX), event_type, 0);

    GLOBAL_INT_DISABLE();

    if(event_type < KE_EVENT_MAX)
    {
        // Set the event in the bit field
        ke_event_env.event_field |= (1 << event_type);
        if(rom_env.krhino_sem_give)
        	rom_env.krhino_sem_give(&ke_event_sem);
    }

    GLOBAL_INT_RESTORE();
    //Trace the event
    TRC_REQ_KE_EVT_SET(event_type);
}

void ke_event_clear(uint8_t event_type)
{
    ASSERT_INFO((event_type < KE_EVENT_MAX), event_type, 0);

    GLOBAL_INT_DISABLE();

    if(event_type < KE_EVENT_MAX)
    {
        // Set the event in the bit field
        ke_event_env.event_field &= ~(1 << event_type);
    }

    GLOBAL_INT_RESTORE();
}

uint8_t ke_event_get(uint8_t event_type)
{
    uint8_t state = 0;

    ASSERT_INFO((event_type < KE_EVENT_MAX), event_type, 0);

    GLOBAL_INT_DISABLE();

    if(event_type < KE_EVENT_MAX)
    {
        // Get the event in the bit field
        state = (ke_event_env.event_field >> event_type) & (0x1);
    }

    GLOBAL_INT_RESTORE();

    return state;
}

uint32_t ke_event_get_all(void)
{
    return ke_event_env.event_field;
}

void ke_event_flush(void)
{
    ke_event_env.event_field = 0;
}

void ke_event_schedule(void)
{
    uint8_t hdl;

    // Get the volatile value
    uint32_t field = ke_event_env.event_field;

    while (field) // Compiler is assumed to optimize with loop inversion
    {
        // Find highest priority event set
        hdl = 32 - (uint8_t) co_clz(field) - 1;

        // Sanity check
        ASSERT_INFO(hdl < KE_EVENT_MAX, hdl, field);

        //Trace the event
        TRC_REQ_KE_EVT_HANDLED(hdl);

        if(ke_event_env.callback[hdl] != NULL)
        {
            // Execute corresponding handler
            (ke_event_env.callback[hdl])();
        }
        else
        {
            ASSERT_ERR(0);
        }

        // Update the volatile value
        field = ke_event_env.event_field;
    }
}


///@} KE_EVT
