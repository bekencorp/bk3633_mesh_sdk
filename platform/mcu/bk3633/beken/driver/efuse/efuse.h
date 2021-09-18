#ifndef _SPI_H_
#define _SPI_H_

/* 
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions


#define EFUSE_REG0X0_CN                  *((volatile unsigned long *) (0x00806c00+0x0*4))
#define POS_EFUSE_REG0X0_VDD25_EN             31
#define POS_EFUSE_REG0X0_OPRT_WDATA             16
#define POS_EFUSE_REG0X0_OPRT_ADDR              8
#define POS_EFUSE_REG0X0_OPRT_DIR               1
#define POS_EFUSE_REG0X0_OPRT_EN               0


#define EFUSE_REG0X1_CN                  *((volatile unsigned long *) (0x00806c00+0x1*4))
#define POS_EFUSE_REG0X1_READ_VALID               8
#define POS_EFUSE_REG0X1_READ_DATA               0


uint8_t efuse_read(uint8_t addr);
void efuse_write(uint8_t addr,uint8_t data);


/// @} UART
#endif /* _UART_H_ */

