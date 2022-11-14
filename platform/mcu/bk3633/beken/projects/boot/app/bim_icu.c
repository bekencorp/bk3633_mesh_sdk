/**
****************************************************************************************
*
* @file icu.c
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
#include "bim_icu.h"      // timer definition
#include "bim_uart2.h"
void icu_init(void)
{
    set_SYS_Reg0x0_jtag_mode(0);    //close JTAG
#if 0
    addSYS_Reg0x0 &= ~(1 << posSYS_Reg0x0_jtag_mode);///close JTAG
    addSYS_Reg0x2 = 0;
    addSYS_Reg0x2 = (1 << posSYS_Reg0x2_core_sel);///16M CLK
#endif
#if 0
    set_SYS_Reg0x2_core_sel(0x01);
    set_SYS_Reg0x2_core_div(0x01);///16M CLK
    setf_SYS_Reg0x17_CLK96M_PWD;
    clrf_SYS_Reg0xd_PLL_PWR_sel;
    setf_SYS_Reg0xb_pwd_on_boostsel;
    addSYS_Reg0x17 = 0x82;
#endif
}
void mcu_clk_switch(uint8_t clk)
{
    switch(clk)
    {
        case MCU_CLK_16M:
        {   
            set_SYS_Reg0x2_core_div(0x1);
            set_SYS_Reg0x2_core_sel(0x01);
        }break;
        case MCU_CLK_80M:
        {   
            clrf_SYS_Reg0x17_CLK96M_PWD;
            set_SYS_Reg0x2_core_div(0x1);            
            set_SYS_Reg0x2_core_sel(0x03);
        }break;
        case MCU_CLK_64M:
        {
            clrf_SYS_Reg0x17_CLK96M_PWD;           
            set_SYS_Reg0x2_core_div(0x1);            
            set_SYS_Reg0x2_core_sel(0x03);
        }break; 
        default:break;
    }
}