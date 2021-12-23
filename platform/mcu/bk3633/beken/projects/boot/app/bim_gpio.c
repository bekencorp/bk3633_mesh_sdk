/**
****************************************************************************************
*
* @file gpio.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken 2009-2016
*
* $Rev: $
*
****************************************************************************************
*/
#include "bim_gpio.h"

void bim_gpio_config(uint8_t gpio, Dir_Type dir, Pull_Type pull)
{
    uint32_t  gpio_temp=0;
    uint8_t port = ((gpio&0xf0)>>4);
    uint8_t  pin = (gpio&0xf);

    switch(dir)
    {
        case OUTPUT:	       
            gpio_temp &= ~(1<<GPIO_INPUT_EN);
            gpio_temp &= ~(1<<GPIO_OUTPUT_EN);
            break;        
        case INPUT:
            gpio_temp |= (1<<GPIO_OUTPUT_EN);
            gpio_temp |= (1<<GPIO_INPUT_EN);
            break;
    	case FLOAT:		
    		gpio_temp &= ~(1<<GPIO_INPUT_EN);
            gpio_temp |= (1<<GPIO_OUTPUT_EN);
            break;
    }

    switch(pull)
    {
    case PULL_HIGH:        
        gpio_temp |= (1<<GPIO_PULL_EN);
        gpio_temp |= (1<<GPIO_PULL_MODE);
        break;
    case PULL_LOW:
        gpio_temp |= (1<<GPIO_PULL_EN);
        gpio_temp &= ~(1<<GPIO_PULL_MODE);
        break;
    case PULL_NONE:
        gpio_temp &= ~(1<<GPIO_PULL_EN);
        break;
    }
    *((volatile unsigned long *) (BASEADDR_GPIO+4*(port*8+pin)))=gpio_temp;
    
}

void bim_gpio_set(uint8_t gpio, uint8_t val)
{
    uint32_t temp=0;
    uint8_t port = ((gpio&0xf0)>>4);
    uint8_t  pin = (gpio&0xf);

    temp = *((volatile unsigned long *) (BASEADDR_GPIO+4*(port*8+pin)));
    if(val)
    {
        temp |= (1<<GPIO_OUTPUT_VA);
    }
    else
    {
        temp &= ~(1<<GPIO_OUTPUT_VA);
    }
    *((volatile unsigned long *) (BASEADDR_GPIO+4*(port*8+pin))) = temp;
    
}

void bim_gpio_triger(uint8_t gpio)
{
	bim_gpio_set(gpio, 1);
	bim_gpio_set(gpio, 0);
}