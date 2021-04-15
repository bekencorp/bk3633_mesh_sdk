/**
****************************************************************************************
*
* @file spi_function.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "spi_pub.h"
#include "spi.h"

#ifdef __SPI_DRIVER__
static spi_dev_t spi;
static uint8_t spi_tx_buf[200];
static uint8_t spi_rx_buf[200];
void _spi_init(void)
{
    spi.port = SPI_PORT0;

    hal_spi_register(spi.port, spi_tx_buf, spi_rx_buf);

    spi.config.clk_sel = 0;
    spi.config.mode = master;
    spi.config.ckpha = 1;
    spi.config.ckpol = 0;
    spi.config.freq_div = 4;
    spi.config.charformat = 0;

    hal_spi_init(&spi);
}

void _spi_slave_init(void)
{
    spi.port = SPI_PORT0;

    hal_spi_register(spi.port, spi_tx_buf, spi_rx_buf);

    spi.config.clk_sel = 0;
    spi.config.mode = slave;
    spi.config.ckpha = 1;
    spi.config.ckpol = 0;
    spi.config.freq_div = 4;
    spi.config.charformat = 0;

    hal_spi_init(&spi);
}

void _spi_send_test(void)
{
    uint8_t test[100];
    int i = 0;

    memset(test, 0xf0, 100);

    while(1)
    {
        hal_spi_send(NULL, test, 100, 0);

        aos_msleep(500);
    }
}

void _spi_reveive_test(void)
{
    uint8_t test[200];
    while(1)
    {
        hal_spi_recv(NULL, test, 200, 0);
        while(!hal_spi_trans_complete());
    }

}

#endif  //__SPI_DRIVER__