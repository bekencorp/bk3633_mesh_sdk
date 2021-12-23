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
#include "bim_intc.h"
#include <string.h>
#include "bim_uart2.h"
#include "bim_wdt.h"
typedef void (*FUNCPTR_T)(void);
void Undefined_Exception(void)
{
    wdt_enable(0X10);
    while(1)
    {
        //uart_putchar("Undefined_Exception\r\n");
    }
}
void SoftwareInterrupt_Exception(void)
{
    wdt_enable(0X10);
    while(1)
    {
        //uart_putchar("SoftwareInterrupt_Exception\r\n");
    }
}
void PrefetchAbort_Exception(void)
{
    wdt_enable(0X10);
    while(1)
    {
        //uart_putchar("PrefetchAbort_Exception\r\n");
    }
}
void DataAbort_Exception(void)
{
    wdt_enable(0X10);
    while(1)
    {
        //uart_putchar("DataAbort_Exception\r\n");
    }
}

void Reserved_Exception(void)
{
    wdt_enable(0X10);
    while(1)
    {
        //uart_putchar("Reserved_Exception\r\n");
    }
}

#pragma ARM
void Irq_Exception(void)
{
    bim_printf("%s ========\n", __func__);
#if 0
    uint32_t IntStat=REG_AHB0_ICU_INT_FLAG;
    
    if(IntStat & INT_STATUS_UART_bit)
        bim_uart_isr();


    REG_AHB0_ICU_INT_FLAG=IntStat;
#endif
}
