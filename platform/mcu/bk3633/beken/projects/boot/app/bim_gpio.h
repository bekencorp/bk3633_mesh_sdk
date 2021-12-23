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
#define BASEADDR_GPIO                                           0x00800A00
#define GPIO_INPUT_VA   0
#define GPIO_OUTPUT_VA  1
#define GPIO_INPUT_EN   2
#define GPIO_OUTPUT_EN  3
#define GPIO_PULL_MODE  4
#define GPIO_PULL_EN    5
#define GPIO_2FUN_EN    6
typedef enum
{
    INPUT,
    OUTPUT,
    FLOAT,
} Dir_Type;
typedef enum
{
    PULL_HIGH,
    PULL_LOW,
    PULL_NONE
} Pull_Type;

void bim_gpio_config(uint8_t gpio, Dir_Type dir, Pull_Type pull);
void bim_gpio_triger(uint8_t gpio);
uint8_t bim_gpio_get_input(uint8_t gpio);
#endif
