/**
****************************************************************************************
*
* @file rwip_driver.c
*
* @brief RW IP Driver SW module used to manage common IP features.
*
* Copyright (C) RivieraWaves 2009-2015
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup RW IP SW main module
 * @ingroup ROOT
 * @brief The RW IP SW main module.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // RW SW configuration

#include <string.h>          // for mem* functions
#include "rwip.h"            // RW definitions
#include "rwip_int.h"        // RW internal definitions
#include "arch.h"            // Platform architecture definition

#if (NVDS_SUPPORT)
#include "nvds.h"            // NVDS definitions
#endif // NVDS_SUPPORT

#if (H4TL_SUPPORT)
#include "h4tl.h"            // H4TL definition
#endif //H4TL_SUPPORT

#include "dbg.h"             // debug definition
#include "ke_mem.h"          // for AES client management

#include "ke.h"              // To check if a kernel event is programmed
#include "ke_event.h"

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
#include "sch_alarm.h"       // for the half slot target ISR
#include "sch_arb.h"         // for the half us target ISR
#include "sch_prog.h"        // for the fifo/clock ISRs
//#include "led.h"
#include "reg_ipcore.h"
#include "aes.h"             // AES result function

#if (BLE_EMB_PRESENT)
#include "rwble.h"           // for sleep and wake-up specific functions
#include "lld.h"             // for AES encryption handler
#endif // (BLE_EMB_PRESENT)
#if (BT_EMB_PRESENT)
#include "rwbt.h"            // for sleep and wake-up specific functions
#include "ld.h"              // for clock interrupt handler
#endif // (BT_EMB_PRESENT)
#elif  (BLE_HOST_PRESENT)
#include "timer.h"
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

#include "co_math.h"         // min/max macros


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Parameters API
struct rwip_param_api      rwip_param;

struct rwip_env_tag        rwip_env;
#if 1//(BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// Local supported commands
struct rwip_prio rwip_priority[RWIP_PRIO_IDX_MAX]={
    #if (BT_EMB_PRESENT)
    {RWIP_PRIO_ACL_DFT,        RWIP_INCR_ACL_DFT},
    {RWIP_PRIO_ACL_ACT,        RWIP_INCR_ACL_ACT},
    {RWIP_PRIO_ACL_RSW,        RWIP_INCR_ACL_RSW},
    {RWIP_PRIO_ACL_SNIFF_DFT,  RWIP_INCR_ACL_SNIFF_DFT},
    {RWIP_PRIO_ACL_SNIFF_TRANS,RWIP_INCR_ACL_SNIFF_TRANS},
    #if MAX_NB_SYNC
    {RWIP_PRIO_SCO_DFT,       RWIP_INCR_SCO_DFT},
    #endif //MAX_NB_SYNC
    {RWIP_PRIO_BCST_DFT,      RWIP_INCR_BCST_DFT},
    {RWIP_PRIO_BCST_ACT,      RWIP_INCR_BCST_ACT},
    {RWIP_PRIO_CSB_RX_DFT,    RWIP_INCR_CSB_RX_DFT},
    {RWIP_PRIO_CSB_TX_DFT,    RWIP_INCR_CSB_TX_DFT},
    {RWIP_PRIO_INQ_DFT,       RWIP_INCR_INQ_DFT},
    {RWIP_PRIO_ISCAN_DFT,     RWIP_INCR_ISCAN_DFT},
    {RWIP_PRIO_PAGE_DFT,      RWIP_INCR_PAGE_DFT},
    {RWIP_PRIO_PAGE_1ST_PKT,  RWIP_INCR_PAGE_1ST_PKT},
    {RWIP_PRIO_PCA_DFT,       RWIP_INCR_PCA_DFT},
    {RWIP_PRIO_PSCAN_DFT,     RWIP_INCR_PSCAN_DFT},
    {RWIP_PRIO_PSCAN_1ST_PKT, RWIP_INCR_PSCAN_1ST_PKT},
    {RWIP_PRIO_SSCAN_DFT,     RWIP_INCR_SSCAN_DFT},
    {RWIP_PRIO_STRAIN_DFT,    RWIP_INCR_STRAIN_DFT},
    #endif // #if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    {RWIP_PRIO_SCAN_DFT,      RWIP_INCR_SCAN_DFT},
    {RWIP_PRIO_AUX_RX_DFT,    RWIP_INCR_AUX_RX_DFT},
    {RWIP_PRIO_INIT_DFT,      RWIP_INCR_INIT_DFT},
    {RWIP_PRIO_CONNECT_DFT,   RWIP_INCR_CONNECT_DFT},
    {RWIP_PRIO_CONNECT_ACT,   RWIP_INCR_CONNECT_ACT},
    {RWIP_PRIO_ADV_DFT,       RWIP_INCR_ADV_DFT},
    {RWIP_PRIO_ADV_HDC_DFT,   RWIP_INCR_ADV_HDC_PRIO_DFT},
    {RWIP_PRIO_ADV_AUX_DFT,   RWIP_INCR_ADV_AUX_DFT},
    {RWIP_PRIO_PER_ADV_DFT,   RWIP_INCR_PER_ADV_DFT},
    {RWIP_PRIO_RPA_RENEW_DFT, RWIP_INCR_RPA_RENEW_DFT},
    #endif // #if (BLE_EMB_PRESENT)
};
#endif//#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/*
 * LOCAL FUNCTIONS
 ****************************************************************************************
 */

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Converts a duration in lp cycles into a duration in half us.
 *
 * The function converts a duration in lp cycles into a duration is half us, according to the
 * low power clock frequency (32768Hz or 32000Hz).
 *
 * To do this the following formula are applied:
 *
 *   Tus = (x*30.517578125)*2 = (30*x + x/2 + x/64 + x/512)*2 = (61*x + (x*8 + x)/256) for a 32.768kHz clock or
 *   Tus = (x*31.25)*2        = (31*x + x/4) * 2              = (62*x + x/2)           for a 32kHz clock
 *
 * @param[in]     lpcycles    duration in lp cycles
 * @param[in|out] error_corr  Insert and retrieve error created by truncating the LP Cycle Time to a half us (32kHz: 1/2 half us | 32.768kHz: 1/256 half-us)
 *
 * @return duration in half us
 ****************************************************************************************
 */
__STATIC uint32_t rwip_lpcycles_2_hus(uint32_t lpcycles, uint32_t *error_corr)
{
    uint32_t res;

    // Sanity check: The number of lp cycles should not be too high to avoid overflow
    ASSERT_ERR(lpcycles < 2000000);

    #if (HZ32000)
    // Compute the sleep duration in us - case of a 32kHz clock and insert previous computed error
    *error_corr = lpcycles + *error_corr;
    // get the truncated value
    res = *error_corr >> 1;
    // retrieve new inserted error
    *error_corr = *error_corr - (res << 1);
    // finish computation
    res = 62 * lpcycles + res;
    #else //HZ32000
    // Compute the sleep duration in half us - case of a 32.768kHz clock and insert previous computed error
    *error_corr = (lpcycles << 3) + lpcycles + *error_corr;
    // get the truncated value
    res = *error_corr >> 8;
    // retrieve new inserted error
    *error_corr = *error_corr - (res << 8);
    // finish computation
    res = 61 * lpcycles + res;
    #endif //HZ32000

    return(res);
}

/**
 ****************************************************************************************
 * @brief Converts a duration in half slots into a number of low power clock cycles.
 * The function converts a duration in half slots into a number of low power clock cycles.
 * Sleep clock runs at either 32768Hz or 32000Hz, so this function divides the value in
 * slots by 10.24 or 10 depending on the case.
 * To do this the following formulae are applied:
 *
 *   N = x * 10.24 = (1024 * x)/100 for a 32.768kHz clock or
 *   N = x * 10                     for a 32kHz clock
 *
 * @param[in] hs_cnt    The value in half slot count
 *
 * @return The number of low power clock cycles corresponding to the slot count
 *
 ****************************************************************************************
 */
__ATTR_ARM int32_t rwip_slot_2_lpcycles(int32_t hs_cnt)
{
    int32_t lpcycles;

    #if HZ32000
    // Sanity check: The number of slots should not be too high to avoid overflow
    ASSERT_ERR(hs_cnt < (0xFFFFFFFF / 10));

    // Compute the low power clock cycles - case of a 32kHz clock
    lpcycles = (hs_cnt * 10) ;
    #else //HZ32000
    // Sanity check: The number of slots should not be too high to avoid overflow
    ASSERT_ERR(hs_cnt < (0xFFFFFFFF >> 10));

    // Compute the low power clock cycles - case of a 32.768kHz clock
    lpcycles = (hs_cnt << 10)/100;
    #endif //HZ32000

    // So reduce little bit sleep duration in order to allow fine time compensation
    // Note compensation will be in range of [1 , 2[ lp cycles if there is no external wake-up
    lpcycles--;

    return(lpcycles);
}



/**
 ****************************************************************************************
 * @brief Converts a duration in us into a duration in lp cycles.
 *
 * The function converts a duration in us into a duration is lp cycles, according to the
 * low power clock frequency (32768Hz or 32000Hz).
 *
 * @param[in] us    duration in us
 *
 * @return duration in lpcycles
 ****************************************************************************************
 */
__STATIC uint32_t rwip_us_2_lpcycles(uint32_t us)
{
    uint32_t lpcycles;

    #if (HZ32000)
    // Compute the low power clock cycles - case of a 32kHz clock
    lpcycles = ((us * 32) + (999)) / 1000;
    #else //HZ32000
    // Compute the low power clock cycles - case of a 32.768kHz clock
    lpcycles = ((us * 32768) + (999999)) / 1000000;
    #endif //HZ32000

    return(lpcycles);
}

/**
 ****************************************************************************************
 * @brief Handles the Half slot timer target
 ****************************************************************************************
 */
void rwip_timer_hs_handler(void)
{
    // disable the timer driver
    rwip_env.timer_hs_target = RWIP_INVALID_TARGET_TIME;
    ip_intcntl1_finetgtintmsk_setf(0);

    // call the default half slot call-back
    sch_alarm_timer_isr();
}

/**
 ****************************************************************************************
 * @brief Handles the Half slot timer target
 ****************************************************************************************
 */
void rwip_timer_hus_handler(void)
{
    // disable the timer driver
    rwip_env.timer_hus_target = RWIP_INVALID_TARGET_TIME;
    ip_intcntl1_timestamptgt1intmsk_setf(0);

    // call the default half slot call-back
    sch_arb_event_start_isr();
}

#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Handles the 10 ms timer target
 ****************************************************************************************
 */
void rwip_timer_10ms_handler(void)
{
    // disable the timer driver
    rwip_env.timer_10ms_target = RWIP_INVALID_TARGET_TIME;
    //bim_printf("%s, %d", __func__, __LINE__);
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    ip_intcntl1_timestamptgt2intmsk_setf(0);
    #elif (BLE_HOST_PRESENT)
    // Stop timer
    timer_set_timeout(0, NULL);
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    // Mark that 10ms timer is over
    ke_event_set(KE_EVENT_KE_TIMER);
}
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Handles crypto event (to provide results out of interrupt context
 ****************************************************************************************
 */
__STATIC void rwip_crypt_evt_handler(void)
{
    uint8_t aes_result[KEY_LEN];

    // Clear event
    ke_event_clear(KE_EVENT_AES_END);

    // Load AES result
    em_rd(aes_result, EM_ENC_OUT_OFFSET, KEY_LEN);
#if (BT_DUAL_MODE || BLE_STD_MODE)
    // inform AES result handler
    // aes_result_handler(CO_ERROR_NO_ERROR, aes_result);
#endif //(BT_DUAL_MODE || BLE_STD_MODE)
}

/**
 ****************************************************************************************
 * @brief Handles crypto interrupt
 ****************************************************************************************
 */
void rwip_crypt_isr_handler(void)
{
    // Prevent going to deep sleep during encryption
    rwip_prevent_sleep_clear(RW_CRYPT_ONGOING);

    // Clear interrupt mask
    ip_intcntl1_cryptintmsk_setf(0);

    // mark that AES is done
    ke_event_set(KE_EVENT_AES_END);
}

/**
 ****************************************************************************************
 * @brief Handles Software requested interrupt
 ****************************************************************************************
 */
void rwip_sw_int_handler(void)
{
    // Disable interrupt
    ip_intcntl1_swintmsk_setf(0);

    // call the SW interrupt handler
    sch_arb_sw_isr();
}

/**
 ****************************************************************************************
 * @brief Wake-up from Core sleep.
 *
 * Compute and apply the clock correction according to duration of the deep sleep.
 ****************************************************************************************
 */
__ATTR_ARM void rwip_wakeup(void)
{
    uint16_t fintetime_correction;
    // duration in half us
    uint32_t dur_hus;
    // duration in half slot
    uint32_t dur_hslot;
    // Get the number of low power sleep period
    uint32_t slp_period = ip_deepslstat_get();

    DBG_SWDIAG(SLEEP, SLEEP, 0);

    // Prevent going to deep sleep until a slot interrupt is received
    rwip_prevent_sleep_set(RW_WAKE_UP_ONGOING);

    // Re-enable external wake-up by default
    ip_deepslcntl_extwkupdsb_setf(0);

    // Compensate the base time counter and fine time counter by the number of slept periods
    dur_hus = rwip_lpcycles_2_hus(slp_period, &(rwip_env.sleep_acc_error));
    // Compute the sleep duration (based on number of low power clock cycles)
    dur_hslot = dur_hus / HALF_SLOT_SIZE;

    // retrieve halfslot sleep duration
    fintetime_correction = (HALF_SLOT_SIZE-1) - (dur_hus - dur_hslot*HALF_SLOT_SIZE);

    // The correction values are then deduced from the sleep duration in us
    ip_clkncntcorr_pack(/*absdelta*/ 1, /*clkncntcorr*/ dur_hus / HALF_SLOT_SIZE);

    // The correction values are then deduced from the sleep duration in us
    ip_finecntcorr_setf(fintetime_correction);

    // Start the correction
    ip_deepslcntl_deep_sleep_corr_en_setf(1);

    // Enable the RWBT slot interrupt
    ip_intcntl1_clknintsrmsk_setf(0);
    ip_intcntl1_set(IP_CLKNINTMSK_BIT);
    ip_intack1_clear(0xFFFFFFFF);

    #if (H4TL_SUPPORT)
    // Restart the flow on the TL
    h4tl_start();
    #endif //H4TL_SUPPORT

    TRC_REQ_WAKEUP();
}



/**
 ****************************************************************************************
 * @brief Restore the core processing after the clock correction
 *
 * Enable the core and check if some timer target has been reached.
 ****************************************************************************************
 */
void rwip_wakeup_end(void)
{
    // get current time
    rwip_time_t current_time = rwip_time_get();

    // Clear slot interrupt, not needed anymore
    ip_intcntl1_clknintmsk_setf(0);

    #if (BT_EMB_PRESENT)
    // Wake-up BT core
    rwbt_sleep_wakeup_end();
    #endif // (BT_EMB_PRESENT)

    #if (BLE_EMB_PRESENT)
    // Wake-up BLE core
    
    //#if	(ROM_REGISTER_CALLBACK)
/*     if(rom_env.rwble_sleep_wakeup_end != NULL)
    {
        rom_env.rwble_sleep_wakeup_end();
    }
    #else */
    rwble_sleep_wakeup_end();
    //#endif //(ROM_REGISTER_CALLBACK)
    #endif // (BLE_EMB_PRESENT)

    // Re-enable default common interrupts
    //ip_intcntl1_set(IP_FIFOINTMSK_BIT | IP_CRYPTINTMSK_BIT | IP_ERRORINTMSK_BIT | IP_SWINTMSK_BIT);

    if(rwip_env.timer_hs_target != RWIP_INVALID_TARGET_TIME)
    {
        // check if half slot timer target is reach
        if(1)//((current_time.hs == rwip_env.timer_hs_target))
        {
            rwip_timer_hs_handler();
        }
        // enable half slot timer interrupt
        else
        {
            ip_intcntl1_finetgtintmsk_setf(1);
        }
    }

    if(rwip_env.timer_hus_target != RWIP_INVALID_TARGET_TIME)
    {
        // check if half us timer target is reach
        if(1)//((current_time.hs == rwip_env.timer_hus_target))
        {
            rwip_timer_hus_handler();
        }
        // enable half us timer interrupt
        else
        {
            ip_intcntl1_timestamptgt1intmsk_setf(1);
        }
    }

    if(rwip_env.timer_10ms_target != RWIP_INVALID_TARGET_TIME)
    {
        // check if 10ms target is reach
        if((current_time.hs == rwip_env.timer_10ms_target))
        {
            rwip_timer_10ms_handler();
        }
        // enable 10ms timer interrupt
        else
        {
            ip_intcntl1_timestamptgt2intmsk_setf(1);
        }
    }

    // Wake up is complete now, so we allow the deep sleep again
    rwip_prevent_sleep_clear(RW_WAKE_UP_ONGOING);
    // If the Controller has event to be sent, give the controller a semphore to run the task.
    ke_event_run();
}

/*
 * GLOBAL FUNCTIONS
 ****************************************************************************************
 */

#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
inline rwip_time_t rwip_time_get(void)
{
    rwip_time_t res;

    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    //Sample the base time count
    ip_slotclk_samp_setf(1);
    
    while (ip_slotclk_samp_getf());		////20190905


    // get base time and offset
    res.hs  = ip_slotclk_sclk_getf();
    res.hus = HALF_SLOT_INV(ip_finetimecnt_get());
    #elif (BLE_HOST_PRESENT)
    // get base time (10 ms unit)
    res.hs  = timer_get_time() << 5;
    res.hus = 0;
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    return res;
}

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
#if (BT_EMB_PRESENT)
void rwip_time_set(uint32_t clock)
{
    ip_slotclk_pack(IP_SAMP_RST, 1 /* clk_upd */, clock & 0x0FFFFFFF);
    while(ip_slotclk_clkn_upd_getf());
}
#endif // (BT_EMB_PRESENT)
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

volatile  uint8_t g_dut_flg = 0;
void rwip_set_dut_mode(uint8_t flg)
{
	g_dut_flg = flg;
}

uint8_t  rwip_get_dut_mode(void)
{
	return g_dut_flg;
}



void rwip_driver_init(bool reset)
{
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    uint8_t length;
    uint8_t sleep_enable = 0;
    uint8_t ext_wakeup_enable;
    #if (BT_DUAL_MODE)
    uint8_t diag_cfg[PARAM_LEN_DIAG_DM_HW];
    #endif // (BT_DUAL_MODE)

    if(!reset)
    {
        // Register AES event
        ke_event_callback_set(KE_EVENT_AES_END, &rwip_crypt_evt_handler);
    }

    // initialize environment
    rwip_env.prevent_sleep     = 0;
    // clear target timer
    rwip_env.timer_10ms_target = RWIP_INVALID_TARGET_TIME;
    rwip_env.timer_hs_target   = RWIP_INVALID_TARGET_TIME;
    rwip_env.timer_hus_target  = RWIP_INVALID_TARGET_TIME;

    if(reset)
    {
        // Reset the IP core
        ip_rwdmcntl_master_soft_rst_setf(1);
        while(ip_rwdmcntl_master_soft_rst_getf());
    }

    // Enable default common interrupts
    ip_intcntl1_set(IP_FIFOINTMSK_BIT | IP_CRYPTINTMSK_BIT | IP_ERRORINTMSK_BIT | IP_SWINTMSK_BIT);

    #if (BT_DUAL_MODE)
    // Read diagport configuration from NVDS
    length = PARAM_LEN_DIAG_DM_HW;
    if(rwip_param.get(PARAM_ID_DIAG_DM_HW, &length, diag_cfg) == PARAM_OK)
    {
        ip_diagcntl_pack(1, diag_cfg[3], 1, diag_cfg[2], 1, diag_cfg[1], 1, diag_cfg[0]);
    }
    else
    {
        ip_diagcntl_set(0);
    }
    #endif // (BT_DUAL_MODE)

    // Activate deep sleep feature if enabled in NVDS and in reset mode
    length = PARAM_LEN_SLEEP_ENABLE;
    if(!reset || rwip_param.get(PARAM_ID_SLEEP_ENABLE, &length, &sleep_enable) != PARAM_OK)
    {
        sleep_enable = 0;
    }
    sleep_enable = 1;
    // check is sleep is enabled
    if(sleep_enable != 0)
    {
        uint16_t twext, twosc, twrm;

        // Set max sleep duration depending on wake-up mode
        if(rwip_param.get(PARAM_ID_EXT_WAKEUP_ENABLE, &length, &ext_wakeup_enable) != PARAM_OK)
        {
            ext_wakeup_enable = 0;
        }
        rwip_env.ext_wakeup_enable = (ext_wakeup_enable != 0) ? true : false;

        // Set max sleep duration depending on wake-up mode
        length = sizeof(rwip_env.sleep_algo_dur);
        if(rwip_param.get(PARAM_ID_SLEEP_ALGO_DUR, &length, (uint8_t*) &rwip_env.sleep_algo_dur) != PARAM_OK)
        {
            // set a default duration: 200 us ==> 400 half us
            rwip_env.sleep_algo_dur = 400;
        }

        // Initialize sleep parameters
        rwip_env.sleep_acc_error   = 0;

        // Get TWrm from NVDS
        length = sizeof(uint16_t);
        if (rwip_param.get(PARAM_ID_RM_WAKEUP_TIME, &length, (uint8_t*)&twrm) != PARAM_OK)
        {
            // Set default values : 625 us
            twrm = 1500;//SLEEP_RM_WAKEUP_DELAY;
        }

        // Get TWosc from NVDS
        length = sizeof(uint16_t);
        if (rwip_param.get(PARAM_ID_OSC_WAKEUP_TIME, &length, (uint8_t*)&twosc) != PARAM_OK)
        {
            // Set default values : 5 ms
            twosc = 1500;//SLEEP_OSC_NORMAL_WAKEUP_DELAY;
        }

        // Get TWext from NVDS
        length = sizeof(uint16_t);
        if (rwip_param.get(PARAM_ID_EXT_WAKEUP_TIME, &length, (uint8_t*)&twext) != PARAM_OK)
        {
            // Set default values : 5 ms
            twext = 1500;//SLEEP_OSC_EXT_WAKEUP_DELAY;
        }

        twrm  = rwip_us_2_lpcycles(twrm);
        twosc = rwip_us_2_lpcycles(twosc);
        twext = rwip_us_2_lpcycles(twext);

        // Program register
        ip_enbpreset_pack(twext, twosc, twext);

        // Configure wake up delay to the highest parameter
        twext = co_max(twext,twrm);
        twext = co_max(twext,twosc);

        // Store wake-up delay in lp cycles
        rwip_env.lp_cycle_wakeup_delay = twext;

        // Set the external wakeup parameter
        //ip_deepslcntl_extwkupdsb_setf(rwip_env.ext_wakeup_enable ? false : true);
        ip_deepslcntl_extwkupdsb_setf(0);
    }
    else
    {
        // ensure that we will never enter in deep sleep
        rwip_prevent_sleep_set(RW_PLF_DEEP_SLEEP_DISABLED);
    }

    #if BT_DUAL_MODE
    // Set BTDM arbiter
    //TODO: restore //ip_prioscharb_pack(/*blepriomode*/ 1, /*bredrpriomode*/ 1);
    #endif //BT_DUAL_MODE
    #elif (BLE_HOST_PRESENT)
    // initialize environment
    rwip_env.prevent_sleep     = 0;
    rwip_env.timer_10ms_target = RWIP_INVALID_TARGET_TIME;
    // enable timer
    timer_enable(true);
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
}

__ATTR_ARM void rwip_prevent_sleep_set(uint16_t prv_slp_bit)
{
    GLOBAL_INT_DISABLE();
    rwip_env.prevent_sleep |= prv_slp_bit;
    GLOBAL_INT_RESTORE();
}

void rwip_prevent_sleep_clear(uint16_t prv_slp_bit)
{
    GLOBAL_INT_DISABLE();
    rwip_env.prevent_sleep &= ~prv_slp_bit;
    GLOBAL_INT_RESTORE();
}

__ATTR_ARM uint16_t rwip_sleep_flag(void)
{
    return rwip_env.prevent_sleep;
}

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)

bool rwip_active_check(void)
{
    bool result = true;

    do
    {
        #if BT_EMB_PRESENT
        if(rwip_env.prevent_sleep & (RW_CSB_NOT_LPO_ALLOWED | RW_BT_ACTIVE_MODE))
            break;
        #endif // BT_EMB_PRESENT

        #if BLE_EMB_PRESENT
        if(rwip_env.prevent_sleep & (RW_BLE_ACTIVE_MODE))
            break;
        #endif // BLE_EMB_PRESENT

        result = false;

    } while (0);

    return result;
}

#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
void rwip_timer_10ms_set(uint32_t target)
{
    GLOBAL_INT_DISABLE();

    if (target != RWIP_INVALID_TARGET_TIME)
    {
        // save target time
        rwip_env.timer_10ms_target = RWIP_10MS_TIME_TO_CLOCK(target);

        #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
         // set the abs timeout in HW
        ip_clkntgt2_set(rwip_env.timer_10ms_target);
        ip_hmicrosectgt2_set(HALF_SLOT_TIME_MAX);

        // if timer is not enabled, it is possible that the irq is raised
        // due to a spurious value, so ack it before
        ip_intack1_timestamptgt2intack_clearf(1);
        ip_intcntl1_timestamptgt2intmsk_setf(1);
        #elif (BLE_HOST_PRESENT)
        // Start timer
        timer_set_timeout(target, rwip_timer_10ms_handler);
        #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    }
    else
    {
        // save target time - not set
        rwip_env.timer_10ms_target = RWIP_INVALID_TARGET_TIME;

        #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
        // disable timer irq
        ip_intcntl1_timestamptgt2intmsk_setf(0);
        #elif (BLE_HOST_PRESENT)
        // Stop timer
        timer_set_timeout(0, NULL);
        #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    }
    GLOBAL_INT_RESTORE();
}

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
void rwip_timer_hs_set(uint32_t target)
{
    // save target time
    rwip_env.timer_hs_target = target;

    if (target != RWIP_INVALID_TARGET_TIME)
    {
        // set the abs timeout in HW
        ip_finetimtgt_finetarget_setf(target);

        // if timer is not enabled, it is possible that the irq is raised
        // due to a spurious value, so ack it before
        ip_intack1_finetgtintack_clearf(1);
        ip_intcntl1_finetgtintmsk_setf(1);
    }
    else
    {
        // disable timer irq
        ip_intcntl1_finetgtintmsk_setf(0);
    }
}

__ATTR_ARM void rwip_timer_hus_set(uint32_t target, uint32_t half_us_delay)
{
    // save target time
    rwip_env.timer_hus_target = target;

    if (target != RWIP_INVALID_TARGET_TIME)
    {
        ASSERT_INFO(half_us_delay < HALF_SLOT_SIZE, half_us_delay, 0);

        // set the abs timeout in HW
        ip_clkntgt1_setf(target);
        ip_hmicrosectgt1_setf(HALF_SLOT_TIME_MAX - half_us_delay);

        // if timer is not enabled, it is possible that the irq is raised
        // due to a spurious value, so ack it before
        ip_intack1_timestamptgt1intack_clearf(1);
        ip_intcntl1_timestamptgt1intmsk_setf(1);
    }
    else
    {
        // disable timer irq
        ip_intcntl1_timestamptgt1intmsk_setf(0);
    }
}

void rwip_aes_encrypt(const uint8_t *key, const uint8_t* val)
{
    // Prevent going to deep sleep during encryption
    rwip_prevent_sleep_set(RW_CRYPT_ONGOING);

    // Copy data to EM buffer
    em_wr(val, EM_ENC_IN_OFFSET, KEY_LEN);

    // copy the key in the register dedicated for the encryption
    ip_aeskey31_0_set(  co_read32p(&(key[0])));
    ip_aeskey63_32_set( co_read32p(&(key[4])));
    ip_aeskey95_64_set( co_read32p(&(key[8])));
    ip_aeskey127_96_set(co_read32p(&(key[12])));

    // Set the pointer on the data to encrypt.
    ip_aesptr_setf(EM_ENC_IN_OFFSET >> 2);

    // enable crypt interrupt (and clear a previous interrupt if needed)
    ip_intack1_cryptintack_clearf(1);
    ip_intcntl1_cryptintmsk_setf(1);

    // start the encryption
    ip_aescntl_aes_start_setf(1);
}

__ATTR_ARM void rwip_sw_int_req(void)
{
    // enable SW interrupt (and clear a previous interrupt if needed)
    ip_intack1_swintack_clearf(1);
    ip_intcntl1_swintmsk_setf(1);
    // start the SW interrupt
    ip_rwdmcntl_swint_req_setf(1);
}

__ATTR_ARM void rwip_set_hs_target_time(uint32_t target)
{
    rwip_env.timer_hus_target = target;
}


#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

///@} RW
