/**
****************************************************************************************
*
* @file pwm.h
*
* @brief pub definations
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/


#ifndef _PWM_H_
#define _PWM_H_

#include "BK3633_RegList.h"

//#define PWM_DEBUG

#ifdef PWM_DEBUG
#define PWM_PRT      os_printf
#define PWM_WARN     warning_prf
#define PWM_FATAL    fatal_prf
#else
#define PWM_PRT      null_prf
#define PWM_WARN     null_prf
#define PWM_FATAL    null_prf
#endif

#define REG_APB1_PWM0_CAP_OUT                        &addPWM0_Reg0x4
#define REG_APB1_PWM1_CAP_OUT                        &addPWM0_Reg0x7
#define REG_APB1_PWM2_CAP_OUT                        &addPWM0_Reg0xa
#define REG_APB1_PWM3_CAP_OUT                        &addPWM1_Reg0x4
#define REG_APB1_PWM4_CAP_OUT                        &addPWM1_Reg0x7
#define REG_APB1_PWM5_CAP_OUT                        &addPWM1_Reg0xa


/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 pwm_ctrl(UINT32 cmd, void *param);

#endif //_PWM_H_
