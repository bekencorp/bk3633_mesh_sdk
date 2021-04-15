/**
 ****************************************************************************************
 *
 * @file hal_timer.h
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken Leonardo 2021
 *
 *
 ****************************************************************************************
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include "timer_pub.h"

typedef void (*hw_timer_cb_t)(void *arg);

typedef struct {
    UINT8        src;   /*us*/
    UINT8        div;
    void         *arg;
} timer_group_config_t;

typedef struct {
    PLAT_TIMER_NUM_E       channel;   /* timer port */
    timer_group_config_t   config; /* timer config */
} hw_timer_group_t;

typedef struct {
    UINT32        cnt;   /*us*/
    hw_timer_cb_t cb;
    void         *arg;
} timer_port_config_t;

typedef struct {
    TIMER_CHAN_E       port;   /* timer port */
    timer_port_config_t   config; /* timer config */
} hw_timer_port_t;


DRIVER_CTRL_RES hw_timer_init(hw_timer_group_t* t, UINT8 enable);

#endif  //_HW_TIMER_H_