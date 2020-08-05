/**
 ****************************************************************************************
 *
 * @file ke_timer.c
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
 * @addtogroup KE_TIMER
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stddef.h>              // standard definition
#include <stdint.h>              // standard integer
#include <stdbool.h>             // standard boolean
#include "arch.h"                // architecture

#include "ke_queue.h"            // kernel queue
#include "ke_mem.h"              // kernel memory
#include "ke_env.h"              // kernel environment
#include "ke_event.h"            // kernel event
#include "ke_timer.h"            // kernel timer
#include "ke_task.h"             // kernel task

#include "dbg_swdiag.h"          // Software diag

#include "rwip.h"                // common driver

#include "dbg.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum timer value
#define KE_TIMER_MASK           (RWIP_MAX_10MS_TIME)
/// Maximum timer value
#define KE_TIMER_DELAY_MAX      (RWIP_MAX_10MS_TIME >> 1)

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

__attribute__((section("STACK_FUNC")))
const struct ke_time_func_tag ke_time_func = {ke_timer_set, ke_timer_clear};


/**
 ****************************************************************************************
 * @brief Compare given time.
 *
 * @param[in] newer     newer time value
 * @param[in] older     older time value
 *
 * @return bool
 ****************************************************************************************
 */
static bool ke_time_cmp(uint32_t newer, uint32_t older)
{
    return (((uint32_t)(newer - older) & KE_TIMER_MASK) <= KE_TIMER_DELAY_MAX);
}

/**
 ****************************************************************************************
 * @brief Check if ke time has passed.
 *
 * @param[in] time     time value to check
 *
 * @return if time has passed or not
 ****************************************************************************************
 */
static bool ke_time_past(uint32_t time)
{
    return ke_time_cmp(ke_time(), time);
}

/**
 ****************************************************************************************
 * @brief Compare timer absolute expiration time.
 *
 * @param[in] timerA Timer to compare.
 * @param[in] timerB Timer to compare.
 *
 * @return true if timerA will expire before timerB.
 ****************************************************************************************
 */
static bool cmp_abs_time(struct co_list_hdr const * timerA, struct co_list_hdr const * timerB)
{
    uint32_t timeA = ((struct ke_timer*)timerA)->time;
    uint32_t timeB = ((struct ke_timer*)timerB)->time;

    return (((uint32_t)(timeA - timeB) & KE_TIMER_MASK) > KE_TIMER_DELAY_MAX);
}

/**
 ****************************************************************************************
 * @brief Compare timer and task IDs callback
 *
 * @param[in] timer           Timer value
 * @param[in] timer_task      Timer task
 *
 * @return timer insert
 ****************************************************************************************
 */
static bool cmp_timer_id(struct co_list_hdr const * timer, uint32_t timer_task)
{
    // trick to pack 2 u16 in u32
    ke_msg_id_t timer_id = timer_task >> 16;
    ke_task_id_t task_id = timer_task & 0xFFFF;

    // insert the timer just before the first one older
    return (timer_id == ((struct ke_timer*)timer)->id)
        && (task_id == ((struct ke_timer*)timer)->task);
}

/**
 ****************************************************************************************
 * @brief Schedule the next timer(s).
 *
 * This function pops the first timer from the timer queue and notifies the appropriate
 * task by sending a kernel message. If the timer is periodic, it is set again;
 * if it is one-shot, the timer is freed. The function checks also the next timers
 * and process them if they have expired or are about to expire.
 ****************************************************************************************
 */
static void ke_timer_schedule(void)
{
    struct ke_timer *timer;
    DBG_SWDIAG(EVT, TIMER, 1);
    for(;;)
    {
        ke_event_clear(KE_EVENT_KE_TIMER);

        // check the next timer
        timer = (struct ke_timer*) ke_env.queue_timer.first;

        if(!timer)
        {
            break;
        }

        if (!ke_time_past(timer->time - 1))
        {
            // timer will expire in more than 10ms, configure the HW
            rwip_timer_10ms_set(timer->time);

            // in most case, we will break here. However, if the timer expiration
            // time has just passed, may be the HW was set too late (due to an IRQ)
            // so we do not exit the loop to process it.
            if (!ke_time_past(timer->time))
            {
                break;
            }
            else
            {
                // remove current timer
                rwip_timer_10ms_set(RWIP_INVALID_TARGET_TIME);
            }
        }

        // at this point, the next timer in the queue has expired or is about to -> pop it
        timer = (struct ke_timer*) ke_queue_pop(&ke_env.queue_timer);

        //Trace the kernel timer
        TRC_REQ_KE_TMR_EXP(timer->time, timer->task, timer->id);

        // notify the task
        ke_msg_send_basic(timer->id, timer->task, TASK_NONE);

        // free the memory allocated for the timer
        ke_free(timer);
    }
    DBG_SWDIAG(EVT, TIMER, 0);
}


/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */


void ke_timer_init(void)
{
    // Register timer event
    ke_event_callback_set(KE_EVENT_KE_TIMER, &ke_timer_schedule);
}

uint32_t ke_time(void)
{
    uint32_t res = 0;
    GLOBAL_INT_DISABLE();
    res = ((rwip_time_get().hs >> 5) & KE_TIMER_MASK);
    GLOBAL_INT_RESTORE();
    return (res);
}

void ke_timer_set(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint32_t delay)
{
    struct ke_timer *timer;
    // Indicate if the HW will have to be reprogrammed
    bool hw_prog = false;
    // Timer time
    uint32_t abs_time;

    // Check if requested timer is first of the list of pending timer
    struct ke_timer *first = (struct ke_timer *) ke_env.queue_timer.first;

    // Delay shall not be more than maximum allowed
    if(delay > KE_TIMER_DELAY_MAX)
    {
        delay = KE_TIMER_DELAY_MAX;
    }
    // Delay should not be zero
    else if(delay == 0)
    {
        delay = 1;
    }

    if(first != NULL)
    {
        if ((first->id == timer_id) && (first->task == task_id))
        {
            // Indicate that the HW tid_ke_timemer will have to be reprogrammed
            hw_prog = true;
        }
    }

    // Extract the timer from the list if required
    timer = (struct ke_timer*) ke_queue_extract(&ke_env.queue_timer, cmp_timer_id, (uint32_t)timer_id << 16 | task_id);

    if (timer == NULL)
    {
        // Create new one
        timer = (struct ke_timer*) ke_malloc(sizeof(struct ke_timer), KE_MEM_KE_MSG);
        ASSERT_ERR(timer);
        timer->id = timer_id;
        timer->task = task_id;
    }

    // update characteristics
    abs_time = ke_time() + delay;
    timer->time = abs_time;


    // Mask calculated time to be sure it's not greater than time counter
    timer->time &= KE_TIMER_MASK;

    // insert in sorted timer list
    ke_queue_insert(&ke_env.queue_timer,
                    (struct co_list_hdr*) timer,
                    cmp_abs_time);

    //Trace the kernel timer
    TRC_REQ_KE_TMR_SET(timer->time, task_id, timer_id);

    // retrieve first timer element
    first = (struct ke_timer *) ke_env.queue_timer.first;

    // check if HW timer set needed
    if (hw_prog || (first == timer))
    {
        rwip_timer_10ms_set(first->time);
    }

    // Check that the timer did not expire before HW prog
    if (ke_time_past(abs_time))
    {
        // Timer already expired, so schedule the timer event immediately
        ke_event_set(KE_EVENT_KE_TIMER);
    }
}

void ke_timer_clear(ke_msg_id_t const timer_id, ke_task_id_t const task_id)
{
    struct ke_timer *timer = (struct ke_timer *) ke_env.queue_timer.first;

    if (ke_env.queue_timer.first != NULL)
    {
        if ((timer->id == timer_id) && (timer->task == task_id))
        {
            // timer found and first to expire! pop it
            ke_queue_pop(&ke_env.queue_timer);

            struct ke_timer *first = (struct ke_timer *) ke_env.queue_timer.first;

            // and set the following timer HW if any
            rwip_timer_10ms_set((first) ? first->time : RWIP_INVALID_TARGET_TIME);

            // Check that the timer did not expire before HW prog
            if ((first) && ke_time_past(first->time))
            {
                // Timer already expired, so schedule the timer event immediately
                ke_event_set(KE_EVENT_KE_TIMER);
            }

            // Check that the timer did not expire before HW prog
            //ASSERT_ERR(!ke_time_past(first->time));
        }
        else
        {
            timer = (struct ke_timer *)
                    ke_queue_extract(&ke_env.queue_timer, cmp_timer_id,
                            (uint32_t)timer_id << 16 | task_id);
        }

        if (timer != NULL)
        {
            //Trace the cleared timer
            TRC_REQ_KE_TMR_CLR(timer->time, timer->task, timer->id);

            // free the cleared timer
            ke_free(timer);
        }
    }
}

bool ke_timer_active(ke_msg_id_t const timer_id, ke_task_id_t const task_id)
{
    struct ke_timer *timer;
    bool result;

    // check the next timer
    timer = (struct ke_timer*) ke_env.queue_timer.first;

    /* scan the timer queue to look for a message element with the same id and destination*/
    while (timer != NULL)
    {
        if ((timer->id == timer_id) &&
            (timer->task == task_id) )
        {
            /* Element has been found                                                   */
            break;
        }

        /* Check  next timer                                                            */
        timer = timer->next;
    }

    /* If the element has been found                                                    */
    if (timer != NULL)
        result = true;
    else
        result = false;

    return(result);

}

void ke_timer_adjust_all(uint32_t delay)
{
    struct ke_timer *timer;

    // check the next timer
    timer = (struct ke_timer*) ke_env.queue_timer.first;

    // iterate through timers, adjust
    while (timer != NULL)
    {
        timer->time += delay;
        timer = timer->next;
    }
}


///@} KE_TIMER
