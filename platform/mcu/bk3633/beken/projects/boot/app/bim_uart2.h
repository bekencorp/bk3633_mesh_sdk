/**
 ****************************************************************************************
 *
 * @file uart.h
 *
 * @brief UART Driver for HCI over UART operation.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _uart2_H_
#define _uart2_H_
/**
 ****************************************************************************************
 * @defgroup UART UART
 * @ingroup DRIVERS
 * @brief UART driver
 *
 * @{
 *
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions

#define UART0_REG0X0                                        *((volatile unsigned long *) (0x00806300+0x0*4))
#define UART0_REG0X1                                         *((volatile unsigned long *) (0x00806300+0x1*4))
#define UART0_REG0X2                                         *((volatile unsigned long *) (0x00806300+0x2*4))
#define UART0_REG0X3                                         *((volatile unsigned long *) (0x00806300+0x3*4))
#define UART0_REG0X4                                         *((volatile unsigned long *) (0x00806300+0x4*4))
#define UART0_REG0X6                                         *((volatile unsigned long *) (0x00806300+0x6*4))
#define UART0_REG0X7                                         *((volatile unsigned long *) (0x00806300+0x7*4))
//addUART2_REG0x0
#define UART2_REG0X0                                        *((volatile unsigned long *) (0x00806a00+0x0*4))
#define POS_UART2_REG0X0_TX_ENABLE                          0
#define POS_UART2_REG0X0_RX_ENABLE                          1
#define POS_UART2_REG0X0_IRDA                               2
#define POS_UART2_REG0X0_LEN                                3
#define POS_UART2_REG0X0_PAR_EN                             5
#define POS_UART2_REG0X0_PAR_MODE                           6
#define POS_UART2_REG0X0_STOP_LEN                           7
#define POS_UART2_REG0X0_CLK_DIVID                          8
//addUART2_REG0x1
#define UART2_REG0X1                                         *((volatile unsigned long *) (0x00806a00+0x1*4))
#define POS_UART2_REG0X1_TX_FIFO_THRESHOLD                   0
#define POS_UART2_REG0X1_RX_FIFO_THRESHOLD                   8
#define POS_UART2_REG0X1_RX_STOP_DETECT_TIME                 16
//addUART2_REG0x2
#define UART2_REG0X2                                         *((volatile unsigned long *) (0x00806a00+0x2*4))
#define POS_UART2_REG0X2_TX_FIFO_COUNT                       0
#define POS_UART2_REG0X2_RX_FIFO_COUNT                       8
#define POS_UART2_REG0X2_TX_FIFO_FULL                        16
#define POS_UART2_REG0X2_TX_FIFO_EMPTY                       17
#define POS_UART2_REG0X2_RX_FIFO_FULL                        18
#define POS_UART2_REG0X2_RX_FIFO_EMPTY                       19
#define POS_UART2_REG0X2_FIFO_WR_READY                       20
#define POS_UART2_REG0X2_FIFO_RD_READY                       21
//addUART2_REG0x3
#define UART2_REG0X3                                         *((volatile unsigned long *) (0x00806a00+0x3*4))
#define POS_UART2_REG0X3_UART_TX_FIFO_DIN                    0
#define POS_UART2_REG0X3_UART_RX_FIFO_DOUT                   8
//addUART2_REG0x4
#define UART2_REG0X4                                         *((volatile unsigned long *) (0x00806a00+0x4*4))
#define POS_UART2_REG0X4_TX_FIFO_NEED_WRITE_MASK             0
#define POS_UART2_REG0X4_RX_FIFO_NEED_READ_MASK              1
#define POS_UART2_REG0X4_RX_FIFO_OVER_FLOW_MASK              2
#define POS_UART2_REG0X4_UART_RX_PARITY_ERR_MASK             3
#define POS_UART2_REG0X4_UART_RX_STOP_ERR_MASK               4
#define POS_UART2_REG0X4_UART_TX_STOP_END_MASK               5
#define POS_UART2_REG0X4_UART_RX_STOP_END_MASK               6
#define POS_UART2_REG0X4_UART_RXD_WAKEUP_MASK                7
//addUART2_REG0x5
#define UART2_REG0X5                                         *((volatile unsigned long *) (0x00806a00+0x5*4))
#define POS_UART2_REG0X5_TX_FIFO_NEED_WRITE                  0
#define POS_UART2_REG0X5_RX_FIFO_NEED_READ                   1
#define POS_UART2_REG0X5_RX_FIFO_OVER_FLOW                   2
#define POS_UART2_REG0X5_UART_RX_PARITY_ERROR                3
#define POS_UART2_REG0X5_UART_RX_STOP_ERROR                  4
#define POS_UART2_REG0X5_UART_TX_STOP_END                    5
#define POS_UART2_REG0X5_UART_RX_STOP_END                    6
#define POS_UART2_REG0X5_UART_RXD_WAKEUP                     7
//addUART2_REG0x6
#define UART2_REG0X6                                         *((volatile unsigned long *) (0x00806a00+0x6*4))
#define POS_UART2_REG0X6_FLOW_CTL_LOW_CNT                    0
#define POS_UART2_REG0X6_FLOW_CTL_HIGH_CNT                   8
#define POS_UART2_REG0X6_FLOW_CONTROL_ENA                    16
#define POS_UART2_REG0X6_RTS_POLARITY_SEL                    17
#define POS_UART2_REG0X6_CTS_POLARITY_SEL                    18
//addUART2_REG0x7
#define UART2_REG0X7                                         *((volatile unsigned long *) (0x00806a00+0x7*4))
#define POS_UART2_REG0X7_UART_WAKE_COUNT                     0
#define POS_UART2_REG0X7_UART_TXD_WAIT_CNT                   10
#define POS_UART2_REG0X7_UART_RXD_WAKE_EN                    20
#define POS_UART2_REG0X7_UART_TXD_WAKE_EN                    21
#define POS_UART2_REG0X7_RXD_NEGEDGE_WAKE_EN                 22
#define UART2_FIFO_MAX_COUNT   128

#define UART2_DRIVER   1

int bim_printf(const char *fmt,...);

void uart2_init(uint32_t baudrate);
void uart2_isr(void);
void uart0_init(uint32_t baudrate);
#endif 

