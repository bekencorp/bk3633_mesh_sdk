/**
 ****************************************************************************************
 *
 * @file user_config.h
 *
 * @brief Configuration of the BT function
 *
 * Copyright (C) Beken 2019
 *
 ****************************************************************************************
 */
#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_
//#include "uart2.h"
//#include "uart.h"

#define VIRTUAL_UART_H4TL		1
#define UART_PRINTF_ENABLE      1 
#define DEBUG_HW                0
#define GPIO_DBG_MSG            0








//DRIVER CONFIG
#define UART0_DRIVER				1
#define UART2_DRIVER				1
#define GPIO_DRIVER					0
#define AUDIO_DRIVER			    1
#define RTC_DRIVER					0
#define ADC_DRIVER					0
#define I2C_DRIVER					0
#define PWM_DRIVER					1
#define USB_DRIVER                  0 
#define SPI_DRIVER                  0 
#define AON_RTC_DRIVER              0



#define uart_printf              uart2_printf


#endif // USER_CONFIG_H_


