/**
****************************************************************************************
*
* @file icu.h
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifndef _ICU_H_
#define _ICU_H_

#include "BK3633_RegList.h"

//#define ICU_DEBUG


#ifdef ICU_DEBUG
#define ICU_PRT      os_printf
#define ICU_WARN     warning_prf
#define ICU_FATAL    fatal_prf
#else
#define ICU_PRT      null_prf
#define ICU_WARN     null_prf
#define ICU_FATAL    null_prf
#endif


#define REG_SYS_PWD                                 &(addSYS_Reg0x3)
#define REG_SYS_CLK_SEL                             &(addSYS_Reg0x4)
#define REG_SYS_INT_EN                              &(addSYS_Reg0x10)
#define REG_ICU_INT_FLAG                            &(addSYS_Reg0x12)
#define REG_ICU_IRQ_ENABLE                          &(addSYS_Reg0x11)
#define REG_SYS_TEST                                &(addSYS_Reg0x17)
#define REG_CLK_GATE_DISABLE                        &(addSYS_Reg0x1e)


//REG0
#define JTAG_MODE_CLOSE                              clrf_SYS_Reg0x0_jtag_mode


//REG2
#define ICU_CORE_SEL(val)                            set_SYS_Reg0x2_core_sel(val)


//REG3
#define ICU_UART1_CLK_PWD_POSI                        (posSYS_Reg0x3_uart0_pwd)
#define ICU_UART1_CLK_PWD_MASK                        (0x01UL << ICU_UART1_CLK_PWD_POSI)

#define ICU_UART2_CLK_PWD_POSI                        (posSYS_Reg0x3_UART2_pwd)
#define ICU_UART2_CLK_PWD_MASK                        (0x01UL << ICU_UART2_CLK_PWD_POSI)

#define ICU_TIMER0_CLK_PWD_POSI                        (posSYS_Reg0x3_tim0_pwd)
#define ICU_TIMER0_CLK_PWD_MASK                        (0x01UL << ICU_TIMER0_CLK_PWD_POSI)

#define ICU_TIMER1_CLK_PWD_POSI                        (posSYS_Reg0x3_tim1_pwd)
#define ICU_TIMER1_CLK_PWD_MASK                        (0x01UL << ICU_TIMER1_CLK_PWD_POSI)

#define ICU_PWM0_CLK_PWD_POSI                        (posSYS_Reg0x3_pwm0_pwd)
#define ICU_PWM0_CLK_PWD_MASK                        (0x01UL << ICU_PWM0_CLK_PWD_POSI)

#define ICU_SPI_CLK_PWD_POSI                         (posSYS_Reg0x3_spi_pwd)
#define ICU_SPI_CLK_PWD_MASK                         (0x01UL << ICU_SPI_CLK_PWD_POSI)

#define ICU_PWM1_CLK_PWD_POSI                        (posSYS_Reg0x3_pwm1_pwd)
#define ICU_PWM1_CLK_PWD_MASK                        (0x01UL << ICU_PWM1_CLK_PWD_POSI)

#define ICU_WDT_CLK_PWD_POSI                         (posSYS_Reg0x3_wdt_pwd)
#define ICU_WDT_CLK_PWD_MASK                         (0x01UL << ICU_WDT_CLK_PWD_POSI)

#define ICU_I2S_CLK_PWD_POSI                         (posSYS_Reg0x3_i2s_pwd)
#define ICU_I2S_CLK_PWD_MASK                         (0x01UL << ICU_I2S_CLK_PWD_POSI)



//REG4
#define SYS_TIMER0_SEL_POS                       	 posSYS_Reg0x4_tim0_sel

#define SYS_TIMER1_SEL_POS                       	 posSYS_Reg0x4_tim1_sel

#define SYS_PWM0_SEL_POS                             posSYS_Reg0x4_pwm0_sel

#define SYS_PWM1_SEL_POS                             posSYS_Reg0x4_pwm1_sel

#define SYS_SPI_SEL_POS                       	     posSYS_Reg0x4_spi_sel


//REGb
#define SET_PWD_BOOST                                setf_SYS_Reg0xb_pwd_on_boostsel


//REG10
#define SYS_PWM1_INT_EN_POS                          posSYS_Reg0x10_int_pwm1_en


//REG17
#define POS_ENB_BURST_SEL                            posSYS_Reg0x17_enb_busrt_sel

/*******************************************************************************
* Function Declarations
*******************************************************************************/

#endif //_ICU_H_

