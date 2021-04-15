#ifndef _SPI_H_
#define _SPI_H_

#include "BK3633_RegList.h"

#define SPI_DEBUG

#ifdef SPI_DEBUG
#define SPI_PRT      os_printf
#define SPI_WARN     warning_prf
#define SPI_FATAL    fatal_prf
#else
#define SPI_PRT      null_prf
#define SPI_WARN     null_prf
#define SPI_FATAL    null_prf
#endif

#define SPI_CFG                           &(addSPI0_Reg0x0)

#define TXINT_LEVEL_BIT                    bitSPI0_Reg0x0_TXFIFO_INT_LEVEL
#define TXINT_LEVEL_X(val)                 (val << posSPI0_Reg0x0_TXFIFO_INT_LEVEL)

#define RXINT_LEVEL_BIT                    bitSPI0_Reg0x0_RXFIFO_INT_LEVEL
#define RXINT_LEVEL_X(val)                 (val << posSPI0_Reg0x0_RXFIFO_INT_LEVEL)


#define TXUDF_EN                           (0x01UL << posSPI0_Reg0x0_TXUDF_EN)
#define RXOVF_EN                           (0x01UL << posSPI0_Reg0x0_RXOVF_EN)
#define TXINT_EN                           (0x01UL << posSPI0_Reg0x0_TXFIFO_INT_EN)
#define RXINT_EN                           (0x01UL << posSPI0_Reg0x0_RXFIFO_INT_EN)
#define SLV_RELEASE_INTEN                  (0x01UL << posSPI0_Reg0x0_SLV_RELEASE_INTEN)
#define SPI_CKR_POSI                       posSPI0_Reg0x0_SPI_CKR
#define SPI_CKR_MASK                       bitSPI0_Reg0x0_SPI_CKR
#define WIRE3_EN                           (0x01UL << posSPI0_Reg0x0_WIRE3_EN)
#define BIT_WDTH                           (0x01UL << posSPI0_Reg0x0_BIT_WDTH)
#define CKPOL                              (0x01UL << posSPI0_Reg0x0_CKPOL)
#define CKPHA                              (0x01UL << posSPI0_Reg0x0_CKPHA)
#define MSTEN                              (0x01UL << posSPI0_Reg0x0_MSTEN)
#define SPIEN                              (0x01UL << posSPI0_Reg0x0_SPIEN)
#define LSB_FIRST                          (0x01UL << posSPI0_Reg0x0_LSB_FIRST)

#define SPI_CN                              &(addSPI0_Reg0x1)
#define TX_EN                               (0x01UL << posSPI0_Reg0x1_TX_EN)
#define RX_EN                               (0x01UL << posSPI0_Reg0x1_RX_EN)
#define TX_FIN_INT_EN                       (0x01UL << posSPI0_Reg0x1_TX_FINISH_INT_EN)
#define RX_FIN_INT_EN                       (0x01UL << posSPI0_Reg0x1_RX_FINISH_INT_EN)
#define TX_TRANS_LEN                        posSPI0_Reg0x1_TX_TRANS_LEN
#define RX_TRANS_LEN                        posSPI0_Reg0x1_RX_TRANS_LEN

#define SPI_STAT                           &(addSPI0_Reg0x2)
#define TXFIFO_WR_READY                    (0x01UL << posSPI0_Reg0x2_TXFIFO_WR_READY)
#define TXFIFO_RD_READY                    (0x01UL << posSPI0_Reg0x2_RXFIFO_RD_READY)
#define TXFIFO_INT                         (0x01UL << posSPI0_Reg0x2_TXFIFO_INT)
#define RXFIFO_INT                         (0x01UL << posSPI0_Reg0x2_RXFIFO_INT)
#define SLV_RELS_INT                       (0x01UL << posSPI0_Reg0x2_SLV_RELEASE_INT)
#define TXUDF                              (0x01UL << posSPI0_Reg0x2_TXUDF)
#define RXOVF                              (0x01UL << posSPI0_Reg0x2_RXOVF)
#define TX_FINISH_INT                      (0x01UL << posSPI0_Reg0x2_TX_FINISH_INT)
#define RX_FINISH_INT                      (0x01UL << posSPI0_Reg0x2_RX_FINISH_INT)
#define TXFIFO_CLR                         (0x01UL << posSPI0_Reg0x2_TXFIFO_CLR)
#define RXFIFO_CLR                         (0x01UL << posSPI0_Reg0x2_RXFIFO_CLR)

#define SPI_DAT                            &(addSPI0_Reg0x3)
#define SPI_DAT_POSI                       (0)
#define SPI_DAT_MASK                       (0xFFFF)

#define SPI_FIFO_DEPTH                      64

typedef enum
{
    READ_READY,
    WRITE_READY
} spi_tx_fifo_ready_e;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
DRIVER_CTRL_RES spi_ctrl(UINT32 cmd, void *param);
UINT32 spi_read(char *user_buf, UINT32 count, UINT32 timeout);
UINT32 spi_write(char *user_buf, UINT32 count, UINT32 timeout);
#endif //_SPI_H_
