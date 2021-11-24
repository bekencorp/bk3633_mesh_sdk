#include <sys/unistd.h>
#include <sys/errno.h>
#include "hal/soc/soc.h"
#include "board.h"
#include "hal.h"

extern hal_flash_init();
extern int errno;
wdg_dev_t  wdg;
uart_dev_t uart_0;

void hal_init(void)
{
    uart_0.port                = STDIO_UART_PORT;
    uart_0.config.baud_rate    = STDIO_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity	   = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode         = MODE_TX;

    hal_uart_init(&uart_0);
    hal_flash_init();
// #ifdef CONFIG_DUT_TEST_CMD
// 	check_and_set_dut_flag();
// 	if(!get_dut_flag())
// #endif
//     {
//         wdg_dev_t  wdg;
//         wdg.config.timeout = 0xffff;
//         hal_wdg_init(&wdg);
//     }
#ifdef CONFIG_DUT_TEST_CMD
	check_and_set_dut_flag();
	if(!get_dut_flag())
#endif
	{
    	hal_aon_wdt_start(0x4ffff * 5);
    	hal_aon_wdt_idle_sleep();
	}

}

void hal_boot(hal_partition_t partition)
{
    uint32_t addr;

    intc_deinit();

    addr = hal_flash_get_info(partition)->partition_start_addr;
    __asm volatile ("BX %0" : : "r" (addr) );
}

void hal_reboot(void)
{
    wdg_dev_t  wdg;

    wdg.config.timeout = 1;
    set_PMU_Reg0x1_boot_rom_en(1);
    hal_wdg_init(&wdg);
    while(1);
}

