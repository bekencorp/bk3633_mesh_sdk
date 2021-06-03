/**
****************************************************************************************
*
* @file rwble.c
*
* @brief RWBLE core interrupt handler
*
* Copyright (C) RivieraWaves 2009-2016
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"      // stack configuration
#if (BT_DUAL_MODE || BLE_STD_MODE)
#include <string.h>           // for mem* functions
#include "co_version.h"
#include "co_math.h"
#include "rwble.h"            // BLE API definition
#include "rwip.h"             // stack main module

#include "ble_util_buf.h"     // BLE EM buffer management
#include "lld.h"              // link layer driver definition
#include "llc.h"              // link layer controller definition
#include "llm.h"              // link layer manager definition

#if (BLE_ISO_PRESENT)
#include "lli.h"              // Link Layer ISO definition
#endif // (BLE_ISO_PRESENT)

#include "ke_event.h"         // kernel event definition

#include "sch_arb.h"          // Scheduling Arbiter
#include "sch_prog.h"         // Scheduling Programmer

#include "dbg.h"              // debug definitions

#include "reg_blecore.h"      // BLE Core registers
#include "user_config.h"
#include "debug_uart.h"
#include "reg_ipcore.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// Environment structure for RW BLE interrupt handler
struct rwble_env_tag
{
    uint32_t irq_mask0;
    uint32_t irq_mask1;
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Environment of the RW BLE interrupt handler
static struct rwble_env_tag rwble_env;

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void rwble_init(bool reset)
{
    if(!reset)
    {
        uint32_t seed;
        rwip_time_t current_time = rwip_time_get();

        seed =  current_time.hs;
        seed += current_time.hus;

        //Init the random seed
        co_random_init(seed);
    }

    // Initialize buffer management system
    ble_util_buf_init(reset);
    //UART_PRINTF("rwble_init reset\r\n");

    #if (BLE_ISO_PRESENT)
    // Initialize ISO descriptor management system
    ble_util_isodesc_init(reset);
    #endif // (BLE_ISO_PRESENT)

    // Initialize the Link Layer Driver
    lld_init(reset);
    //UART_PRINTF("lld_init OK\r\n");

    #if(BLE_CENTRAL || BLE_PERIPHERAL)
    // Initialize the Link Layer Controller
    llc_init(reset);
    //UART_PRINTF("llc_init OK\r\n");
    #endif // (BLE_CENTRAL || BLE_PERIPHERAL)

    // Initialize the Link Layer Manager
    llm_init(reset);
    //UART_PRINTF("llm_init OK\r\n");
    
    #if (BLE_ISO_PRESENT)
    // Initialize the Link Layer ISO
    lli_init(reset);
    #endif // (BLE_ISO_PRESENT)

    if(reset)
    {
        // Turn on BLE Core
        ble_rwblecntl_rwble_en_setf(1);
        //UART_PRINTF("Turn on BLE Core OK\r\n");
    }
}

#if BT_DUAL_MODE
bool rwble_activity_ongoing_check(void)
{
    // check that a BLE activity is ongoing (advertising, scan, initiating, connection)
    return llm_activity_ongoing_check(); //meger from rw-ble-sw-v10_0_5m 20/03/16	
  //  return (false); // TODO check that a BLE activity is ongoing (advertising, scan, initiating, connection)
}
#endif //BT_DUAL_MODE

__BLEIRQ void rwble_isr(void)
{
    DBG_SWDIAG(ISR, BLE, 1);

    // Loop until no more interrupts have to be handled
    while (1)
    {
        // Check BLE interrupt status and call the appropriate handlers
        uint32_t irq_stat      = ble_intstat0_get();
        //UART_PRINTF("%s irq_stat(%x) \r\n",__func__, irq_stat );  	

        if (irq_stat == 0)
            break;

        // Error interrupt
        if (irq_stat & BLE_ERRORINTSTAT_BIT)
        {
            // Clear the interrupt
            ble_intack0_errorintack_clearf(1);
            //UART_PRINTF("err:%x \r\n",ble_errortypestat_get());  
            ASSERT_INFO(0, ble_errortypestat_get(), (ble_errortypestat_get()>>16));
        }
    }

    DBG_SWDIAG(ISR, BLE, 0);
}

__ATTR_ARM void rwble_sleep_enter(void)
{
    // Keep currently enabled interrupts
    rwble_env.irq_mask1 = ip_intcntl1_get();
    rwble_env.irq_mask0 = ble_intcntl0_get();  
    
    // Mask all interrupts
    ble_intcntl0_set(0);
    // Clear possible pending IRQs
    ble_intack0_clear(0xFFFFFFFF);

    // Mask all interrupts except sleep IRQ
    ip_intcntl1_set(IP_SLPINTMSK_BIT);
    // Clear possible pending IRQs
    ip_intack1_clear(0xFFFFFFFF);
}

void rwble_sleep_wakeup_end(void)
{
    // Restore enabled interrupts
    ble_intcntl0_set(rwble_env.irq_mask0 | ble_intcntl0_get());

    ip_intcntl1_set( (rwble_env.irq_mask1 | ip_intcntl1_get()) );
}

#endif //(BT_DUAL_MODE || BLE_STD_MODE) 
///@} RWBTINT
