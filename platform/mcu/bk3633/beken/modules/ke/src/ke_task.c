/**
 ****************************************************************************************
 *
 * @file ke_task.c
 *
 * @brief This file contains the implementation of the kernel task management.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup TASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"       // stack configuration

#include <stddef.h>            // standard definition
#include <stdint.h>            // standard integer
#include <stdbool.h>           // standard boolean
#include <string.h>            // memcpy defintion

#include "ke_task.h"           // kernel task
#include "ke_env.h"            // kernel environment
#include "ke_queue.h"          // kernel queue
#include "ke_event.h"          // kernel event
#include "ke_mem.h"            // kernel memory

#include "dbg_swdiag.h"        // Software diag

#include "dbg.h"
/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * STRUCTURES DEFINTIONS
 ****************************************************************************************
 */

/// KE TASK element structure
struct ke_task_elem
{
    struct ke_task_desc const * p_desc;
};

/// KE TASK environment structure
struct ke_task_env_tag
{
    struct ke_task_elem task_list[TASK_MAX];
};


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// KE TASK environment
static struct ke_task_env_tag ke_task_env;

__attribute__((section("STACK_FUNC")))
const struct ke_task_func_tag ke_task_func = {ke_task_create, ke_state_set, ke_state_get};


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief Compare destination task callback.
 *
 * @param[in] msg          kernel message
 * @param[in] dest_id      destination id
 *
 * @return bool
 ****************************************************************************************
 */
static bool cmp_dest_id(struct co_list_hdr const * msg, uint32_t dest_id)
{
    return ((struct ke_msg*)msg)->dest_id == dest_id;
}

/**
 ****************************************************************************************
 * @brief Reactivation of saved messages.
 *
 * This primitive looks for all the messages destined to the task ke_task_id that
 * have been saved and inserts them into the sent priority queue. These
 * messages will be scheduled at the next scheduler pass.
 *
 * @param[in] ke_task_id    Destination Identifier
 ****************************************************************************************
 */
static void ke_task_saved_update(ke_task_id_t const ke_task_id)
{
    struct ke_msg * msg;

    for(;;)
    {
        // if the state has changed look in the Save queue if a message
        // need to be handled
        msg = (struct ke_msg*) ke_queue_extract(&ke_env.queue_saved,
                                                &cmp_dest_id,
                                                (uint32_t) ke_task_id);

        if (msg == NULL) break;

        // Insert it back in the sent queue
        GLOBAL_INT_DISABLE();
        ke_queue_push(&ke_env.queue_sent, (struct co_list_hdr*)msg);
        GLOBAL_INT_RESTORE();

        // trigger the event
        ke_event_set(KE_EVENT_KE_MESSAGE);
    }

    return;
}


/**
 ****************************************************************************************
 * @brief Search message handler function matching the msg id
 *
 * @param[in] msg_id          Message identifier
 * @param[in] msg_handler_tab Pointer to the message handlers table
 * @param[in] msg_cnt         Number of message handlers in the table
 *
 * @return                  Pointer to the message handler (NULL if not found)
 *
 ****************************************************************************************
 */
static ke_msg_func_t ke_handler_search(ke_msg_id_t const msg_id, struct ke_msg_handler const *msg_handler_tab, uint16_t msg_cnt)
{
    // Get the message handler function by parsing the message table
    for (int i = (msg_cnt-1); 0 <= i; i--)
    {
        if ((msg_handler_tab[i].id == msg_id)
                || (msg_handler_tab[i].id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERR(msg_handler_tab[i].func);

            return msg_handler_tab[i].func;
        }
    }

    // If we execute this line of code, it means that we did not find the handler
    return NULL;
}

/**
 ****************************************************************************************
 * @brief Retrieve appropriate message handler function of a task
 *
 * @param[in]  msg_id   Message identifier
 * @param[in]  task_id  Task instance identifier
 *
 * @return              Pointer to the message handler (NULL if not found)
 *
 ****************************************************************************************
 */
static ke_msg_func_t ke_task_handler_get(ke_msg_id_t const msg_id, ke_task_id_t const task_id)
{
    ke_msg_func_t func = NULL;
    int idx = KE_IDX_GET(task_id);
    int type = KE_TYPE_GET(task_id);
    struct ke_task_desc const * p_task_desc = NULL;

    ASSERT_INFO(type < TASK_MAX, msg_id, type);

    p_task_desc = ke_task_env.task_list[type].p_desc;

    // inactive task have no instance, drop message automatically for inactive tasks
    if(p_task_desc->idx_max != 0)
    {
        ASSERT_INFO(p_task_desc != NULL, task_id, msg_id);
        ASSERT_INFO((idx < p_task_desc->idx_max), task_id, msg_id);

        // If the idx found is out of range return NULL
        if(idx < p_task_desc->idx_max)
        {
            // Search the message handler in the table
            if (p_task_desc->msg_handler_tab)
            {
                func = ke_handler_search(msg_id, p_task_desc->msg_handler_tab, p_task_desc->msg_cnt);
            }
        }
    }

    return func;
}


/**
 ****************************************************************************************
 * @brief Scheduler entry point.
 *
 * This function is the scheduler of messages. It tries to get a message
 * from the sent queue, then try to get the appropriate message handler
 * function (from the current state, or the default one). This function
 * is called, then the message is saved or freed.
 ****************************************************************************************
 */
static void ke_task_schedule(void)
{
    DBG_SWDIAG(EVT, MESSAGE, 1);
    // Process one message at a time to ensure that events having higher priority are
    // handled in time
    do
    {
        int msg_status;
        struct ke_msg *msg;
        // Get a message from the queue
        GLOBAL_INT_DISABLE();
        msg = (struct ke_msg*) ke_queue_pop(&ke_env.queue_sent);
        msg->hdr.next = KE_MSG_NOT_IN_QUEUE;
        GLOBAL_INT_RESTORE();
        if (msg == NULL) break;

        // check if message already free
        if(ke_is_free(msg))
        {
            ASSERT_INFO(0,msg->id, msg->dest_id);
            break;
        }

        // Retrieve a pointer to the task instance data
        ke_msg_func_t func = ke_task_handler_get(msg->id, msg->dest_id);

        // sanity check
        ASSERT_WARN(func != NULL, msg->id, msg->dest_id);

        // Call the message handler
        if (func != NULL)
        {
            msg_status = func(msg->id, ke_msg2param(msg), msg->dest_id, msg->src_id);
        }
        else
        {
            msg_status = KE_MSG_CONSUMED;
        }

        //Trace handled kernel message
        TRC_REQ_KE_MSG_HANDLED(msg->id, msg->dest_id, msg->src_id, msg_status);

        switch (msg_status)
        {
        case KE_MSG_CONSUMED:
            // Free the message
            ke_msg_free(msg);
            break;

        case KE_MSG_NO_FREE:
            break;

        case KE_MSG_SAVED:
            // The message has been saved
            // Insert it at the end of the save queue
            ke_queue_push(&ke_env.queue_saved, (struct co_list_hdr*) msg);
            break;

        default:
            ASSERT_ERR(0);
            break;
        } // switch case
    } while(0);

    // Verify if we can clear the event bit
    GLOBAL_INT_DISABLE();
    if (co_list_is_empty(&ke_env.queue_sent))
        ke_event_clear(KE_EVENT_KE_MESSAGE);
    GLOBAL_INT_RESTORE();
    DBG_SWDIAG(EVT, MESSAGE, 0);
}


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void ke_task_init(void)
{
    memset(&ke_task_env, 0, sizeof(ke_task_env));

    // Register message event
    ke_event_callback_set(KE_EVENT_KE_MESSAGE, &ke_task_schedule);
}

uint8_t ke_task_create(uint8_t task_type, struct ke_task_desc const * p_task_desc)
{
    uint8_t status = KE_TASK_OK;

    GLOBAL_INT_DISABLE();

    do
    {
        ASSERT_INFO(task_type < TASK_MAX, task_type, TASK_MAX);

        if(task_type >= TASK_MAX)
        {
            status = KE_TASK_CAPA_EXCEEDED;
            break;
        }

        if(ke_task_env.task_list[task_type].p_desc != NULL)
        {
            status = KE_TASK_ALREADY_EXISTS;
            break;
        }

        // Save task descriptor
        ke_task_env.task_list[task_type].p_desc = p_task_desc;
    } while(0);

    GLOBAL_INT_RESTORE();

    return (status);
}

uint8_t ke_task_delete(uint8_t task_type)
{
    uint8_t status = KE_TASK_OK;
    GLOBAL_INT_DISABLE();

    do
    {
        if(task_type >= TASK_MAX)
        {
            status = KE_TASK_UNKNOWN;
            break;
        }

        // Clear task descriptor
        ke_task_env.task_list[task_type].p_desc = NULL;
    } while(0);

    GLOBAL_INT_RESTORE();

    return (status);
}


void ke_state_set(ke_task_id_t const id, ke_state_t const state_id)
{
    int idx = KE_IDX_GET(id);
    int type = KE_TYPE_GET(id);
    struct ke_task_desc const * p_task_desc = NULL;
    ke_state_t *ke_stateid_ptr = NULL;

    // sanity checks
    ASSERT_ERR(type < TASK_MAX);

    if(type < TASK_MAX)
    {
        p_task_desc = ke_task_env.task_list[type].p_desc;
    }

    ASSERT_INFO(p_task_desc != NULL, type, idx);
    ASSERT_INFO((idx < p_task_desc->idx_max), idx, p_task_desc->idx_max);

    // If the idx found is out of range return NULL
    if(idx < p_task_desc->idx_max)
    {
        // Get the state
        ke_stateid_ptr = &p_task_desc->state[idx];

        ASSERT_ERR(ke_stateid_ptr);

        // set the state
        if (*ke_stateid_ptr != state_id)
        {
            *ke_stateid_ptr = state_id;

            // if the state has changed update the SAVE queue
            ke_task_saved_update(id);
        }
    }
}


ke_state_t ke_state_get(ke_task_id_t const id)
{
    int idx = KE_IDX_GET(id);
    int type = KE_TYPE_GET(id);
    struct ke_task_desc const * p_task_desc = NULL;
    ke_state_t state = 0xFF;

    ASSERT_ERR(type < TASK_MAX);

    if(type < TASK_MAX)
    {
        p_task_desc = ke_task_env.task_list[type].p_desc;
    }

    ASSERT_INFO(p_task_desc != NULL, type, idx);
    ASSERT_INFO((idx < p_task_desc->idx_max), idx, p_task_desc->idx_max);

    // If the idx found is out of range return NULL
    if(idx < p_task_desc->idx_max)
    {
        state = p_task_desc->state[idx];
    }

    // Get the state
    return state;
}

int ke_msg_discard(ke_msg_id_t const msgid, void const *param,
                   ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

int ke_msg_save(ke_msg_id_t const msgid, void const *param,
                ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    return KE_MSG_SAVED;
}



void ke_task_msg_flush(ke_task_id_t task)
{
    // Free messages in sent queue
    struct ke_msg *msg = (struct ke_msg*) co_list_pick(&ke_env.queue_sent);
    struct ke_msg *msg_to_delete = NULL;
    bool s_queue_flushed = false;

    while(msg != NULL)
    {
        while(msg != NULL)
        {
            // check if current message have to be handled by task to flush
            if(KE_TYPE_GET(msg->dest_id) == task)
            {
                // update message pointer
                msg_to_delete = msg;
                msg = (struct ke_msg *)msg->hdr.next;

                // delete message
                co_list_extract(&ke_env.queue_sent, &(msg_to_delete->hdr));
                ke_msg_free(msg_to_delete);
            }
            else
            {
                // go to next message
                msg = (struct ke_msg *)msg->hdr.next;
            }
        }

        // Flush messages in save queue
        if(!s_queue_flushed)
        {
            msg = (struct ke_msg*) co_list_pick(&ke_env.queue_saved);
            s_queue_flushed = true;
        }
    }
}



ke_task_id_t ke_task_check(ke_task_id_t task)
{
    uint8_t idx  = KE_IDX_GET(task);
    uint8_t type = KE_TYPE_GET(task);

    // check if task type exist
    if(   (type > TASK_MAX)
       || (ke_task_env.task_list[type].p_desc == NULL)
    // check that task instance exist
       || (idx > ke_task_env.task_list[type].p_desc->idx_max))
    {
        task = TASK_NONE;
    }

    return task;
}

/// @} TASK
