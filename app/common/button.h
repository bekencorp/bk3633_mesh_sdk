#ifndef __BUTTON_H
#define __BUTTON_H

#include "gpio_pub.h"

#define VIBRATION_ELIMINATE_TIME    80
#define VIBRATION_QUICK_CONFIRM     20
#define VIBRATION_MISS_MAX          3
#define KEYSTROKE_TIME              300

#define BUTTON_PRESSDOWN            GPIO_PIN_LOW
#define BUTTON_IDLE                 GPIO_PIN_HIGH

typedef enum
{
    BUTTON_S1 = 0,
    BUTTON_SUM,

} button_num_e;

typedef enum
{
    BUTTON_UNDEF  = 0,
    BUTTON_CLICK,
    BUTTON_LONG_PRESS,

} button_press_mode_e;

typedef struct
{
    button_num_e seq_num;
    GPIO_INDEX gpio_index;
    gpio_irq_trigger_t trigger;
    void (*isr_handler)(void *arg);
    void* handler_arg;

} gpio_button_s;

typedef struct
{
    uint8_t     press_mode;
    uint8_t     press_cnt;

} cnt_set_s;

typedef struct
{
    button_num_e current_button;
    uint8_t vib_cnt;
    cnt_set_s cnt_set[BUTTON_SUM];

} gpio_button_env;

void button_control_init(void);


#endif  //__BUTTON_H