#include "include.h"
#include "arm_arch.h"

#include "dd_pub.h"

#include "uart_pub.h"
#include "gpio_pub.h"
#include "icu_pub.h"
#include "wdt_pub.h"
#include "pwm_pub.h"
#include "flash_pub.h"
#include "spi_pub.h"
#include "i2s_pub.h"
#include "aon_wdt_pub.h"

#if CFG_USE_STA_PS
#include "power_save_pub.h"
#endif


static DD_INIT_S dd_init_tbl[] =
{
    /* name*/              /* init function*/          /* exit function*/

    {ICU_DEV_NAME,          icu_init,                   icu_exit},

    {WDT_DEV_NAME,          wdt_init,                   wdt_exit},

    {GPIO_DEV_NAME,         gpio_init,                  gpio_exit},
    
    {UART1_DEV_NAME,        uart1_init,                 uart1_exit},

    {UART2_DEV_NAME,        uart2_init,                 uart2_exit},

    {FLASH_DEV_NAME,        flash_init,                 flash_exit},
    
    {PWM_DEV_NAME,          pwm_init,                   pwm_exit},

    {AON_WDT_DEV_NAME,      aon_wdt_init,               aon_wdt_exit},
    // {SPI_DEV_NAME,          spi_init,                   spi_exit},

    //{I2S_DEV_NAME,          i2s_init,                   i2s_exit},

#if CFG_USE_STA_PS
    //{"power_save",       sctrl_sta_ps_init,                NULLPTR},
#endif

    {NULL,                  NULLPTR,                    NULLPTR}
};

void g_dd_init(void)
{
    UINT32 i;
    UINT32 tbl_count;
    DD_INIT_S *dd_element;

    tbl_count = sizeof(dd_init_tbl) / sizeof(DD_INIT_S);
    for(i = 0; i < tbl_count; i ++)
    {
        dd_element = &dd_init_tbl[i];
        if(dd_element->dev_name && dd_element->init)
        {
            (dd_element->init)();
        }
        else
        {
            return;
        }
    }
}

void g_dd_exit(void)
{
    UINT32 i;
    UINT32 tbl_count;
    DD_INIT_S *dd_element;

    tbl_count = sizeof(dd_init_tbl) / sizeof(DD_INIT_S);
    for(i = 0; i < tbl_count; i ++)
    {
        dd_element = &dd_init_tbl[i];
        if(dd_element->dev_name && dd_element->exit)
        {
            (dd_element->exit)();
        }
        else
        {
            return;
        }
    }
}

// EOF
