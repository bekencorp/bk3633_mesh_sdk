/* main.c - light demo */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef CONFIG_DUT_TEST_CMD


#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include <hal/soc/gpio.h>
#include <hal/soc/pwm.h>
#include "inc/time_scene_server.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif

#include "light_board.h"
#include "uart_test_cmd.h"
#include "intc_pub.h"

#include "JX_app.h"

volatile uint8_t g_dut_flag = 0;

uint8_t g_fcc_testing = 0;

static cpu_stack_t *uart_dut_task;//[UART_TEST_TASK_STACK/sizeof(cpu_stack_t)];
static ktask_t uart_dut_task_obj;

extern volatile DUT_TEST_BUG_T *gp_dut_data;

//void check_and_set_dut_flag(void)
//{
//	gpio_dev_t gpio;
//	uint32_t gpio02_val;
//	gpio.port =  GPIO_P02;
//	gpio.config = INPUT_PULL_DOWN;
//	hal_gpio_init(&gpio);
//
//	hal_gpio_input_get(&gpio, &gpio02_val);
//
//    if(1 == gpio02_val)
//    {
//    	g_dut_flag= 1;
//    }
//	else
//	{
//		g_dut_flag = 0;
//	}
//}

extern u8 ATE_mode;
uint8_t get_ATE_flag(void)
{
	if(icu_get_reset_reason() == REG_ATE)
	{
    	g_dut_flag= 1;
		ATE_mode =1;
		icu_set_reset_reason(REG_NONE);

	}
	else
	{
    	g_dut_flag= 0;
		ATE_mode =0;
	}

	// printf("%s %d flag:%d \r\n", __func__, __LINE__, g_dut_flag);

	return g_dut_flag;
}

uint8_t get_fcc_testing_flg()
{
    return g_fcc_testing;
}

uint8_t set_fcc_testing_flg(uint8_t flg)
{
	g_fcc_testing = flg;
}

void dut_init(void)
{
    int ret;
    //add genie app init func
    genie_flash_init();

    hci_driver_init();

    ret = bt_enable(NULL);
    if (ret) {
        BT_INFO("init err %d", ret);
    }
}

static void uart_dut_main(void)
{

	while(1)
	{

		hal_aon_wdt_feed();

		if(gp_dut_data->dut_len != 0)
		{
			//the cmd too long
			if(gp_dut_data->dut_len > HCI_DATA_LEN)
			{
				bk_printf("uart2 cmd too long\r\n");
			}
			else
			{
				//hci cmd
				if (uart_rx_dut_cmd(&(gp_dut_data->dut_data[0]), gp_dut_data->dut_len) == 0)
				{
				}
				else
				{
					printf("uart2 recved unkown data: ");
					
					for(uint8_t i = 0; i < gp_dut_data->dut_len; i++)
					{
						printf("%02X ", gp_dut_data->dut_data[i]);
					}
					printf("\r\n");
				}
			}
			gp_dut_data->dut_len = 0;
		    memset(gp_dut_data->dut_data, 0, HCI_DATA_LEN);
		}
	}
}

int uart_dut_task_create(void)
{
   uart_dut_task = (uint32_t *)aos_malloc(UART_TEST_TASK_STACK);
   return krhino_task_create(&uart_dut_task_obj, "uart_dut_cmd", NULL,
    		                 UART_DUT_TASK_PRIO, 0, uart_dut_task,
    		                 UART_TEST_TASK_STACK / sizeof(cpu_stack_t),
                             (task_entry_t)uart_dut_main, 1);
}

int uart_dut_init(void)
{
    int ret;

	printf("%s, %d\r\n", __func__, __LINE__);

    gp_dut_data = (struct DUT_TEST_BUG_T *)aos_malloc(sizeof(DUT_TEST_BUG_T));

    if (gp_dut_data == NULL) {
        return ENOMEM;
    }

    memset((struct DUT_TEST_BUG_T *)gp_dut_data, 0, sizeof(DUT_TEST_BUG_T));

    UART_PRINTF("%s, %d\r\n", __func__, __LINE__);

    ret = uart_dut_task_create();
    if (ret != 0) {
        UART_PRINTF("Error: Failed to create uart_test thread: %d\r\n", ret);
    }

    return 0;

}

int dut_test_start(int argc, char **argv)
{
	printf("++++ %s +++++\r\n", __func__);
	dut_init();

	uart_dut_init();

    uart_test_init();


	app_rf_power_init(); //set 0x40
	app_xtal_cal_init();  // set 0x08 for 7db
    return 0;
}

#endif

