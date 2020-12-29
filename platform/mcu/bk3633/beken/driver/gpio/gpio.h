/**
 ****************************************************************************************
 *
 * @file gpio.h
 *
 * @brief gpio Driver for gpio operation.
 *
 * Copyright (C) Beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _GPIO_H_
#define _GPIO_H_



#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions
#include "BK3633_RegList.h"
/**
 ****************************************************************************************
 * @defgroup GPIO 
 * @ingroup DRIVERS
 * @brief GPIO driver
 *
 * @{
 *
 ****************************************************************************************
 */

#define BASEADDR_GPIO                                           0x00800A00
 
#define GPIO_INPUT_VA   0
#define GPIO_OUTPUT_VA  1
#define GPIO_INPUT_EN   2
#define GPIO_OUTPUT_EN  3
#define GPIO_PULL_MODE  4
#define GPIO_PULL_EN    5
#define GPIO_2FUN_EN    6

#define REG_GPIO_CFG_BASE                       BASEADDR_AON_GPIO
#define REG_GPIOx_CFG(dest_reg, port,pin)       dest_reg = (REG_GPIO_CFG_BASE + (port)*0x08*0x04 + (pin)* 0x04)

#define REG_APB5_GPIO_WUATOD_STAT               addAON_GPIO_Reg0x35
#define REG_APB5_GPIO_WUATOD_ENABLE             addAON_GPIO_Reg0x33
#define REG_APB5_GPIO_WUATOD_TYPE_LOW           addAON_GPIO_Reg0x30
#define REG_APB5_GPIO_WUATOD_TYPE_HIGH          addAON_GPIO_Reg0x31

#define setf_GPIO_2nd_Fun_Ena(p)            ((*(volatile unsigned long *)p) |= (1<<GPIO_2FUN_EN))
#define get_GPIO_Input_Monitor(p)           (((*(volatile unsigned long *)p) & 0x80) >> 7)

 typedef enum
{
    INPUT,
    OUTPUT,
    FLOAT,
    HIRESI,
}Dir_Type;

typedef enum
{
    PULL_HIGH,
    PULL_LOW,
    PULL_NONE
}Pull_Type;
 

#define	BlueLedPort		0x11
#define	BlueLedOn()		REG_APB5_GPIOB_DATA |= 0x02
#define	BlueLedOff()	REG_APB5_GPIOB_DATA &= (~0x02)
#define	BlueLedToggle()	REG_APB5_GPIOB_DATA ^= 0x02


#define	RedLedPort		0x27
#define	RedLedOn()		REG_APB5_GPIOC_DATA |= 0x80
#define	RedLedOff()		REG_APB5_GPIOC_DATA &= (~0x80)
#define	RedLedToggle()	REG_APB5_GPIOC_DATA ^= 0x80



void gpio_init(void);
void gpio_config(uint8_t gpio, Dir_Type dir, Pull_Type pull);
uint8_t gpio_get_input(uint8_t gpio);
uint8_t gpio_get_output(uint8_t gpio);
void gpio_set(uint8_t gpio, uint8_t val);
void gpio_target(uint8_t gpio);

void gpio_isr(void);

void DEBUG_MSG(uint8_t x);

#if SYSTEM_SLEEP
void gpio_sleep(void);
void gpio_wakeup(void);
#endif
 
#if GPIO_DBG_MSG
 void gpio_debug_msg_init(void);
#endif 

void gpio_triger(uint8_t gpio);

typedef void (*GPIO_INT_CALLBACK_T)(uint8_t idx);

void gpio_cb_register(GPIO_INT_CALLBACK_T cb);
 
 
#endif // _GPIO_H_
