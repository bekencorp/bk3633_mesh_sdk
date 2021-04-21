#include "genie_flash.h"
#include "uart_pub.h"

void erase_reboot_uart_cmd_handler(char *para)
{
    UART_PRINTF("%s \r\n", __func__);
    genie_flash_erase_userdata();
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
	xtal_cal_set(g_cal_data);

	UART_PRINTF("[%s] g_cal_data = %02X\r\n" ,__func__, g_cal_data);

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
	rf_power_set(g_rf_power_level);
	UART_PRINTF("[%s] g_rf_power_level = %02X\r\n" ,__func__, g_rf_power_level);
}

void app_rf_power_level_save()
{
	genie_flash_write_userdata(GFI_RF_POWER_LEVEL, &g_rf_power_level, sizeof(g_rf_power_level));
	UART_PRINTF("[%s] g_rf_power_level = %02X\r\n" ,__func__, g_rf_power_level);

}


void set_xtal_cal_cmd_handler(char *para)
{
	app_xtal_cal_set(*para);
} 

void save_xtal_cal_cmd_handler(char *para)
{
	app_xtal_cal_save();
} 

void set_rf_power_cmd_handler(char *para)
{
	
	app_rf_power_set(*para);
} 

void save_rf_power_cmd_handler(char *para)
{
	app_rf_power_level_save();
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
		printf("sync send hci cmd error, error code = %X\r\n", err);
       // return err;
    }
	
    return 0;
}
