/**
****************************************************************************************
*
* @file icu.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "include.h"
#include "arm_arch.h"

#include "icu_pub.h"
#include "icu.h"
#include "BK3633_RegList.h"
#include "reg_intc.h"

#include "drv_model_pub.h"
#include "pwm_pub.h"

static UINT8 mcu_sleep_mode;

extern volatile uint32_t XVR_ANALOG_REG_BAK[32];

static SDD_OPERATIONS icu_op =
{
    icu_ctrl
};

/*******************************************************************/
static void icu_set_sleep_mode(MCU_SLEEP_MODE mode)
{
    if(mode >= MCU_SLEEP_MODE_NUM)
    {
        return;
    }

    mcu_sleep_mode = mode;
}

static void icu_get_sleep_mode(MCU_SLEEP_MODE* mode)
{
    *mode = mcu_sleep_mode;
}



#define LDO_MODE_IN_SLEEP          1
void cpu_reduce_voltage_sleep(void)
{
    printf("%s, %d \r\n\r\n", __func__, __LINE__);
   uint32_t tmp_reg;
	uint8_t delay_nop=0;

    //set_flash_clk(0x08);
    set_SYS_Reg0x2_core_sel(0x01);
    set_SYS_Reg0x2_core_div(0x0);
#if(LDO_MODE_IN_SLEEP)
    ///
	addXVR_Reg0x6 = 0x8587CC00;//0x80B7CE20  ;
	XVR_ANALOG_REG_BAK[6] = 0x8587CC00;//0x80B7CE20;
	addXVR_Reg0xa = 0x9C03F86B;//0x9C27785B  ;
	XVR_ANALOG_REG_BAK[0xa] = 0x9C03F86B;//0x9C27785B;
#endif
    
    setf_SYS_Reg0x17_enb_busrt_sel; 
    setf_SYS_Reg0x17_CLK96M_PWD;
    setf_SYS_Reg0x17_HP_LDO_PWD;
    setf_SYS_Reg0x17_cb_bias_pwd;

   
    tmp_reg = addSYS_Reg0x17 | 0x08;
    
#if(!LDO_MODE)
    set_PMU_Reg0x14_voltage_ctrl_work_aon(0x05);
    set_PMU_Reg0x14_voltage_ctrl_work_core(0x05);
#endif
    set_SYS_Reg0x2_core_sel(0x00);

    addSYS_Reg0x17 = tmp_reg;

    setf_SYS_Reg0x1_CPU_PWD;  ////sleep

    delay_nop++;
    addSYS_Reg0x17 = 0x80;

    delay_nop++;
    delay_nop++;
    addPMU_Reg0x14=0x6666;
    delay_nop++;
	delay_nop++;
	delay_nop++;
    set_SYS_Reg0x2_core_sel(0x01);

#if(LDO_MODE_IN_SLEEP)
    ///
    addXVR_Reg0x6 = 0x85A7CC00;//0x8097CE20  ;
	XVR_ANALOG_REG_BAK[6] = 0x85A7CC00;//0x8097CE20;
    addXVR_Reg0xa = 0x9C03F86F;//0x9C27785B  ;
    XVR_ANALOG_REG_BAK[0xa] = 0x9C03F86F;//0x9C27785B;
#endif
}

void cpu_wakeup(void)
{   
    printf("%s, %d \r\n\r\n", __func__, __LINE__);
    addSYS_Reg0x17 = 0x80;
    
            set_SYS_Reg0x2_core_div(0x1);            
            set_SYS_Reg0x2_core_div(0x1);              

    //set_flash_clk(0x01);  	   
}

void cpu_idle_sleep(void)
{
    printf("%s, %d \r\n\r\n", __func__, __LINE__);
    clrf_PMU_Reg0x14_sleep_sel; 
    setf_SYS_Reg0x1_CPU_PWD;
}








void icu_init(void)
{
    UINT32 param, reg;

    sddev_register_dev(ICU_DEV_NAME, &icu_op);

    JTAG_MODE_CLOSE;   ///close JTAG

    param = ICU_MCU_CLK_SEL_16M;
    sddev_control(ICU_DEV_NAME, CMD_ICU_MCU_CLK_SEL, &param);

    SET_PWD_BOOST;

    reg = 1 << POS_ENB_BURST_SEL;
    REG_WRITE(REG_SYS_TEST, reg);

//    set_PMU_Reg0x14_voltage_ctrl_sleep_aon(0x04);
  //  set_PMU_Reg0x14_voltage_ctrl_sleep_core(0x04);

    icu_set_sleep_mode(MCU_REDUCE_VO_SLEEP);
}

void icu_exit(void)
{
    sddev_unregister_dev(ICU_DEV_NAME);
}

UINT32 icu_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret, reg;
    UINT8  dev, posi;

    ret = ICU_SUCCESS;

    switch(cmd)
    {

        case CMD_ICU_CLKGATING_DISABLE:
            reg = REG_READ(REG_CLK_GATE_DISABLE);
            reg |= (*(UINT32 *)param);
            REG_WRITE(REG_CLK_GATE_DISABLE, reg);
            break;

        case CMD_ICU_CLKGATING_ENABLE:
            reg = REG_READ(REG_CLK_GATE_DISABLE);
            reg &= ~(*(UINT32 *)param);
            REG_WRITE(REG_CLK_GATE_DISABLE, reg);
            break;

        case CMD_ICU_MCU_CLK_SEL:
            switch(*(UINT32 *)param) {
                case ICU_MCU_CLK_SEL_16M: {
                    ICU_CORE_SEL(0x01);
                    set_SYS_Reg0x2_core_div(0x0);
                    setf_SYS_Reg0x17_CLK96M_PWD;
                    clrf_SYS_Reg0xd_PLL_PWR_sel;
                    break;
                }
                case ICU_MCU_CLK_SEL_32M: {
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 20);
                    XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 17);
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 16);
                    addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
                    clrf_SYS_Reg0x17_CLK96M_PWD;
                    setf_SYS_Reg0xd_PLL_PWR_sel;
                    set_SYS_Reg0x2_core_div(0x2);            
                    ICU_CORE_SEL(0x03);
                    break;
                }
                case ICU_MCU_CLK_SEL_48M: {
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 20);
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 17);
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 16);
                    addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
                    clrf_SYS_Reg0x17_CLK96M_PWD;
                    setf_SYS_Reg0xd_PLL_PWR_sel;
                    set_SYS_Reg0x2_core_div(0x2);         
                    ICU_CORE_SEL(0x03);
                    break;
                }
                case ICU_MCU_CLK_SEL_64M: {
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 20);
                    XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 17);
                    XVR_ANALOG_REG_BAK[9]|= (0x01 << 16);
                    addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
                    clrf_SYS_Reg0x17_CLK96M_PWD;
                    setf_SYS_Reg0xd_PLL_PWR_sel;
                    set_SYS_Reg0x2_core_div(0x1);            
                    ICU_CORE_SEL(0x03);
                    break;
                }
                case ICU_MCU_CLK_SEL_80M: {
                    XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 20);
                    XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 17);
                    XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 16);
                    addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
                    clrf_SYS_Reg0x17_CLK96M_PWD;
                    setf_SYS_Reg0xd_PLL_PWR_sel;
                    set_SYS_Reg0x2_core_div(0x1);            
                    ICU_CORE_SEL(0x03);
                    break;
                }
                default:
                    break;
            }
            break;

    /*     case CMD_ICU_CLK_32K_SRC_SEL:
            reg = *(UINT32 *)param & 0x1;
            REG_WRITE(ICU_CLK_32K_SEL, reg);
            break; */

        case CMD_CLK_PWR_UP:
            dev = *(PWM_CHAN_E *)param;

            switch(dev)
            {
                case CLK_PWR_DEV_PWM0:
                case CLK_PWR_DEV_PWM1:
                case CLK_PWR_DEV_PWM2:
                    posi = SYS_PWM0_PWD_POS;
                    reg  = REG_READ(REG_SYS_PWD);
                    reg &= ~(0x01UL << posi);
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_PWM3:
                case CLK_PWR_DEV_PWM4:
                case CLK_PWR_DEV_PWM5:
                    posi = SYS_PWM1_PWD_POS;
                    reg  = REG_READ(REG_SYS_PWD);
                    reg &= ~(0x01UL << posi);
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_UART1:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg &= ~ICU_UART1_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_UART2:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg &= ~ICU_UART2_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_WDT:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg &= ~ICU_WDT_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_I2S_PCM:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg &= ~ICU_I2S_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                default:
                    break;
            }
            break;
        case CMD_CLK_PWR_DOWN:
            dev = *(UINT8 *)param;
            switch(dev)
            {
                case CLK_PWR_DEV_PWM0:
                case CLK_PWR_DEV_PWM1:
                case CLK_PWR_DEV_PWM2:
                    posi = SYS_PWM0_PWD_POS;
                    reg  = REG_READ(REG_SYS_PWD);
                    reg |= (0x01UL << posi);
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_PWM3:
                case CLK_PWR_DEV_PWM4:
                case CLK_PWR_DEV_PWM5:
                    posi = SYS_PWM1_PWD_POS;
                    reg  = REG_READ(REG_SYS_PWD);
                    reg |= (0x01UL << posi);
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_UART1:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg |= ICU_UART1_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_UART2:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg |= ICU_UART2_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_WDT:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg |= ICU_WDT_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                case CLK_PWR_DEV_I2S_PCM:
                    reg  = REG_READ(REG_SYS_PWD);
                    reg |= ICU_I2S_CLK_PWD_MASK;
                    REG_WRITE(REG_SYS_PWD, reg);
                    break;
                default:
                    break;
            }
            break;

        case CMD_CONF_PWM_PCLK:
            dev = *(PWM_CHAN_E *)param;
            switch(dev)
            {
                case PWM0:
                case PWM1:
                case PWM2: 
                    posi = SYS_PWM0_SEL_POS;
                    break;
                case PWM3:
                case PWM4:
                case PWM5: 
                    posi = SYS_PWM1_SEL_POS;
                    break;
            }

            reg = REG_READ(REG_SYS_CLK_SEL);
            reg &= ~(0x03 << posi);
            reg |= (0x01 << posi);
            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;

        case CMD_CONF_PWM_LPOCLK:
            dev = *(PWM_CHAN_E *)param;
            switch(dev)
            {
                case PWM0:
                case PWM1:
                case PWM2: 
                    posi = SYS_PWM0_SEL_POS;
                    break;
                case PWM3:
                case PWM4:
                case PWM5: 
                    posi = SYS_PWM1_SEL_POS;
                    break;
            }

            reg = REG_READ(REG_SYS_CLK_SEL);
            reg &= ~(0x03 << posi);
            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;

        case CMD_ICU_INT_DISABLE:
            reg = REG_READ(REG_SYS_INT_EN);
            reg &= ~(*(UINT32 *)param);
            REG_WRITE(REG_SYS_INT_EN, reg);
            break;

        case CMD_ICU_INT_ENABLE:
            reg = REG_READ(REG_SYS_INT_EN);
            reg |= (*(UINT32 *)param);
            REG_WRITE(REG_SYS_INT_EN, reg);
            break;

        case CMD_ICU_GLOBAL_INT_DISABLE:
            reg = REG_READ(REG_ICU_IRQ_ENABLE);
            reg &= ~(*(UINT32 *)param);
            REG_WRITE(REG_ICU_IRQ_ENABLE, reg);
            break;

        case CMD_ICU_GLOBAL_INT_ENABLE:
            reg = REG_READ(REG_ICU_IRQ_ENABLE);
            reg |= (*(UINT32 *)param);
            REG_WRITE(REG_ICU_IRQ_ENABLE, reg);
            break;

        case CMD_GET_INTR_STATUS:
            ret = REG_READ(REG_ICU_INT_FLAG);
            break;

        case CMD_CLR_INTR_STATUS:
            ASSERT(param);
            reg = REG_READ(REG_ICU_INT_FLAG);
            reg |= *(UINT32 *)param; ///write 1 to clear interrupt status
            REG_WRITE(REG_ICU_INT_FLAG, reg);
            break;

        case CMD_SET_SLEEP_MODE:
            icu_set_sleep_mode(*(MCU_SLEEP_MODE *)param);
            break;

        case CMD_GET_SLEEP_MODE:
            icu_get_sleep_mode((MCU_SLEEP_MODE *)param);
            break;


    #if 0
        case CMD_ARM_WAKEUP:
            reg = (*(UINT32*)param);
            REG_WRITE(ICU_ARM_WAKEUP_EN, reg);            
            break;
    #endif
        default:
            break;
    }
    return ret;
}

// EOF
