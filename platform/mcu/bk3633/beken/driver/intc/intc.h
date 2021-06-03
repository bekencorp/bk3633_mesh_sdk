/**
 ****************************************************************************************
 *
 * @file intc.h
 *
 * @brief Declaration of the Reference Interrupt Controller (INTC) API.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _INTC_H_
#define _INTC_H_

#include "compiler.h"
#include "doubly_list.h"
#include "generic.h"
#include "BK3633_RegList.h"

//#define INTC_DEBUG

#ifdef INTC_DEBUG
#define INTC_PRT       os_printf
#define INTC_WPRT      warning_prf
#else
#define INTC_PRT       os_null_printf
#define INTC_WPRT      os_null_printf
#endif

#define INTC_MAX_COUNT                    32

//IRQ
#define INT_STATUS_PWM0_bit      (0x01<<0)
#define INT_STATUS_PWM1_bit      (0x01<<1)
#define INT_STATUS_TMR0_bit      (0x01<<2)
#define INT_STATUS_TMR1_bit      (0x01<<3)
#define INT_STATUS_UART0_bit     (0x01<<4)
#define INT_STATUS_UART2_bit     (0x01<<5)
#define INT_STATUS_SPI0_bit      (0x01<<6)
#define INT_STATUS_I2C0_bit      (0x01<<7)
#define INT_STATUS_ADC_bit       (0x01<< 8)
#define INT_STATUS_AON_GPIO_bit  (0x01<< 9)
#define INT_STATUS_RTC_bit       (0x01<< 10)
#define INT_STATUS_I2S_bit       (0x01<< 11)
#define INT_STATUS_AON_RTC_bit   (0x01<< 12)

typedef struct
{
    LIST_HEADER_T list;

    FUNCPTR isr_func;

    UINT8  pri;            // priority
    UINT8  int_num;
    UINT16 resv;
} ISR_T;

typedef struct
{
    LIST_HEADER_T isr;

    UINT8 max;     // maximum interrupt number
    UINT8 min;     // minum interrupt number
} ISR_LIST_T;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize and configure the reference INTCTRL.
 * This function configures the INTC according to the system needs.
 ****************************************************************************************
 */
void intc_init(void);
void intc_irq(void);
void intc_fiq(void);

#endif // _INTC_H_
