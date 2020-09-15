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


#define ICU_CLK_SRC_SEL_CLK_SRC_SEL_POSI             (0)
#define ICU_CLK_SRC_SEL_CLK_SRC_SEL_MASK             (0x03UL << ICU_CLK_SRC_SEL_CLK_SRC_SEL_POSI)

#define ICU_CLK_SRC_SEL_SLEEP_SEL_POSI               (2)
#define ICU_CLK_SRC_SEL_SLEEP_SEL_MASK               (0x01UL << ICU_CLK_SRC_SEL_SLEEP_SEL_POSI)
#define SET_SLEEP_DOWN_POWER                         (1)
#define ICU_CLK_SRC_SEL_JTAG_MODE_POSI               (9)
#define ICU_CLK_SRC_SEL_JTAG_MODE_MASK               (0x01UL << ICU_CLK_SRC_SEL_JTAG_MODE_POSI)
#define CLOSE_JTAG_MODE()                            clrf_SYS_Reg0x0_jtag_mode
#define OPEN_JTAG_MODE()                             setf_SYS_Reg0x0_jtag_mode
#define ICU_CLK_SRC_SEL_PLL_SEL_POSI                 (10)
#define ICU_CLK_SRC_SEL_PLL_SEL_MASK                 (0x01UL << ICU_CLK_SRC_SEL_PLL_SEL_POSI)


#define ICU_CORE_CLK_PWD_POSI                        (0)
#define ICU_CORE_CLK_DIV_POSI                        (1)
#define ICU_CORE_CLK_PWD_MASK                        (0x01UL << ICU_CORE_CLK_PWD_POSI)
#define ICU_CORE_CLK_DIV_MASK                        (0x7FUL << ICU_CORE_CLK_DIV_POSI)

#define ICU_ADC_CLK_CON                              &(REG_AHB0_ICU_ADCCLKCON)
#define ICU_ADC_CLK_PWD_POSI                         (0)
#define ICU_ADC_CLK_DIV_POSI                         (1)
#define ICU_ADC_CLK_PWD_MASK                         (0x01UL << ICU_ADC_CLK_ADC_PWD_POSI)
#define ICU_ADC_CLK_DIV_MASK                         (0x7FUL << ICU_ADC_CLK_DIV_POSI)

#define ICU_UART_CLK_PWD_POSI                        (posSYS_Reg0x3_uart0_pwd)
#define ICU_UART_CLK_DIV_POSI                        (1)
#define ICU_UART_CLK_PWD_MASK                        (0x01UL << ICU_UART_CLK_PWD_POSI)

#define ICU_I2C_CLK_CON                              &(REG_AHB0_ICU_I2CCLKCON)
#define ICU_I2C_CLK_PWD_POSI                         (0)
#define ICU_I2C_CLK_DIV_POSI                         (1)
#define ICU_I2C_CLK_PWD_MASK                         (0x01UL << ICU_I2C_CLK_PWD_POSI)
#define ICU_I2C_CLK_DIV_MASK                         (0x7FUL << ICU_I2C_CLK_DIV_POSI)

#define ICU_SPI_CLK_CON                              &(REG_AHB0_ICU_SPICLKCON)
#define ICU_SPI_CLK_PWD_POSI                         (0)
#define ICU_SPI_CLK_DIV_POSI                         (1)
#define ICU_SPI_CLK_PWD_MASK                         (0x01UL << ICU_SPI_CLK_PWD_POSI)
#define ICU_SPI_CLK_DIV_MASK                         (0x7FUL << ICU_SPI_CLK_DIV_POSI)

#define ICU_BLE_CLK_CON                              &(REG_AHB0_ICU_BLECLKCON)
#define ICU_BLE_CLK_PWD_POSI                         (0)
#define ICU_BLE_CLK_PWD_MASK                         (0x01UL << ICU_BLE_CLK_PWD_POSI)

#define ICU_WDT_CLK_PWD_POSI                         (posSYS_Reg0x3_wdt_pwd)
#define ICU_WDT_CLK_PWD_MASK                         (0x01UL << ICU_WDT_CLK_PWD_POSI)

#define ICU_I2S_CLK_PWD_POSI                         (posSYS_Reg0x3_i2s_pwd)
#define ICU_I2S_CLK_PWD_MASK                         (0x01UL << ICU_I2S_CLK_PWD_POSI)


#define ICU_ANA_CTL                                  &(REG_AHB0_ICU_ANA_CTL)

#define ICU_CPU_STATUS                               &(REG_AHB0_ICU_CPU_STATUS)

#define ICU_INT_ENABLE                               &(REG_AHB0_ICU_INT_ENABLE)
#define ICU_PWM0_INT_EN_POSI                         (0)
#define ICU_PWM1_INT_EN_POSI                         (1)
#define ICU_PWM2_INT_EN_POSI                         (2)
#define ICU_PWM3_INT_EN_POSI                         (3)
#define ICU_PWM4_INT_EN_POSI                         (4)
#define ICU_UART_INT_EN_POSI                         (5)
#define ICU_SPI_INT_EN_POSI                          (6)
#define ICU_I2C_INT_EN_POSI                          (7)
#define ICU_ADC_INT_EN_POSI                          (8)
#define ICU_GPIO_INT_EN_POSI                         (9)
#define ICU_USB_INT_EN_POSI                          (11)
#define ICU_RTC_INT_EN_POSI                          (12)
#define ICU_PWM5_INT_EN_POSI                         (13)
#define ICU_PWM_3DS_INT_EN_POSI                      (14)
#define ICU_BLE_INT_EN_POSI                          (15)
#define ICU_UART2_INT_EN_POSI                        (16)
#define ICU_SDM_INT_EN_POSI                          (17)
#define ICU_I2S_INT_EN_POSI                          (18)


#define ICU_IRQ_EN_POSI                              (0)
#define ICU_FIQ_EN_POSI                              (1)



#define ICU_FLASH                                    &(REG_AHB0_ICU_FLASH)

#define ICU_ANALOG_MODE                              &(REG_AHB0_ICU_ANALOG_MODE)

#define ICU_ANALOG0_PWD                              &(REG_AHB0_ICU_ANALOG0_PWD)

#define ICU_ANALOG1_PWD                              &(REG_AHB0_ICU_ANALOG1_PWD)

#define ICU_DIGITAL_PWD                              &(REG_AHB0_ICU_DIGITAL_PWD)


#define ICU_GPIO_DEEP_WAKEN                          &(REG_AHB0_ICU_DEEP_SLEEP0)

#define ICU_GPIO_SLEEP                               &(REG_AHB0_ICU_DEEP_SLEEP1)

#define ICU_CLK_GATE_DISABLE                         &(addSYS_Reg0x1e)




/*******************************************************************************
* Function Declarations
*******************************************************************************/

#endif //_ICU_H_ 

