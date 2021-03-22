/**
****************************************************************************************
*
* @file icu_pub.h
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#ifndef _ICU_PUB_H_
#define _ICU_PUB_H_

#include "typedef.h"

#define ICU_FAILURE                (1)
#define ICU_SUCCESS                (0)

#define ICU_DEV_NAME                "icu"

#define ICU_CMD_MAGIC              (0xe220000)
enum
{
    CMD_ICU_CLKGATING_DISABLE = ICU_CMD_MAGIC + 1,
    CMD_ICU_CLKGATING_ENABLE,
    CMD_ICU_MCU_CLK_SEL,
    CMD_ICU_CLK_32K_SRC_SEL,
    CMD_ICU_INT_DISABLE,
    CMD_ICU_INT_ENABLE,
    CMD_ICU_GLOBAL_INT_DISABLE,
    CMD_ICU_GLOBAL_INT_ENABLE,
    CMD_GET_INTR_STATUS,
    CMD_CLR_INTR_STATUS,
    CMD_GET_INTR_RAW_STATUS,
    CMD_CLR_INTR_RAW_STATUS,
    CMD_CLK_PWR_DOWN,
    CMD_CLK_PWR_UP,
    CMD_CONF_PWM_PCLK,
    CMD_CONF_PWM_LPOCLK,
	CMD_SPI_CLK_SEL,
    CMD_SET_SLEEP_MODE,
    CMD_GET_SLEEP_MODE,
};


/* 1: power down; 0: power up */
#define SYS_PWD_I2S_BIT                                    (1 << 19)
#define SYS_PWD_DMA_BIT                                    (1 << 18)
#define SYS_PWD_RWBT_BIT                                   (1 << 17)
#define SYS_PWD_BK24_BIT                                   (1 << 16)
#define SYS_PWD_WDT_BIT                                    (1 << 15)
#define SYS_PWD_EFUSE_BIT                                  (1 << 14)
#define SYS_PWD_SPI_BIT                                    (1 << 13)
#define SYS_PWD_I2C_BIT                                    (1 << 12)
#define SYS_PWD_USB_BIT                                    (1 << 11)
#define SYS_PWD_SADC_BIT                                   (1 << 10)
#define SYS_PWD_RTC_BIT                                    (1 << 8)
#define SYS_PWD_PWM1_BIT                                   (1 << 6)
#define SYS_PWD_PWM0_BIT                                   (1 << 4)
#define SYS_PWD_TIMER1_BIT                                 (1 << 3)
#define SYS_PWD_TIMER0_BIT                                 (1 << 2)
#define SYS_PWD_UART1_BIT                                  (1 << 1)
#define SYS_PWD_UART0_BIT                                  (1 << 0)
/* CMD_ICU_CLKGATING_DISABLE CMD_ICU_CLKGATING_ENABLE */
#define CLKGATE_PWM_BIT                      (1 << 9)
#define CLKGATE_XVR_BIT                      (1 << 8)
#define CLKGATE_SADC_BIT                     (1 << 7)
#define CLKGATE_RTC_BIT                      (1 << 6)
#define CLKGATE_GPIO_BIT                     (1 << 5)
#define CLKGATE_I2C_BIT                      (1 << 4)
#define CLKGATE_SPI_BIT                      (1 << 3)
#define CLKGATE_UART_BIT                     (1 << 2)
#define CLKGATE_WATCHDOG_BIT                 (1 << 1)
#define CLKGATE_APB_BIT                      (1 << 0)

/* CMD_ICU_MCU_CLK_SEL */
#define ICU_MCU_CLK_SEL_16M                  (1)
#define ICU_MCU_CLK_SEL_32M                  (2)
#define ICU_MCU_CLK_SEL_48M                  (3)
#define ICU_MCU_CLK_SEL_64M                  (4)
#define ICU_MCU_CLK_SEL_80M                  (5)

/* CMD_CLK_32K_SRC_SEL */
#define CLK_32K_SRC_SEL_EXTERNAL             (0)
#define CLK_32K_SRC_SEL_INTERNAL             (1)

/* CMD_ICU_INT_DISABLE CMD_ICU_INT_ENABLE */
#define FIQ_BLE_BIT                          (1 << 15)
#define FIQ_PWM0_BIT                         (1 << 0)
#define IRQ_UART2_BIT                        (1 << 16)
#define IRQ_PWM5_BIT                         (1 << 13)
#define IRQ_RTC_BIT                          (1 << 12)
#define IRQ_GPIO_BIT                         (1 << 9)
#define IRQ_ADC_BIT                          (1 << 8)
#define IRQ_I2S_PCM_BIT                      (1 << 7)
#define IRQ_SPI_BIT                          (1 << 6)
#define IRQ_UART1_BIT                        (1 << 5)


/* CMD_ICU_GLOBAL_INT_DISABLE CMD_ICU_GLOBAL_INT_ENABLE */
#define GINTR_FIQ_BIT                        (1 << 1)
#define GINTR_IRQ_BIT                        (1 << 0)

/* CMD_CLK_PWR_DOWN CMD_CLK_PWR_UP */
typedef enum
{
	CLK_PWR_DEV_PWM0 = 0,
	CLK_PWR_DEV_PWM1,
	CLK_PWR_DEV_PWM2,
	CLK_PWR_DEV_PWM3,
	CLK_PWR_DEV_PWM4,
	CLK_PWR_DEV_PWM5,
    CLK_PWR_DEV_UART1,
	CLK_PWR_DEV_UART2,
	CLK_PWR_DEV_WDT,
	CLK_PWR_DEV_I2S_PCM,
    CLK_PWR_DEV_SPI,
} icu_clk_pwr_dev;


typedef enum
{
	MCU_NO_SLEEP = 0,
	MCU_IDLE_SLEEP,         //cpu idle,clk run	
    MCU_REDUCE_VO_SLEEP,    //cpu idle,16M,PLL STOP,reduce cpu voltage
    MCU_DEEP_SLEEP,         //cpu idle,16M,PLL STOP

    MCU_SLEEP_MODE_NUM,
} MCU_SLEEP_MODE;



/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void icu_init(void);
extern void icu_exit(void);
extern UINT32 icu_ctrl(UINT32 cmd, void *param);

void cpu_reduce_voltage_sleep(void);
void cpu_wakeup(void);
void cpu_idle_sleep(void);

#endif //_ICU_PUB_H_ 

