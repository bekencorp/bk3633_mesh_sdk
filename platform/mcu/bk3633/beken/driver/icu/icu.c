#include "include.h"
#include "arm_arch.h"

#include "icu_pub.h"
#include "icu.h"
#include "BK3633_RegList.h"
#include "reg_intc.h"

#include "drv_model_pub.h"


static SDD_OPERATIONS icu_op =
{
    icu_ctrl
};

/*******************************************************************/
void icu_init(void)
{

    UINT32 param;

    sddev_register_dev(ICU_DEV_NAME, &icu_op);

/* 	REG_AHB0_ICU_FLASH &= ~(0xff << 16);
	REG_AHB0_ICU_FLASH |= (0x15 << 16); 
	REG_AHB0_ICU_CPU_STATUS  = 0x771;  // spi vol       hh

    //REG_AHB0_ICU_LPO_CLK_ON |= (0x01 << 4);
    
    REG_AHB0_ICU_DIGITAL_PWD = REG_AHB0_ICU_DIGITAL_PWD & (~0X02);

    REG_AHB0_ICU_CORECLKCON = 0X00;

    REG_AHB0_ICU_CLKSRCSEL = 0X000001F9; //usr 16m

    REG_AHB0_ICU_ANA_CTL &= ~(0X01 << 6); */

    set_SYS_Reg0x2_core_sel(0x01);
    set_SYS_Reg0x2_core_div(0x01);///16M CLK
    setf_SYS_Reg0x17_CLK96M_PWD;
    clrf_SYS_Reg0xd_PLL_PWR_sel;
    setf_SYS_Reg0xb_pwd_on_boostsel;
    addSYS_Reg0x17 = 0x82;

}

void icu_exit(void)
{
    sddev_unregister_dev(ICU_DEV_NAME);
}

UINT32 icu_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret, reg;
    UINT8  dev, posi;
    //os_printf("%s cmd %d\r\n", __func__, cmd);
    ret = ICU_SUCCESS;

    switch(cmd)
    {

    case CMD_ICU_CLKGATING_DISABLE:
        reg = REG_READ(ICU_CLK_GATE_DISABLE);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_CLK_GATE_DISABLE, reg);
        break;

    case CMD_ICU_CLKGATING_ENABLE:
        reg = REG_READ(ICU_CLK_GATE_DISABLE);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_CLK_GATE_DISABLE, reg);
        break;

    case CMD_ICU_MCU_CLK_SEL:
    	//if(*(UINT32 *)param == ICU_MCU_CLK_SEL_16M)
    	{
/* 			REG_AHB0_ICU_FLASH &= ~(0xff << 16);
			REG_AHB0_ICU_FLASH |= (0x15 << 16);
			REG_AHB0_ICU_CPU_STATUS  = 0x611;  // spi vol

            REG_AHB0_ICU_DIGITAL_PWD = REG_AHB0_ICU_DIGITAL_PWD & (~0X02);

            REG_AHB0_ICU_CORECLKCON = 0X00;

            REG_AHB0_ICU_CLKSRCSEL = 0X000001F9; //usr 16m

            REG_AHB0_ICU_ANA_CTL |= (0X01 << 6); */
            set_SYS_Reg0x2_core_sel(0x01);
            set_SYS_Reg0x2_core_div(0x01);///16M CLK
            setf_SYS_Reg0xb_pwd_on_boostsel;
            addSYS_Reg0x17 = 0x82;

    	}
/*     	else if(*(UINT32 *)param == ICU_MCU_CLK_SEL_64M)
    	{
			REG_AHB0_ICU_FLASH &= ~(0xff << 16);
			REG_AHB0_ICU_FLASH |= (0x15 << 16);
			REG_AHB0_ICU_CPU_STATUS  = 0x611;  // spi vol

			REG_AHB0_ICU_DIGITAL_PWD = REG_AHB0_ICU_DIGITAL_PWD & (~0X02);

			REG_AHB0_ICU_CORECLKCON = 0X00; //clk div 1

			REG_AHB0_ICU_CLKSRCSEL = 0X000005FB; //usr PLL CLK SELT 64M

			REG_AHB0_ICU_ANA_CTL |= (0X01 << 6);

    	} */
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
            case CLK_PWR_DEV_UART:
                reg  = REG_READ(REG_SYS_PWD);
                reg &= ~ICU_UART_CLK_PWD_MASK;
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
            case CLK_PWR_DEV_UART:
                reg  = REG_READ(REG_SYS_PWD);
                reg |= ICU_UART_CLK_PWD_MASK;
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
        reg = REG_READ(ICU_INT_ENABLE);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_INT_ENABLE, reg);
        break;

    case CMD_ICU_INT_ENABLE:
        reg = REG_READ(REG_SYS_INT_EN);
        reg |= (*(UINT32 *)param);
        REG_WRITE(REG_SYS_INT_EN, reg);
        break;

    case CMD_ICU_GLOBAL_INT_DISABLE:
        reg = REG_READ(ICU_IRQ_ENABLE);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(ICU_IRQ_ENABLE, reg);
        break;

    case CMD_ICU_GLOBAL_INT_ENABLE:
        reg = REG_READ(ICU_IRQ_ENABLE);
        reg |= (*(UINT32 *)param);
        REG_WRITE(ICU_IRQ_ENABLE, reg);
        break;

    case CMD_GET_INTR_STATUS:
        ret = REG_READ(ICU_INT_FLAG);
        break;

    case CMD_CLR_INTR_STATUS:
        ASSERT(param);
        reg = REG_READ(ICU_INT_FLAG);
        reg |= *(UINT32 *)param; ///write 1 to clear interrupt status
        REG_WRITE(ICU_INT_FLAG, reg);
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
