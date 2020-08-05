#include "include.h"
#include "arm_arch.h"

#include "pwm.h"
#include "pwm_pub.h"
#include "pwm_init.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"

static SDD_OPERATIONS pwm_op =
{
    pwm_ctrl
};

void (*p_PWM_Int_Handler[PWM_CHANNEL_NUMBER_MAX])(UINT8) = {NULL};

static void pwm_gpio_configuration(UINT8 chan, UINT8 enable)
{
    UINT32 ret;
    UINT32 param;

    switch(chan)
    {
    case PWM0:
        param = GFUNC_MODE_PWM0;
        break;

    case PWM1:
        param = GFUNC_MODE_PWM1;
        break;

    case PWM2:
        param = GFUNC_MODE_PWM2;
        break;

    case PWM3:
        param = GFUNC_MODE_PWM3;
        break;

    case PWM4:
        param = GFUNC_MODE_PWM4;
        break;

    case PWM5:
        param = GFUNC_MODE_PWM5;
        break;

    default:
        break;
    }

	if(enable)
	{
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
	}
	else
	{
		param = GPIO_CFG_PARAM(param, GMODE_INPUT);
    	ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	}
    ASSERT(GPIO_SUCCESS == ret);
}

static void pwm_icu_configuration(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 ret;
    UINT32 prm;

    /* set clock power down of icu module*/
    if(pwm_param->channel >= PWM_COUNT)
    {
		PWM_WARN("pwm_iconfig_fail\r\n");
        goto exit_icu;
    }

    PWM_PRT("%s, enable %d\n", __func__, enable);

 	if(enable)
	{
		prm = pwm_param->channel;
		ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&prm);
		ASSERT(ICU_SUCCESS == ret);

		if(PWM_CLK_32K == pwm_param->cfg.bits.clk)
		{
			prm = pwm_param->channel;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_LPOCLK, (void *)&prm);
		}
		else
		{
			prm = pwm_param->channel;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&prm);
		}
		ASSERT(ICU_SUCCESS == ret);
	}
	else
	{
    	ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&prm);
		ASSERT(ICU_SUCCESS == ret);
	}

exit_icu:

    return;
}

static void init_pwm_param(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 value;

    if((pwm_param == NULL)
            || (pwm_param->channel >= PWM_COUNT)
            || (pwm_param->duty_cycle > pwm_param->end_value))
    {
        return;
    }

	if(pwm_param->cfg.bits.mode != PMODE_TIMER)
	{
		pwm_gpio_configuration(pwm_param->channel, enable);
	}
	
    pwm_icu_configuration(pwm_param, enable);

    addPWM0_Reg0x0 &= ~(0x7 << (5 * pwm_param->channel + posPWM0_Reg0x0_pwm0_mode));
    addPWM0_Reg0x0 |= (pwm_param->cfg.bits.mode << (5 * pwm_param->channel + posPWM0_Reg0x0_pwm0_mode)); //set mode
            
    set_PWM0_Reg0x0_pre_div(pwm_param->pre_divid);

    addPWM0_Reg0x0 &= ~(0x3 << (2 * pwm_param->channel + posPWM0_Reg0x0_pwm0_cpedg_sel));
    addPWM0_Reg0x0 |= (pwm_param->cpedg_sel << (2 * pwm_param->channel + posPWM0_Reg0x0_pwm0_cpedg_sel));//set cpedg_sel
    
    addPWM0_Reg0x0 &= ~(0x1 << (pwm_param->channel + posPWM0_Reg0x0_pwm0_ctnu_mod));
    addPWM0_Reg0x0 |= (pwm_param->contiu_mode << (pwm_param->channel + posPWM0_Reg0x0_pwm0_ctnu_mod));



    switch(pwm_param->channel)
    {
        case 0:
        {
            set_AON_GPIO_Reg0x8_GPIO8_Config(0xc0);
            addPWM0_Reg0x2 = pwm_param->end_value;
            addPWM0_Reg0x3 = pwm_param->duty_cycle;
                           
        }break;
        
        case 1:
        {
            set_AON_GPIO_Reg0x9_GPIO9_Config(0xc0);
            addPWM0_Reg0x5 = pwm_param->end_value;
            addPWM0_Reg0x6 = pwm_param->duty_cycle;
        
        }break;
        
        case 2:
        {
            set_AON_GPIO_Reg0xa_GPIO10_Config(0xc0);
            addPWM0_Reg0x8 = pwm_param->end_value;
            addPWM0_Reg0x9 = pwm_param->duty_cycle;
        
        }break;
        
        default:break;       
    }

    p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;


    addPWM0_Reg0x0 |= (pwm_param->cfg.bits.int_en << (5 * pwm_param->channel + posPWM0_Reg0x0_pwm0_int_en));
    
    if(pwm_param->cfg.bits.en)
    {
        addPWM0_Reg0x0  |= (pwm_param->cfg.bits.en << (5 * pwm_param->channel + posPWM0_Reg0x0_pwm0_en));
    }else
    {
        addPWM0_Reg0x0  &= ~(pwm_param->cfg.bits.en << (5 * pwm_param->channel + posPWM0_Reg0x0_pwm0_en));
    }
    intc_enable(0);
}

static UINT16 pwm_capture_value_get(UINT8 ucChannel)
{
    return REG_READ(REG_APB_BK_PWMn_CAP_ADDR(ucChannel));
}

static void pwm_int_handler_clear(UINT8 ucChannel)
{
    p_PWM_Int_Handler[ucChannel] = NULL;
}

void pwm_init(void)
{
    intc_service_register(IRQ_PWM0, PRI_FIQ_PWM0, pwm_isr);
    intc_service_register(IRQ_PWM1, PRI_IRQ_PWM1, pwm_isr);

    sddev_register_dev(PWM_DEV_NAME, &pwm_op);
}
/* 
void pwm_config(PWM_DRV_DESC *pwm_drv_desc)
{
    if(pwm_drv_desc == NULL)
    {
        return;
    }
    if (pwm_drv_desc->channel > PWM_CHANNEL_NUMBER_MAX)
    {
        return;
    }
    if (pwm_drv_desc->duty_cycle > pwm_drv_desc->end_value)
    {
        return;
    }
    
    
    //Config clk
    ICU_PWM_CLK_PWM_X_PWD_CLEAR(pwm_drv_desc->channel);
    if (pwm_drv_desc->mode & 0x10)
    {
        // select 16MHz
        ICU_PWM_CLK_PWM_X_SEL_16MHZ(pwm_drv_desc->channel);
    }
    else
    {
        // select 32KHz
        ICU_PWM_CLK_PWM_X_SEL_32KHZ(pwm_drv_desc->channel);
    }
    //Config duty_cycle and end value
    REG_PWM_X_CNT(pwm_drv_desc->channel) = 
        ((((unsigned long)pwm_drv_desc->duty_cycle << PWM_CNT_DUTY_CYCLE_POSI) & PWM_CNT_DUTY_CYCLE_MASK)
       + (((unsigned long)pwm_drv_desc->end_value << PWM_CNT_END_VALUE_POSI) & PWM_CNT_END_VALUE_MASK));

    REG_PWM_CTRL = (REG_PWM_CTRL & (~(0x0F << (0x04 *  pwm_drv_desc->channel))))
             | ((pwm_drv_desc->mode & 0x0F) << (0x04 *  pwm_drv_desc->channel));

    if (pwm_drv_desc->mode & 0x02)    // int enable
    {
        // install interrupt handler
        p_PWM_Int_Handler[pwm_drv_desc->channel] = pwm_drv_desc->p_Int_Handler;
        //ICU_INT_ENABLE_SET(ICU_INT_ENABLE_IRQ_PWM_X_MASK(pwm_drv_desc->channel));
        REG_AHB0_ICU_INT_ENABLE |=  (ICU_INT_ENABLE_IRQ_PWM_X_MASK(pwm_drv_desc->channel)); 
    }
    else
    {
        p_PWM_Int_Handler[pwm_drv_desc->channel] = NULL;
        ICU_INT_ENABLE_CLEAR(ICU_INT_ENABLE_IRQ_PWM_X_MASK(pwm_drv_desc->channel));
    }
    
    // enable GPIO second function
    if ((pwm_drv_desc->mode & 0x0C) != 0x04)
    {
        REG_APB5_GPIOB_CFG &= (~ GPIO_CONFIG_X_SECOND_FUNCTION_MASK(pwm_drv_desc->channel));
    }

    p_PWM_Int_Handler[pwm_drv_desc->channel] = pwm_drv_desc->p_Int_Handler;
} */


void pwm_exit(void)
{
    sddev_unregister_dev(PWM_DEV_NAME);
}

UINT32 pwm_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = PWM_SUCCESS;
    UINT32 ucChannel;
    UINT32 value;
    pwm_param_t *p_param;
    pwm_capture_t *p_capture;
    printf(" +++++ %s, cmd 0x%x ++++++\n", __func__, cmd);
    switch(cmd)
    {
    case CMD_PWM_UNIT_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value |= (1 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_UINT_DISABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value &= ~(3 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_IR_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value |= (2 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_IR_DISABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        value = REG_READ(PWM_CTL);
        value &= ~(2 << (ucChannel * 4));
        REG_WRITE(PWM_CTL, value);
        break;
    case CMD_PWM_IR_CLEAR:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        pwm_int_handler_clear(ucChannel);
        break;
    case CMD_PWM_INIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 1);
        break;
    case CMD_PWM_CAP_GET:
        p_capture = (pwm_capture_t *)param;
        if(p_capture->ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        p_capture->value = pwm_capture_value_get(p_capture->ucChannel);
        break;
	case CMD_PWM_DEINIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 0);
		break;
    default:
        ret = PWM_FAILURE;
        break;
    }

    return ret;
}

void pwm_isr(void)
{
    int i;
    unsigned long ulIntStatus;
    gpio_set(0x27, 1);
    ulIntStatus = REG_PWM_INTR;
    //PWM_PRT("+++ %s +++\n", __func__);
    for (i=PWM_CHANNEL_NUMBER_MAX; i>=0; i--)
    {
        if (ulIntStatus & (0x01<<i))
        {
            if (p_PWM_Int_Handler[i] != NULL)
            {
                (void)p_PWM_Int_Handler[i]((unsigned char)i);
            }
        }
    }
    do
    {
        REG_PWM_INTR = ulIntStatus;
    } while (REG_PWM_INTR & ulIntStatus & REG_PWM_INTR_MASK);   // delays
    gpio_set(0x27, 0);
}

