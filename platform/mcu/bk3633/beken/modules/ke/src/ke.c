/**
 ****************************************************************************************
 *
 * @file ke.c
 *
 * @brief This file contains the kernel definition.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup KE
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"        // stack configuration

#include "arch.h"               // architecture
#include "ke.h"                 // kernel
#include "ke_env.h"             // kernel environment
#include "ke_mem.h"             // kernel memory
#include "ke_msg.h"             // kernel message
#include "ke_event.h"           // kernel event
#include "ke_queue.h"           // kernel queue
#include "ke_task.h"            // kernel task
#include "ke_timer.h"           // kernel timer
#include <string.h>             // used for memset.


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Kernel environment
struct ke_env_tag ke_env;


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void ke_init(void)
{
#if (KERNEL_MEM_RW)
    // ke_mem_init MUST be called first to be able to allocate memory right from start
    memset(ke_env.heap, 0, sizeof(struct mblock_free*) * KE_MEM_BLOCK_MAX);
    memset(ke_env.heap_size, 0, sizeof(uint16_t) * KE_MEM_BLOCK_MAX);
#endif // KERNEL_MEM_RW
    #if (KE_PROFILING)
    ke_env.max_heap_used = 0;
    memset(ke_env.heap_used, 0, sizeof(uint16_t) * KE_MEM_BLOCK_MAX);
    #endif //KE_PROFILING

    // Initialize event module
    ke_event_init();

    // initialize the kernel message queue, mandatory before any message can be transmitted
    ke_env.queue_saved.first = NULL;
    ke_env.queue_saved.last = NULL;
    ke_env.queue_sent.first = NULL;
    ke_env.queue_sent.last = NULL;
    ke_env.queue_timer.first = NULL;
    ke_env.queue_timer.last = NULL;

    // Initialize task module
    ke_task_init();

    // Initialize timer module
    ke_timer_init();
}

void ke_flush(void)
{
    // free all pending message(s)
    while(1)
    {
        struct ke_msg *msg = (struct ke_msg*) ke_queue_pop(&ke_env.queue_sent);
        if(msg == NULL)
            break;
        ke_msg_free(msg);
    }
    // free all saved message(s)
    while(1)
    {
        struct ke_msg *msg = (struct ke_msg*) ke_queue_pop(&ke_env.queue_saved);
        if(msg == NULL)
            break;
        ke_msg_free(msg);
    }
    // free all timers
    while(1)
    {
        struct ke_timer *timer = (struct ke_timer*) ke_queue_pop(&ke_env.queue_timer);
        if(timer == NULL)
            break;
        ke_free(timer);
    }
}

bool ke_sleep_check(void)
{
    return (ke_event_get_all() == 0);
}

#if (KE_PROFILING)
enum KE_STATUS ke_stats_get(uint8_t* max_msg_sent,
                uint8_t* max_msg_saved,
                uint8_t* max_timer_used,
                uint16_t* max_heap_used)
{
    *max_msg_sent   = ke_env.queue_sent.maxcnt;
    *max_msg_saved  = ke_env.queue_saved.maxcnt;
    *max_timer_used = ke_env.queue_timer.maxcnt;
    *max_heap_used  = ke_env.max_heap_used;

    return KE_SUCCESS;
}
#endif //KE_PROFILING

///@} KE
