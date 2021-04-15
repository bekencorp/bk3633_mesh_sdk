#include "func_pub.h"
//#include "intc.h"
#include "uart_pub.h"
#include "RomCallFlash.h"
#include "app.h"
#include "gpio.h"
#include "rf.h"
#include "k_mm.h"

#if CFG_UART_DEBUG
#include "uart_debug_pub.h"
#endif

#if CFG_SDIO
#include "sdio_intf_pub.h"
#endif

#if CFG_USB
#include "fusb_pub.h"
#endif

#if 1
#if (BLE_ADV_TEST_MODE)
#include "lld_adv_test.h"
#endif

void rom_env_init(struct rom_env_tag *api)
{
    os_printf("%s L %d\r\n", __func__, __LINE__);
	memset(&rom_env,0,sizeof(struct rom_env_tag));

	//rom_env.rwip_reset = rwip_reset;

#if SYSTEM_SLEEP
	rom_env.rwip_prevent_sleep_set = rwip_prevent_sleep_set;
    rom_env.rwip_prevent_sleep_clear = rwip_prevent_sleep_clear;
	rom_env.rwip_sleep_lpcycles_2_us = rwip_sleep_lpcycles_2_us;
	rom_env.rwip_us_2_lpcycles = rwip_us_2_lpcycles;
	rom_env.rwip_wakeup_delay_set = rwip_wakeup_delay_set;
#endif

	rom_env.appm_init = appm_init;

	rom_env.platform_reset = platform_reset;

	rom_env.assert_err = assert_err;

	rom_env.assert_param = assert_param;

	rom_env.hci_event_process = hci_event_process;

    rom_env.gpio_config = gpio_config;
    rom_env.gpio_triger = gpio_triger;
    rom_env.rf_init = rf_init;
    rom_env.malloc = krhino_mm_alloc;
    rom_env.free = krhino_mm_free;
    rom_env.os_print = os_printf;
#if 1
	rom_env.krhino_sem_create = krhino_sem_create;

	rom_env.krhino_sem_give = krhino_sem_give;

#endif
}
#endif

UINT32 func_init(void)
{
    //char temp_mac[6];

    //cfg_param_init();
    // load mac, init mac first
    //wifi_get_mac_address(temp_mac, CONFIG_ROLE_NULL);
	
    //FUNC_PRT("[FUNC]rwnxl_init\r\n");
    //rwnxl_init();
    os_printf("%s \r\n", __func__);
	rom_env_init(&rom_env);
    os_printf("%s \r\n", __func__);
#if 0//CFG_UART_DEBUG 
	#ifndef KEIL_SIMULATOR
    FUNC_PRT("[FUNC]uart_debug_init\r\n");   
    uart_debug_init();
	#endif
#endif


#if 0//CFG_SUPPORT_CALIBRATION
    FUNC_PRT("[FUNC]calibration_main\r\n");
    calibration_main();
    #if CFG_SUPPORT_MANUAL_CALI
    manual_cal_load_default_txpwr_tab(manual_cal_load_txpwr_tab_flash());
    #endif
    #if CFG_SARADC_CALIBRATE
    manual_cal_load_adc_cali_flash();
    #endif
    #if CFG_USE_TEMPERATURE_DETECT
    manual_cal_load_temp_tag_flash();
    #endif
	
    #if (CFG_SOC_NAME != SOC_BK7231)
    //bk7011_band_detect();
    manual_cal_load_lpf_iq_tag_flash();
    manual_cal_load_xtal_tag_flash();
    #endif // (CFG_SOC_NAME != SOC_BK7231)
#endif    

#if 0//CFG_SDIO
    FUNC_PRT("[FUNC]sdio_intf_init\r\n");
    sdio_intf_init();
#endif

#if 0//CFG_SDIO_TRANS
    FUNC_PRT("[FUNC]sdio_intf_trans_init\r\n");
    sdio_trans_init();
#endif


#if 0//CFG_USB
    FUNC_PRT("[FUNC]fusb_init\r\n");
    fusb_init();
#endif

#if 0//CFG_ROLE_LAUNCH
    rl_init();
#endif

	#if 0//CFG_ENABLE_BUTTON
	key_initialization();
	#endif

    FUNC_PRT("[FUNC]func_init OVER!!!\r\n\r\n");
    return 0;
}

// eof
