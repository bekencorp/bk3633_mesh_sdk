#ifndef _SPI_PUB_H_
#define _SPI_PUB_H_

#include "driver_pub.h"

#define SPI_DEV_NAME                "spi"

enum
{
    CMD_SPI_UNIT_ENABLE = SPI_CMD_MAGIC + 1,
    CMD_SPI_SET_MSTEN,
    CMD_SPI_SET_CKPHA,
    CMD_SPI_SET_CKPOL,
    CMD_SPI_SET_BITWIDTH,
    CMD_SPI_SET_CKR,
    CMD_SPI_RXINT_EN,
    CMD_SPI_TXINT_EN,
    CMD_SPI_RXOVR_EN,
    CMD_SPI_TXOVR_EN,
    CMD_SPI_RXFIFO_CLR,
    CMD_SPI_TXFIFO_CLR,
    CMD_SPI_RXINT_MODE,
    CMD_SPI_TXINT_MODE,
    CMD_SPI_INIT_MSTEN,
    CMD_SPI_REGIST_TX,
    CMD_SPI_REGIST_RX,
    CMD_SPI_DEREGIST,
    CMD_SPI_TRANS_DONE,
};

typedef struct
{
    UINT8 tx_remain_data_cnt;
    UINT8 rx_remain_data_cnt;
    UINT8 *p_tx_buf;
    UINT8 *p_rx_buf;
    UINT8 trans_done;
    UINT8 send_done;
}  spi_trans_t;

typedef enum
{
    SPI_PORT0 = 0x00UL,
    //SPI_PORT1 = 0x01UL,
    SPI_DEV_NUM,
} spi_port;

typedef enum
{
    slave,
    master
} spi_msten;

typedef enum
{
    CKPHA_CLK1 = 0x00UL,
    CKPHA_CLK2 = 0x01UL
} spi_ckpha;

typedef enum
{
    CKPOL_L = 0x00UL,
    CKPOL_H = 0x01UL
} spi_ckpol;

typedef enum
{
    SPI_CHARFORMAT_8BIT = 0x00UL,
    SPI_CHARFORMAT_16BIT = 0x01UL
} spi_charformat;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void spi_init(void);
void spi_exit(void);
void spi_isr(void);
#endif //_SPI_PUB_H_
