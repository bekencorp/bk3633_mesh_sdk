#ifndef __WDT_H__
#define __WDT_H__
#define BASEADDR_AON_WDT                                        0x00800c00
#define WDT_REG0X0                                              *((volatile unsigned long *) (0x00800c00+0x0*4))
#define WDT_REG0X1                                              *((volatile unsigned long *) (0x00800c00+0x1*4))
#define WDT_REG0X2                                              *((volatile unsigned long *) (0x00800c00+0x2*4))
#define WDKEY_ENABLE1           0x5A
#define WDKEY_ENABLE2           0xA5
void wdt_feed(void);
void wdt_disable(void);
void wdt_enable(uint32_t wdt_cnt);
#endif      /* __DRIVER_WDT_H__ */
