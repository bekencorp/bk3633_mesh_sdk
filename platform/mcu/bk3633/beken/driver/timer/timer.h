/**
****************************************************************************************
*
* @file timer.h
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifndef _TIMER_H_
#define _TIMER_H_

//#define TIMER_DEBUG


#ifdef TIMER_DEBUG
#define TIMER_PRT      os_printf
#define TIMER_WARN     warning_prf
#define TIMER_FATAL    fatal_prf
#else
#define TIMER_PRT      null_prf
#define TIMER_WARN     null_prf
#define TIMER_FATAL    null_prf
#endif


#define REG_TIMER0_BASE     BASEADDR_TIMER0
#define REG_TIMER0_CFG      &(addTIMER0_Reg0x3)


#define REG_TIMER1_BASE     BASEADDR_TIMER1
#define REG_TIMER1_CFG      &(addTIMER1_Reg0x3)

#define POSE_TIMERx_CLK_DIV     posTIMER0_Reg0x3_clk_div
#define MASK_TIMERx_CLK_DIV     bitTIMER0_Reg0x3_clk_div

#define POSE_TIMER_INT_START    posTIMER0_Reg0x3_timer0_int


#define REG_TIMERx_VAL(dest_reg, idx)      dest_reg = ((idx>=TIMER3?REG_TIMER1_BASE:REG_TIMER0_BASE) + (idx%TIMER3)* 0x04)
#define REG_TIMERx_CFG(dest_reg, idx)      dest_reg = (idx>=TIMER3?REG_TIMER1_CFG:REG_TIMER0_CFG)


#endif      //_TIMER_H_