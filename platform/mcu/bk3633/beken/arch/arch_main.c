/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ******** ********************************************************************************
 */


/*
 * INCLUDES
 ****************************************************************************************
 */
 
#include "rwip_config.h" // RW SW configuration

#include "arch.h"      // architectural platform definitions
#include <stdlib.h>    // standard lib functions
#include <string.h>    // standard lib functions
#include <stdio.h>    // standard lib functions
#include <stddef.h>    // standard definitions
#include <stdint.h>    // standard integer definition
#include <stdbool.h>   // boolean definition
#include <typedef.h>

#include <k_api.h>

#include "uart_pub.h"      	// UART initialization
#include "func_pub.h"
#include "rwip.h"      // RW SW initialization

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
#include "rf.h"        // RF initialization
#endif // BLE_EMB_PRESENT || BT_EMB_PRESENT

#if (BLE_APP_PRESENT)
#include "app.h"       // application functions
#endif // BLE_APP_PRESENT

#include "user_config.h"
#include "intc_pub.h"
#include "fake_clock_pub.h"
#include "em_map.h"
#include "icu_pub.h"

/**
 ****************************************************************************************
 * @addtogroup DRIVERS
 * @{
 *
 *
 * ****************************************************************************************
 */


/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */
extern void Delay_ms(int num) ;

#if ((UART_PRINTF_EN) &&(UART_DRIVER))
void assert_err(const char *condition, const char * file, int line)
{
	//while(1)
    #if (UART_PRINTF_EN && UART_DRIVER)	
	stack_printf("%s,condition %s,file %s,line = %d\r\n",__func__,condition,file,line);
    #endif
 // Delay_ms(5000);
}

void assert_param(int param0, int param1, const char * file, int line)
{
//	Delay_ms(5000);
    #if (UART_PRINTF_EN && UART_DRIVER)	
	stack_printf("%s,param0 = 0x%x,param1 = 0x%x,file = %s,line = %d\r\n",__func__,param0,param1,file,line);
    #endif
  
}

void assert_warn(int param0, int param1, const char * file, int line)
{
//	 Delay_ms(5000);
    #if (UART_PRINTF_EN && UART_DRIVER)	
	 stack_printf("%s,param0 = 0x%x,param1 = 0x%x,file = %s,line = %d\r\n",__func__,param0,param1,file,line);
    #endif
 
}

void dump_data(uint8_t* data, uint16_t length)
{
	//Delay_ms(5000);
    #if (UART_PRINTF_EN && UART_DRIVER)	
	stack_printf("%s,data = 0x%x,length = 0x%x,file = %s,line = %d\r\n",__func__,data,length);
    #endif
 
}
#else
void assert_err(const char *condition, const char * file, int line)
{
  
}

void assert_param(int param0, int param1, const char * file, int line)
{
  
}

void assert_warn(int param0, int param1, const char * file, int line)
{
 
}

void dump_data(uint8_t* data, uint16_t length)
{
 
}
#endif //UART_PRINTF_EN

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void platform_reset(uint32_t error)
{
    //void (*pReset)(void);
	
	UART_PRINTF("error = %x\r\n", error);

    // Disable interrupts
    GLOBAL_INT_STOP();

    #if UART_PRINTF_EN
    // Wait UART transfer finished
    uart_finish_transfers();
    #endif //UART_PRINTF_EN


    if(error == RESET_AND_LOAD_FW || error == RESET_TO_ROM)
    {
        // Not yet supported
    }
    else
    {
        //Restart FW
        //pReset = (void * )(0x0);
        //pReset();
	    //wdt_enable(10);
		while(1);
    }
}

/**
 *******************************************************************************
 * @brief RW main function.
 *
 * This function is called right after the booting process has completed.
 *
 * @return status   exit status
 *******************************************************************************
 */

void ble_handler(void *arg)
{
	ble_hdr_arg_t *hdr_arg = (ble_hdr_arg_t *)arg;
	uint32_t      cont_loop_cnt=0;
	UART_PRINTF("ble_handler start!\r\n");
	/*
     ***************************************************************************
     * Platform initialization
     ***************************************************************************
     */ 
		
#if SYSTEM_SLEEP	
	uint8_t sleep_type = 0;
#endif
    // Initialize random process
    srand(1);

    /*
      ***************************************************************************
      * RW SW stack initialization
      ***************************************************************************
    */
    UART_PRINTF("Initialize RW SW stack\r\n");
    // Initialize RW SW stack
    rwip_func.rwip_init(0);

    // Set the CPU default clock to 80M Hz.
    UINT32 param = ICU_MCU_CLK_SEL_80M;
    sddev_control(ICU_DEV_NAME, CMD_ICU_MCU_CLK_SEL, &param);
    //flash_init();

    if(hdr_arg->public_addr)
    {
        // Set the BDADDR
    	rwip_func.rwip_set_bd_address(hdr_arg->public_addr);
    	printf("set ble bd addr: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				hdr_arg->public_addr->addr[5],
				hdr_arg->public_addr->addr[4],
				hdr_arg->public_addr->addr[3],
				hdr_arg->public_addr->addr[2],
				hdr_arg->public_addr->addr[1],
				hdr_arg->public_addr->addr[0]);
    }

    //intc_service_register(FIQ_BLE, PRI_FIQ_BLE, rwip_func.rwble_isr);
	intc_service_register(FIQ_BTDM, 20, rwip_func.rwip_isr);
    intc_enable(FIQ_BTDM);

	printf("%s \n", __func__);

    /*
     ***************************************************************************
     * Main loop
     ***************************************************************************
     */
	UART_PRINTF("ble driver start!\r\n");

    krhino_add_mm_region(g_kmm_head, 
                        (void *)(REG_EM_ET_BASE_ADDR + EM_BLE_END + 1), (size_t)(EM_BT_SIZE -  EM_BLE_END - 4));

	if(hdr_arg->ready_sem)	krhino_sem_give(hdr_arg->ready_sem);
	krhino_sem_give(&ke_event_sem);
    while(1)
    {
    	cont_loop_cnt++;
		if(!krhino_sem_take(&ke_event_sem, -1)) {
			cont_loop_cnt = 0;
		}

        //schedule all pending events
        rwip_func.rwip_schedule();
    }
}

/**
 *******************************************************************************
 * @brief system exception function.
 *
 * This function is system exception function
 *
 * @return
 *******************************************************************************
 */
void print_exception_addr(unsigned int pc, unsigned int lr, unsigned int sp)
{
    cpu_intrpt_save();
    os_printf("pc is %x, lr is %x, sp is %x\n", pc, lr, sp);
    while (1);
}

/**
 *******************************************************************************
 * @brief SOC system initial function.
 *
 * This function is called right after the booting process has completed.
 *
 * @return
 *******************************************************************************
 */
void soc_system_init(void)
{
	os_printf("%s \r\n", __func__);
    func_init();
    hal_init();
}


/**
 *******************************************************************************
 * @brief system entry function.
 *
 * This function is system entry point
 *
 * @return
 *******************************************************************************
 */
void entry_main(void)
{
    sys_start(); // goto AOS system start
}

/// @} DRIVERS
