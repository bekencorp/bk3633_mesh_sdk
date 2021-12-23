/**
 ****************************************************************************************
 *
 * @file bim_intc.c
 *
 * @brief Definition of the Interrupt Controller (INTCTRL) API.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */
#include <stdint.h>
//#include "compiler.h"
//#include "BK3633_RegList.h"
/*****************************************************/
//              don't change this !!!!!!!
#define ISR_MAX_NUM  2
#define ISR_FIQ_ISR_IDX  0
#define ISR_IRQ_ISR_IDX  1
#define ISR_MAX_NUM_PROTOCOL_USERS  19
#define PWM0_FIQ_ISR_IDX                                0
#define PWM1_IRQ_ISR_IDX                                1
#define PWM2_IRQ_ISR_IDX                                2
#define PWM3_IRQ_ISR_IDX                                3
#define PWM4_IRQ_ISR_IDX                                4
#define UART_IRQ_ISR_IDX                                5
#define SPI_IRQ_ISR_IDX                                 6
#define I2C_IRQ_ISR_IDX                                 7
#define ADC_IRQ_ISR_IDX                                 8
#define GPIO_IRQ_ISR_IDX                                9
#define RES_ISR_IDX0                                    10
#define USB_IRQ_ISR_IDX                                 11
#define RTC_IRQ_ISR_IDX                                 12
#define PWM5_IRQ_ISR_IDX                                13
#define PWM_3DSIRQ_ISR_IDX                              14
#define BLE_FIQ_ISR_IDX                                 15
#define UART2_IRQ_ISR_IDX                               16
#define SDM_IRQ_ISR_IDX                                 17
#define I2S_IRQ_ISR_IDX                                 18
/***************************************************/
