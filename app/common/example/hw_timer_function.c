/**
****************************************************************************************
*
* @file hw_timer_function.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifdef __HW_TIMER_DRIVER__

hw_timer_group_t t_g;
hw_timer_port_t t_p;

void t_timer_cb(void)
{
    os_printf("~~~~~~~~timer_cb stop\r\n");
    hw_timer_port_stop(&t_p);
}

void _hw_timer_init(void)
{
    t_g.channel = PLAT_TIMER1;
    t_g.config.src = TIMER_SRC_16M;
    t_g.config.div = 15;
    hw_timer_init(&t_g, DRIVER_ENABLE);
}

void _hw_timer_start(void)
{
    t_p.port = TIMER5;
    t_p.config.cnt = 20;            //cnt in 100us
    t_p.config.cb = t_timer_cb;
    hw_timer_port_start(&t_p);
}

#endif  //__HW_TIMER_DRIVER__