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