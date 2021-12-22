/**
 ****************************************************************************************
 *
 * @file deep_sleep.h
 *
 * @brief deep sleep interface
 *
 * Copyright (C) Beken 2021-2023
 *
 *
 ****************************************************************************************
 */
#ifndef DEEP_SLEEP_H_
#define DEEP_SLEEP_H_

/*
 * GPIO level trigger
 */
typedef enum {
    GPIO_TRIGGER_RISING_EDGE  = 0x2, /* Interrupt triggered at input signal's rising edge  */
    GPIO_TRIGGER_FALLING_EDGE = 0x3, /* Interrupt triggered at input signal's falling edge */
} gpio_trigger_t;

extern void mem_is_enable_check_bypass(bool val);
extern void deep_sleep_wake_by_gpio(uint32_t gpio_idx, gpio_trigger_t trigger);
extern void deep_sleep(void) ;
#endif