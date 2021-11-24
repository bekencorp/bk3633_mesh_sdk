
#include "uart_test_cmd.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <errno.h>
#include <stdlib.h>
#include <k_config.h>
#include <k_api.h>
#include "uart.h"
#include "uart_pub.h"

#if (CONFIG_UART_TEST_CMD)
#define UART_TIME_OUT 200
static cpu_stack_t *uart_test_task;//[UART_TEST_TASK_STACK/sizeof(cpu_stack_t)];
static ktask_t uart_test_task_obj;
static struct uart_test_st *uart_tp     = NULL;

static int volatile uart_test_enable   = 1;


static const struct uart_command uart_test_cmds[] = {
    {"uart_rease_reboot",				erase_reboot_uart_cmd_handler},
    {"uart_reboot",						reboot_uart_cmd_handler},
    {"uart_lpn_set",					lpn_set_uart_cmd_handler},

    {"uart_xtal_cal_set",				set_xtal_cal_cmd_handler},
    {"uart_xtal_cal_save",              save_xtal_cal_cmd_handler},
    {"uart_power_set",  				set_rf_power_cmd_handler},
    {"uart_power_save",              	save_rf_power_cmd_handler},
#ifdef CONFIG_DUT_TEST_CMD
	{"uart_fcc_tx",                		rf_fcc_tx_test_cmd_handler},
	{"uart_fcc_stop_tx", 		   		rf_fcc_test_stop_cmd_handler},
#endif

};

uint8_t ch_str_2_u8(char *p_input_char, uint8_t *p_outup_num)
{
	uint8_t i;

	*p_outup_num = 0;

	for(i = 0; i < 2; i++)
	{
		if(p_input_char[i] >= '0' && p_input_char[i] <= '9')
		{
			*p_outup_num = ((*p_outup_num)* 0x10) + (p_input_char[i] - '0');
		}
		else if(p_input_char[i] >= 'A' && p_input_char[i] <= 'F')
		{
			*p_outup_num = ((*p_outup_num) * 0x10) + (p_input_char[i] - 'A' + 10);
		}
		else if(p_input_char[i] >= 'a' && p_input_char[i] <= 'f')
		{
			*p_outup_num = ((*p_outup_num)* 0x10) + (p_input_char[i] - 'a' + 10);
		}
		else 
		{
			break;
		}		
	}

	//param input error
	if(p_input_char[i] != '\0' && p_input_char[i] != ' ')
	{
		printf("[%s] input param error!\n", __func__);
		*p_outup_num = 0;
		return 0xFF;
	}
	
	return (i + 1);
}

uint8_t ch_str_2_u16(char *p_input_char, uint16_t *p_outup_num)
{
	uint8_t i;

	*p_outup_num = 0;

	for(i = 0; i < 4; i++)
	{
		if(p_input_char[i] >= '0' && p_input_char[i] <= '9')
		{
			*p_outup_num = ((*p_outup_num)* 0x10) + (p_input_char[i] - '0');
		}
		else if(p_input_char[i] >= 'A' && p_input_char[i] <= 'F')
		{
			*p_outup_num = ((*p_outup_num) * 0x10) + (p_input_char[i] - 'A' + 10);
		}
		else if(p_input_char[i] >= 'a' && p_input_char[i] <= 'f')
		{
			*p_outup_num = ((*p_outup_num)* 0x10) + (p_input_char[i] - 'a' + 10);
		}
		else 
		{
			break;
		}		
	}

	//param input error
	if(p_input_char[i] != '\0' && p_input_char[i] != ' ')
	{
		*p_outup_num = 0;
		printf("[%s] input param error!\n", __func__);
		return 0xFF;
	}
	
	return (i + 1);
}

uint8_t ch_str_2_u32(char *p_input_char, uint32_t *p_outup_num)
{
	uint8_t i;

	*p_outup_num = 0;

	for(i = 0; i < 8; i++)
	{
		if(p_input_char[i] >= '0' && p_input_char[i] <= '9')
		{
			*p_outup_num = ((*p_outup_num)* 0x10) + (p_input_char[i] - '0');
		}
		else if(p_input_char[i] >= 'A' && p_input_char[i] <= 'F')
		{
			*p_outup_num = ((*p_outup_num) * 0x10) + (p_input_char[i] - 'A' + 10);
		}
		else if(p_input_char[i] >= 'a' && p_input_char[i] <= 'f')
		{
			*p_outup_num = ((*p_outup_num)* 0x10) + (p_input_char[i] - 'a' + 10);
		}
		else 
		{
			break;
		}		
	}

	//param input error
	if(p_input_char[i] != '\0' && p_input_char[i] != ' ')
	{
		*p_outup_num = 0;
		printf("[%s] input param error!\n", __func__);
		return 0xFF;
	}
	
	return (i + 1);
}


static int uart_get_input(char *inbuf, unsigned int *bp)
{
    char c;

    if (inbuf == NULL) {
        UART_PRINTF("inbuf_null\r\n");
        return 0;
    }

    while(uart_test_getchar(&c) == 1 && (*bp) < UART_TEST_INBUF_SIZE)
	{

        inbuf[*bp] = c;
        (*bp)++;
    }

    return (*bp);
}

//
static void filt_uart1_data(char *inbuf, uint8_t *p_len)
{

	if(*p_len == 0)
	{
		return;
	}

	for(uint8_t i = 0; i  < *p_len - 1; i++)
	{
		//recive more than 1 msg 
		if((inbuf[i] == '\r') && (inbuf[i + 1] == '\n'))
		{
			inbuf[i] = '\0';
			*p_len = i + 1;
			return; 
		}
	}


	//the last data is not "\r\n"
	if(inbuf[*p_len - 1] != '\0')
	{	
		uart_tp->inbuf[*p_len - 1] = '\0';
		*p_len = *p_len + 1;
	}
	
}

static int handle_uart_cmd(char *inbuf, uint8_t  *p_len)
{
    int i = 0;

	uint8_t cmd_name_len;
	
	uint8_t inbuf_len = strlen(inbuf);

	if(inbuf == NULL || p_len == NULL || *p_len == 0)
	{
		return;
	}
	
    for (i = 0; i < sizeof(uart_test_cmds)/sizeof(uart_test_cmds[0]); i++) 
    {

		cmd_name_len = strlen(uart_test_cmds[i].cmd_name);

		if(inbuf_len < cmd_name_len)
		{
			continue;
		}
		
		//has param input, and has matched
		if((memcmp(uart_test_cmds[i].cmd_name, inbuf, cmd_name_len) == 0)
		   && (inbuf[cmd_name_len] == ' ' || inbuf[cmd_name_len] == '\0'))
		{
			//no param
			if(inbuf[cmd_name_len] == '\0')
			{
				uart_test_cmds[i].func(NULL);
			}
			else
			{
				uart_test_cmds[i].func(&inbuf[cmd_name_len + 1]);
			}
			return 0;
		}
    }

	printf("%s: can not fine cmd (%s) \r\n", __func__, inbuf);
    return 1;
}


static void print_not_support_cmd(int op_ret)
{
    bk_printf("command '0x%x' not found\r\n", op_ret);
}

static void uart_test_main(void)
{

    while (uart_test_enable) 
	{
        int   op_ret;
        char *msg = NULL;
		unsigned int i = 0;

        if (uart_get_input(uart_tp->inbuf, &uart_tp->bp)) 
		{

			/*
			for(i = 0;  i < uart_tp->bp; i++)
			{
				printf("%02X ", uart_tp->inbuf[i]);
			}
			*/

			if (get_dut_flag() && (uart_rx_dut_cmd(uart_tp->inbuf, uart_tp->bp) == 0))
			{
				;
			}
			else
			{
				filt_uart1_data(uart_tp->inbuf, &uart_tp->bp);

				if( uart_tp->inbuf[0] == 'A' && uart_tp->inbuf[1] == 'T' && uart_tp->bp > 2) //jinxun AT cmd
				{
					printf("AT cmd : \r\n");
					for(int i = 0; i <  uart_tp->bp;  i++)
					{
						printf("%c", uart_tp->inbuf[i]);
					}
					printf("\r\n");
					
				}
				else
				{
					handle_uart_cmd(uart_tp->inbuf, &(uart_tp->bp));
				}
			}
	        uart_tp->bp = 0;
        }
    }

    bk_printf("UART CMD exited\r\n");
    aos_free(uart_tp);
    uart_tp = NULL;

    aos_task_exit(0);
}

static void exit_uart_test_cmd(char *buf, int len, int argc, char **argv)
{
    uart_test_enable = 0;
    return;
}
int uart_test_task_create(void)
{
   uart_test_task = (uint32_t *)aos_malloc((UART_TEST_TASK_STACK));
   return krhino_task_create(&uart_test_task_obj, "uart_test_cmd", NULL,
    		                 UART_TEST_TASK_PRIO, 0, uart_test_task,
    		                 UART_TEST_TASK_STACK / sizeof(cpu_stack_t),
                             (task_entry_t)uart_test_main, 1);
}

int uart_test_getchar(char *inbuf)
{
    int        ret       = -1;
    uint32_t   recv_size = 0;
    uart_dev_t uart_stdio;

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = UART1_PORT;
	
    ret = hal_uart_recv_II(&uart_stdio, inbuf, 1, &recv_size, UART_TIME_OUT);

    if ((ret == 0) && (recv_size == 1)) 
	{
		return 1;
    } 
	else 
	{
        return 0;
    }
}

int uart_test_init(void)
{
    int ret;

    printf("%s, %d\r\n", __func__, __LINE__);

    uart_tp = (struct uart_test_st *)aos_malloc(sizeof(struct uart_test_st));

    if (uart_tp == NULL) {
        return ENOMEM;
    }

    memset((void *)uart_tp, 0, sizeof(struct uart_test_st));

    ret = uart_test_task_create();
    if (ret != 0) {
        UART_PRINTF("Error: Failed to create uart_test thread: %d\r\n", ret);
    }

    return 0;

init_general_err:
    if (uart_tp) {
        aos_free(uart_tp);
        uart_tp = NULL;
    }

    return ret;
}

#endif