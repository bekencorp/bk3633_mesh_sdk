#ifndef _ICU_H_
#define _ICU_H_

#include "BK3633_RegList.h"

#define ICU_DEBUG


#ifdef ICU_DEBUG
#define ICU_PRT      os_printf
#define ICU_WARN     warning_prf
#define ICU_FATAL    fatal_prf
#else
#define ICU_PRT      null_prf
#define ICU_WARN     null_prf
#define ICU_FATAL    null_prf
#endif



#define ICU_INT_FLAG                                 REG_SYS_INT_STATUS
#define ICU_IRQ_ENABLE                               REG_SYS_INT_PRI

#define ICU_UART1_CLK_PWD_POSI                        (posSYS_Reg0x3_uart0_pwd)
#define ICU_UART1_CLK_PWD_MASK                        (0x01UL << ICU_UART1_CLK_PWD_POSI)

#define ICU_UART2_CLK_PWD_POSI                        (posSYS_Reg0x3_UART2_pwd)
#define ICU_UART2_CLK_PWD_MASK                        (0x01UL << ICU_UART2_CLK_PWD_POSI)

#define ICU_WDT_CLK_PWD_POSI                         (posSYS_Reg0x3_wdt_pwd)
#define ICU_WDT_CLK_PWD_MASK                         (0x01UL << ICU_WDT_CLK_PWD_POSI)

#define ICU_I2S_CLK_PWD_POSI                         (posSYS_Reg0x3_i2s_pwd)
#define ICU_I2S_CLK_PWD_MASK                         (0x01UL << ICU_I2S_CLK_PWD_POSI)

#define ICU_CLK_GATE_DISABLE                         &(addSYS_Reg0x1e)




/*******************************************************************************
* Function Declarations
*******************************************************************************/

#endif //_ICU_H_ 

