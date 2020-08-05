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
	uart_printf("%s,condition %s,file %s,line = %d\r\n",__func__,condition,file,line);
    #endif
 // Delay_ms(5000);
}

void assert_param(int param0, int param1, const char * file, int line)
{
//	Delay_ms(5000);
    #if (UART_PRINTF_EN && UART_DRIVER)	
	uart_printf("%s,param0 = 0x%x,param1 = 0x%x,file = %s,line = %d\r\n",__func__,param0,param1,file,line);
    #endif
  
}

void assert_warn(int param0, int param1, const char * file, int line)
{
//	 Delay_ms(5000);
    #if (UART_PRINTF_EN && UART_DRIVER)	
	 uart_printf("%s,param0 = 0x%x,param1 = 0x%x,file = %s,line = %d\r\n",__func__,param0,param1,file,line);
    #endif
 
}

void dump_data(uint8_t* data, uint16_t length)
{
	//Delay_ms(5000);
    #if (UART_PRINTF_EN && UART_DRIVER)	
	uart_printf("%s,data = 0x%x,length = 0x%x,file = %s,line = %d\r\n",__func__,data,length);
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

//锟斤拷锟斤拷锟斤拷锟杰碉拷原始锟斤拷锟捷ｏ拷锟斤拷要锟斤拷锟斤拷录锟斤拷锟斤拷锟绞憋拷锟斤拷峁�
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
	//	wdt_enable(10);
		while(1);
    }
}

#if 0

#if MAC78da07bcd71b

uint8_t Dev_Mac[6] = {0x78,0xda,0x07,0xbc,0xd7,0x1b};

uint8_t Auth_Value[16] = 
{
	[0] = 0xe5,[1] = 0xf2,[2] = 0xd3,[3] = 0x6a,
	[4] = 0x86,[5] = 0x37,[6] = 0xce,[7] = 0xad,
	[8] = 0xa2,[9] = 0x7a,[10] = 0x28,[11] = 0x10,
	[12] = 0x4d,[13] = 0x0a,[14] = 0xdc,[15] = 0xed,
};
	 
#elif MAC78da07bcd71c
uint8_t Dev_Mac[6] = {0x78,0xda,0x07,0xbc,0xd7,0x1c};

uint8_t Auth_Value[16] = 
{
	[0] = 0x3d,[1] = 0x67,[2] = 0x33,[3] = 0xa7,
	[4] = 0x12,[5] = 0xef,[6] = 0x67,[7] = 0xc2,
	[8] = 0x98,[9] = 0x68,[10] = 0x9b,[11] = 0x6c,
	[12] = 0x25,[13] = 0x9e,[14] = 0xa6,[15] = 0xa8,
};
#elif 	MAC78da07bcd71d
uint8_t Dev_Mac[6] = {0x78,0xda,0x07,0xbc,0xd7,0x1d};

uint8_t Auth_Value[16] = 
{
	[0] = 0xa6,[1] = 0x2f,[2] = 0x94,[3] = 0xad,
	[4] = 0xc3,[5] = 0x44,[6] = 0x6f,[7] = 0xdd,
	[8] = 0x6f,[9] = 0x5f,[10] = 0x67,[11] = 0x2a,
	[12] = 0x20,[13] = 0xb4,[14] = 0x2a,[15] = 0xc5,
};
#endif


#endif//
//for debug
#if 0
void bdaddr_env_init(void)
{    
	struct bd_addr co_bdaddr;    
	//flash_read_data(&co_bdaddr.addr[0],0x400e3,6);  
    flash_read_data(&co_bdaddr.addr[0],0x600e3,6);//current mesh bdaddress move to 600e3        
	if(co_bdaddr.addr[0]!=0xff ||co_bdaddr.addr[1]!=0xff||
	   co_bdaddr.addr[2]!=0xff||co_bdaddr.addr[3]!=0xff||        
	   co_bdaddr.addr[4]!=0xff||co_bdaddr.addr[5]!=0xff )    
	{        
		memcpy(&co_default_bdaddr,&co_bdaddr,6); 
		//CHARLES_PRINTF("Use Flash!!!\r\n");
	}
	//memcpy(co_default_bdaddr.addr,Dev_Mac,6);
	//CHARLES_PRINTF("co_default_bdaddr: %x%x%x%x%x%x\r\n",co_default_bdaddr.addr[0],
    //co_default_bdaddr.addr[1],co_default_bdaddr.addr[2],co_default_bdaddr.addr[3],
    //co_default_bdaddr.addr[4],co_default_bdaddr.addr[5]);
#if 0 //use Flash value
#if MAC78da07bcd71b
	
		
	  co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0x1b;
#elif MAC78da07bcd71c
		co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0x1c;
#elif 	MAC78da07bcd71d
		co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0x1d;
#elif    MAC78da07bcd7A1//#1
        co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0xA1;
#elif    MAC78da07bcd7A2//#2
        co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0xA2;
#elif    MAC78da07bcd7A3//#3
        co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0xA3;
#elif    MAC78da07bcd7A4//#4
        co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
		co_default_bdaddr.addr[1] = 0xda;
		co_default_bdaddr.addr[2] = 0x07;
		co_default_bdaddr.addr[3] = 0xbc;
		co_default_bdaddr.addr[4] = 0xd7;
		co_default_bdaddr.addr[5] = 0xA4;
#elif    MAC78da07bcd7A5//#5
        co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
        co_default_bdaddr.addr[1] = 0xda;
        co_default_bdaddr.addr[2] = 0x07;
        co_default_bdaddr.addr[3] = 0xbc;
        co_default_bdaddr.addr[4] = 0xd7;
        co_default_bdaddr.addr[5] = 0xA5;
#elif    MAC78da07bcd7A6//#6
        co_default_bdaddr.addr[0] = 0x78;//78da07bcd71b;
        co_default_bdaddr.addr[1] = 0xda;
        co_default_bdaddr.addr[2] = 0x07;
        co_default_bdaddr.addr[3] = 0xbc;
        co_default_bdaddr.addr[4] = 0xd7;
        co_default_bdaddr.addr[5] = 0xA6;
#endif
#endif

} 
#endif

#if 0
void user_timer_cb(unsigned char ucChannel)
{
	static uint32_t cnt = 0;
	cnt++;
	if(cnt > 50000)
	{
		pwm_disable(ucChannel);
		icu_set_sleep_mode(0);
		rwip_prevent_sleep_clear(BK_DRIVER_TIMER_ACTIVE);
#if (UART_PRINTF_EN && UART_DRIVER)	
		uart_printf("app_timer_cb end %d\r\n", cnt);
#endif
	}
}


void user_timer_init(void)
{
	icu_set_sleep_mode(0);
	rwip_prevent_sleep_set(BK_DRIVER_TIMER_ACTIVE);
	PWM_DRV_DESC timer_desc;

	timer_desc.channel = 1;            				  
    timer_desc.mode    = 1<<0 | 1<<1 | 1<<2 | 0<<4;   
    timer_desc.end_value  = 32768;                      
    timer_desc.duty_cycle = 0;                        
    timer_desc.p_Int_Handler = user_timer_cb;  		  
	pwm_init(&timer_desc);
}
#endif

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

	//get System sleep flag
	//system_sleep_init();
	
    // Initialize the exchange memory interface
    //emi_init();
	  
	//rwip_eif_api_init();
#if 0
	UART_PRINTF("The ble hci driver build at %s, %s\r\n", __TIME__, __DATE__);
	//UART_PRINTF("float abcd = %f\r\n",3.259);
#endif
    //flash_advance_init();
    //bdaddr_env_init();
	
#if  0//bk encrypt interface test
	code_sanity_check();
#endif

#if 0
	 // Initialize NVDS module
    struct nvds_env_tag env;
	env.flash_read = &flash_read;
	env.flash_write = &flash_write;
	env.flash_erase = &flash_erase;
	nvds_init(env);
	//nvds_null_init();
#endif
	
    //rom_env_init(&rom_env);

	#if  0 //(ALI_MESH)

	nvds_status = nvds_get(NVDS_TAG_POWER_RESET_CNT, 1, &g_reset_cnt);
	
	if(nvds_status == NVDS_OK)
	{
		UART_PRINTF("def g_reset_cnt= %d\r\n",g_reset_cnt);
		if(g_reset_cnt < 5)
		{
			g_reset_cnt++;
			nvds_put(NVDS_TAG_POWER_RESET_CNT, 1, &g_reset_cnt);
		}
	}
	else if(nvds_status == NVDS_TAG_NOT_DEFINED)
	{
			g_reset_cnt = 1;
			UART_PRINTF("nodef g_reset_cnt = %d\r\n",g_reset_cnt);
			nvds_put(NVDS_TAG_POWER_RESET_CNT, 1, &g_reset_cnt);
	}
	#endif
	

    /*
      ***************************************************************************
      * RW SW stack initialization
      ***************************************************************************
    */
    UART_PRINTF("Initialize RW SW stack\r\n");
    // Initialize RW SW stack
    rwip_func.rwip_init(0);

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
    /*else
    {
    	struct bd_addr bd_addr;

    	///get default public bd address from flash 0x07B000
    	flash_read(&bd_addr, sizeof(struct bd_addr), 0x7B000);
    	rwip_func.rwip_set_bd_address(&bd_addr);
		printf("default bd address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				bd_addr.addr[5],
				bd_addr.addr[4],
				bd_addr.addr[3],
				bd_addr.addr[2],
				bd_addr.addr[1],
				bd_addr.addr[0]);
    }*/

    //intc_service_register(FIQ_BLE, PRI_FIQ_BLE, rwip_func.rwble_isr);
	intc_service_register(FIQ_BTDM, 20, rwip_func.rwip_isr);
    intc_enable(FIQ_BTDM);



#if 0
	REG_AHB0_ICU_INT_ENABLE |= (0x01 << 15); //BLE INT
	REG_AHB0_ICU_IRQ_ENABLE = 0x03;

    // finally start interrupt handling
    GLOBAL_INT_START();
#endif

	//** for test
	#if 0
	UART_PRINTF("dump ke_task status\r\n");
	for(uint16_t i=0; i<TASK_MAX; i++)
		UART_PRINTF("task id:%d. status:%d\r\n", i, ke_task_check(i));
	#endif
	printf("=========== %s ++++++++++++++++++++ \n", __func__);
	//ke_time_func.ke_timer_set(APP_PERIOD_TIMER, TASK_APP, 200);
	//**

    /*
     ***************************************************************************
     * Main loop
     ***************************************************************************
     */
	UART_PRINTF("ble driver start!\r\n");

	fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);

	if(hdr_arg->ready_sem)	krhino_sem_give(hdr_arg->ready_sem);
	krhino_sem_give(&ke_event_sem);
    while(1) 
    {
    	cont_loop_cnt++;
		if(!krhino_sem_take(&ke_event_sem, hdr_arg->task_timeout)) {
			cont_loop_cnt = 0;
		}

    	///debug timer
    	/*if(!ke_timer_active(APP_PERIOD_TIMER,TASK_APP))
    		ke_timer_set(APP_PERIOD_TIMER,TASK_APP,200);*/

    	//schedule all pending events
		rwip_func.rwip_schedule();
		  	
#if 0
	    // Checks for sleep have to be done with interrupt disabled
    	GLOBAL_INT_DISABLE();

		oad_updating_user_section_pro();
		
        if(wdt_disable_flag==1)
       	{
			 wdt_disable();	
        } 
#if 0//SYSTEM_SLEEP	
		// Check if the processor clock can be gated
    	sleep_type = rwip_sleep();	
		if((sleep_type & RW_MCU_DEEP_SLEEP) == RW_MCU_DEEP_SLEEP)
    	{	
			// 1:idel  0:reduce voltage
    		if(icu_get_sleep_mode()) 
    		{
				cpu_idle_sleep();
			}
			else
			{
				cpu_reduce_voltage_sleep();
			}
    	}else if((sleep_type & RW_MCU_IDLE_SLEEP) == RW_MCU_IDLE_SLEEP)
		{
			cpu_idle_sleep();
		}
#endif
		Stack_Integrity_Check();
    	GLOBAL_INT_RESTORE();
#endif
    }
}

#if 0
#if (UART_DRIVER)
static void uart_rx_handler(uint8_t *buf, uint8_t len)
{
	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("0x%x ", buf[i]);
	}
	uart_printf("\r\n");
}
#endif

void rwip_eif_api_init(void)
{
	uart_api.read = &uart_read;
	uart_api.write = &uart_write;
	uart_api.flow_on = &uart_flow_on;
	uart_api.flow_off = &uart_flow_off;
}

const struct rwip_eif_api* rwip_eif_get(uint8_t type)
{
    const struct rwip_eif_api* ret = NULL;
    switch(type)
    {
        case RWIP_EIF_AHI:
        {
            ret = &uart_api;
        }
        break;
        #if (BLE_EMB_PRESENT) || (BT_EMB_PRESENT)
        case RWIP_EIF_HCIC:
        {
            ret = &uart_api;
        }
        break;
        #elif !(BLE_EMB_PRESENT) || !(BT_EMB_PRESENT)
        case RWIP_EIF_HCIH:
        {
            ret = &uart_api;
        }
        break;
        #endif 
        default:
        {
            ASSERT_INFO(0, type, 0);
        }
        break;
    }
    return ret;
}

static void Stack_Integrity_Check(void)
{
	if ((REG_PL_RD(STACK_BASE_UNUSED)!= BOOT_PATTERN_UNUSED))
	{
		while(1)
		{
			uart_putchar("Stack_Integrity_Check STACK_BASE_UNUSED fail!\r\n");
		}
	}
	
	if ((REG_PL_RD(STACK_BASE_SVC)!= BOOT_PATTERN_SVC))
	{
		while(1)
		{
			uart_putchar("Stack_Integrity_Check STACK_BASE_SVC fail!\r\n");
		}
	}
	
	if ((REG_PL_RD(STACK_BASE_FIQ)!= BOOT_PATTERN_FIQ))
	{
		while(1)
		{
			uart_putchar("Stack_Integrity_Check STACK_BASE_FIQ fail!\r\n");
		}
	}
	
	if ((REG_PL_RD(STACK_BASE_IRQ)!= BOOT_PATTERN_IRQ))
	{
		while(1)
		{
			uart_putchar("Stack_Integrity_Check STACK_BASE_IRQ fail!\r\n");
		}
	}
	
}
#endif


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

    //fclk_init(FCLK_PWM_ID, RHINO_CONFIG_TICKS_PER_SECOND);

    //hal_init();

    //hw_start_hal();
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
