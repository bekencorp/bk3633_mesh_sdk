#include "genie_flash.h"
#include "uart_test_cmd.h"
#include "uart_pub.h"

void erase_reboot_uart_cmd_handler(char *para)
{
    UART_PRINTF("%s", __func__);
    genie_flash_erase_userdata();
    genie_flash_erase_reliable();
    aos_reboot();
}