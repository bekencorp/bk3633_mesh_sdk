/**
 ****************************************************************************************
 *
 * @file bim_app.h
 *
 * @brief interface
 *
 * Copyright (C) Beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _BIM_APP_H_
#define _BIM_APP_H_

#define BIT(n)    (1UL << (n))

struct boot_func_tag
{
	uint32_t (*get_gpio_value)(void);
};
extern const struct boot_func_tag boot_func;

uint32_t get_gpio_value(void);
#endif