/**
****************************************************************************************
*
* @file gpio.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2009-2021
*
* $Rev: $
*
****************************************************************************************
*/

#include <stddef.h>     // standard definition
#include "BK3633_RegList.h"
#include "gpio.h"
#include "intc_pub.h"
#include "gpio_pub.h"


static SDD_OPERATIONS gpio_op =
{ 
    gpio_ctrl
};

static GPIO_INT_CALLBACK_T gpio_int_cb = NULL; 

void gpio_config(UINT32 index, Dir_Type dir, Pull_Type pull)
{

    uint32_t  gpio_temp=0;
    uint32_t value;
    uint32_t* dest_reg;

    uint8_t idx = GPIO_PORT2ID(index);

    REG_GPIOx_CFG(dest_reg, idx);

    value = REG_READ(dest_reg);

    switch(dir)
    {
        case OUTPUT:	       
            value &= ~(1<<GPIO_INPUT_EN);
            value &= ~(1<<GPIO_OUTPUT_EN);
            break;        
        case INPUT:
            value |= (1<<GPIO_OUTPUT_EN);
            value |= (1<<GPIO_INPUT_EN);
            break;
    	case FLOAT:		
    		value &= ~(1<<GPIO_INPUT_EN);
            value |= (1<<GPIO_OUTPUT_EN);
            break;
    }

    switch(pull)
    {
        case PULL_HIGH:
            value |= (1<<GPIO_PULL_EN);
            value |= (1<<GPIO_PULL_MODE);
            break;
        case PULL_LOW:
            value |= (1<<GPIO_PULL_EN);
            value &= ~(1<<GPIO_PULL_MODE);
            break;
        case PULL_NONE:
            value &= ~(1<<GPIO_PULL_EN);
            break;
    }

    value &= ~(1<<GPIO_2FUN_EN);
    value &= ~(1<<GPIO_INPUT_M);

    REG_WRITE(dest_reg, value);
}

void gpio_config_func(UINT32 index)
{
    uint32_t*  p_add;
    uint8_t idx = GPIO_PORT2ID(index);

    REG_GPIOx_CFG(p_add, idx);

    setf_GPIO_2nd_Fun_Ena(p_add);
}

static void gpio_enable_second_function(UINT32 func_mode)
{
    GPIO_INDEX gpio;
    switch(func_mode)
    {
        case GFUNC_MODE_UART1:
    	    gpio_config(GPIO_UART1_TX, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_UART1_TX);
    	    gpio_config(GPIO_UART1_RX, INPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_UART1_RX);
            break;

        case GFUNC_MODE_UART2:
    	    gpio_config(GPIO_UART2_TX, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_UART2_TX);
    	    gpio_config(GPIO_UART2_RX, INPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_UART2_RX);
            break;

        case GFUNC_MODE_I2C:
    	    gpio_config(GPIO_I2C_SCL, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_I2C_SCL);
    	    gpio_config(GPIO_I2C_SDA, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_I2C_SDA);
            break;

        case GFUNC_MODE_SPI_MST:
    	    gpio_config(GPIO_SPI_SCK, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_SPI_SCK);
    	    gpio_config(GPIO_SPI_MOSI, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_SPI_MOSI);
    	    gpio_config(GPIO_SPI_MOSO, INPUT, PULL_NONE);
    	    gpio_config_func(GPIO_SPI_MOSO);
    	    gpio_config(GPIO_SPI_NSS, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_SPI_NSS);
            break;

        case GFUNC_MODE_SPI_SLV:
    	    gpio_config(GPIO_SPI_SCK, INPUT, PULL_NONE);
    	    gpio_config_func(GPIO_SPI_SCK);
    	    gpio_config(GPIO_SPI_MOSI, INPUT, PULL_NONE);
    	    gpio_config_func(GPIO_SPI_MOSI);
    	    gpio_config(GPIO_SPI_MOSO, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_SPI_MOSO);
    	    gpio_config(GPIO_SPI_NSS, INPUT, PULL_NONE);
    	    gpio_config_func(GPIO_SPI_NSS);
            break;

        case GFUNC_MODE_PWM0:
        case GFUNC_MODE_PWM1:
        case GFUNC_MODE_PWM2:
        case GFUNC_MODE_PWM3:
        case GFUNC_MODE_PWM4:
        case GFUNC_MODE_PWM5:
    	    gpio = GPIO_PWM_0+(func_mode-GFUNC_MODE_PWM0);
    	    gpio_config_func(gpio);
            break;

        case GFUNC_MODE_ADC1:
        case GFUNC_MODE_ADC2:
        case GFUNC_MODE_ADC3:
        case GFUNC_MODE_ADC4:
        case GFUNC_MODE_ADC5:
        case GFUNC_MODE_ADC6:
        case GFUNC_MODE_ADC7:
    	    gpio = GPIO_ADC_CH1+(func_mode-GFUNC_MODE_ADC1);
    	    gpio_config(gpio, INPUT, PULL_NONE);
    	    gpio_config_func(gpio);
            break;

        default:
            break;
    }

    return;
}

UINT8 gpio_get_input(UINT32 index)
{

    UINT32*  p_add;

    UINT8 idx = GPIO_PORT2ID(index);

    REG_GPIOx_CFG(p_add, idx);

    UINT32 value = REG_READ(p_add);

    return (value & (1 << GPIO_INPUT_VA));

}

static void gpio_set(UINT32 index, UINT8 val)
{
    UINT32*  p_add;
    UINT8 idx = GPIO_PORT2ID(index);

    REG_GPIOx_CFG(p_add, idx);

    UINT32 value = REG_READ(p_add);

    if(val)
    {
        value |= (1<<GPIO_OUTPUT_VA);
    }
    else
    {
        value &= ~(1<<GPIO_OUTPUT_VA);
    }

    REG_WRITE(p_add, value);
}

void gpio_output_reverse(UINT32 index)
{
    UINT32*  p_add;
    UINT8 idx = GPIO_PORT2ID(index);
    UINT8 sta;

    REG_GPIOx_CFG(p_add, idx);

    UINT32 value = REG_READ(p_add);
    sta = (value >> GPIO_OUTPUT_VA) & 0x01;
    sta = (sta + 1) & 0x01;

    gpio_set(index, sta);
}


void gpio_init(void)
{
	sddev_register_dev(GPIO_DEV_NAME, &gpio_op);
}


void gpio_exit(void)
{
    sddev_unregister_dev(GPIO_DEV_NAME);
}

void gpio_triger(uint8_t gpio)
{
	gpio_set(gpio, 1);
	gpio_set(gpio, 0);
}


void gpio_cb_register(GPIO_INT_CALLBACK_T cb)
{
	if(cb)
	{
		gpio_int_cb = cb;
	}
}

void gpio_isr(void)
{
    UINT8 i;
    UINT32 value = 0;

    value = REG_READ(REG_GPIO_INT_STAT1);

    for (i = 0; i < GPIO_SUM; i++)
    {
        if (value & (0x01UL << i))
        {
        	if(gpio_int_cb)
        	{
        		(*gpio_int_cb)(i);
        	}
        }
    }

    REG_WRITE(REG_GPIO_INT_STAT1, value);
}

void gpio_int_disable(UINT32 index)
{
	UINT8 idx = GPIO_PORT2ID(index);
    UINT32 value = 0;

    value = REG_READ(REG_GPIO_INT_EN1);
    value &= ~(0x01 << idx);
    REG_WRITE(REG_GPIO_INT_EN1, value);
}

void gpio_int_enable(UINT32 index, UINT32 mode, void (*p_Int_Handler)(unsigned char))
{
    UINT32 param;
    UINT32 value = 0, reg = 0, pos = 0;
	UINT8 idx = GPIO_PORT2ID(index);

    if((index >= GPIONUM) || (index&0x08))
    {
        os_printf("gpio_id_cross_border\r\n");
        return;
    }

    // intc_service_register(IRQ_GPIO, PRI_IRQ_GPIO, gpio_isr);
    gpio_cb_register(p_Int_Handler);

    if(mode == GPIO_INT_LEVEL_FALLING || mode == GPIO_INT_LEVEL_LOW)
    {
        gpio_config(index, INPUT, PULL_HIGH);
    }
    else
    {
        gpio_config(index, INPUT, PULL_LOW);
    }

    if(idx <= 15)
    {
        reg = REG_GPIO_INT_TYPE_L;
        pos = idx*2;
    }
    else
    {
        reg = REG_GPIO_INT_TYPE_H;
        pos = (idx-16)*2;
    }

    value = REG_READ(reg);
    value &= (~(GPIO_INT_MASK << pos));
    value |= (mode << pos);
    REG_WRITE(reg, value);

    value = REG_READ(REG_GPIO_INT_EN1);
    value |= (0x01 << idx);
    REG_WRITE(REG_GPIO_INT_EN1, value);

    value = REG_READ(REG_GPIO_INT_STAT1);
    value |= (0x01 << idx);
    REG_WRITE(REG_GPIO_INT_STAT1, value);

    value = REG_READ(REG_GPIO_WAKE_UP);
    value |= (0x01 << idx);
    REG_WRITE(REG_GPIO_WAKE_UP, value);

    intc_enable(IRQ_GPIO);
}

UINT32 gpio_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = DRIV_SUCCESS;

    switch(cmd)
    {
        case CMD_GPIO_CFG:
        {
            UINT32 id;
            UINT32 mode;
            Dir_Type dir;
            Pull_Type pull;

            id = GPIO_CFG_PARAM_DEMUX_ID(*(UINT32 *)param);
            mode = GPIO_CFG_PARAM_DEMUX_MODE(*(UINT32 *)param);

            if(mode== GMODE_INPUT_PULLUP)
            {
                dir  = INPUT;
                pull = PULL_HIGH;
            }
            else if(mode== GMODE_INPUT_PULLDOWN)
            {
                dir  = INPUT;
                pull = PULL_LOW;
            }
            else if(mode== GMODE_INPUT)
            {
                dir  = INPUT;
                pull = PULL_NONE;
            }
            else if(mode== GMODE_OUTPUT)
            {
                dir  = OUTPUT;
                pull = PULL_NONE;
            }
            else if (mode== GMODE_DISABLE)
            {
                dir  = FLOAT;
                pull = PULL_NONE;
            }
            
            else
            {
                break;
            }

            gpio_config(id, dir, pull);

            break;
        }

        case CMD_GPIO_OUTPUT_REVERSE:
            ASSERT(param);

            gpio_output_reverse(*(UINT32 *)param);
            break;

        case CMD_GPIO_OUTPUT:
        {
            UINT32 id;
            UINT32 val;

            id = GPIO_OUTPUT_DEMUX_ID(*(UINT32 *)param);
            val = GPIO_OUTPUT_DEMUX_VAL(*(UINT32 *)param);

            gpio_set(id, val);
            break;
        }

        case CMD_GPIO_INPUT:
        {
            UINT32 id;
            UINT32 val;

            ASSERT(param);

            id = *(UINT32 *)param;
            val = gpio_get_input(id);

            ret = val;
            break;
        }

        case CMD_GPIO_ENABLE_SECOND:
        {
            UINT32 second_mode;

            ASSERT(param);

            second_mode = *(UINT32 *)param;
            gpio_enable_second_function(second_mode);
            break;
        }
        case CMD_GPIO_INT_ENABLE:
        {
            GPIO_INT_ST *ptr = param;
            gpio_int_enable(ptr->id, ptr->mode, ptr->phandler);
            break;
        }
        case CMD_GPIO_INT_DISABLE:
        {
            UINT32 id ;
            id = *(UINT32 *)param;
            gpio_int_disable(id);
            break;
        }
        default:
            break;
    }

    return ret;
}




