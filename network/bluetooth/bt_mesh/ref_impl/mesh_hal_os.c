/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef CONFIG_MESH_STACK_ALONE

#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_CORE)

#include <common/log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "atomic.h"
#include "errno.h"

#ifdef OSAL_RHINO
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_ringbuf.h>
#include <k_obj.h>
#include <k_sem.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_stats.h>
#include <k_time.h>
#include <k_task.h>
#include <port.h>
#endif

#include <net/buf.h>

int k_sem_init(struct k_sem *sem, unsigned int initial_count,
               unsigned int limit)
{
    int ret;
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = krhino_sem_create(&sem->sem, "ble", initial_count);
    return ret;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    tick_t ticks;

    if (timeout == K_FOREVER) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(timeout);
    }
    return krhino_sem_take(&sem->sem, ticks);
}

int k_sem_give(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    krhino_sem_give(&sem->sem);
    return 0;
}

int k_sem_delete(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    krhino_sem_del(&sem->sem);
    return 0;
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
#ifdef OSAL_RHINO
    sem_count_t count;

    krhino_sem_count_get(&sem->sem, &count);
    return (int)count;
#endif
    return 0;
}

int64_t k_uptime_get()
{
    return aos_now_ms();
}

u32_t k_uptime_get_32()
{
    return (u32_t)aos_now_ms();
}

typedef void (*task_entry_t)(void *args);
int k_thread_create(struct k_thread *new_thread, const name_t *name, k_thread_stack_t *stack,
                    size_t stack_size, k_thread_entry_t entry, void *p1,
                    void *p2, void *p3, int prio, u32_t options, s32_t delay)
{
    if (!stack) { // Use dynamic stack if no static one provided
        int ret;

        ret = aos_task_new_ext(&(new_thread->task), "ble", (task_entry_t)entry, p1,
                               stack_size, prio);
        if (ret) {
            SYS_LOG_ERR("create ble task fail\n");
        }

        return ret;
    } else { // Use static statck if provided
        ktask_t *task_obj;

        task_obj = krhino_mm_alloc(sizeof(ktask_t));
        if (task_obj == NULL) {
            return RHINO_NO_MEM;
        }

        /* Free the below handle whenever task exits. */
        new_thread->task.hdl = (void *)task_obj;

        stack_size = stack_size / sizeof(cpu_stack_t);
        return krhino_task_create(task_obj, "ble", p1, prio, 0,
                                  (cpu_stack_t *)stack,
                                  stack_size ,
                                  (task_entry_t)entry,
                                  delay == 0 ? 1: 0);
    }
}

int k_yield(void)
{
    return 0;
}

unsigned int irq_lock(void)
{
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    return cpsr;
}

void irq_unlock(unsigned int key)
{
    CPSR_ALLOC();
    cpsr = key;
    RHINO_CPU_INTRPT_ENABLE();
}

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    int ret;
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,handle %p,args %p", timer, handle, args);
    timer->handler = handle;
    timer->args    = args;
    ret =
      krhino_timer_create(&timer->timer, "AOS", (timer_cb_t)(timer->handler),
                          krhino_ms_to_ticks(1000), 0, args, 0);
    if (ret) {
        BT_DBG("fail to create a timer");
    }
}

void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    int ret;
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,timeout %u", timer, timeout);
    timer->timeout  = timeout;
    timer->start_ms = aos_now_ms();

    ret = krhino_timer_stop(&timer->timer);
    if (ret) {
        BT_DBG("fail to stop timer");
    }

    ret = krhino_timer_change(&timer->timer, krhino_ms_to_ticks(timeout),
                              krhino_ms_to_ticks(timeout));
    if (ret) {
        BT_DBG("fail to change timeout");
    }

    ret = krhino_timer_start(&timer->timer);
    if (ret) {
        BT_DBG("fail to start timer");
    }
}

void k_timer_stop(k_timer_t *timer)
{
    int ret;
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p", timer);
    ret = krhino_timer_stop(&timer->timer);
    timer->timeout = 0;
    if (ret) {
        BT_DBG("fail to stop timer");
    }
}

void k_sleep(s32_t duration)
{
    aos_msleep(duration);
}

long long k_now_ms()
{
    return aos_now_ms();
}

int k_event_create(kevent_t *event, const name_t *name, uint32_t flags)
{
    kstat_t ret;

    ret = krhino_event_create(event, name, flags);
    if (ret) {
        BT_ERR("event create failed");
    }
    return ret;
}

int k_event_delete(kevent_t *event)
{
    kstat_t ret;

    ret = krhino_event_del(event);
    if (ret) {
        BT_ERR("event delete failed");
    }
    return ret;
}

int k_event_get(kevent_t *event, uint32_t flags, uint32_t *actl_flags, int32_t ticks) 
{
    kstat_t ret;
    
    ret = krhino_event_get(event, flags, RHINO_OR_CLEAR, actl_flags, ticks);
    if (ret) {
        BT_ERR("event get failed %d", ret);
    }
    return ret;
}

void k_event_set(kevent_t *event, uint32_t flags)
{
    kstat_t ret;

    krhino_event_set(event, flags, RHINO_OR);
    if (ret) {
        BT_ERR("event set failed %d", ret);
    }
    return ret;
}
#endif
