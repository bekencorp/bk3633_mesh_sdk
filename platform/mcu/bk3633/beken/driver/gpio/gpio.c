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

#include <stddef.h>     // standard definition
#include "BK3633_RegList.h"
#include "gpio.h"
#include "timer.h"      // timer definition

#include "rwip.h"       // SW interface
//#include "h4tl.h"
//#include "nvds.h"       // NVDS

//#include "dbg.h"
#include "icu.h"
//#include "rf.h"
#include "app.h"
//#include "app_task.h"
#include "uart.h"
#include "user_config.h"

#include "icu_pub.h"
#include "intc_pub.h"
#include "gpio_pub.h"
#include "drv_model_pub.h"


static SDD_OPERATIONS gpio_op =
{
    gpio_ctrl
};

static GPIO_INT_CALLBACK_T gpio_int_cb = NULL; 

void gpio_config(uint8_t gpio, Dir_Type dir, Pull_Type pull)
{

    uint32_t  gpio_temp=0;
    uint8_t port = ((gpio&0xf0)>>4);
    uint8_t  pin = (gpio&0xf);
    uint32_t* dest_reg;

    REG_GPIOx_CFG(dest_reg, port, pin);

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

    *(volatile unsigned long *)dest_reg = gpio_temp;

}

void gpio_config_func(uint8_t gpio)
{
    uint32_t*  p_add;
    uint8_t port = (gpio&0xf0)>>4;
    uint8_t  pin = gpio&0xf;

    REG_GPIOx_CFG(p_add, port, pin);

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
    	    gpio_config(GPIO_UART1_RX, INPUT, PULL_NONE);
    	    gpio_config_func(GPIO_UART1_RX);
            break;

        case GFUNC_MODE_UART2:
    	    gpio_config(GPIO_UART2_TX, OUTPUT, PULL_HIGH);
    	    gpio_config_func(GPIO_UART2_TX);
    	    gpio_config(GPIO_UART2_RX, INPUT, PULL_NONE);
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
    	    gpio_config(gpio, OUTPUT, PULL_HIGH);
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

uint8_t gpio_get_input(uint8_t gpio)
{

    uint32_t*  p_add;
    uint8_t port = (gpio&0xf0)>>4;
    uint8_t  pin = gpio&0xf;

    REG_GPIOx_CFG(p_add, port, pin);
	
    return get_GPIO_Input_Monitor(p_add);
}

void gpio_set(uint8_t gpio, uint8_t val)
{

    uint32_t*  p_add;
    uint8_t port = ((gpio&0xf0)>>4);
    uint8_t  pin = (gpio&0xf);

    REG_GPIOx_CFG(p_add, port, pin);

    if(val)
    {
        *p_add |= (1<<GPIO_OUTPUT_VA);
    }
    else
    {
        *p_add &= ~(1<<GPIO_OUTPUT_VA);
    }

}

#if GPIO_DBG_MSG
void gpio_debug_msg_init()
{

	*(GPIO_CFG[0]) = 0X0000ff;		    	
	*(GPIO_DATA[0]) = 0;
	REG_AHB0_ICU_DIGITAL_PWD |= (0X01 << 4);
}
#endif

void gpio_init(void)
{
	sddev_register_dev(GPIO_DEV_NAME, &gpio_op);

#if DEBUG_HW
	*(GPIO_CFG[2]) = 0XFF0000;
	*(GPIO_DATA[2]) = 0;
	REG_AHB0_ICU_DIGITAL_PWD |= (0X01 << 4);
	//gpio_config(0x17, OUTPUT, PULL_NONE);
	//gpio_set(0x17, 0);
#endif

#if GPIO_DBG_MSG
    
	//gpio_debug_msg_init();
	gpio_config(0x26, OUTPUT, PULL_NONE);
	gpio_config(0x27, OUTPUT, PULL_NONE);
	gpio_config(0x30, OUTPUT, PULL_NONE);
	gpio_config(0x31, OUTPUT, PULL_NONE);
	gpio_config(0x32, OUTPUT, PULL_NONE);
	// while(1)
	{
	    gpio_set(0x27, 1);
	    gpio_set(0x27, 0);
	    for (int j = 0; j < 5000; j++)
	    {
			;
	    }
	}
#endif
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
    int i;
    unsigned long ulIntStatus;

    ulIntStatus = REG_APB5_GPIO_WUATOD_STAT;
    for (i = 0; i < GPIO_SUM; i++)
    {
        if (ulIntStatus & (0x01UL << i))
        {
        	if(gpio_int_cb)
        	{
        		(*gpio_int_cb)(i);
        	}
        }
    }

    REG_APB5_GPIO_WUATOD_STAT = ulIntStatus;
}

void gpio_int_disable(UINT32 index)
{
	uint8_t idx = ((index&0x30)>>1)+(index&0x7);
    REG_APB5_GPIO_WUATOD_ENABLE &= ~(0x01 << idx);
}

void gpio_int_enable(UINT32 index, UINT32 mode, void (*p_Int_Handler)(unsigned char))
{
    UINT32 param;
	uint8_t idx = ((index&0x30)>>1)+(index&0x7);
    if((index >= GPIONUM) || (index&0x08))
    {
        os_printf("gpio_id_cross_border\r\n");
        return;
    }

    intc_service_register(IRQ_GPIO, PRI_IRQ_GPIO, gpio_isr);
    gpio_cb_register(p_Int_Handler);
    //param = IRQ_GPIO_BIT;
    //sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

    mode &= 0x01;
    if(mode == 1)
    {
        gpio_config(index, INPUT, PULL_HIGH);
    }
    else
    {
        gpio_config(index, INPUT, PULL_LOW);
    }

    REG_APB5_GPIO_WUATOD_TYPE = (REG_APB5_GPIO_WUATOD_TYPE & (~(0x01 << idx))) | (mode << idx);
    REG_APB5_GPIO_WUATOD_ENABLE |= (0x01 << idx);
}

UINT32 gpio_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;
    ret = GPIO_SUCCESS;

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
        else
        {
        	break;
        }

        gpio_config(id, dir, pull);

        break;
    }

    case CMD_GPIO_OUTPUT_REVERSE:
        ASSERT(param);

        //gpio_output_reverse(*(UINT32 *)param);
        break;

    case CMD_GPIO_OUTPUT:
    {
        UINT32 id;
        UINT32 val;

        id = GPIO_OUTPUT_DEMUX_ID(*(UINT32 *)param);
        val = GPIO_OUTPUT_DEMUX_VAL(*(UINT32 *)param);

        //gpio_output(id, val);
        gpio_set(id, val);
        break;
    }

    case CMD_GPIO_INPUT:
    {
        UINT32 id;
        UINT32 val;

        ASSERT(param);

        id = *(UINT32 *)param;
        //val = gpio_input(id);
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



void DEBUG_MSG(uint8_t x)
{
#if GPIO_DBG_MSG
	REG_APB5_GPIOA_DATA = x & 0xff;
	gpio_triger(GPIOD_4);
#endif
}


