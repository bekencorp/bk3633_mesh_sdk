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

//#include "boot.h"      // boot definition
#include "rwip.h"      // RW SW initialization
//#include "syscntl.h"   // System control initialization
//#include "emi.h"       // EMI initialization
#include "intc.h"      // Interrupt initialization
#include "timer.h"     // TIMER initialization
#include "icu.h"
#include "uart.h"      	// UART initialization
#include "flash.h"     // Flash initialization
//#include "led.h"       // Led initialization
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
#include "rf.h"        // RF initialization
#endif // BLE_EMB_PRESENT || BT_EMB_PRESENT

#if (BLE_APP_PRESENT)
#include "app.h"       // application functions
#endif // BLE_APP_PRESENT

//#include "nvds.h"         // NVDS definitions

//#include "reg_assert_mgr.h"
#include "BK3633_RegList.h"
//#include "RomCallFlash.h"
#include "gpio.h"
#include "pwm.h"
//#include "audio.h"
//#include "app_task.h"
//#include "ir.h"
//#include "oads.h"
// #include "wdt.h"
#include "user_config.h"
#include "intc_pub.h"
#include "rwble.h"
//#include "lld_util.h"

//#include "ke_event.h"
#include "ke_timer.h"
#include "app_task.h"

//#include "rf_test.h"
#include "fake_clock_pub.h"
#include "em_map.h"
#include "icu_pub.h"
#include "gpio_pub.h"

/**
 ****************************************************************************************
 * @addtogroup DRIVERS
 * @{
 *
 *
 * ****************************************************************************************
 */

#ifdef CONFIG_DUT_TEST_CMD
void check_and_set_dut_flag(void)
{    
    gpio_dev_t gpio;
    uint32_t gpio02_val;
    gpio.port =  GPIO_P02;
    gpio.config = INPUT_PULL_DOWN;
	hal_gpio_init(&gpio);
	hal_gpio_input_get(&gpio, &gpio02_val);

	if(1 == gpio02_val)
	{
		rwip_func.rwip_set_dut_mode(1);
	}
	else
	{
		rwip_func.rwip_set_dut_mode(0);
	}
}

uint8_t get_dut_flag(void)
{
	uint8_t dut_flg;
	dut_flg = rwip_func.rwip_get_dut_mode();
	return dut_flg;
}
#endif


/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// Description of unloaded RAM area content
struct unloaded_area_tag
{
    // status error
    uint32_t error;
};


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Creation of uart external interface api
struct rwip_eif_api uart_api;

#if !(BLE_EMB_PRESENT) && !(BT_EMB_PRESENT)
// Creation of uart second external interface api

#endif // !BLE_EMB_PRESENT && !(BT_EMB_PRESENT)

#if (PLF_DEBUG)
/// Variable to enable infinite loop on assert
volatile int dbg_assert_block = 1;
#endif //PLF_DEBUG


/// Variable storing the reason of platform reset
uint32_t error = RESET_NO_ERROR;

uint32_t critical_sec_cnt = 0;

/*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */
extern void Delay_ms(int num) ;

static void Stack_Integrity_Check(void);

//extern void code_sanity_check(void);

#if (UART_DRIVER)
void uart_rx_handler(uint8_t *buf, uint8_t len);
#endif

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

#if 1
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

//0102210630355cff0078b69d5538dd22
uint8_t encrypt_key_array[16] = 
{
	0x01, 0x02, 0x21, 0x06,
	0x30, 0x35, 0x5c, 0xff,
	0x00, 0x78, 0xb6, 0x9d,
	0x55, 0x38, 0xdd, 0x22
};
#endif

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
//float abcd = 0.5;
//extern struct rom_env_tag rom_env;

void rwip_eif_api_init(void);
//int main(void)
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
    // UINT32 param = ICU_MCU_CLK_SEL_80M; //ICU_MCU_CLK_SEL_16M;
    // sddev_control(ICU_DEV_NAME, CMD_ICU_MCU_CLK_SEL, &param);

	
#ifdef CONFIG_DUT_TEST_CMD
    if(get_dut_flag())
	{
        core_peri_clk_freq_set(6, 6);   //3, 1  
    }
	else
#endif
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
	//intc_service_register(FIQ_BTDM, 20, rwip_func.rwip_isr);
    intc_enable(FIQ_BTDM);
    intc_enable(FIQ_BLE);
	printf("%s \n", __func__);

    /*
     ***************************************************************************
     * Main loop
     ***************************************************************************
     */
	UART_PRINTF("ble driver start!\r\n");
    printf("ble driver start, EM_BLE_END 0x%x!\r\n", EM_BLE_END);
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

void enrty_boot(void)
{
    //GPIO detect level demo.
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
