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
static UINT8 icu_clk_sel = 0;

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

void cpu_reduce_voltage_sleep()
{
    uint32_t tmp_reg;
    //set_AON_GPIO_Reg0x4_GPIO4_Config(0x002); //set GPIO4 OUTPUT HIGH
    //set_AON_GPIO_Reg0x4_GPIO4_Config(0x000); //set GPIO4 OUTPUT LOW
    set_SYS_Reg0x1_gotosleep(1);
    set_SYS_Reg0x1_gotosleep(0);

//    set_PMU_Reg0x14_voltage_ctrl_work_aon(0x06);
//    set_PMU_Reg0x14_voltage_ctrl_work_core(0x06);
    //ICU_PRT("rv:%x\r\n",addPMU_Reg0x14);

   set_PMU_Reg0x14_voltage_ctrl_sleep_aon(03);
//    
   set_PMU_Reg0x14_voltage_ctrl_sleep_core(0x03);
       
   setf_PMU_Reg0x14_sleep_sel; // don't set if use reduce viltage sleep

           
    set_SYS_Reg0x2_core_sel(0x01);
    set_SYS_Reg0x2_core_div(0x0);
    
    setf_SYS_Reg0x17_enb_busrt_sel;
    
    setf_SYS_Reg0x17_CLK96M_PWD;
    setf_SYS_Reg0x17_HP_LDO_PWD;
    setf_SYS_Reg0x17_cb_bias_pwd;


    tmp_reg = addSYS_Reg0x17 | 0x08;
    
    //set_AON_GPIO_Reg0x4_GPIO4_Config(0x002); //set GPIO4 OUTPUT HIGH
    //set_AON_GPIO_Reg0x4_GPIO4_Config(0x000); 
    //set_AON_GPIO_Reg0x4_GPIO4_Config(0x002); //set GPIO4 OUTPUT HIGH
    //set_AON_GPIO_Reg0x4_GPIO4_Config(0x000); 
    set_PMU_Reg0x14_voltage_ctrl_work_aon(0x03);
    set_PMU_Reg0x14_voltage_ctrl_work_core(0x03);
    
    set_SYS_Reg0x2_core_sel(0x00);
    
    addSYS_Reg0x17 = tmp_reg;
    
    setf_SYS_Reg0x1_CPU_PWD;  
    
    addSYS_Reg0x17 = 0x82;
    set_SYS_Reg0x2_core_sel(0x01);
    set_PMU_Reg0x14_voltage_ctrl_work_aon(0x06);
    set_PMU_Reg0x14_voltage_ctrl_work_core(0x06);

}

void cpu_wakeup(void)
{   
    ICU_PRT("%s, %d \r\n", __func__, __LINE__);
    addSYS_Reg0x17 = 0x80;
    
    sddev_control(ICU_DEV_NAME, CMD_ICU_MCU_CLK_SEL, &icu_clk_sel);              

    //set_flash_clk(0x01);  	   
}

void cpu_idle_sleep(void)
{
    ICU_PRT("%s, %d \r\n", __func__, __LINE__);
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
    icu_clk_sel = ICU_MCU_CLK_SEL_16M;
}

void icu_exit(void)
{
    sddev_unregister_dev(ICU_DEV_NAME);
}

UINT32 icu_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret, reg;
    UINT8  dev;

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
            icu_clk_sel = *(UINT32 *)param;
            switch(icu_clk_sel) {
                case ICU_MCU_CLK_SEL_16M: {
                    ICU_CORE_SEL(0x01);
                    set_SYS_Reg0x2_core_div(0x1);
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

        case CMD_CLK_PWR_UP:

    	    dev = *(icu_clk_pwr_dev *)param;
            reg  = REG_READ(REG_SYS_PWD);

            switch(dev)
            {
                case CLK_PWR_DEV_PWM0:
                case CLK_PWR_DEV_PWM1:
                case CLK_PWR_DEV_PWM2:
                	reg &= ~ICU_PWM0_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_PWM3:
                case CLK_PWR_DEV_PWM4:
                case CLK_PWR_DEV_PWM5:
                	reg &= ~ICU_PWM1_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_UART1:
                    reg &= ~ICU_UART1_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_UART2:
                    reg &= ~ICU_UART2_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_WDT:
                    reg &= ~ICU_WDT_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_I2S_PCM:
                    reg &= ~ICU_I2S_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_SPI:
                    reg &= ~ICU_SPI_CLK_PWD_MASK;
                    break;

                default:
                    break;
            }

            REG_WRITE(REG_SYS_PWD, reg);
            break;

        case CMD_CLK_PWR_DOWN:

    	    dev = *(icu_clk_pwr_dev *)param;
            reg  = REG_READ(REG_SYS_PWD);

            switch(dev)
            {
                case CLK_PWR_DEV_PWM0:
                case CLK_PWR_DEV_PWM1:
                case CLK_PWR_DEV_PWM2:
                	reg |= ICU_PWM0_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_PWM3:
                case CLK_PWR_DEV_PWM4:
                case CLK_PWR_DEV_PWM5:
                	reg |= ICU_PWM1_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_UART1:
                    reg |= ICU_UART1_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_UART2:
                    reg |= ICU_UART2_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_WDT:
                    reg |= ICU_WDT_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_I2S_PCM:
                    reg |= ICU_I2S_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_SPI:
                    reg |= ICU_SPI_CLK_PWD_MASK;
                    break;

                default:
                    break;
            }

            REG_WRITE(REG_SYS_PWD, reg);
            break;

        case CMD_CONF_PWM_PCLK:

            dev = *(icu_clk_pwr_dev *)param;
            reg = REG_READ(REG_SYS_CLK_SEL);

            switch(dev)
            {
                case CLK_PWR_DEV_PWM0:
                case CLK_PWR_DEV_PWM1:
                case CLK_PWR_DEV_PWM2:
                    reg &= ~(0x03 << SYS_PWM0_SEL_POS);
                    reg |= (0x01 << SYS_PWM0_SEL_POS);
                    break;

                case CLK_PWR_DEV_PWM3:
                case CLK_PWR_DEV_PWM4:
                case CLK_PWR_DEV_PWM5:
                    reg &= ~(0x03 << SYS_PWM1_SEL_POS);
                    reg |= (0x01 << SYS_PWM1_SEL_POS);
                    break;
            }
            
            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;

        case CMD_CONF_PWM_LPOCLK:

            dev = *(icu_clk_pwr_dev *)param;
            reg = REG_READ(REG_SYS_CLK_SEL);

            switch(dev)
            {
                case CLK_PWR_DEV_PWM0:
                case CLK_PWR_DEV_PWM1:
                case CLK_PWR_DEV_PWM2: 
                    reg &= ~(0x03 << SYS_PWM0_SEL_POS);
                    break;

                case CLK_PWR_DEV_PWM3:
                case CLK_PWR_DEV_PWM4:
                case CLK_PWR_DEV_PWM5:
                    reg &= ~(0x03 << SYS_PWM1_SEL_POS);
                    break;
            }

            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;

        case CMD_SPI_CLK_SEL:
            reg = REG_READ(REG_SYS_CLK_SEL);
            reg &= ~(0x01 << SYS_SPI_SEL_POS);
            reg |= (*(uint8_t *)param) << SYS_SPI_SEL_POS;
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

        default:
            break;
    }
    return ret;
}

// EOF
