#include <stdint.h>        // standard integer definition
#include <string.h>        // string manipulation
#include <stddef.h>        // standard definition
#include "bim_wdt.h"
#include "BK3633_RegList.h"
void wdt_disable(void)
{
    WDT_REG0X2=0x00;//close WDT
    WDT_REG0X1=WDKEY_ENABLE1;
    WDT_REG0X1=WDKEY_ENABLE2;

    set_WDT_Reg0x0_WD_PERIOD(0) ;
    set_WDT_Reg0x0_WDKEY(0x5a);
    set_WDT_Reg0x0_WDKEY(0xa5);
}

void wdt_enable(uint32_t wdt_cnt)
{
    WDT_REG0X1=0x00;
    WDT_REG0X2  = wdt_cnt;
    WDT_REG0X1=WDKEY_ENABLE1;
    WDT_REG0X1=WDKEY_ENABLE2;
}
void wdt_feed(void)
{
    WDT_REG0X1=WDKEY_ENABLE1;
    WDT_REG0X1=WDKEY_ENABLE2;
}
