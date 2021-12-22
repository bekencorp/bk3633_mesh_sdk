/**
****************************************************************************************
*
* @file pwm.c
*
* @brief pwm driver
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "include.h"

#include "pwm.h"
#include "pwm_pub.h"
#include "pwm_init.h"

#include "driver_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"

static SDD_OPERATIONS pwm_op =
{
    pwm_ctrl
};

void (*p_PWM_Int_Handler[PWM_COUNT])(UINT8) = {NULL};

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
		param = GPIO_CFG_PARAM(param, GMODE_DISABLE);
    	ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	}
    ASSERT(DRIV_SUCCESS == ret);
}

static void pwm_icu_configuration(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 ret;
    icu_clk_pwr_dev prm;

    /* set clock power down of icu module*/
    if(pwm_param->channel >= PWM_COUNT)
    {
		PWM_WARN("pwm_iconfig_fail\r\n");
        goto exit_icu;
    }

    PWM_PRT("%s, enable %d\n", __func__, enable);

    switch(pwm_param->channel)
    {
        case PWM0:
        case PWM1:
        case PWM2:
            prm = CLK_PWR_DEV_PWM0;
            break;
        case PWM3:
        case PWM4:
        case PWM5:
            prm = CLK_PWR_DEV_PWM1;
            break;
    }

 	if(enable)
	{
		ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&prm);
		ASSERT(DRIV_SUCCESS == ret);

		if(PWM_CLK_32K == pwm_param->cfg.bits.clk)
		{
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_LPOCLK, (void *)&prm);
		}
		else
		{
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&prm);
		}
		ASSERT(DRIV_SUCCESS == ret);
	}
	else
	{
    	ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&prm);
		ASSERT(DRIV_SUCCESS == ret);
	}

exit_icu:

    return;
}

void pwm_duty_cycle(pwm_param_t *pwm_param)
{

    switch(pwm_param->channel)
    {
        case PWM0:
        {
            REG_APB1_PWM0_END_VALUE_CFG = pwm_param->end_value;
            REG_APB1_PWM0_DUTY_CYC_CFG = pwm_param->duty_cycle;
                           
        }break;
        
        case PWM1:
        {
            REG_APB1_PWM1_END_VALUE_CFG = pwm_param->end_value;
            REG_APB1_PWM1_DUTY_CYC_CFG = pwm_param->duty_cycle;
        
        }break;
        
        case PWM2:
        {
            REG_APB1_PWM2_END_VALUE_CFG = pwm_param->end_value;
            REG_APB1_PWM2_DUTY_CYC_CFG = pwm_param->duty_cycle;
        
        }break;

        case PWM3:
        {
            REG_APB1_PWM3_END_VALUE_CFG = pwm_param->end_value;
            REG_APB1_PWM3_DUTY_CYC_CFG = pwm_param->duty_cycle;
                           
        }break;
        
        case PWM4:
        {
            REG_APB1_PWM4_END_VALUE_CFG = pwm_param->end_value;
            REG_APB1_PWM4_DUTY_CYC_CFG = pwm_param->duty_cycle;
        
        }break;
        
        case PWM5:
        {
            REG_APB1_PWM5_END_VALUE_CFG = pwm_param->end_value;
            REG_APB1_PWM5_DUTY_CYC_CFG = pwm_param->duty_cycle;
        
        }break;
        
        default:break;       
    }
}

static void init_pwm_param(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 value;
    UINT32* pwm_reg;
    UINT8 channel_offset, intc_group;

    if((pwm_param == NULL)
            || (pwm_param->channel >= PWM_COUNT)
            || ((pwm_param->duty_cycle > pwm_param->end_value) & enable))
    {
        PWM_WARN("init_pwm_param fail\r\n");
        return;
    }

    if(pwm_param->channel >= PWM0 && pwm_param->channel <= PWM2)
    {
        pwm_reg = (UINT32*)BASEADDR_PWM0;
        channel_offset = pwm_param->channel;
        intc_group = IRQ_PWM0;
        set_PWM0_Reg0x0_pre_div(pwm_param->pre_divid);
    }
    else if(pwm_param->channel >= PWM3 && pwm_param->channel <= PWM5)
    {
        pwm_reg = (UINT32*)BASEADDR_PWM1;
        channel_offset = pwm_param->channel - PWM3;
        intc_group = IRQ_PWM1;
        set_PWM1_Reg0x0_pre_div(pwm_param->pre_divid);
    }
    
    pwm_icu_configuration(pwm_param, enable);

    if(enable)
    {
        //pwm restart might fail if duty_cycle was cleared when disabled  --Leonardo

        *pwm_reg &= ~(0x7 << (5 * channel_offset + posPWM0_Reg0x0_pwm0_mode));
        *pwm_reg |= ((pwm_param->cfg.bits.mode & 0x7) << (5 * channel_offset + posPWM0_Reg0x0_pwm0_mode)); //set mode

        *pwm_reg &= ~(0x3 << (2 * channel_offset + posPWM0_Reg0x0_pwm0_cpedg_sel));
        *pwm_reg |= ((pwm_param->cpedg_sel & 0x3) << (2 * channel_offset + posPWM0_Reg0x0_pwm0_cpedg_sel));//set cpedg_sel
        
        *pwm_reg &= ~(0x1 << (channel_offset + posPWM0_Reg0x0_pwm0_ctnu_mod));
        *pwm_reg |= ((pwm_param->contiu_mode & 0x1) << (channel_offset + posPWM0_Reg0x0_pwm0_ctnu_mod));

        pwm_duty_cycle(pwm_param);
    }


    if(pwm_param->cfg.bits.int_en)
    {
        *pwm_reg |= (pwm_param->cfg.bits.int_en << (5 * channel_offset + posPWM0_Reg0x0_pwm0_int_en));
    }
    else
    {
        *pwm_reg &= ~(pwm_param->cfg.bits.int_en << (5 * channel_offset + posPWM0_Reg0x0_pwm0_int_en));
    }
    
    if(pwm_param->cfg.bits.en)
    {
        *pwm_reg  |= (pwm_param->cfg.bits.en << (5 * channel_offset + posPWM0_Reg0x0_pwm0_en));
    }else
    {
        *pwm_reg  &= ~(pwm_param->cfg.bits.en << (5 * channel_offset + posPWM0_Reg0x0_pwm0_en));
    }

    if(pwm_param->cfg.bits.int_en)
    {
        p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;
        intc_enable(intc_group);
    }
    else
    {
        p_PWM_Int_Handler[pwm_param->channel] = NULL;
        //intc_disable(intc_group);
    }
    
	if(pwm_param->cfg.bits.mode != PMODE_TIMER)
	{
		pwm_gpio_configuration(pwm_param->channel, enable);
	}
}

static UINT16 pwm_capture_value_get(UINT8 ucChannel)
{
    UINT32 cap_reg;

    switch(ucChannel)
    {
        case PWM0:
            cap_reg = REG_APB1_PWM0_CAP_OUT;
            break;
        case PWM1:
            cap_reg = REG_APB1_PWM1_CAP_OUT;
            break;
        case PWM2:
            cap_reg = REG_APB1_PWM2_CAP_OUT;
            break;
        case PWM3:
            cap_reg = REG_APB1_PWM3_CAP_OUT;
            break;
        case PWM4:
            cap_reg = REG_APB1_PWM4_CAP_OUT;
            break;
        case PWM5:
            cap_reg = REG_APB1_PWM5_CAP_OUT;
            break;
    }

    return REG_READ(cap_reg);
}

static void pwm_int_handler_clear(UINT8 ucChannel)
{
    p_PWM_Int_Handler[ucChannel] = NULL;
}

void pwm_init(void)
{
    intc_service_register(IRQ_PWM0, PRI_FIQ_PWM0, pwm_isr);
    intc_service_register(IRQ_PWM1, PRI_FIQ_PWM1, pwm_isr);

    sddev_register_dev(PWM_DEV_NAME, &pwm_op);
}

void pwm_exit(void)
{
    sddev_unregister_dev(PWM_DEV_NAME);
}

UINT32 pwm_ctrl(UINT32 cmd, void *param)
{
    DRIVER_CTRL_RES ret = DRIV_SUCCESS;
    UINT32 ucChannel;
    UINT32 value;
    pwm_param_t *p_param;
    pwm_capture_t *p_capture;

    switch(cmd)
    {
    case CMD_PWM_UNIT_ENABLE:
    case CMD_PWM_UINT_DISABLE:
    case CMD_PWM_IR_ENABLE:
    case CMD_PWM_IR_DISABLE:
        ucChannel = (*(UINT32 *)param);
        UINT32* pwm_reg;
        UINT8 channel_offset;

        if(ucChannel > 5)
        {
            ret = DRIV_FAIL;
            break;
        }

        if(ucChannel >= PWM0 && ucChannel <= PWM2)
        {
            pwm_reg = (UINT32*)BASEADDR_PWM0;
            channel_offset = ucChannel;
        }
        else if(ucChannel >= PWM3 && ucChannel <= PWM5)
        {
            pwm_reg = (UINT32*)BASEADDR_PWM1;
            channel_offset = ucChannel - PWM3;
        }

        value = REG_READ(pwm_reg);
        if(cmd == CMD_PWM_UNIT_ENABLE)
        {
            value |= (1 << (5 * channel_offset + posPWM0_Reg0x0_pwm0_en));
        }
        else if(cmd == CMD_PWM_UINT_DISABLE)
        {
            value &= ~(1 << (5 * channel_offset + posPWM0_Reg0x0_pwm0_en));
        }
        else if(cmd == CMD_PWM_IR_ENABLE)
        {
            value |= (1 << (5 * channel_offset + posPWM0_Reg0x0_pwm0_int_en));
        }
        else if(cmd == CMD_PWM_IR_DISABLE)
        {
            value &= ~(1 << (5 * channel_offset + posPWM0_Reg0x0_pwm0_int_en));
        }

        REG_WRITE(pwm_reg, value);
        break;
    case CMD_PWM_IR_CLEAR:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = DRIV_FAIL;
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
            ret = DRIV_FAIL;
            break;
        }
        p_capture->value = pwm_capture_value_get(p_capture->ucChannel);
        break;
    case CMD_PWM_DUTY_CYC_CHG:
        p_param = (pwm_param_t *)param;
        pwm_duty_cycle(p_param);
        break;
	case CMD_PWM_DEINIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 0);
		break;
    default:
        ret = DRIV_FAIL;
        break;
    }

    return ret;
}

void pwm_isr(void)
{
    UINT8 i;
    UINT32 ulIntStatus0, ulIntStatus1;
    ulIntStatus0 = REG_PWM0_INTR;
    ulIntStatus1 = REG_PWM1_INTR;
    //PWM_PRT("+++ %s ulIntStatus0 0x%x, ulIntStatus1 0x%x +++\n", __func__, ulIntStatus0, ulIntStatus1);
    for (i=0; i<PWM_COUNT; i++)
    {
        if (ulIntStatus0 & (0x01<<i))
        {
            if (p_PWM_Int_Handler[i] != NULL)
            {
                (void)p_PWM_Int_Handler[i]((unsigned char)i);
            }
        }

        if (ulIntStatus1 & (0x01<<i))
        {
            if (p_PWM_Int_Handler[i+3] != NULL)
            {
                (void)p_PWM_Int_Handler[i+3]((unsigned char)i+3);
            }
        }
    }
    do
    {
        REG_PWM0_INTR = ulIntStatus0;
        REG_PWM1_INTR = ulIntStatus1;
    } while ((REG_PWM0_INTR & ulIntStatus0 & REG_PWM0_INTR_MASK) || (REG_PWM1_INTR & ulIntStatus1 & REG_PWM1_INTR_MASK));   // delays
}

