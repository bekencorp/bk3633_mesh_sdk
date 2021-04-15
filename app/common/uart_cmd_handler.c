#include "uart_pub.h"

void erase_reboot_uart_cmd_handler(char *para)
{
    UART_PRINTF("%s \r\n", __func__);
    genie_flash_erase_userdata();
    genie_flash_erase_reliable();
    aos_reboot();
}

void reboot_cmd_handler(char *para)
{
    UART_PRINTF("%s \r\n", __func__);
    aos_msleep(100);
    aos_reboot();
}