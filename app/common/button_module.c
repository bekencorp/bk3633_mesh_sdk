#include "button.h"


static gpio_button_s gpio_button_list[];

static void s1_isr_handler(void *arg)
{
    button_pattern_recognize(BUTTON_S1);
}

static void s1_button_func(cnt_set_s set)
{
    UART_PRINTF("%s, %d, mode %d, cnt %d\r\n", __func__, __LINE__, set.press_mode, set.press_cnt);
}

void button_func_handler(button_num_e seq_num, cnt_set_s set)
{
    switch(seq_num)
    {
        case BUTTON_S1:
            s1_button_func(set);
        default:
            break;
    }

}

uint8_t get_gpio_pin_id(button_num_e seq)
{
    if(seq < BUTTON_SUM)
    {
        return gpio_button_list[seq].gpio_index;
    }
    else
    {
        return 0xFF;
    }
}

void button_module_init()
{
    gpio_button_s s1 = gpio_button_list[BUTTON_S1];
    gpio_dev_t s1_gpio;
    s1_gpio.port = s1.gpio_index;
    hal_gpio_enable_irq(&s1_gpio, s1.trigger, s1.isr_handler, s1.handler_arg);

    button_env_register(BUTTON_S1);
}

static gpio_button_s gpio_button_list[] = {
    {BUTTON_S1, GPIO_P02, IRQ_TRIGGER_FALLING_EDGE, s1_isr_handler, NULL},
};

