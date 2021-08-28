/**
****************************************************************************************
*
* @file hal_timer.c
*
* @brief timer function user interface
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "hw_timer.h"
#include "uart_pub.h"

#ifdef __HW_TIMER_DRIVER__

DRIVER_CTRL_RES hw_timer_init(hw_timer_group_t* t, UINT8 enable)
{
    cmd_timer_init_p para;
    DRIVER_CTRL_RES ret;

    para.channel = t->channel;
    para.enable = enable;
    para.src = t->config.src;
    para.div = t->config.div;

    ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_ENABLE, &para);

    return ret;
}

DRIVER_CTRL_RES hw_timer_port_start(hw_timer_port_t* p)
{
    cmd_timer_port_p para;
    DRIVER_CTRL_RES ret;

    para.t_num = p->port;
    para.cnt = p->config.cnt;
    para.p_TIMER_Int_Handler = p->config.cb;

    ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_START, &para);

    return ret;
}

DRIVER_CTRL_RES hw_timer_port_stop(hw_timer_port_t* p)
{
    cmd_timer_port_p para;
    DRIVER_CTRL_RES ret;

    para.t_num = p->port;

    ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_STOP, &para);

    return ret;
}

#endif  //__HW_TIMER_DRIVER__