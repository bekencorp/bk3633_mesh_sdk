#include <stdint.h>   
#include "BK3633_RegList.h"
#include "gpio_pub.h"
#include "deep_sleep.h"

#ifdef __DEEP_SLEEP__ 

void cpu_delay( volatile unsigned int times)
{
    while(times--) ;
}

/*****************
 *  void mem_is_enable_check_bypass(bool val)
 * val:
 * 	1: decrease reboot time.
 *  0: normal reboot time
 * result:
 * 	not reboot from rom: 21.07ms    
 *  reboot from rom: 35.96ms
 *  normal wake up: 102.41ms
 * 
 * *******************************************************/
void mem_is_enable_check_bypass(bool val)
{
	set_PMU_Reg0x1_mchk_bypass(val);
}

/*****************
 *  deep_sleep_wake_by_gpio()
 * reg example use:
 * 	addAON_GPIO_Reg0x3 = 0x3C;	//input pull up
 *  addPMU_Reg0x3 = 0x08;	// gpio03 deep wake enalbe
 * 
 * *******************************************************/
void deep_sleep_wake_by_gpio(uint32_t gpio_idx, gpio_trigger_t trigger)
{
	GPIO_INT_ST int_struct;

	int_struct.id = gpio_idx;
	int_struct.mode = trigger;
	int_struct.phandler = NULL;

	bk_gpio_enable_irq(&int_struct);
}

#if 0	//this api is not ready for used.
void deep_sleep_wake_by_timer(void)
{
	int temp_number;

	setf_PMU_Reg0x1_mchk_bypass;//bypass memcheck
	set_SYS_Reg0x10_int_aon_rtc_en(0x1);//int enable
	
	//timer set	
	//setf_rtc_aon_Reg0x0_rtc_cnt_reset;//reset
	cpu_delay( 100 ) ;
	//clrf_rtc_aon_Reg0x0_rtc_cnt_reset;//clear reset
	addrtc_aon_Reg0x1 = 1400;//up
	addrtc_aon_Reg0x2 = 5000;//tick
	cpu_delay( 100 ) ;
	addrtc_aon_Reg0x0 |= 0x40;//open clk
	//setf_rtc_aon_Reg0x0_rtc_tick_int_en;
	setf_rtc_aon_Reg0x0_rtc_aon_int_en;
	setf_SYS_Reg0x10_int_aon_gpio_en;
	clrf_rtc_aon_Reg0x0_rtc_cnt_stop;
	temp_number = addPMU_Reg0x2;
	temp_number++;
	addPMU_Reg0x2 = temp_number;
//	addPMU_Reg0x4 = 0x0100;//deep wake timer enalbe
	if(temp_number>=6)
	{
		addPMU_Reg0x4 = 0x0100;
	}
	else if(temp_number>=4)
	{
		addPMU_Reg0x4 = 0x0000;
	}
	else
	{
    	addPMU_Reg0x4 = 0x0100;//deep wake timer enalbe
	}	
}
#endif

void DeepSleep_Enable(void)
{  	
    // Write 0x3633 to Deep Sleep Word Register to enable deep-sleep 
    set_PMU_Reg0x4_gotosleep(0x3633);
}

void deep_sleep(void)
{
	addXVR_Reg0x6 = 0x85A7CC00;
	cpu_delay( 1000 ) ;		
		
	addXVR_Reg0x7 = 0xAA023FC0;		
	cpu_delay( 1000 ) ;			

	addXVR_Reg0xa = 0x9C04785F;	
	cpu_delay( 1000 ) ;		
		
	addXVR_Reg0x1c= 0x919CDDC5;
	cpu_delay( 1000 ) ;			

	printf("go to deep sleep .. \r\n");			
	DeepSleep_Enable();
}

/***************  example test ************************
 * 
   <project>.mk   for example: light.mk
	deep_sleep = 1
	//define "deep_sleep" to use deep sleep function.
 * 
    mem_is_enable_check_bypass(1);	//use this api while want to decrease reboot time.
    deep_sleep_wake_by_gpio(GPIO_P03, GPIO_TRIGGER_RISING_EDGE);	//deep sleep wake up by gpio.
    deep_sleep();	//for deep sleep.
*
**********************************************************/

#endif


