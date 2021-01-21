
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

static cpu_stack_t uart_test_task[UART_TEST_TASK_STACK/sizeof(cpu_stack_t)];
static ktask_t uart_test_task_obj;
static struct uart_test_st *uart_tp     = NULL;

static int volatile uart_test_enable   = 1;


static const struct uart_command uart_test_cmds[] = {
    {UART_CMD_ERASE_REBOOT,               erase_reboot_uart_cmd_handler},

};

static int uart_get_input(char *inbuf, unsigned int *bp)
{
    char c;

    if (inbuf == NULL) {
        UART_PRINTF("inbuf_null\r\n");
        return 0;
    }

    while (uart_test_getchar(&c) == 1 && (*bp) < UART_TEST_INBUF_SIZE - 1) {
        if (inbuf[(*bp) - 1] == UART_END_CHAR && c == UART_RET_CHAR)
        {
            inbuf[*bp - 1] = '\0';
            *bp        = 0;
            return 1;
        }

        inbuf[*bp] = c;
        (*bp)++;
    }

    return 0;
}

static int handle_uart_cmd(char *inbuf)
{
    int i = 0;
    char *index = inbuf;
    char *para = NULL;
    uint16_t opcode = ((uint16_t)(*index++)<<8) | (*index++);

    if((*index) != '\0')
    {
        para = index;
    }

    for (i = 0; i < sizeof(uart_test_cmds)/sizeof(uart_test_cmds[0]); i++) 
    {
        if(opcode == uart_test_cmds[i].op)
        {
            uart_test_cmds[i].func(para);
            return 0;
        }
    }

    return opcode;
}


static void print_not_support_cmd(int op_ret)
{
    bk_printf("command '0x%x' not found\r\n", op_ret);
}

static void uart_test_main(void)
{
    while (uart_test_enable) {
        int   op_ret;
        char *msg = NULL;

        if (uart_get_input(uart_tp->inbuf, &uart_tp->bp)) {
            msg = uart_tp->inbuf;

            bk_printf("~~~~income command \r\n");

            op_ret = handle_uart_cmd(msg);
            if (op_ret) {
                print_not_support_cmd(op_ret);
            }

            bk_printf("\r\n");
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
   return krhino_task_create(&uart_test_task_obj, "uart_test_cmd", NULL,
    		                 UART_TEST_TASK_PRIO, 0, uart_test_task,
    		                 sizeof(uart_test_task) / sizeof(cpu_stack_t),
                             (task_entry_t)uart_test_main, 1);
}

int uart_test_getchar(char *inbuf)
{
    int        ret       = -1;
    uint32_t   recv_size = 0;
    uart_dev_t uart_stdio;

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = UART1_PORT;

    ret = hal_uart_recv_II(&uart_stdio, inbuf, 1, &recv_size, HAL_WAIT_FOREVER);

    if ((ret == 0) && (recv_size == 1)) {
        return 1;
    } else {
        return 0;
    }
}

int uart_test_init(void)
{
    int ret;

    UART_PRINTF("%s, %d\r\n", __func__, __LINE__);

    uart_tp = (struct uart_test_st *)aos_malloc(sizeof(struct uart_test_st));
    memset(uart_tp, 0, sizeof(struct uart_test_st));

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