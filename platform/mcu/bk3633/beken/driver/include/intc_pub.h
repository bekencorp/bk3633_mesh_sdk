#ifndef _INTC_PUB_H_
#define _INTC_PUB_H_

#include "generic.h"

#define FIQ_IRQ_END                      (18)

#define FIQ_BLE                          (20)
#define FIQ_BTDM                         (22)


#define IRQ_AON_RTC                      (12)
#define IRQ_I2S_PCM                      (11)
#define IRQ_RTC                          (10)
#define IRQ_GPIO                         (9)
#define IRQ_ADC                          (8)
#define IRQ_I2C                          (7)
#define IRQ_SPI                          (6)
#define IRQ_UART2                        (5)
#define IRQ_UART1                        (4)
#define IRQ_TIMER1                       (3)
#define IRQ_TIMER0                       (2)
#define IRQ_PWM1                         (1)
#define IRQ_PWM0                         (0)
#define PRI_FIQ_BLE                      (31)

#define PRI_IRQ_I2S_PCM                  (21)
#define PRI_IRQ_SPI                      (20)
#define PRI_IRQ_GPIO                     (19)
#define PRI_FIQ_TIMER1                   (18)
#define PRI_FIQ_TIMER0                   (17)
#define PRI_FIQ_PWM1                     (16)
#define PRI_FIQ_PWM0                     (15)
#define PRI_IRQ_UART2                    (14)
#define PRI_IRQ_UART1                    (13)


extern void intc_service_register(UINT8 int_num, UINT8 int_pri, FUNCPTR isr);
extern void intc_spurious(void);
extern void intc_enable(int index);
extern void intc_disable(int index);

#endif // _INTC_PUB_H_
// eof
