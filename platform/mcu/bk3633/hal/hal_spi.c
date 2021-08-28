/**
****************************************************************************************
*
* @file hal_spi.c
*
* @brief spi initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "spi_pub.h"
#include "icu_pub.h"
#include "spi.h"
#include "intc_pub.h"
#include "drv_model_pub.h"
#include "common/log.h"

#ifdef __SPI_DRIVER__
typedef struct {
    spi_dev_t*  spi_p;
    uint8_t*    spi_tx_buf;
    uint32_t     tb_size;
    uint8_t*    spi_rx_buf;
    uint32_t     rb_size;
} hal_spi_module;


int8_t hal_spi_register(spi_port port, uint8_t* txbuf, uint8_t* rxbuf)
{
    if(port >= SPI_DEV_NUM)
    {
        return -1;
    }

    DD_HANDLE spi_hdl;
    int32_t status;

    spi_hdl = ddev_open(SPI_DEV_NAME, &status, 0);

    if(txbuf != NULL)
    {
        ddev_control(spi_hdl, CMD_SPI_REGIST_TX, txbuf);
    }

    if(rxbuf != NULL)
    {
        ddev_control(spi_hdl, CMD_SPI_REGIST_RX, rxbuf);
    }

    ddev_close(spi_hdl);


    return 0;
}

int8_t hal_spi_deregister(spi_port port)
{
    if(port >= SPI_DEV_NUM)
    {
        return -1;
    }

    DD_HANDLE spi_hdl;
    int32_t status;

    spi_hdl = ddev_open(SPI_DEV_NAME, &status, 0);

    ddev_control(spi_hdl, CMD_SPI_DEREGIST, 0);

    ddev_close(spi_hdl);

    return 0;
}

int32_t hal_spi_init(spi_dev_t *spi)
{
	int ret = 0;
    int32_t status;
    DD_HANDLE spi_hdl;

    if(spi->port >= SPI_DEV_NUM)
    {
        return -1;
    }

    spi_hdl = ddev_open(SPI_DEV_NAME, &status, 0);

    uint8_t clk = spi->config.clk_sel;
    sddev_control(ICU_DEV_NAME, CMD_SPI_CLK_SEL, (void *)&clk);

    spi_msten mode = spi->config.mode;
    ddev_control(spi_hdl, CMD_SPI_INIT_MSTEN, &mode);

    spi_ckpha ckpha = spi->config.ckpha;
    ddev_control(spi_hdl, CMD_SPI_SET_CKPHA, &ckpha);

    spi_ckpol ckpol = spi->config.ckpol;
    ddev_control(spi_hdl, CMD_SPI_SET_CKPOL, &ckpol);

    uint8_t freq_div = spi->config.freq_div;
    ddev_control(spi_hdl, CMD_SPI_SET_CKR, &freq_div);

    spi_charformat charformat = spi->config.charformat;
    ddev_control(spi_hdl, CMD_SPI_SET_BITWIDTH, &charformat);

    ddev_close(spi_hdl);

    intc_enable(IRQ_SPI);

    return ret;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t status;
    DD_HANDLE spi_hdl;

    spi_hdl = ddev_open(SPI_DEV_NAME, &status, 0);

    ddev_write(spi_hdl, data, size, timeout);

    ddev_close(spi_hdl);

    return 0;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t status;
    DD_HANDLE spi_hdl;

    spi_hdl = ddev_open(SPI_DEV_NAME, &status, 0);

    UINT8 enable = 1;
    ddev_control(spi_hdl, CMD_SPI_RXINT_EN, &enable);

    ddev_read(spi_hdl, data, size, timeout);

    ddev_close(spi_hdl);

    return 0;
}

int8_t hal_spi_trans_complete(void)
{
    int32_t status;
    DD_HANDLE spi_hdl;

    spi_hdl = ddev_open(SPI_DEV_NAME, &status, 0);

    UINT8 trans_done = 0;
    ddev_control(spi_hdl, CMD_SPI_TRANS_DONE, &trans_done);

    ddev_close(spi_hdl);
    
    return trans_done;
}

#endif //__SPI_DRIVER__