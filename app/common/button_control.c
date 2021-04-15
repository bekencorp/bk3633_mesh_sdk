#include <zephyr.h>
#include "button.h"

#if (CONFIG_GPIO_BUTTONS)

struct k_timer vibration_eliminate_timer;
struct k_timer keystroke_timer;

static gpio_button_env b_env;

static void button_cnt_increase(button_num_e seq_num, button_press_mode_e mode)
{
    if(seq_num >= BUTTON_SUM)
    {
        return;
    }

    cnt_set_s* set = &b_env.cnt_set[seq_num];

    if(set->press_mode == mode || set->press_mode == BUTTON_UNDEF)
    {
        set->press_cnt ++;
    }
    else
    {
        /* Replace the last press mode with the new one */
        set->press_cnt = 1;
    }

    set->press_mode = mode;
}

static void button_income(button_num_e button)
{
    b_env.current_button = button;
}

static button_num_e get_button_num(void)
{
    return b_env.current_button;
}

static void button_cnt_reset(button_num_e seq_num)
{
    if(seq_num >= BUTTON_SUM)
    {
        return;
    }

    cnt_set_s* set = &b_env.cnt_set[seq_num];

    set->press_mode = BUTTON_UNDEF;
    set->press_cnt = 0;
}

static void button_accomlishment(button_num_e seq)
{
    button_func_handler(seq, b_env.cnt_set[seq]);
    button_cnt_reset(seq);
    button_income(BUTTON_SUM);
    b_env.vib_cnt = 0;
}

int button_pattern_recognize(button_num_e seq_num)
{
    if(seq_num >= BUTTON_SUM)
    {
        return -1;
    }

    if(get_button_num() == BUTTON_SUM)
    {
        button_income(seq_num);
        k_timer_start(&vibration_eliminate_timer, VIBRATION_ELIMINATE_TIME);
    }
    else if(k_timer_is_started(&vibration_eliminate_timer))
    {
        k_timer_start(&vibration_eliminate_timer, VIBRATION_ELIMINATE_TIME);
    }
    else if(k_timer_is_started(&keystroke_timer))
    {
        k_timer_stop(&keystroke_timer);
        button_cnt_increase(seq_num, BUTTON_CLICK);
        k_timer_start(&vibration_eliminate_timer, VIBRATION_ELIMINATE_TIME);
    }

    return 0;

}

static uint32_t button_pin_stat_get(button_num_e seq)
{
    gpio_dev_t gpio;
    uint32_t state;

    gpio.port = get_gpio_pin_id(seq);

    hal_gpio_input_get(&gpio, &state);

    return state;
}

static void _vib_eli_timer_cb(void *p_timer, void *args)
{
    uint32_t value;
    button_num_e seq = get_button_num();

    k_timer_stop(&vibration_eliminate_timer);

    if(seq < BUTTON_SUM)
    {
        value = button_pin_stat_get(seq);
    }
    else
    {
        return;
    }

    if(value == BUTTON_IDLE)
    {
        if(b_env.vib_cnt < VIBRATION_MISS_MAX)
        {
            k_timer_start(&vibration_eliminate_timer, VIBRATION_QUICK_CONFIRM);
            b_env.vib_cnt ++;
        }
        else
        {
            button_accomlishment(seq);
        }

    }
    else
    {
        b_env.vib_cnt = 0;
        k_timer_start(&keystroke_timer, KEYSTROKE_TIME);
    }

}

static void _keystroke_timer_cb(void *p_timer, void *args)
{
    button_num_e seq = get_button_num();
    uint32_t value;
    button_press_mode_e mode;

    k_timer_stop(&keystroke_timer);

    if(seq == BUTTON_SUM)
    {
        return;
    }
    else
    {
        value = button_pin_stat_get(seq);

        if(value == BUTTON_PRESSDOWN)
        {
            mode = BUTTON_LONG_PRESS;
        }
        else
        {
            mode = BUTTON_CLICK;
        }

        button_cnt_increase(seq, mode);
        button_accomlishment(seq);
    }

}

void button_env_register(button_num_e seq_num)
{
    memset(&b_env.cnt_set[seq_num], 0, sizeof(cnt_set_s));
}

void button_control_init(void)
{
    memset(&b_env, 0 ,sizeof(gpio_button_env));
    b_env.current_button = BUTTON_SUM;

    k_timer_init(&vibration_eliminate_timer, _vib_eli_timer_cb, NULL);
    k_timer_init(&keystroke_timer, _keystroke_timer_cb, NULL);

    button_module_init();
}


#endif  //CONFIG_GPIO_BUTTONS