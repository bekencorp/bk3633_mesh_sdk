#include <stdint.h>        // standard integer definition
#include <string.h>        // string manipulation
#include <stddef.h>     // standard definition
#include "user_config.h"
//#include "uart2.h"
#include "gpio.h"
#include "adc.h"
#include "adc_pub.h"
#include "icu.h"
#include "bk3633_reglist.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "adc_pub.h"
#include "gpio_pub.h"
#include "driver_pub.h"
#include "rwip.h"           // For HW AES accelerator

uint16_t g_adc_value;
volatile uint8_t adc_flag;
uint16_t  referance_voltage = 0xffff;
extern volatile uint32_t XVR_ANALOG_REG_BAK[16];


void adc_isr(void);

UINT32 adc_ctrl(UINT32 cmd, void *param)
{
}


static SDD_OPERATIONS adc_op =
{
    adc_ctrl
};

void adc_exit(void)
{
    sddev_unregister_dev(ADC_DEV_NAME);
}


/************************************************************************
//ADC参考电压默认为1.05V
//确保ADC稳定采样，ADC口需要接个10nf到地的电容
*************************************************************************/
void adc_hw_init(uint8_t channel,uint8_t mode)
{
	uint32_t cfg;
    uint8_t param = GFUNC_MODE_ADC1 + channel - 1;

	//enable adc clk
	SET_SADC_POWER_UP;
	//set special as peripheral func
	//gpio_config(0x30 + channel,SC_FUN,PULL_NONE); 

    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);

	//set adc mode/channel/wait clk
	cfg  = ( (mode << POS_SADC_REG0X0_CFG0_MODE ) 
	       | (channel << POS_SADC_REG0X0_CFG0_CHNL) 
	       | (0x01 << POS_SADC_REG0X0_CFG0_SETING));
	
	cfg |= ((18 << POS_SADC_REG0X0_CFG0_SAMP_RATE) 
	      | (5 << POS_SADC_REG0X0_CFG0_PRE_DIV)
          | (0x0 << POS_SADC_REG0X0_CFG0_ADC_FILTER)
          | (0x01 << POS_SADC_REG0X0_CFG0_INT_CLEAR));
    
    SADC_REG0X0_CFG0=cfg;
	//REG_APB7_ADC_CFG |= (0x01 << BIT_ADC_EN);//不能先使能ADC，不然ADC FIFO满时没有读出再次启动ADC就不会有中断

    SADC_REG0X2_CFG1 = ((1<<POS_SADC_REG0X2_CHANN_EXPAND)|(1<<POS_SADC_REG0X2_STEADY_CTRL));
    SADC_REG0X3_CFG2 = (3<<POS_SADC_REG0X3_STA_CTRL);
    
	SYS_REG0X10_INT_EN |= (0x01 << POS_SYS_REG0X10_INT_EN_ADC);

    if(mode==3)
        SADC_REG0X0_CFG0 |= SET_ADC_EN;
	
}

void adc_init()
{
	uint32_t cfg;

	uint8_t channel = 7;
	uint8_t mode = 1;

	printf("+++++++++%s+++++\n", __func__);
	
	// intc_service_register(IRQ_ADC, PRI_IRQ_ADC, adc_isr);

    sddev_register_dev(ADC_DEV_NAME, &adc_op);
	
    adc_hw_init(7, 1);
}


void adc_deinit(uint8_t channel)
{
    gpio_config(0x30 + channel,INPUT,PULL_HIGH);

    SADC_REG0X0_CFG0 &= ~(SET_ADC_EN+(0x03 << POS_SADC_REG0X0_CFG0_MODE ));
    SYS_REG0X10_INT_EN &= ~(0x01 << POS_SYS_REG0X10_INT_EN_ADC);
    SET_SADC_POWER_DOWN;
}
void adc_isr(void)
{
	SADC_REG0X0_CFG0 |= (0x01 << POS_SADC_REG0X0_CFG0_INT_CLEAR);
	
    adc_flag=1;	
	//printf("+++++++adc_isr++++++++\r\n");
	
    if((SADC_REG0X0_CFG0&0x03)==0x03)
        printf("adc_value=%x\r\n",SADC_REG0X4_DAT);
    
}


extern void Delay_us(int num);
uint16_t adc_get_value(uint8_t channel,uint8_t mode)
{   
    uint16_t adc_cnt;
    adc_cnt=0;
    adc_flag =0;

	//printf("(========%s\r\n", __func__);
    if((SADC_REG0X0_CFG0&0x03)==0x03)
        return 0;
    
    SADC_REG0X0_CFG0 |= SET_ADC_EN+(mode << POS_SADC_REG0X0_CFG0_MODE )+(channel << POS_SADC_REG0X0_CFG0_CHNL);

	    
    while (!adc_flag)  
    {
        adc_cnt++;       
        if(adc_cnt>300)
        {
            break;			
        }
        Delay_us(10);
    } 
    if(adc_flag==1)
    {
        g_adc_value=SADC_REG0X4_DAT>>2;

       // printf("g_adc_value=%d,channel=%x\r\n",g_adc_value,channel);
        
    }
    
    SADC_REG0X0_CFG0 &= ~(SET_ADC_EN+(0x03 << POS_SADC_REG0X0_CFG0_MODE )+(0x0f << POS_SADC_REG0X0_CFG0_CHNL));
	
	//printf("%s-----)\r\n", __func__);
    return g_adc_value;     
}

/**************************************************************************
//注意确保转换值的稳定性，ADC口需要加个10nf到地的电容
//ADC校准
//校准ADC需要给芯片一个稳定的供电压，然后算ADC参考电压
//这个函数校准默认使用3V电源供电,内部分压后为0.75V
*************************************************************************/
#define CALIB_COUNT 1
#define STABL_VALT 75///分压后的0.75V稳定电压


#define ADC_2200_mV 2280

#define REG_IP_RD(addr)              (*(volatile uint32_t *)(addr))
#define REG_IP_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)


static void ip_deepslcntl_set(uint32_t value)
{
    REG_IP_WR(0x00820030, value);
}


#define REG_AHB10_RW_DEEPSLCNTL     (*((volatile unsigned long *)   0x00820030))
#define REG_AHB10_RW_DEEPSLTIME     (*((volatile unsigned long *)   0x00820034))
#define REG_AHB10_RW_DEEPSLDUR     (*((volatile unsigned long *)    0x00820038))
#define REG_AHB10_RW_ENBPRESET     (*((volatile unsigned long *)    0x0082003C))
#define REG_AHB10_RW_FINECNTCORR     (*((volatile unsigned long *)  0x00820040))
#define REG_AHB10_RW_BASETIMECNTCORR    (*((volatile unsigned long *)   0x00820044))

#define RW_ENBPRESET_TWEXT_bit     21
#define RW_ENBPRESET_TWOSC_bit     10
#define RW_ENBPRESET_TWRW_bit     0



void rw_sleep(void)
{

 //goto sleep

 clrf_SYS_Reg0x3_rwbt_pwd;//open rw clk
    setf_SYS_Reg0xd_OSC_en_sel;

 set_SYS_Reg0x1e_rfu(0xf);
 REG_AHB10_RW_DEEPSLTIME=0x00000;//sleep time ;finite
    REG_AHB10_RW_ENBPRESET = (0x40<<RW_ENBPRESET_TWEXT_bit)|
    (0x40<<RW_ENBPRESET_TWOSC_bit)|(0x40<<RW_ENBPRESET_TWRW_bit);
    REG_AHB10_RW_DEEPSLCNTL=0x00000007;//BLE sleep
}



#define LOW_VOlTAGE 2280 //2.2 mV
void check_low_volt_sleep(void)
{
#if(ADC_DRIVER) 
    uint8_t i;
    float calib_temp=0;
    
    XVR_ANALOG_REG_BAK[7] |= (1<<19);
    addXVR_Reg0x7 = XVR_ANALOG_REG_BAK[7];
	
    adc_hw_init(7, 1);

    for(i=0;i<CALIB_COUNT;i++)
    {
        calib_temp += adc_get_value(7,1);
    }
    
    referance_voltage= (uint16)((calib_temp * 1000 /CALIB_COUNT) /256 * 1.05 * 4);
    printf("referance_voltage=%d mV\r\n",referance_voltage);

    XVR_ANALOG_REG_BAK[7] &= ~(1<<19);
    addXVR_Reg0x7 = XVR_ANALOG_REG_BAK[7];

	if(referance_voltage < 2100 && referance_voltage != 0)
	{
	
		addSYS_Reg0x3 = 0xfffffff;

		set_PMU_Reg0x4_gotosleep(0x3633);
		while(1);

	}
    
#endif
}

