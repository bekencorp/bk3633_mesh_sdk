

#ifndef _ADC_H_
#define _ADC_H_



#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions

/*
#define REG_APB7_ADC_CFG                (*((volatile unsigned long *)   0x00806700))
#define BIT_ADC_MODE                    0
#define BIT_ADC_EN                      2
#define BIT_ADC_CHNL                    3
#define BIT_ADC_WAIT_CLK_SETTING        7
#define BIT_ADC_INT_CLEAR               8
#define BIT_ADC_PRE_DIV                 9
#define BIT_ADC_DIV1_MODE				15
#define BIT_ADC_SAMPLE_RATE             16
#define BIT_ADC_FILTER             		22
#define BIT_ADC_BUSY             		29
#define BIT_ADC_FIFO_EMPTY              30
#define BIT_ADC_FIFO_FULL               31
#define SET_ADC_MODE                    (0x3 << BIT_ADC_MODE    )
*/

#define BASEADDR_SADC                                           0x00806700
#define SADC_REG0X0_CFG0                                          *((volatile unsigned long *) (BASEADDR_SADC+0x0*4))
#define POS_SADC_REG0X0_CFG0_MODE                                 0
#define POS_SADC_REG0X0_CFG0_EN                                   2
#define POS_SADC_REG0X0_CFG0_CHNL                                 3
#define POS_SADC_REG0X0_CFG0_SETING                               7
#define POS_SADC_REG0X0_CFG0_INT_CLEAR                            8
#define POS_SADC_REG0X0_CFG0_PRE_DIV                              9
#define POS_SADC_REG0X0_CFG0_32M_MODE                             15
#define POS_SADC_REG0X0_CFG0_SAMP_RATE                            16
#define POS_SADC_REG0X0_CFG0_ADC_FILTER                           22
#define POS_SADC_REG0X0_CFG0_ADC_BUSY                             29
#define POS_SADC_REG0X0_CFG0_FIFO_EMPTY                           30
#define POS_SADC_REG0X0_CFG0_FIFO_FULL                            31
#define SET_ADC_EN                      (0x1 << POS_SADC_REG0X0_CFG0_EN)

#define SADC_REG0X1_DAT                                        (*((volatile unsigned long *)   (BASEADDR_SADC+0x1*4)))

#define SADC_REG0X2_CFG1                                       (*((volatile unsigned long *)   (BASEADDR_SADC+0x2*4)))
#define POS_SADC_REG0X2_CHANN_EXPAND                           16
#define POS_SADC_REG0X2_STEADY_CTRL                             5
#define POS_SADC_REG0X2_ALMOST_CFG                              0

#define SADC_REG0X3_CFG2                                       (*((volatile unsigned long *)   (BASEADDR_SADC+0x3*4)))
#define POS_SADC_REG0X3_OVER_FLOW                               3
#define POS_SADC_REG0X3_SAT_ENABLE                              2
#define POS_SADC_REG0X3_STA_CTRL                                0

#define SADC_REG0X4_DAT                                        (*((volatile unsigned long *)   (BASEADDR_SADC+0x4*4)))



#define SYS_REG0X3_POWER                   *((volatile unsigned long *) (0x00800000+0x3*4))             
#define POS_SYS_REG0X3_SADC_PWD            10
#define SET_SADC_POWER_UP                  SYS_REG0X3_POWER &= ~(1<<POS_SYS_REG0X3_SADC_PWD)  

#define SYS_REG0X10_INT_EN                                       *((volatile unsigned long *) (0x00800000+0x10*4))
#define POS_SYS_REG0X10_INT_EN_ADC                               8



void adc_init();
void adc_isr(void);
uint16_t adc_get_value(uint8_t channel,uint8_t mode);
void adc_deinit(uint8_t channel);
void uart_check_low_volt_and_sleep(void);


#endif //



