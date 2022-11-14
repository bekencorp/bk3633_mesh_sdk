/**
 ****************************************************************************************
 *
 * @file intc.c
 *
 * @brief Definition of the Interrupt Controller (INTCTRL) API.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "compiler.h"
#include "intc.h"
#include "intc_pub.h"

#include "include.h"
#include "arm_arch.h"
#include "drv_model_pub.h"
#include "icu_pub.h"
#include "mem_pub.h"
#include "uart_pub.h"
#if CFG_SUPPORT_ALIOS
#include "ll.h"

extern void do_irq( void );
extern void do_fiq( void );
extern void do_swi( void );
#else
#include "power_save_pub.h"
#endif

#include "user_config.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void intc_spurious(void)
{
    ASSERT(0);
}

void intc_enable(int index)
{
    UINT32 param;

    param = (1UL << index);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

void intc_disable(int index)
{
    UINT32 param;

    param = (1UL << index);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

void rf_ps_wakeup_isr_idle_int_cb()
{
#if ( CONFIG_APP_MP3PLAYER == 1 )
    UINT32 irq_status;

    irq_status = sddev_control(ICU_DEV_NAME, CMD_GET_INTR_STATUS, 0);

    if(irq_status & 1<<IRQ_I2S_PCM)
    {
    irq_status &= 1<<IRQ_I2S_PCM;
    i2s_isr();
    sddev_control(ICU_DEV_NAME, CMD_CLR_INTR_STATUS, &irq_status);
    }
#endif    
}
#include "rwip.h"
#include "reg_intc.h"
#define INT_STATUS_RWBLE_bit     (0x01<<20)
#define REG_ICU_INT_FLAG                            &(addSYS_Reg0x12)
void intc_irq(void)
{
#if 0
    UINT32 irq_status;
    INTC_PRT("%s\n", __func__);
    irq_status = sddev_control(ICU_DEV_NAME, CMD_GET_INTR_STATUS, 0);
    //irq_status = irq_status & 0xFFFF;
	if(0 == irq_status & 0x7FFE)
	{
	    #if (! CFG_USE_STA_PS)
		INTC_PRT("irq:dead,%0x%x\r\n", irq_status);
        #endif
	}
	INTC_PRT("%s status 0x%x\n", __func__, irq_status);
    //irq_status &= 0x7FFF;
    sddev_control(ICU_DEV_NAME, CMD_CLR_INTR_STATUS, &irq_status);

    intc_hdl_entry(irq_status);
#endif 
    
	uint32_t irq_status = 0;
	uint32_t IntStat = intc_status_get();

    // call the function handler
	if(IntStat & INT_STATUS_UART0_bit)
	{
		irq_status |= INT_STATUS_UART0_bit;
        uart1_isr();
	}
    if(IntStat & INT_STATUS_UART2_bit)
	{
		irq_status |= INT_STATUS_UART2_bit;
        uart2_isr();
	}
	
#if (ADC_DRIVER)
		if(IntStat & INT_STATUS_ADC_bit)
		{
			irq_status |= INT_STATUS_ADC_bit;
			adc_isr();
		}
#endif
#if (AON_RTC_DRIVER)
	if(IntStat & INT_STATUS_AON_RTC_bit)
	{
		irq_status |= INT_STATUS_AON_RTC_bit;
		aon_rtc_isr();
	}
#endif


#if (GPIO_DRIVER)
	if(IntStat & INT_STATUS_AON_GPIO_bit)
	{
		irq_status |= INT_STATUS_AON_GPIO_bit;
		gpio_isr();
	}
#endif
#if (SPI_DRIVER)
    if(IntStat & INT_STATUS_SPI0_bit)
    {
        irq_status |= INT_STATUS_SPI0_bit;
        spi_isr();
    }
#endif
#if (I2C_DRIVER)	
    if(IntStat & INT_STATUS_I2C0_bit)
    {
        irq_status |= INT_STATUS_I2C0_bit;
        i2c_isr();
    }
#endif
#if (PWM_DRIVER)
    if(IntStat & INT_STATUS_PWM0_bit)
    {
        irq_status |= INT_STATUS_PWM0_bit;
        pwm_isr();
    }
    if(IntStat & INT_STATUS_PWM1_bit)
    {
        irq_status |= INT_STATUS_PWM1_bit;
        pwm_isr();
    }
#endif

#if(USB_DRIVER)
    if(IntStat & INT_STATUS_USB_bit)
    {
    	irq_status |= INT_STATUS_USB_bit;
    	USB_InterruptHandler();
    }
#endif

    if(IntStat & BIT(22))  //BIT
    {
        irq_status |= BIT(22); //INT_STATUS_RWDM_bit
        rwip_func.rwip_isr();
    }
    
    if(IntStat & BIT(20))  
    {
        irq_status |= INT_STATUS_RWBLE_bit;
        rwip_func.rwble_isr();
    }

	intc_status_clear(irq_status);
    
}

void intc_fiq(void)
{
    uint32_t IntStat;
    UINT32 fiq_status = 0;

    IntStat = intc_status_get();
    if(IntStat & BIT(22))  //BIT
    {
        fiq_status |= BIT(22); //INT_STATUS_RWDM_bit
        rwip_func.rwip_isr();
    }
    
    if(IntStat & BIT(20))  
    {
        fiq_status |= INT_STATUS_RWBLE_bit;
        rwip_func.rwble_isr();
    }
    intc_status_clear(fiq_status);
}

void intc_init(void)
{
    UINT32 param;
    INTC_PRT("%s\r\n", __func__);

#if CFG_SUPPORT_ALIOS
    *((volatile uint32_t *)0x400004) = &do_irq;
    *((volatile uint32_t *)0x400008) = &do_fiq;
    *((volatile uint32_t *)0x40000C) = &do_swi;
#endif
  
//sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_ENABLE, &param);
    INTC_PRT("SYS_Reg0x10:%x,SYS_Reg0x11:%x\r\n",addSYS_Reg0x10,addSYS_Reg0x11);
    addSYS_Reg0x10 = 0; // int enable 0:disable 1::enable
    addSYS_Reg0x11 = 0; // priority; 0: irq  1:fiq

    // setf_SYS_Reg0x11_int_rwble_pri; // 1:fiq
    // setf_SYS_Reg0x11_int_rwdm_pri; // 1:fiq
    // setf_SYS_Reg0x11_int_rwbt_pri; // 1:fiq
    return;
}

void intc_deinit(void)
{
    UINT32 param;
	
    for( int i = 0; i<=FIQ_IRQ_END; i++)
	{
        intc_disable(i);
	}
	
    param = GINTR_FIQ_BIT | GINTR_IRQ_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_DISABLE, &param);

    return;
}

/// @}
