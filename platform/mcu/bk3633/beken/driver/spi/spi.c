/**
****************************************************************************************
*
* @file spi.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "include.h"
#include "arm_arch.h"

#include "spi.h"
#include "spi_pub.h"
#include "gpio_pub.h"
#include "icu_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "mcu_ps_pub.h"

#ifdef __SPI_DRIVER__

volatile spi_trans_t spi_trans;

static DD_OPERATIONS spi_op =
{
    NULL,
    NULL,
    spi_read,
    spi_write,
    spi_ctrl
};

static void spi_active(BOOLEAN val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~SPIEN;
    }
    else if(val == 1)
    {
        value |= SPIEN;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_set_msten(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~MSTEN;
    }
    else if(val == 1)
    {
        value |= MSTEN;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_set_ckpha(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~CKPHA;
    }
    else if(val == 1)
    {
        value |= CKPHA;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_set_skpol(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~CKPOL;
    }
    else if(val == 1)
    {
        value |= CKPOL;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_set_bit_wdth(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~BIT_WDTH;
    }
    else if(val == 1)
    {
        value |= BIT_WDTH;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_set_ckr(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    value &= (~SPI_CKR_MASK);
    value |= (val << SPI_CKR_POSI);
    REG_WRITE(SPI_CFG, value);
}

static void spi_rxint_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~RXINT_EN;
    }
    else if(val == 1)
    {
        value |= RXINT_EN;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_txint_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~TXINT_EN;
    }
    else if(val == 1)
    {
        value |= TXINT_EN;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_rxovr_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~RXOVF_EN;
    }
    else if(val == 1)
    {
        value |= RXOVF_EN;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_txovr_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(val == 0)
    {
        value &= ~TXUDF_EN;
    }
    else if(val == 1)
    {
        value |= TXUDF_EN;
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_rxfifo_clr(void)
{
    UINT32 value;

    value = REG_READ(SPI_STAT);
    value |= RXFIFO_CLR;
    REG_WRITE(SPI_STAT, value);
}

static void spi_txfifo_clr(void)
{
    UINT32 value;

    value = REG_READ(SPI_STAT);
    value |= TXFIFO_CLR;
    REG_WRITE(SPI_STAT, value);
}

static void spi_rxint_mode(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(0 <= val && val <= 3)
    {
        value = (value & (~RXINT_LEVEL_BIT)) | RXINT_LEVEL_X(val);
    }
    REG_WRITE(SPI_CFG, value);
}

static void spi_txint_mode(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CFG);
    if(0 <= val && val <= 3)
    {
        value = (value & (~TXINT_LEVEL_BIT)) | TXINT_LEVEL_X(val);
    }

    REG_WRITE(SPI_CFG, value);
}

static void spi_gpio_configuration(UINT8 msten)
{
    UINT32 param;

    if(msten == slave)
    {
        param = GFUNC_MODE_SPI_SLV;
    }
    else
    {
        param = GFUNC_MODE_SPI_MST;
    }
    
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

static void spi_icu_configuration()
{
    UINT32 prm = CLK_PWR_DEV_SPI;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&prm);
}

static void spi_tx_finish_int_clr()
{
    UINT32 value = 0;

    value = REG_READ(SPI_STAT);
    value |= TX_FINISH_INT;
    REG_WRITE(SPI_STAT, value);

    value = REG_READ(SPI_CN);
    value &= (~TX_EN) & (~TX_FIN_INT_EN);
    REG_WRITE(SPI_CN, value);

}

static void spi_rx_finish_int_clr()
{
    UINT32 value = 0;

    value = REG_READ(SPI_STAT);
    value |= RX_FINISH_INT;
    REG_WRITE(SPI_STAT, value);

    value = REG_READ(SPI_CN);
    value &= (~RX_EN) & (~RX_FIN_INT_EN);
    REG_WRITE(SPI_CN, value);

}

static int8_t spi_txfifo_get_ready(spi_tx_fifo_ready_e type)
{
    UINT32 value = 0;

    value = REG_READ(SPI_STAT);

    if(type == READ_READY)
    {
        value &= TXFIFO_RD_READY;
    }
    else if(type == WRITE_READY)
    {
        value &= TXFIFO_WR_READY;
    }
    else
    {
        value = -1;
    }

    return value;
}

static void spi_regist_tx(UINT8* txbuf)
{
    spi_trans.p_tx_buf = txbuf;
}

static void spi_regist_rx(UINT8* rxbuf)
{
    spi_trans.p_rx_buf = rxbuf;
}

static void spi_deregist()
{
    spi_trans.p_tx_buf = NULL;
    spi_trans.p_rx_buf = NULL;
}

static void spi_init_msten(UINT8 msten)
{
    UINT32 value = 0;

    spi_icu_configuration();
    spi_gpio_configuration(msten);

    if(msten == slave)
    {
        value = SPIEN
                & (~MSTEN)
                & (~LSB_FIRST)
                & (~WIRE3_EN)
                & (~SLV_RELEASE_INTEN)
                | (RXINT_EN)
                & (~TXINT_EN)
                & (~RXOVF_EN)
                & (~TXUDF_EN)
                & (~RXINT_LEVEL_BIT) | RXINT_LEVEL_X(0)
                & (~TXINT_LEVEL_BIT) | TXINT_LEVEL_X(0);
    }
    else if(msten == master)
    {
        value = SPIEN
                | MSTEN
                & (~LSB_FIRST)
                & (~WIRE3_EN)
                & (~RXINT_EN)
                & (~TXINT_EN)
                & (~RXOVF_EN)
                & (~TXUDF_EN)
                & (~RXINT_LEVEL_BIT) | RXINT_LEVEL_X(1)
                & (~TXINT_LEVEL_BIT) | TXINT_LEVEL_X(1);
    }
    REG_WRITE(SPI_CFG, value);

    value = REG_READ(SPI_STAT);
    value |= TXFIFO_CLR | RXFIFO_CLR;
    REG_WRITE(SPI_STAT, value);

}

void spi_init(void)
{
    SPI_PRT("%s, %d \r\n", __func__, __LINE__);
    intc_service_register(IRQ_SPI, PRI_IRQ_SPI, spi_isr);

    ddev_register_dev(SPI_DEV_NAME, &spi_op);

    memset(&spi_trans, 0, sizeof(spi_trans_t));
}

void spi_exit(void)
{
    ddev_unregister_dev(SPI_DEV_NAME);
}

static void spi_buf_trans(void)
{
    UINT32 value = 0;
    UINT32 data_num, i;

    spi_trans.send_done = 0;

    value = REG_READ(SPI_STAT);
    value |= TXFIFO_CLR;
    REG_WRITE(SPI_STAT, value);
 
    data_num = spi_trans.tx_remain_data_cnt>SPI_FIFO_DEPTH?SPI_FIFO_DEPTH:spi_trans.tx_remain_data_cnt;

    value = REG_READ(SPI_CN);
    value &= (~(0xFFF << TX_TRANS_LEN));
    value |= (data_num << TX_TRANS_LEN);
    REG_WRITE(SPI_CN, value);

    for(i=0; i<data_num; i++)
    {
        REG_WRITE(SPI_DAT, *(spi_trans.p_tx_buf+i));
    }

    spi_trans.tx_remain_data_cnt -= data_num;
    memcpy(spi_trans.p_tx_buf, spi_trans.p_tx_buf+i, spi_trans.tx_remain_data_cnt);

    value = REG_READ(SPI_CN);
    value |= TX_EN | TX_FIN_INT_EN;
    REG_WRITE(SPI_CN, value);

    while(!spi_trans.send_done);

    if(spi_trans.tx_remain_data_cnt)
    {
        spi_buf_trans();
    }
    else
    {
        SPI_PRT("spi_buf_trans end\r\n");
        return;
    }
    
}

static void spi_buf_receiv(void)
{
    UINT32 value = 0;
    UINT32 data_num, i = 0;


    while(spi_txfifo_get_ready(READ_READY) && spi_trans.rx_remain_data_cnt)
    {
        *(spi_trans.p_rx_buf+i) = (REG_READ(SPI_DAT) & 0xFF);
        spi_trans.rx_remain_data_cnt--;
        i++;
    }

    value = REG_READ(SPI_STAT);
    value |= RXFIFO_CLR;
    REG_WRITE(SPI_STAT, value);

    if(spi_trans.rx_remain_data_cnt)
    {
        value = REG_READ(SPI_CN);
        value &= (~(0xFFF << RX_TRANS_LEN));
        value |= (spi_trans.rx_remain_data_cnt << RX_TRANS_LEN) | RX_EN | RX_FIN_INT_EN ;
        REG_WRITE(SPI_CN, value);
    }
    else
    {
        spi_trans.trans_done = 1;
    }

}

UINT32 spi_write(char *user_buf, UINT32 count, UINT32 timeout)
{

    if(count <= 0)
    {
		return -1;	
	}

    if(spi_trans.p_tx_buf == NULL)
    {
        return -1;
    }

    //while(spi_trans.tx_remain_data_cnt);

    memcpy(spi_trans.p_tx_buf, user_buf, count);
    spi_trans.tx_remain_data_cnt = count;

    UINT32 value = 0;

    while(!spi_txfifo_get_ready(WRITE_READY));

    spi_buf_trans();

    return 0;
}

volatile UINT8 read_flag;
UINT32 spi_read(char *user_buf, UINT32 count, UINT32 timeout)
{
    UINT16 tempdata;
    UINT32 value = 0;

    if(count <= 0)
    {
		return -1;	
	}	

    if(spi_trans.p_rx_buf == NULL)
    {
        return -1;
    }

    value = REG_READ(SPI_CN);
    value &= (~(0xFFF << RX_TRANS_LEN));
    value |= (count << RX_TRANS_LEN) | RX_EN | RX_FIN_INT_EN ;
    value |= TX_EN | TX_FIN_INT_EN ;
    REG_WRITE(SPI_CN, value);

    spi_trans.trans_done = 0;

    memset(spi_trans.p_rx_buf, 0, count);
    spi_trans.rx_remain_data_cnt = count;

    //while(!spi_txfifo_get_ready(READ_READY));

    memcpy(user_buf, spi_trans.p_rx_buf, count);
    return 0;
}

static void spi_trans_done(UINT8 *trans_done)
{
    *trans_done = (spi_trans.trans_done == 1);
}

DRIVER_CTRL_RES spi_ctrl(UINT32 cmd, void *param)
{
    DRIVER_CTRL_RES ret = DRIV_SUCCESS;

    peri_busy_count_add();

    switch(cmd)
    {
    case CMD_SPI_UNIT_ENABLE:
        spi_active(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_MSTEN:
        spi_set_msten(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKPHA:
        spi_set_ckpha(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKPOL:
        spi_set_skpol(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_BITWIDTH:
        spi_set_bit_wdth(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKR:
        spi_set_ckr(*(UINT8 *)param);
        break;
    case CMD_SPI_RXINT_EN:
        spi_rxint_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_TXINT_EN:
        spi_txint_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_RXOVR_EN:
        spi_rxovr_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_TXOVR_EN:
        spi_txovr_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_RXFIFO_CLR:
        spi_rxfifo_clr();
        break;
    case CMD_SPI_TXFIFO_CLR:
        spi_txfifo_clr();
        break;
    case CMD_SPI_RXINT_MODE:
        spi_rxint_mode(*(UINT8 *)param);
        break;
    case CMD_SPI_TXINT_MODE:
        spi_txint_mode(*(UINT8 *)param);
        break;
    case CMD_SPI_INIT_MSTEN:
        spi_init_msten(*(UINT8 *)param);
        break;
    case CMD_SPI_REGIST_TX:
        spi_regist_tx((UINT8 *)param);
        break;
    case CMD_SPI_REGIST_RX:
        spi_regist_rx((UINT8 *)param);
        break;
    case CMD_SPI_DEREGIST:
        spi_deregist();
        break;
    case CMD_SPI_TRANS_DONE:
        spi_trans_done((UINT8 *)param);
        break;
    default:
        ret = DRIV_FAIL;
        break;
    }

    peri_busy_count_dec();

    return ret;
}

void spi_isr(void)
{
    UINT8 i;
    UINT8 txfin, rxfin, slave_int, rxfin_int;
    UINT32 ctrl, status, cn;
    volatile UINT8 fifo_empty_num, data_num, rxfifo_empty;

	data_num = 0; /*fix warning by clang analyzer*/
	fifo_empty_num = 0; /*fix warning by clang analyzer*/
	
    ctrl = REG_READ(SPI_CFG);
    status = REG_READ(SPI_STAT);
    cn = REG_READ(SPI_CN);

    SPI_PRT("%s, %d SPI_STAT 0x%x, SPI_CFG 0x%x, SPI_CN 0x%x\r\n", __func__, __LINE__, status, ctrl, cn);

    txfin = (status & TX_FINISH_INT)?1:0;
    rxfin = (status & RXFIFO_INT)?1:0;
    rxfin_int = (status & RX_FINISH_INT)?1:0;

    slave_int = (status & SLV_RELS_INT)?1:0;

    if(slave_int)
    {
        UINT32 value = 0;

        value = REG_READ(SPI_STAT);
        value |= SLV_RELS_INT;
        REG_WRITE(SPI_STAT, value);

        read_flag = ~read_flag;

    }


    if(txfin)
    {

        spi_tx_finish_int_clr();

        spi_trans.send_done = 1;

    }

    if(rxfin | rxfin_int)
    {
        spi_rx_finish_int_clr();

        if(spi_trans.rx_remain_data_cnt)
        {
            spi_buf_receiv();
        }
    }

}

#endif //__SPI_DRIVER__

