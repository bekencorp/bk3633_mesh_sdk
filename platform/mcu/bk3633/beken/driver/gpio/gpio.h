/**
 ****************************************************************************************
 *
 * @file gpio.h
 *
 * @brief gpio Driver for gpio operation.
 *
 * Copyright (C) Beken Leonardo 2009-2021
 *
 *
 ****************************************************************************************
 */

#ifndef _GPIO_H_
#define _GPIO_H_



#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions
#include "typedef.h"
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
 
#define GPIO_INPUT_VA   0
#define GPIO_OUTPUT_VA  1
#define GPIO_INPUT_EN   2
#define GPIO_OUTPUT_EN  3
#define GPIO_PULL_MODE  4
#define GPIO_PULL_EN    5
#define GPIO_2FUN_EN    6
#define GPIO_INPUT_M    7

#define REG_GPIO_CFG_BASE                       BASEADDR_AON_GPIO
#define REG_GPIOx_CFG(dest_reg, idx)            dest_reg = (REG_GPIO_CFG_BASE + (idx)* 0x04)

#define REG_GPIO_INT_STAT1                          &(addAON_GPIO_Reg0x35)
#define REG_GPIO_INT_EN1                            &(addAON_GPIO_Reg0x33)
#define REG_GPIO_INT_TYPE_L                         &(addAON_GPIO_Reg0x30)
#define GPIO_INT_MASK                               0x03
#define REG_GPIO_INT_TYPE_H                         &(addAON_GPIO_Reg0x31)

#define REG_GPIO_WAKE_UP                            &(addPMU_Reg0x3)

#define setf_GPIO_2nd_Fun_Ena(p)            ((*(volatile unsigned long *)p) |= (1<<GPIO_2FUN_EN))

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

void gpio_init(void);
void gpio_config(UINT32 index, Dir_Type dir, Pull_Type pull);

#if SYSTEM_SLEEP
void gpio_sleep(void);
void gpio_wakeup(void);
#endif

void gpio_triger(uint8_t gpio);

typedef void (*GPIO_INT_CALLBACK_T)(uint8_t idx);

 
 
#endif // _GPIO_H_
