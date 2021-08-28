#include "genie_flash.h"
#include "uart_pub.h"

#include "JX_app.h"

#define ARRAY_LEN(array)   (sizeof((array))/sizeof((array)[0]))

u8 ATE_mode =0;

uint8_t AT_S_MODE_IN[] ="AT+S_MODE=1892_AEBF\r\n";
uint8_t AT_S_MODE_OUT[] ="AT+S_MODE=1289_ABEF\r\n";
uint8_t AT_S_MODE_OK[] ="AT+S_MODE=OK\r\n";

uint8_t AT_RESET[] ="AT+RESET\r\n";
uint8_t AT_RESET_OK[] ="AT+RESET=OK\r\n";

uint8_t AT_VERSION[] ="AT+VERSION?\r\n";
uint8_t AT_VERSION_OK[] ="AT+VERSION=01.02\r\n";

uint8_t AT_CHECKSUM[] ="AT+CHECKSUM?\r\n";
uint8_t AT_CHECKSUM_OK[] ="AT+CHECKSUM=8EAF2C2E\r\n";

uint8_t AT_MAC[] ="AT+MAC?\r\n";
uint8_t AT_MAC_ADDR[] ="AT+MAC=0123456789AB\r\n";

uint8_t AT_MAC_WR[] ="AT+MAC=";
uint8_t AT_MAC_WR_OK[] ="AT+MAC=OK\r\n";

uint8_t AT_TRI_TUPLE[] ="AT+TRI_TUPLE?\r\n";
uint8_t AT_TRI_TUPLE_ADDR[] ="AT+TRI_TUPLE=31323334.0123456789AB.00112233445566778899AABBCCDDEEFF\r\n";

uint8_t AT_TRI_TUPLE_WR[] ="AT+TRI_TUPLE=";
uint8_t AT_TRI_TUPLE_WR_OK[] ="AT+TRI_TUPLE=OK\r\n";

uint8_t AT_FOUR_TUPLE[] ="AT+FOUR_TUPLE?\r\n";
uint8_t AT_FOUR_TUPLE_ADDR[] ="AT+FOUR_TUPLE=31323334.0123456789AB.00112233445566778899AABBCCDDEEFF.0011\r\n";

uint8_t AT_FOUR_TUPLE_WR[] ="AT+FOUR_TUPLE=";
uint8_t AT_FOUR_TUPLE_WR_OK[] ="AT+FOUR_TUPLE=OK\r\n";

uint8_t AT_SLEEP[] ="AT+MODE=SLEEP\r\n";
uint8_t AT_SLEEP_OK[] ="AT+MODE=OK\r\n";

uint8_t AT_GPIO[] ="AT+GPIO=";
uint8_t AT_GPIO_OK[] ="AT+GPIO=OK\r\n";			//P02 IO?=1(P02?P03?P04?P05?P06?P07?P31?P32?P10?P11?P12?P13?P06?P33)

uint8_t AT_FREQ_SET[] ="AT+FREQ_SET=";
uint8_t AT_FREQ_SET_OK[] ="AT+FREQ_SET=OK\r\n";

uint8_t AT_FREQ_SAVE[] ="AT+FREQ_SAVE=";
uint8_t AT_FREQ_SAVE_OK[] ="AT+FREQ_SAVE=OK\r\n";

uint8_t AT_PWR_SET[] ="AT+PWR_SET=";
uint8_t AT_PWR_SET_OK[] ="AT+PWR_SET=OK\r\n";

uint8_t AT_PWR_SAVE[] ="AT+PWR_SAVE=";
uint8_t AT_PWR_SAVE_OK[] ="AT+PWR_SAVE=OK\r\n";

uint8_t AT_SINGLE_WAVE_ON[] ="AT+SINGLE_WAVE_ON=";
uint8_t AT_SINGLE_WAVE_ON_OK[] ="AT+SINGLE_WAVE_ON=OK\r\n";

uint8_t AT_SINGLE_WAVE_OFF[] ="AT+SINGLE_WAVE_OFF";
uint8_t AT_SINGLE_WAVE_OFF_OK[] ="AT+SINGLE_WAVE_OFF=OK\r\n";

void uart_check_func(uint8_t *pBuffer, uint8_t len);
int Convert_Dec_To_Ascii(u8 *AsciiData, int Length, int ConvertData);
int Convert_Hex_To_Ascii(u8 *AsciiData, int Length, u32 ConvertData);
u32 Convert_Buffer_To_Hex(u8 *ConvertData, int Length);
u32 Convert_Buffer_To_Dec(u8 *ConvertData, int Length);
u8 *Buffer_Part_Cmp(u8 *Buffer1, int Length1, u8 *Buffer2, int Length2);

void erase_reboot_uart_cmd_handler(char *para)
{
	u8 i;

    UART_PRINTF("%s \r\n", __func__);
    //genie_flash_erase_userdata();
    genie_flash_erase_reliable();
    genie_flash_delete_seq();

    aos_reboot();
}

void reboot_uart_cmd_handler(char *para)
{
    UART_PRINTF("%s \r\n", __func__);
    aos_reboot();
}

void lpn_set_uart_cmd_handler(char *para)
{
    UART_PRINTF("+++ %s +++\n", __func__);
    //bt_mesh_lpn_set(true);
}

uint8_t g_cal_data = 0;
void app_xtal_cal_set(uint8_t cal_data)
{
	if(cal_data>0x7f)
	{
		g_cal_data = 0x7f;
	}
	else
	{
		g_cal_data = cal_data;
	}

	xtal_cal_set(g_cal_data);

	UART_PRINTF("[%s] g_cal_data = %02X\r\n" ,__func__, g_cal_data);
}

void app_xtal_cal_save()
{
	genie_flash_write_userdata(GFI_XTAL_CAL, &g_cal_data, sizeof(g_cal_data));
	UART_PRINTF("[%s] g_cal_data = %02X\r\n" ,__func__, g_cal_data);
}

void app_xtal_cal_init(void)
{
	genie_flash_read_userdata(GFI_XTAL_CAL, &g_cal_data, sizeof(g_cal_data));
	// UART_PRINTF("[%s] g_cal_data = %02X\r\n" ,__func__, g_cal_data);

	if(g_cal_data==0)
	{
		g_cal_data =80;
	}
//	g_cal_data =80;
	xtal_cal_set(g_cal_data);

	// UART_PRINTF("[%s] g_cal_data = %02X\r\n" ,__func__, g_cal_data);
}

uint8_t g_rf_power_level = 0;

void app_rf_power_set(uint8_t power_level)
{
	if(power_level> 0x0F)
	{
		g_rf_power_level = 0x0F;
	}
	else
	{
		g_rf_power_level = 	power_level;
	}

	rf_power_set(power_level);
	UART_PRINTF("[%s] g_rf_power_level = %02X\r\n" ,__func__, g_rf_power_level);
}

void app_rf_power_init(void)
{
	genie_flash_read_userdata(GFI_RF_POWER_LEVEL, &g_rf_power_level, sizeof(g_rf_power_level));

	// UART_PRINTF("[%s] g_rf_power_level = %02X\r\n" ,__func__, g_rf_power_level);
	if(g_rf_power_level==0)
	{
		g_rf_power_level =8;
	}
//	g_rf_power_level =2;

	rf_power_set(g_rf_power_level);
	// UART_PRINTF("[%s] g_rf_power_level = %02X\r\n" ,__func__, g_rf_power_level);
}

void app_rf_power_level_save(void)
{
	genie_flash_write_userdata(GFI_RF_POWER_LEVEL, &g_rf_power_level, sizeof(g_rf_power_level));
	UART_PRINTF("[%s] g_rf_power_level = %02X\r\n" ,__func__, g_rf_power_level);
}

void set_xtal_cal_cmd_handler(char *para)
{

	uint8_t cal;
	ch_str_2_u8(para, &cal);
	app_xtal_cal_set(cal);
}

void save_xtal_cal_cmd_handler(char *para)
{
	app_xtal_cal_save();
}

void set_rf_power_cmd_handler(char *para)
{
	uint8_t power_level;
	ch_str_2_u8(para, &power_level);
	
	app_rf_power_set(power_level);
}

void save_rf_power_cmd_handler(char *para)
{
	app_rf_power_level_save();
}

#ifdef CONFIG_DUT_TEST_CMD
void rf_fcc_tx_test_cmd_handler(char *para)
{
	uint8_t k;
	ch_str_2_u8(para, &k);
	rf_fcc_tx_test(k);
}

void rf_fcc_test_stop_cmd_handler(char *para)
{
	rf_fcc_tx_test_stop();
}
#endif

void printf_rf_xvr_handler(char *para)
{
	print_xvf();
}

int uart_rx_dut_cmd(uint8_t *data, uint8_t len)
{
	uint16_t opcode;
	int err;
    struct net_buf *buf;

	uint8_t *p_hci_data;

	//recieve hci cmd
	if((data[0] != 1) || (len < 4) || (data[3] != (len - 4)))
	{
		return -1;
	}

	//k_sleep(10);

	opcode = (uint16_t)(data[2] << 8) + (uint16_t)data[1]; 

	buf = bt_hci_cmd_create(opcode, data[3]);

	p_hci_data = net_buf_add(buf, data[3]);

	memcpy(p_hci_data, &data[4], data[3]); 

	printf("send hci cmd: opcode  = 0x%04X, param: ", opcode);

	for(int i = 2; i < buf->len; i++)
	{
		printf("%02X ", buf->data[i]);
	}
	printf("\r\n");

	
    err = bt_hci_cmd_send_sync(opcode, buf, NULL); 
    if (err) {
		printf("uart_rx_dut_cmd: bt_hci_cmd_send_sync, error code = %X\r\n", err);
       // return err;
    }
	
    return 0;
}

void uart2_send(u8 *pBuffer, u8 len)
{
	u8 i;

	for(i=0; i<len; i++)
	{
		bk_send_byte(UART2_PORT, pBuffer[i]);
	}
}

//AT cmd
void uart_check_func(uint8_t *pBuffer, uint8_t len)
{
	u8 i;
	u8 temp =0;

	u8 pos =0;
	u8 value =0;
	u8 pin_num =0;

	u8 respone_buff[32];

    printf("pBuffer:%s len:%d \r\n", pBuffer, len);
	if(Buffer_Part_Cmp(pBuffer, len, AT_S_MODE_IN, ARRAY_LEN(AT_S_MODE_IN)) !=NULL)
	{
		printf("%s %d \r\n", __func__, __LINE__);
		ATE_mode =1;
		uart2_send(AT_S_MODE_OK, ARRAY_LEN(AT_S_MODE_OK));
		if(get_dut_flag()==0)
		{
			icu_set_reset_reason(REG_ATE);
			k_sleep(50);
			aos_reboot();
		}
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_S_MODE_OUT, ARRAY_LEN(AT_S_MODE_OUT)) !=NULL)
	{
		printf("%s %d \r\n", __func__, __LINE__);
		ATE_mode =0;
		uart2_send(AT_S_MODE_OK, ARRAY_LEN(AT_S_MODE_OK));
	}
	printf("%s %d ATE_mode:%d\r\n", __func__, __LINE__, ATE_mode);

	if(ATE_mode ==0)
		return;

	if(Buffer_Part_Cmp(pBuffer, len, AT_RESET, ARRAY_LEN(AT_RESET)) !=NULL)
	{
		uart2_send(AT_RESET_OK, ARRAY_LEN(AT_RESET_OK));

	    aos_reboot();
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_VERSION, ARRAY_LEN(AT_VERSION)) !=NULL)
	{
		temp =(u8)(PROJECT_SW_VERSION>>8);
		Convert_Hex_To_Ascii(AT_VERSION_OK+11, 2, temp);

		temp =(u8)(PROJECT_SW_VERSION>>0);
		Convert_Hex_To_Ascii(AT_VERSION_OK+14, 2, temp);

		uart2_send(AT_VERSION_OK, ARRAY_LEN(AT_VERSION_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_CHECKSUM, ARRAY_LEN(AT_CHECKSUM)) !=NULL)
	{
		u8 checksum[4];

		genie_flash_read_checksum(checksum);

		pos =12;
		for(i=0; i<4; i++)
		{
			temp =checksum[3-i];
			pos +=Convert_Hex_To_Ascii(AT_CHECKSUM_OK+pos, 2, temp);
		}
		uart2_send(AT_CHECKSUM_OK, ARRAY_LEN(AT_CHECKSUM_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_MAC, ARRAY_LEN(AT_MAC)) !=NULL)
	{
		u8 pid[4];
		u8 mac[6];
		u8 key[16];
		u8 cid[2];

		genie_flash_read_fourtuple(pid, mac, key, cid);

		pos =7;
		for(i=0; i<6; i++)
		{
			temp =mac[5-i];
			pos +=Convert_Hex_To_Ascii(AT_MAC_ADDR+pos, 2, temp);
		}
		uart2_send(AT_MAC_ADDR, ARRAY_LEN(AT_MAC_ADDR));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_MAC_WR, ARRAY_LEN(AT_MAC_WR)) !=NULL)
	{
		u8 pid[4];
		u8 mac[6];
		u8 key[16];
		u8 cid[2];

		genie_flash_read_fourtuple(pid, mac, key, cid);

		pos =ARRAY_LEN(AT_MAC_WR);
//		printf("len:%d pos:%d data:%x\r\n", len, pos, pBuffer[pos]);
		if(len < pos + 12)
		{
			return;
		}

		for(i=0; i<6; i++)
		{
			mac[5-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}

		genie_flash_write_fourtuple(pid, mac, key, cid);
		uart2_send(AT_MAC_WR_OK, ARRAY_LEN(AT_MAC_WR_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_TRI_TUPLE, ARRAY_LEN(AT_TRI_TUPLE)) !=NULL)
	{
		u8 pid[4];
		u8 mac[6];
		u8 key[16];

		genie_flash_read_trituple(pid, mac, key);

		pos =13;
		for(i=0; i<4; i++)
		{
			temp =pid[3-i];
			pos +=Convert_Hex_To_Ascii(AT_TRI_TUPLE_ADDR+pos, 2, temp);
		}
		AT_TRI_TUPLE_ADDR[pos++] ='.';

		for(i=0; i<6; i++)
		{
			temp =mac[5-i];
			pos +=Convert_Hex_To_Ascii(AT_TRI_TUPLE_ADDR+pos, 2, temp);
		}
		AT_TRI_TUPLE_ADDR[pos++] ='.';

		for(i=0; i<16; i++)
		{
			temp =key[15-i];
			pos +=Convert_Hex_To_Ascii(AT_TRI_TUPLE_ADDR+pos, 2, temp);
		}

		uart2_send(AT_TRI_TUPLE_ADDR, ARRAY_LEN(AT_TRI_TUPLE_ADDR));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_TRI_TUPLE_WR, ARRAY_LEN(AT_TRI_TUPLE_WR)) !=NULL)
	{
		u8 pid[4];
		u8 mac[6];
		u8 key[16];

		pos =ARRAY_LEN(AT_TRI_TUPLE_WR);
//		printf("len:%d pos:%d data:%x\r\n", len, pos, pBuffer[pos]);
		if(len < pos + 8+12+32)
		{
			return;
		}

		for(i=0; i<4; i++)
		{
			pid[3-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}
		pos ++;

		for(i=0; i<6; i++)
		{
			mac[5-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}
		pos ++;

		for(i=0; i<16; i++)
		{
			key[15-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}

		genie_flash_write_trituple(pid, mac, key);
		uart2_send(AT_TRI_TUPLE_WR_OK, ARRAY_LEN(AT_TRI_TUPLE_WR_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_FOUR_TUPLE, ARRAY_LEN(AT_FOUR_TUPLE)) !=NULL)
	{
		u8 pid[4];
		u8 mac[6];
		u8 key[16];
		u8 cid[2];

		genie_flash_read_fourtuple(pid, mac, key, cid);

		pos =14;
		for(i=0; i<4; i++)
		{
			temp =pid[3-i];
			pos +=Convert_Hex_To_Ascii(AT_FOUR_TUPLE_ADDR+pos, 2, temp);
		}
		AT_FOUR_TUPLE_ADDR[pos++] ='.';

		for(i=0; i<6; i++)
		{
			temp =mac[5-i];
			pos +=Convert_Hex_To_Ascii(AT_FOUR_TUPLE_ADDR+pos, 2, temp);
		}
		AT_FOUR_TUPLE_ADDR[pos++] ='.';

		for(i=0; i<16; i++)
		{
			temp =key[15-i];
			pos +=Convert_Hex_To_Ascii(AT_FOUR_TUPLE_ADDR+pos, 2, temp);
		}
		AT_FOUR_TUPLE_ADDR[pos++] ='.';

		for(i=0; i<2; i++)
		{
			temp =cid[1-i];
			pos +=Convert_Hex_To_Ascii(AT_FOUR_TUPLE_ADDR+pos, 2, temp);
		}

		uart2_send(AT_FOUR_TUPLE_ADDR, ARRAY_LEN(AT_FOUR_TUPLE_ADDR));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_FOUR_TUPLE_WR, ARRAY_LEN(AT_FOUR_TUPLE_WR)) !=NULL)
	{
		u8 pid[4];
		u8 mac[6];
		u8 key[16];
		u8 cid[2];

		pos =ARRAY_LEN(AT_FOUR_TUPLE_WR);
//		printf("len:%d pos:%d data:%x\r\n", len, pos, pBuffer[pos]);
		if(len < pos + 8+12+32+4)
		{
			return;
		}

		for(i=0; i<4; i++)
		{
			pid[3-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}
		pos ++;

		for(i=0; i<6; i++)
		{
			mac[5-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}
		pos ++;

		for(i=0; i<16; i++)
		{
			key[15-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}
		pos ++;

		for(i=0; i<2; i++)
		{
			cid[1-i] =(u8)Convert_Buffer_To_Hex(pBuffer +pos, 2);
			pos +=2;
		}

		genie_flash_write_fourtuple(pid, mac, key, cid);
		uart2_send(AT_FOUR_TUPLE_WR_OK, ARRAY_LEN(AT_FOUR_TUPLE_WR_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_SLEEP, ARRAY_LEN(AT_SLEEP)) !=NULL)
	{
		uart2_send(AT_SLEEP_OK, ARRAY_LEN(AT_SLEEP_OK));

//		deep_sleep();
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_GPIO, ARRAY_LEN(AT_GPIO)) !=NULL)
	{
		gpio_dev_t gpio_temp;

		pos =ARRAY_LEN(AT_GPIO);
		if(len < pos +5)
		{
			return;
		}

		pBuffer +=pos;
		pBuffer ++;			//'P'

		pin_num =(pBuffer[0] -'0')<<4;
		pin_num +=pBuffer[1] -'0';

		value =pBuffer[3];
		value =value =='1'? 1: 0;
		printf("%s %d pbuffer:%s value:%x \r\n", __func__, __LINE__, pBuffer, value);

		switch(pin_num)
		{
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
				gpio_temp.port =  pin_num;
				gpio_temp.config = OUTPUT_PUSH_PULL;
				hal_gpio_init(&gpio_temp);
				if(value)
				{
					hal_gpio_output_high(&gpio_temp);
				}
				else
				{
					hal_gpio_output_low(&gpio_temp);
				}

				uart2_send(AT_GPIO_OK, ARRAY_LEN(AT_GPIO_OK));
				break;
			default:
				break;
		}
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_FREQ_SET, ARRAY_LEN(AT_FREQ_SET)) !=NULL)
	{
		uint8_t freq_value =0;

		pos =ARRAY_LEN(AT_FREQ_SET);
		pBuffer +=pos;

		pos =0;
		value =0;
		while(pBuffer[0] >='0' && pBuffer[0] <='9' && pos <=2)
		{
			value *=10;
			value +=pBuffer[0] -'0';
			pBuffer++;
			pos ++;
		}

		if(value <2 && value >80)
		{
			value =2;
		}
		freq_value =value;

		rf_fcc_test_stop_cmd_handler(NULL);
		app_xtal_cal_set(freq_value);
		uart2_send(AT_FREQ_SET_OK, ARRAY_LEN(AT_FREQ_SET_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_FREQ_SAVE, ARRAY_LEN(AT_FREQ_SAVE)) !=NULL)
	{
		uint8_t freq_value =0;

		pos =ARRAY_LEN(AT_FREQ_SAVE);
		pBuffer +=pos;

		pos =0;
		value =0;
		while(pBuffer[0] >='0' && pBuffer[0] <='9' && pos <=2)
		{
			value *=10;
			value +=pBuffer[0] -'0';
			pBuffer++;
			pos ++;
		}

		if(value <2 && value >80)
		{
			value =2;
		}
		freq_value =value;

		printf("freq_value %d\r\n", freq_value);
		g_cal_data =freq_value;
		rf_fcc_test_stop_cmd_handler(NULL);
		app_xtal_cal_save();

		uart2_send(AT_FREQ_SAVE_OK, ARRAY_LEN(AT_FREQ_SAVE_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_PWR_SET, ARRAY_LEN(AT_PWR_SET)) !=NULL)
	{
		uint8_t pwr_value =0;

		pos =ARRAY_LEN(AT_PWR_SET);
		pBuffer +=pos;

		pos =0;
		value =0;
		while(pBuffer[0] >='0' && pBuffer[0] <='9' && pos <=2)
		{
			value *=10;
			value +=pBuffer[0] -'0';
			pBuffer++;
			pos ++;
		}

		if(value <1 && value >15)
		{
			value =1;
		}
		pwr_value =value;

		rf_fcc_test_stop_cmd_handler(NULL);
		app_rf_power_set(pwr_value);

		uart2_send(AT_PWR_SET_OK, ARRAY_LEN(AT_PWR_SET_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_PWR_SAVE, ARRAY_LEN(AT_PWR_SAVE)) !=NULL)
	{
		uint8_t pwr_value =0;

		pos =ARRAY_LEN(AT_PWR_SAVE);
		pBuffer +=pos;

		pos =0;
		value =0;
		while(pBuffer[0] >='0' && pBuffer[0] <='9' && pos <=2)
		{
			value *=10;
			value +=pBuffer[0] -'0';
			pBuffer++;
			pos ++;
		}

		if(value <1 && value >15)
		{
			value =1;
		}
		pwr_value =value;


		g_rf_power_level =pwr_value;
		rf_fcc_test_stop_cmd_handler(NULL);
		app_rf_power_level_save();

		uart2_send(AT_PWR_SAVE_OK, ARRAY_LEN(AT_PWR_SAVE_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_SINGLE_WAVE_ON, ARRAY_LEN(AT_SINGLE_WAVE_ON)) !=NULL)
	{
		uint8_t freq_value =0;

		pos =ARRAY_LEN(AT_SINGLE_WAVE_ON);
		pBuffer +=pos;

		pos =0;
		value =0;
		while(pBuffer[0] >='0' && pBuffer[0] <='9' && pos <=2)
		{
			value *=10;
			value +=pBuffer[0] -'0';
			pBuffer++;
			pos ++;
		}

		if(value >39)
		{
			value =39;
		}
		freq_value =value;

		rf_fcc_tx_test_cmd_handler(&freq_value);

		uart2_send(AT_SINGLE_WAVE_ON_OK, ARRAY_LEN(AT_SINGLE_WAVE_ON_OK));
	}
	else if(Buffer_Part_Cmp(pBuffer, len, AT_SINGLE_WAVE_OFF, ARRAY_LEN(AT_SINGLE_WAVE_OFF)) !=NULL)
	{
		rf_fcc_test_stop_cmd_handler(NULL);

		uart2_send(AT_SINGLE_WAVE_OFF_OK, ARRAY_LEN(AT_SINGLE_WAVE_ON_OK));
	}
	else
	{

	}
}


int Convert_Dec_To_Ascii(u8 *AsciiData, int Length, int ConvertData)
{
	int i;
	u8 ConvertDataTemp;

	u8 Temp =0;

	if(ConvertData < 0)
	{
		Temp =1;
		ConvertData =fabs(ConvertData);
	}

	for(i=0; i<Length; i++)
	{
		ConvertDataTemp =ConvertData %10;
		AsciiData[i] =ConvertDataTemp +'0';
		ConvertData /=10;
	}

	if(Temp)
	{
		AsciiData[0] ='-';
		AsciiData ++;
	}

	for(i=0; i<Length/2; i++)
	{
		ConvertDataTemp =AsciiData[i];
		AsciiData[i] =AsciiData[Length-1-i];
		AsciiData[Length-1-i] =ConvertDataTemp;
	}

	return Length;
}


int Convert_Hex_To_Ascii(u8 *AsciiData, int Length, u32 ConvertData)
{
	int i;
	u8 ConvertDataTemp;

	for(i=0; i<Length; i++)
	{
		ConvertDataTemp =ConvertData %16;
		if(ConvertDataTemp<0x0A)
		{
			AsciiData[i] =ConvertDataTemp +'0';
		}
		else
		{
			AsciiData[i] =ConvertDataTemp +'A'-10;
		}
		ConvertData /=16;
	}
	for(i=0; i<Length/2; i++)
	{
		ConvertDataTemp =AsciiData[i];
		AsciiData[i] =AsciiData[Length -i -1];
		AsciiData[Length -i -1] =ConvertDataTemp;
	}

	return Length;
}


u32 Convert_Buffer_To_Hex(u8 *ConvertData, int Length)
{
	int i;
	u32 HexData =0x00;
	int LengthTemp =0;

	LengthTemp =ARRAY_LEN(ConvertData);
//	printf("Convert_Buffer_To_Hex:%d length:%d\r\n", LengthTemp, Length);
	if(LengthTemp < Length)
	{
		Length =LengthTemp;
	}

	for(i=0; i<Length; i++)
	{
//		printf("Convert_Buffer_To_Hex:i:%d data:%x\r\n", i, ConvertData[i]);
		HexData <<=4;
		if(ConvertData[i] >='0' && ConvertData[i] <='9')
		{
			HexData +=ConvertData[i] -'0';
		}
		else if(ConvertData[i] >='a' && ConvertData[i] <='f')
		{
			HexData +=ConvertData[i] -'a'+10;
		}
		else if(ConvertData[i] >='A' && ConvertData[i] <='F')
		{
			HexData +=ConvertData[i] -'A'+10;
		}
	}
	return HexData;
}


u32 Convert_Buffer_To_Dec(u8 *ConvertData, int Length)
{
	int i;
	u32 DecData =0;
	int LengthTemp =0;

	LengthTemp =strlen((const char *)ConvertData);
	if(LengthTemp < Length)
	{
		Length =LengthTemp;
	}

	for(i=0; i<Length; i++)
	{
		if(ConvertData[i] >='0' && ConvertData[i] <='9')
		{
			DecData *=10;
			DecData +=ConvertData[i] -'0';
		}
		else
		{
			break;
		}
	}
	return DecData;
}


u8 *Buffer_Part_Cmp(u8 *Buffer1, int Length1, u8 *Buffer2, int Length2)
{
	int i,j=0;

	for(i=0; i<Length1; i++)
	{
		if(Buffer2[0] ==Buffer1[i])
		{
			if(Length1 -i <Length2)
			{
				return NULL;
			}

			for(j=1; j<Length2; j++)
			{
				if(Buffer2[0+j] !=Buffer1[i+j])
				{
					break;
				}
			}

			if(j == Length2)
			{
				return Buffer1+i;
			}
		}
	}

	return NULL;
}

