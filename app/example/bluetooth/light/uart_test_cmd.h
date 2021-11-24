#ifndef __UART_TEST_CMD_H
#define __UART_TEST_CMD_H

#include <stdint.h>

#define UART_TEST_TASK_STACK    1048

#define UART_CMD_READ_PERIOD    200

#define UART_CMD_MAX_NAME_LEN   20
#define UART_CMD_MAX_ARG_LEN    5
#define UART_CMD_MAX_ARG_NUM    5

#define UART_TEST_TASK_PRIO     (AOS_DEFAULT_APP_PRI + 2)


#define UART_TEST_INBUF_SIZE (100)

#define UART_DUT_TASK_PRIO     (AOS_DEFAULT_APP_PRI + 3)


#define UART_RET_CHAR '\n'      //0x0a
#define UART_END_CHAR '\r'      //0x0d

typedef enum {
    UART_CMD_NONE                       = 0x8A00,
    UART_CMD_ERASE_REBOOT               = 0x8A01,
    UART_CMD_REBOOT                     = 0x8a02,
    UART_CMD_LNP_SET                    = 0x8a03,

	UART_CMD_XTAL_CAL_SET               = 0x8b01,
	UART_CMD_XTAL_CAL_SAVE              = 0x8b02,
	UART_CMD_RF_POWER_SET               = 0x8b03,
	UART_CMD_RF_POWER_SAVE              = 0x8b04,
	UART_CMD_RF_XVR                     = 0x8b05,
	UART_CMD_RF_FCC_TEST                = 0x8b06,
	UART_CMD_RF_FCC_TEST_STOP			= 0x8b07,

	
} uart_cmd_opcode_e;

struct uart_command {
    char cmd_name[UART_CMD_MAX_NAME_LEN + 2];
    void (*func)(char *para);

    //The whole command end with UART_END_CHAR UART_RET_CHAR (0x0d 0x0a)
};

struct uart_test_st {

    char inbuf[UART_TEST_INBUF_SIZE];
    unsigned int bp; 

};

int uart_test_init(void);

void erase_reboot_uart_cmd_handler(char *para);
void reboot_uart_cmd_handler(char *para);
void lpn_set_uart_cmd_handler(char *para);

void set_xtal_cal_cmd_handler(char *para);
void save_xtal_cal_cmd_handler(char *para);
void set_rf_power_cmd_handler(char *para);
void save_rf_power_cmd_handler(char *para);
void printf_rf_xvr_handler(char *para);

#ifdef CONFIG_DUT_TEST_CMD
void rf_fcc_tx_test_cmd_handler(char *para);
void rf_fcc_test_stop_cmd_handler(char *para);
#endif

#endif  //__UART_TEST_CMD_H