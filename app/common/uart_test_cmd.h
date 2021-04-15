#ifndef __UART_TEST_CMD_H
#define __UART_TEST_CMD_H

#include <stdint.h>

#if (CONFIG_UART_TEST_CMD)

#define UART_TEST_TASK_STACK    2048

#define UART_CMD_READ_PERIOD    200

#define UART_CMD_MAX_NAME_LEN   10
#define UART_CMD_MAX_ARG_LEN    5
#define UART_CMD_MAX_ARG_NUM    5

#define UART_TEST_TASK_PRIO     (AOS_DEFAULT_APP_PRI + 2)

#define UART_TEST_INBUF_SIZE    (UART_CMD_MAX_NAME_LEN + 1 + \
                    (UART_CMD_MAX_ARG_LEN + 1) * UART_CMD_MAX_ARG_NUM + 2)

#define UART_RET_CHAR '\n'      //0x0a
#define UART_END_CHAR '\r'      //0x0d

typedef enum {
    UART_CMD_NONE                       = 0x8A00,
    UART_CMD_ERASE_REBOOT               = 0x8A01,
} uart_cmd_opcode_e;

struct uart_command {
    uart_cmd_opcode_e op;
    void (*func)(char *para);

    //The whole command end with UART_END_CHAR UART_RET_CHAR (0x0d 0x0a)
};

struct uart_test_st {

    char inbuf[UART_TEST_INBUF_SIZE];
    unsigned int bp; 

};

int uart_test_init(void);
#endif

void erase_reboot_uart_cmd_handler(char *para);
void reboot_cmd_handler(char *para);

#endif  //__UART_TEST_CMD_H