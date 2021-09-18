/**
 ****************************************************************************************
 *
 * @file ke_msg.c
 *
 * @brief This file contains the scheduler primitives called to create or delete
 * a task. It contains also the scheduler itself.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup MSG
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stddef.h>          // standard definition
#include <stdint.h>          // standard integer definition
#include <stdbool.h>         // standard boolean definition
#include <string.h>          // string definition
#include "arch.h"            // platform architecture define

#include "rwip_config.h"     // stack configuration

#include "ke_queue.h"        // kernel queue defines
#include "ke_msg.h"          // kernel message defines
#include "ke_task.h"         // kernel task defines
#include "ke_mem.h"          // kernel memory defines

#include "ke_event.h"        // kernel event defines
#include "ke_env.h"          // kernel environment defines

#include "dbg.h"

__attribute__((section("STACK_FUNC")))
const struct ke_msg_func_tag ke_msg_func = {ke_msg_alloc, ke_msg_send};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void *ke_msg_alloc(ke_msg_id_t const id, ke_task_id_t const dest_id,
                   ke_task_id_t const src_id, uint16_t const param_len)
{
    struct ke_msg *msg = (struct ke_msg*) ke_malloc(sizeof(struct ke_msg) + param_len, KE_MEM_KE_MSG);
    void *param_ptr = NULL;

    ASSERT_ERR(msg != NULL);
    msg->hdr.next  = KE_MSG_NOT_IN_QUEUE;
    msg->id        = id;
    msg->dest_id   = dest_id;
    msg->src_id    = src_id;
    msg->param_len = param_len;

    param_ptr = ke_msg2param(msg);

    memset(param_ptr, 0, param_len);

    return param_ptr;
}

void ke_msg_send(void const *param_ptr)
{
    struct ke_msg * msg = ke_param2msg(param_ptr);
    // Enqueue the message. Protect against IRQs as messages can be sent from IRQ
    GLOBAL_INT_DISABLE();
    ke_queue_push(&ke_env.queue_sent, (struct co_list_hdr*)msg);
    GLOBAL_INT_RESTORE();

    // Trace sent kernel message
    TRC_REQ_KE_MSG_SEND(msg);

    // trigger the event
    ke_event_set(KE_EVENT_KE_MESSAGE);
}

void ke_msg_send_basic(ke_msg_id_t const id, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    void *no_param = ke_msg_alloc(id, dest_id, src_id, 0);
    ke_msg_send(no_param);
}

void ke_msg_forward(void const *param_ptr, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct ke_msg * msg = ke_param2msg(param_ptr);

    // update the source and destination of the message
    msg->dest_id = dest_id;
    msg->src_id  = src_id;

    // send the message
    ke_msg_send(param_ptr);
}

void ke_msg_forward_new_id(void const *param_ptr,
                           ke_msg_id_t const msg_id, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct ke_msg * msg = ke_param2msg(param_ptr);

    // update the id of the message
    msg->id      = msg_id;
    // update the source and destination of the message
    msg->dest_id = dest_id;
    msg->src_id  = src_id;

    // send the message
    ke_msg_send(param_ptr);
}

void ke_msg_free(struct ke_msg const *msg)
{
    ke_free( (void*) msg);
}


ke_msg_id_t ke_msg_dest_id_get(void const *param_ptr)
{
    struct ke_msg * msg = ke_param2msg(param_ptr);

    return msg->dest_id;
}


ke_msg_id_t ke_msg_src_id_get(void const *param_ptr)
{
    struct ke_msg * msg = ke_param2msg(param_ptr);

    return msg->src_id;
}


bool ke_msg_in_queue(void const *param_ptr)
{
    struct ke_msg * msg = ke_param2msg(param_ptr);

    return (msg->hdr.next != KE_MSG_NOT_IN_QUEUE);
}
/// @} MSG
