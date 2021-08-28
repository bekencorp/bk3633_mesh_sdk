/**
****************************************************************************************
*
* @file hal_aon_wdt.c
*
* @brief aon_wdt function user interface
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "hal_aon_wdt.h"

DRIVER_CTRL_RES hal_aon_wdt_start(uint32_t wdt_period)
{
#if (CONFIG_DUT_TEST_CMD)
	if(!get_dut_flag())
#endif
	{
	    bk_aon_wdt_period(wdt_period);
	    bk_aon_wdt_feed();
	}
    return DRIV_SUCCESS;
}

DRIVER_CTRL_RES hal_aon_wdt_stop(void)
{

	uint32_t para = 0;
#if (CONFIG_DUT_TEST_CMD)
	if(!get_dut_flag())
#endif
	{
	
		bk_aon_wdt_period(para);
    	bk_aon_wdt_feed();
    }
    return DRIV_SUCCESS;
}

DRIVER_CTRL_RES hal_aon_wdt_normal_mode(void)
{
#if (CONFIG_DUT_TEST_CMD)
	if(!get_dut_flag())
#endif
	{
    	bk_aon_wdt_mode(AON_WDT_NORMAL);
	}
    return DRIV_SUCCESS;
}

DRIVER_CTRL_RES hal_aon_wdt_deep_sleep(void)
{
#if (CONFIG_DUT_TEST_CMD)
	if(!get_dut_flag())
#endif
	{
		
    	bk_aon_wdt_mode(AON_WDT_DEEP_SLEEP);
    }
    return DRIV_SUCCESS;
}

DRIVER_CTRL_RES hal_aon_wdt_idle_sleep(void)
{
#if (CONFIG_DUT_TEST_CMD)
	if(!get_dut_flag())
#endif
	{
		
    	bk_aon_wdt_mode(AON_WDT_IDLE_SLEEP);
	}
    return DRIV_SUCCESS;
}

DRIVER_CTRL_RES hal_aon_wdt_feed(void)
{
#if (CONFIG_DUT_TEST_CMD)
	if(!get_dut_flag())
#endif
	{	
    	bk_aon_wdt_feed();
	}
    return DRIV_SUCCESS;
}