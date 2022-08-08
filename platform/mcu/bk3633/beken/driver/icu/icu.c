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
#include "timer_pub.h"
#include "BK3633_RegList.h"
#include "reg_intc.h"

#include "drv_model_pub.h"
#include "pwm_pub.h"

enum SYS_CLK_SRC
{
    CLK_SRC_32K = 0x01 << 0,
	CLK_SRC_XTAL = 0x01 << 1,	
    CLK_SRC_DPLL_64M = 0x01 << 2,//XVR[9] bits[20][17][16] 000:80M ,101:64M,111:96M
    CLK_SRC_DPLL_80M = 0x01 << 3,
    CLK_SRC_DPLL_96M = 0x01 << 4,

};

#define MCU_CLK_16M   1 // XTAL
#define MCU_CLK_32M	  2 //DPLL_64M / 2,DPLL_96M / 3
#define MCU_CLK_40M	  3 //DPLL_80M / 2
#define MCU_CLK_48M   4 //DPLL_96M
#define MCU_CLK_64M	  5 //DPLL_64M
#define MCU_CLK_80M   6 //DPLL_80M

#define PERI_CLK_16M   1 // XTAL
#define PERI_CLK_32M	2 //DPLL_64M / 2,DPLL_96M / 3
#define PERI_CLK_40M	3 //DPLL_80M / 2
#define PERI_CLK_48M   4 //DPLL_96M
#define PERI_CLK_64M   5 //DPLL_64M
#define PERI_CLK_80M   6 //DPLL_80M

#ifndef BIT
#define BIT(n)        (1 << n)   
#endif

static UINT8 mcu_sleep_mode;
static UINT8 icu_clk_sel = 0;

static uint8_t cpu_clk_freq_default = MCU_CLK_80M;
static uint8_t peri_clk_freq_default = PERI_CLK_40M;

static uint8_t core_clk_src_default; // cpu clk
static uint8_t peri_clk_src_default; // flash clk
static uint8_t dpll_clk_src_default; // dpll clk
static uint8_t core_clk_div; // 
static uint8_t peri_clk_div; //

static uint8_t core_clk_src_sel; // cpu clk

extern volatile uint32_t XVR_ANALOG_REG_BAK[32];

static uint8_t clk_freq_check2src_set(void);
static void clk_src_div_clac(void);

extern void set_flash_clk_xtal16M(void);

static SDD_OPERATIONS icu_op =
{
    icu_ctrl
};
uint8_t system_sleep_status = 0;
static volatile uint8_t reduce_voltage_set=0;

static void Delay_us(int num)
{
    volatile int x, y;
    for(y = 0; y < num; y ++ )
    {
        for(x = 0; x < 10; x++);
    }
}

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

__ATTR_APP_CODE void cpu_reduce_voltage_sleep()
{
    uint32_t tmp_reg;
    uint32_t calc_num = 0;
    
  //  uart_printf("sleep\r\n");
    /**  set flash & cpu core use xtal***/
    set_flash_clk_xtal16M();
    //addPMU_Reg0x2 = 0x11;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    set_SYS_Reg0x2_core_sel(0x01);
    //addPMU_Reg0x2 = 0x12;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    set_SYS_Reg0x2_core_div(0x0);
    //addPMU_Reg0x2 = 0x13;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    setf_SYS_Reg0x17_enb_busrt_sel;
    //addPMU_Reg0x2 = 0x14;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    setf_SYS_Reg0x17_CLK96M_PWD;
    //addPMU_Reg0x2 = 0x15;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    setf_SYS_Reg0x17_HP_LDO_PWD;
    //addPMU_Reg0x2 = 0x16;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    setf_SYS_Reg0x17_cb_bias_pwd;
    //addPMU_Reg0x2 = 0x17;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    tmp_reg = addSYS_Reg0x17 | 0x08;
    //addPMU_Reg0x2 = 0x18;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    system_sleep_status = 1;
    //addPMU_Reg0x2 = 0x19;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    set_PMU_Reg0x14_voltage_ctrl_work_aon(0x07);
    //addPMU_Reg0x2 = 0x20;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    set_PMU_Reg0x14_voltage_ctrl_work_core(0x07);
    //addPMU_Reg0x2 = 0x21;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    /**  set cpu core use 32k***/
    set_SYS_Reg0x2_core_sel(0x00);
    //addPMU_Reg0x2 = 0x22;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    addSYS_Reg0x17 = tmp_reg;
    //addPMU_Reg0x2 = 0x23;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    setf_SYS_Reg0x1_CPU_PWD;  
    //addPMU_Reg0x2 = 0x24;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    addSYS_Reg0x17 = 0x80; // open 96M
   //delay 30us        
    //Delay_us(30);
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    //addPMU_Reg0x2 = 0x25;
   // uart_printf("cpuwake\r\n");
    /**  set cpu core use xtal***/
    set_SYS_Reg0x2_core_sel(0x01);
    //addPMU_Reg0x2 = 0x26;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    addPMU_Reg0x14=0x7777; // config work & sleep cpu voltage
    //addPMU_Reg0x2 = 0x27;
    asm volatile ("nop"::);
    asm volatile ("nop"::);
}

void cpu_wakeup(void)
{  
   //
    if(system_sleep_status == 1)
    {
              
        mcu_default_clk_switch(); // 

        system_sleep_status = 0;
    }	   
}
//__RAM_CODE
  void cpu_idle_sleep(void)
{

    if(system_sleep_status == 0)
    {
        clrf_PMU_Reg0x14_sleep_sel; 
        setf_SYS_Reg0x1_CPU_PWD;  
    }
}
void mcu_default_clk_switch(void)
{  
    set_SYS_Reg0x2_core_div(core_clk_div);
    set_SYS_Reg0x2_core_sel(core_clk_src_sel);
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

    // set_PMU_Reg0x14_voltage_ctrl_sleep_aon(0x04);
    // set_PMU_Reg0x14_voltage_ctrl_sleep_core(0x04);

    setf_PMU_Reg0x1_wdt_reset_ctrl;     //set wdt_reset_ctrl to use always_on field.
    setf_PMU_Reg0x1_wdt_reset_ctrl1;    //set wdt_reset_ctrl1 to use always_on field.

    icu_set_sleep_mode(MCU_REDUCE_VO_SLEEP);
    icu_clk_sel = ICU_MCU_CLK_SEL_16M;
    set_PMU_Reg0x1_boot_rom_en(0);
}

void icu_exit(void)
{
    sddev_unregister_dev(ICU_DEV_NAME);
}

uint32_t icu_get_reset_reason(void)
{
    uint32_t reg0x2_val;

    reg0x2_val = addPMU_Reg0x2 & 0xffffffff;
    // printf("%s reg0x2_val %x \r\n", __func__, reg0x2_val);

    return reg0x2_val;
}

void icu_set_reset_reason(uint32_t reson_data)
{
    GLOBAL_INT_DISABLE();
    addPMU_Reg0x2 = reson_data;
    GLOBAL_INT_RESTORE();
    ///printf("%s, set reset reason=%x\r\n", __func__, addPMU_Reg0x2);   
}

uint8_t core_peri_clk_freq_set(uint8_t core_clk, uint8_t peri_clk)
{
    uint8_t stu = 0;
    cpu_clk_freq_default = core_clk;
    peri_clk_freq_default = peri_clk;
    stu = clk_freq_check2src_set();
    return stu;
}
void core_clk_freq_src_get(uint8_t *clk_src,uint8_t *clk_freq)
{
    *clk_src = core_clk_src_default ;
    *clk_freq = cpu_clk_freq_default ;
}

void peri_clk_freq_src_get(uint8_t *clk_src, uint8_t *clk_freq)
{
    *clk_src = peri_clk_src_default;
            
    *clk_freq = peri_clk_freq_default ;
}

void core_peri_clk_div_get(uint8_t *core_div, uint8_t *peri_div)
{
    *core_div = core_clk_div;
            
    *peri_div = peri_clk_div ;
}

static uint8_t clk_freq_check2src_set(void)
{
    uint32_t cpu_clk_src_choose = 0;
    uint32_t perip_clk_src_choose = 0;
    uint32_t src_choose_tmp = 0;
    uint8_t stu = 0;
    switch(cpu_clk_freq_default)
    {
        case MCU_CLK_16M:
        {
            cpu_clk_src_choose = CLK_SRC_XTAL | CLK_SRC_DPLL_64M | CLK_SRC_DPLL_80M | CLK_SRC_DPLL_96M;
        }break;
        
        case MCU_CLK_32M:
        {   cpu_clk_src_choose = CLK_SRC_DPLL_64M | CLK_SRC_DPLL_96M;
        }break;
        
        case MCU_CLK_40M:
        {   cpu_clk_src_choose = CLK_SRC_DPLL_80M;
        }break;
        
        case MCU_CLK_48M:
        {
            cpu_clk_src_choose = CLK_SRC_DPLL_96M;
        }break;
        
        case MCU_CLK_64M:
        {
            cpu_clk_src_choose = CLK_SRC_DPLL_64M;
        }break;
        
        case MCU_CLK_80M:
        {
            cpu_clk_src_choose = CLK_SRC_DPLL_80M;
        }break;
        
        default:
        {
        
        }break;
    
    }

    switch(peri_clk_freq_default)
    {
        case PERI_CLK_16M:
        {
            perip_clk_src_choose = CLK_SRC_XTAL | CLK_SRC_DPLL_64M ;
        }break;
        
        case PERI_CLK_32M:
        {   perip_clk_src_choose = CLK_SRC_DPLL_64M ;
        }break;
        
        case PERI_CLK_40M:
        {   perip_clk_src_choose = CLK_SRC_DPLL_80M;
        }break;
        
        case PERI_CLK_48M:
        {
            perip_clk_src_choose = CLK_SRC_DPLL_96M;
        }break;
        
        case PERI_CLK_64M:
        {
            perip_clk_src_choose = CLK_SRC_DPLL_64M;
        }break;
        
        case PERI_CLK_80M:
        {
            perip_clk_src_choose = CLK_SRC_DPLL_80M;
        }break;
        
        default:
        {
        
        }break;
    
    }
    
    // printf("perip_clk_src_choose = 0x%08x\r\n",perip_clk_src_choose);
    src_choose_tmp = (perip_clk_src_choose & cpu_clk_src_choose) & 0x1E;
    // printf("src_choose_tmp = 0x%08x\r\n",src_choose_tmp);

    if(src_choose_tmp)
    {
        if(src_choose_tmp & CLK_SRC_XTAL)
        {
            core_clk_src_default = CLK_SRC_XTAL; // cpu clk
            peri_clk_src_default = CLK_SRC_XTAL; // flash clk
            dpll_clk_src_default = CLK_SRC_DPLL_96M; // dpll clk

        }
        else if(src_choose_tmp & CLK_SRC_DPLL_64M)
        {
            core_clk_src_default = CLK_SRC_DPLL_64M; // cpu clk
            peri_clk_src_default = CLK_SRC_DPLL_64M; // flash clk
            dpll_clk_src_default = CLK_SRC_DPLL_64M; // dpll clk
        
        }else if(src_choose_tmp & CLK_SRC_DPLL_80M)
        {
            core_clk_src_default = CLK_SRC_DPLL_80M; // cpu clk
            peri_clk_src_default = CLK_SRC_DPLL_80M; // flash clk
            dpll_clk_src_default = CLK_SRC_DPLL_80M; // dpll clk
        
        }else if(src_choose_tmp & CLK_SRC_DPLL_96M)
        {
            core_clk_src_default = CLK_SRC_DPLL_96M; // cpu clk
            peri_clk_src_default = CLK_SRC_DPLL_96M; // flash clk
            dpll_clk_src_default = CLK_SRC_DPLL_96M; // dpll clk
        }
        
        if(cpu_clk_src_choose & CLK_SRC_XTAL)
        {
            
            core_clk_src_default = CLK_SRC_XTAL; // peri clk
           
        }
        else if(perip_clk_src_choose & CLK_SRC_XTAL)
        {
            
            peri_clk_src_default = CLK_SRC_XTAL; // peri clk
           
        }
    
    }else   
    {
        if(cpu_clk_src_choose & CLK_SRC_XTAL)
        {
            
            core_clk_src_default = CLK_SRC_XTAL; // peri clk
            peri_clk_src_default = perip_clk_src_choose;
            dpll_clk_src_default = perip_clk_src_choose;
           
        }
        else if(perip_clk_src_choose & CLK_SRC_XTAL)
        {
            
            peri_clk_src_default = CLK_SRC_XTAL; // peri clk
            core_clk_src_default = cpu_clk_src_choose; // peri clk
            dpll_clk_src_default = cpu_clk_src_choose;
           
        }
        else 
        {
            core_clk_src_default = CLK_SRC_XTAL; // peri clk
            peri_clk_src_default = CLK_SRC_XTAL;
            dpll_clk_src_default = CLK_SRC_DPLL_96M;
            peri_clk_freq_default = PERI_CLK_16M;
            cpu_clk_freq_default = MCU_CLK_16M;
            stu = 1;
        }
    }
    
    //printf("core_clk_src_default = 0x%08x\r\n", core_clk_src_default);
    //printf("peri_clk_src_default = 0x%08x\r\n", peri_clk_src_default);
    //printf("dpll_clk_src_default = 0x%08x\r\n", dpll_clk_src_default);
    //printf("cpu_clk_freq_default = 0x%08x\r\n", cpu_clk_freq_default);
    //printf("peri_clk_freq_default = 0x%08x\r\n", peri_clk_freq_default);
    
    switch(dpll_clk_src_default)
    {
        case CLK_SRC_DPLL_64M:
        {
            XVR_ANALOG_REG_BAK[9]|= (0x01 << 20);
            XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 17);
            XVR_ANALOG_REG_BAK[9]|= (0x01 << 16);
            addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
            clrf_SYS_Reg0x17_CLK96M_PWD;
            setf_SYS_Reg0xd_PLL_PWR_sel;
        }break;
        
        case CLK_SRC_DPLL_80M:
        {            
            XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 20);
            XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 17);
            XVR_ANALOG_REG_BAK[9]&= ~(0x01 << 16);
            addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
            clrf_SYS_Reg0x17_CLK96M_PWD;
            setf_SYS_Reg0xd_PLL_PWR_sel;
        
        }break;
        
        case CLK_SRC_DPLL_96M:
        {       
            XVR_ANALOG_REG_BAK[9]|= (0x01 << 20);
            XVR_ANALOG_REG_BAK[9]|= (0x01 << 17);
            XVR_ANALOG_REG_BAK[9]|= (0x01 << 16);
            addXVR_Reg0x9 = XVR_ANALOG_REG_BAK[9];
            clrf_SYS_Reg0x17_CLK96M_PWD;
            setf_SYS_Reg0xd_PLL_PWR_sel;
        
        }break;
        
        default:break;
    }
    clk_src_div_clac();
    Delay_us(30);
       
    return stu;  
}

static void clk_src_div_clac(void)
{
    core_clk_div = 1;
    peri_clk_div = 1;
    switch(dpll_clk_src_default)
    {
        case CLK_SRC_DPLL_64M:
        {
            if(core_clk_src_default != CLK_SRC_XTAL)
            {
                if(cpu_clk_freq_default == MCU_CLK_32M)
                {
                    core_clk_div = 2;
                }else if(cpu_clk_freq_default == MCU_CLK_64M)
                {
                    core_clk_div = 1;
                }
                core_clk_src_sel = 0x03;                
            }else
            {
                core_clk_src_sel = 0x01;
            }
            if(peri_clk_src_default != CLK_SRC_XTAL)
            {
                if(peri_clk_freq_default == PERI_CLK_32M)
                {
                    peri_clk_div = 2;
                }else if(cpu_clk_freq_default == PERI_CLK_64M)
                {
                    peri_clk_div = 1;
                }
            }
        }break;
        
        case CLK_SRC_DPLL_80M:
        { 
            if(core_clk_src_default != CLK_SRC_XTAL)
            {
                if(cpu_clk_freq_default == MCU_CLK_40M)
                {
                    core_clk_div = 2;
                }else if(cpu_clk_freq_default == MCU_CLK_80M)
                {
                    core_clk_div = 1;
                }                
                core_clk_src_sel = 0x03;                
            }else
            {
                core_clk_src_sel = 0x01;
            }
            if(peri_clk_src_default != CLK_SRC_XTAL)
            {
                if(peri_clk_freq_default == PERI_CLK_40M)
                {
                    peri_clk_div = 2;
                }else if(cpu_clk_freq_default == PERI_CLK_80M)
                {
                    peri_clk_div = 1;
                }
            }
        
        }break;
        
        case CLK_SRC_DPLL_96M:
        {     
            if(core_clk_src_default != CLK_SRC_XTAL)
            {
                if(cpu_clk_freq_default == MCU_CLK_32M)
                {
                    core_clk_div = 3;
                }else if(cpu_clk_freq_default == MCU_CLK_48M)
                {
                    core_clk_div = 2;
                }                              
                core_clk_src_sel = 0x03;                
            }else
            {
                core_clk_src_sel = 0x01;
            }
            
            if(peri_clk_src_default != CLK_SRC_XTAL)
            {
                if(peri_clk_freq_default == PERI_CLK_48M)
                {
                    peri_clk_div = 2;
                }              
            }
        
        }break;
        
        default:break;
    }
    
}

UINT32 icu_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret, reg;
    UINT8  dev;

    ret = DRIV_SUCCESS;

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
                	reg &= ~ICU_PWM0_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_PWM1:
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

#ifdef __SPI_DRIVER__
                case CLK_PWR_DEV_SPI:
                    reg &= ~ICU_SPI_CLK_PWD_MASK;
                    break;
#endif
#ifdef __HW_TIMER_DRIVER__
                case CLK_PWR_DEV_TIMER0:
                    reg &= ~ICU_TIMER0_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_TIMER1:
                    reg &= ~ICU_TIMER1_CLK_PWD_MASK;
                    break;
#endif

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
                	reg |= ICU_PWM0_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_PWM1:
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

#ifdef __SPI_DRIVER__
                case CLK_PWR_DEV_SPI:
                    reg |= ICU_SPI_CLK_PWD_MASK;
                    break;
#endif
#ifdef __HW_TIMER_DRIVER__
                case CLK_PWR_DEV_TIMER0:
                    reg |= ICU_TIMER0_CLK_PWD_MASK;
                    break;

                case CLK_PWR_DEV_TIMER1:
                    reg |= ICU_TIMER1_CLK_PWD_MASK;
                    break;
#endif

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
                    reg &= ~(0x03 << SYS_PWM0_SEL_POS);
                    reg |= (0x01 << SYS_PWM0_SEL_POS);
                    break;

                case CLK_PWR_DEV_PWM1:
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
                    reg &= ~(0x03 << SYS_PWM0_SEL_POS);
                    break;

                case CLK_PWR_DEV_PWM1:
                    reg &= ~(0x03 << SYS_PWM1_SEL_POS);
                    break;
            }

            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;
#ifdef __HW_TIMER_DRIVER__
        case CMD_TIMER_CLK_SEL:

            dev = ((TIMER_SRC_SEL_C *)param)->num;
            reg = REG_READ(REG_SYS_CLK_SEL);

            switch(dev)
            {
                case PLAT_TIMER0:
                    reg &= ~(0x01 << SYS_TIMER0_SEL_POS);
                    reg |= (((TIMER_SRC_SEL_C *)param)->src) << SYS_TIMER0_SEL_POS;
                    break;
                case PLAT_TIMER1:
                    reg &= ~(0x01 << SYS_TIMER1_SEL_POS);
                    reg |= (((TIMER_SRC_SEL_C *)param)->src) << SYS_TIMER1_SEL_POS;
                    break;
            }

            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;
#endif
#ifdef __SPI_DRIVER__
        case CMD_SPI_CLK_SEL:
            reg = REG_READ(REG_SYS_CLK_SEL);
            reg &= ~(0x01 << SYS_SPI_SEL_POS);
            reg |= (*(uint8_t *)param) << SYS_SPI_SEL_POS;
            REG_WRITE(REG_SYS_CLK_SEL, reg);
            break;
#endif

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
