/**
****************************************************************************************
*
* @file timer.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "timer.h"
#include "timer_pub.h"
#include "intc_pub.h"
#include "drv_model_pub.h"
#include "icu_pub.h"
#include <misc/util.h>

#ifdef __HW_TIMER_DRIVER__

typedef struct
{
    UINT8 clk_src;
    UINT8 clk_div;
    UINT8 enable;
    UINT32 t_cnt[TIM_NUM_PER_CHA];
    void (*p_TIMER_Int_Handler[TIM_NUM_PER_CHA])(void);
} PLAT_TIMER_ENV;

static PLAT_TIMER_ENV t_env[PLAT_TIMER_CNT];

static UINT32 timer_ctrl(UINT32 cmd, void *param);

static SDD_OPERATIONS timer_op =
{
    timer_ctrl
};

static DRIVER_CTRL_RES timer_initial(PLAT_TIMER_NUM_E channel, UINT8 src, UINT8 div, UINT8 enable)
{
    icu_clk_pwr_dev dev;
    UINT32* timer_reg;
    UINT32 value;
    PLAT_TIMER_ENV* env;
    UINT8 irq_num;
    DRIVER_CTRL_RES ret = DRIV_SUCCESS;

    switch(channel)
    {
        case PLAT_TIMER0:
            dev = CLK_PWR_DEV_TIMER0;
            env = &t_env[0];
            irq_num = IRQ_TIMER0;
            break;
        case PLAT_TIMER1:
            dev = CLK_PWR_DEV_TIMER1;
            env = &t_env[1];
            irq_num = IRQ_TIMER1;
            break;
        default:
            ret = DRIV_WRONG_PARA;
            return ret;
    }

    if(enable == DRIVER_ENABLE && src != TIMER_SRC_32K && src != TIMER_SRC_16M)
    {
        ret = DRIV_WRONG_PARA;
        return ret;
    }

    if(enable == DRIVER_ENABLE)
    {
        TIMER_SRC_SEL_C prm;
        prm.num = channel;
        prm.src = src;

        ret = sddev_control(ICU_DEV_NAME, CMD_TIMER_CLK_SEL, (void *)&prm);

        TIMER_CHAN_E n;
        
        for(n = 0; n < 3; n++)
        {
            REG_TIMERx_VAL(timer_reg, n + channel*TIM_NUM_PER_CHA);
            value = 0;
            REG_WRITE(timer_reg, value);
        }

        REG_TIMERx_CFG(timer_reg, channel*TIM_NUM_PER_CHA);
        value = REG_READ(timer_reg);
        value &= ~MASK_TIMERx_CLK_DIV;
        value |= (div << POSE_TIMERx_CLK_DIV);
        REG_WRITE(timer_reg, value);

        ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&dev);
        intc_enable(irq_num);
    }
    else
    {
        ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&dev);
        intc_disable(irq_num);
    }

    env->clk_src = src;
    env->clk_div = div;
    env->enable = enable;

    return ret;
}

static DRIVER_CTRL_RES timer_start(TIMER_CHAN_E port, UINT32 cnt, void* cb)
{
    DRIVER_CTRL_RES ret = DRIV_SUCCESS;
    UINT32 count = cnt * TIMER_CNT_TO_US;
    UINT32* timer_reg;
    UINT32 value;
    PLAT_TIMER_ENV* env;
    PLAT_TIMER_NUM_E channel = port/TIM_NUM_PER_CHA;
    UINT8 seq = port%TIM_NUM_PER_CHA;

    env = &t_env[channel];

    REG_TIMERx_VAL(timer_reg, port);
    value = count;
    REG_WRITE(timer_reg, value);

    env->t_cnt[seq] = count;

    REG_TIMERx_CFG(timer_reg, port);
    value = REG_READ(timer_reg);
    value |= 0x01 << seq;
    REG_WRITE(timer_reg, value);

    if(cb)
    {
        env->p_TIMER_Int_Handler[seq] = cb;
    }
    else
    {
        env->p_TIMER_Int_Handler[seq] = NULL;
    }

    return ret;
}

static DRIVER_CTRL_RES timer_stop(TIMER_CHAN_E port)
{
    DRIVER_CTRL_RES ret = DRIV_SUCCESS;
    PLAT_TIMER_NUM_E channel = port/TIM_NUM_PER_CHA;
    UINT8 seq = port%TIM_NUM_PER_CHA;
    UINT32* timer_reg;
    UINT32 value;

    REG_TIMERx_CFG(timer_reg, port);
    value = REG_READ(timer_reg);
    value &= ~(0x01 << seq);
    REG_WRITE(timer_reg, value);

    return ret;
}

static void timer_isr(void)
{
    UINT32 t0IntSta, t1IntSta;
    UINT8 i;

    t0IntSta = REG_READ(REG_TIMER0_CFG);
    t1IntSta = REG_READ(REG_TIMER1_CFG);

    for(i = 0; i < TIM_NUM_PER_CHA; i++)
    {
        if((t0IntSta >> POSE_TIMER_INT_START) & (0x01 << i))
        {
            if(t_env[0].p_TIMER_Int_Handler[i])
            {
                t_env[0].p_TIMER_Int_Handler[i]();
            }
        }

        if((t1IntSta >> POSE_TIMER_INT_START) & (0x01 << i))
        {
            if(t_env[1].p_TIMER_Int_Handler[i])
            {
                t_env[1].p_TIMER_Int_Handler[i]();
            }           
        }
    }
    
    REG_WRITE(REG_TIMER0_CFG, t0IntSta);
    REG_WRITE(REG_TIMER1_CFG, t1IntSta);
}

void timer_init(void)
{
    intc_service_register(IRQ_TIMER0, PRI_FIQ_TIMER0, timer_isr);
    intc_service_register(IRQ_TIMER1, PRI_FIQ_TIMER1, timer_isr);

    sddev_register_dev(TIMER_DEV_NAME, &timer_op);

    memset(t_env, 0, ARRAY_SIZE(t_env));
}

void timer_exit(void)
{
    sddev_unregister_dev(TIMER_DEV_NAME);
}

static UINT32 timer_ctrl(UINT32 cmd, void *param)
{
    DRIVER_CTRL_RES ret = DRIV_SUCCESS;

    switch(cmd)
    {
        case CMD_TIMER_UNIT_ENABLE:
            ret = timer_initial(((cmd_timer_init_p*)param)->channel, ((cmd_timer_init_p*)param)->src, ((cmd_timer_init_p*)param)->div, ((cmd_timer_init_p*)param)->enable);
            break;

        case CMD_TIMER_START:
            ret = timer_start(((cmd_timer_port_p*)param)->t_num, ((cmd_timer_port_p*)param)->cnt, ((cmd_timer_port_p*)param)->p_TIMER_Int_Handler);
            break;

        case CMD_TIMER_STOP:
            ret = timer_stop(((cmd_timer_port_p*)param)->t_num);
            break;
    }

}

#endif __HW_TIMER_DRIVER__