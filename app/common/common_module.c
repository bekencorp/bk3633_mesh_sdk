#include "common_module.h"

#if (CONFIG_GPIO_BUTTONS)
#include "button.h"
#endif

#include "uart_test_cmd.h"

int common_module_init(void)
{
#if (CONFIG_UART_TEST_CMD)
    uart_test_init();
#endif

#if (CONFIG_GPIO_BUTTONS)
    button_control_init();
#endif
}