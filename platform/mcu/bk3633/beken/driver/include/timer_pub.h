/**
 ****************************************************************************************
 *
 * @file timer_pub.h
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken Leonardo 2021
 *
 *
 ****************************************************************************************
 */

#ifndef _TIMER_PUB_H_
#define _TIMER_PUB_H_

#include "driver_pub.h"

#define TIMER_DEV_NAME               ("timer")


#define TIMER_SRC_32K       0
#define TIMER_SRC_16M       1

#define TIMER_CNT_TO_US     100

typedef enum
{
    CMD_TIMER_UNIT_ENABLE = TIMER_CMD_MAGIC + 0,
    CMD_TIMER_START,
    CMD_TIMER_STOP,
} PLAT_TIMER_CMD_E;

typedef enum
{
    TIMER0 = 0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER_CNT,
} TIMER_CHAN_E;

typedef enum
{
    PLAT_TIMER0 = 0,
    PLAT_TIMER1,
    PLAT_TIMER_CNT,
} PLAT_TIMER_NUM_E;

#define TIM_NUM_PER_CHA     (TIMER_CNT/PLAT_TIMER_CNT)

typedef struct
{
    PLAT_TIMER_NUM_E num;
    UINT8            src;
} TIMER_SRC_SEL_C;

typedef struct
{
    PLAT_TIMER_NUM_E   channel;
    UINT8              enable;
    UINT8              src;
    UINT8              div;
} cmd_timer_init_p;

typedef struct
{
    TIMER_CHAN_E   t_num;
    UINT32         cnt;
    void (*p_TIMER_Int_Handler)(void);
} cmd_timer_port_p;



void timer_init(void);
void timer_exit(void);

#endif  //_TIMER_PUB_H_