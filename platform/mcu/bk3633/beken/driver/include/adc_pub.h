#ifndef _ADC_PUB_H_
#define _ADC_PUB_H_

#define ADC_FAILURE                (1)
#define ADC_SUCCESS                (0)

#define ADC_DEV_NAME                "adc"

//#define SPI_CMD_MAGIC              (0xe250000)
//enum
//{

//};
/*
typedef struct
{
    UINT8 tx_remain_data_cnt;
    UINT8 rx_remain_data_cnt;
    UINT8 *p_tx_buf;
    UINT8 *p_rx_buf;
    UINT8 trans_done;
} spi_trans_t;*/
/*
typedef enum
{
    i2c_slave,
    i2c_master
} i2c_msten;*/

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void adc_init(void);
void adc_exit(void);
void uart_check_low_volt_and_sleep(void);

#endif //_I2C_PUB_H_
