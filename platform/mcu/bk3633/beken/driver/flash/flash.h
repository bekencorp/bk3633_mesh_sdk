/**
 ****************************************************************************************
 *
 * @file flash.h
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken Leonardo 2009-2021
 *
 *
 ****************************************************************************************
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>               // standard integer functions
#include "BK3633_RegList.h"


#define REG_FLASH_OPERATE_SW            (&addFLASH_Reg0x0)
#define REG_FLASH_DATA_SW_FLASH         (&addFLASH_Reg0x1)
#define REG_FLASH_DATA_FLASH_SW         (&addFLASH_Reg0x2)
#define REG_FLASH_RDID_DATA_FLASH       (&addFLASH_Reg0x4)
#define REG_FLASH_SR_DATA_CRC_CNT       (&addFLASH_Reg0x5)
#define REG_FLASH_CONF                  (&addFLASH_Reg0x7)

#define BIT_ADDRESS_SW                  posFLASH_Reg0x0_addr_sw_reg
#define SET_ADDRESS_SW                  bitFLASH_Reg0x0_addr_sw_reg

#define BIT_OP_TYPE_SW                  posFLASH_Reg0x0_op_type_sw
#define SET_OP_TYPE_SW                  bitFLASH_Reg0x0_op_type_sw

#define BIT_OP_SW                       posFLASH_Reg0x0_op_sw
#define SET_OP_SW                       (1 << BIT_OP_SW)
#define BIT_WP_VALUE                    posFLASH_Reg0x0_wp_value
#define DISABLE_WP_VALUE                (1 << BIT_WP_VALUE)
#define BIT_BUSY_SW                     posFLASH_Reg0x0_busy_sw
#define FLASH_BUSY_BIT                  (0x1      << BIT_BUSY_SW)

#define BIT_FLASH_CLK_CONF              posFLASH_Reg0x7_flash_clk_conf
#define FLASH_CLK_MASK                  bitFLASH_Reg0x7_flash_clk_conf
#define BIT_MODE_SEL                    posFLASH_Reg0x7_mode_sel
#define BIT_FWREN_FLASH_CPU             posFLASH_Reg0x7_fwren_flash_cpu
#define BIT_WRSR_DATA                   posFLASH_Reg0x7_wrsr_data
#define WRSR_DATA_MASK                  bitFLASH_Reg0x7_wrsr_data
#define BIT_CRC_EN                      posFLASH_Reg0x7_crc_en

#define MODEL_SEL_MASK					0x1F
#define SET_MODE_SEL                    (MODEL_SEL_MASK << BIT_MODE_SEL)
#define SET_FWREN_FLASH_CPU             (1 << BIT_FWREN_FLASH_CPU)
#define SET_WRSR_DATA                   bitFLASH_Reg0x7_wrsr_data
#define SET_CRC_EN                      bitFLASH_Reg0x7_crc_en

#define MASK_SR_DATA_FLASH              bitFLASH_Reg0x5_sr_data_flash
#define M_VALUE_MASK					bitFLASH_Reg0x5_m_value
#define M_VALUE_POST					posFLASH_Reg0x5_m_value

///Flash type code used to select the correct erasing and programming algorithm
#define FLASH_TYPE_UNKNOWN             0

/// Common for EMBED BEKEN FLASH Family

#define GD_FLASH_1	 0XC84013
#define GD_MD25D40   0x514013
#define GD_GD25WD40  0xc86413


#define MX_FLASH_1	0XC22314
#define XTX_FLASH_1	0X1C3113

#define BY25Q80		0xe04014
#define PN25f04		0xe04013
#define MX_FLASH_4M 0XC22313

#define P25Q40U     0X856013


/// flash operation command type(decimal)
typedef enum {
	FLASH_OPCODE_WREN    = 1,
	FLASH_OPCODE_WRDI    = 2,
	FLASH_OPCODE_RDSR    = 3,
	FLASH_OPCODE_WRSR    = 4,
	FLASH_OPCODE_READ    = 5,
	FLASH_OPCODE_RDSR2   = 6,
	FLASH_OPCODE_WRSR2   = 7,
	FLASH_OPCODE_PP      = 12,
	FLASH_OPCODE_SE      = 13,
	FLASH_OPCODE_BE1     = 14,
	FLASH_OPCODE_BE2     = 15,
	FLASH_OPCODE_CE      = 16,
	FLASH_OPCODE_DP      = 17,
	FLASH_OPCODE_RFDP    = 18,
	FLASH_OPCODE_RDID    = 20,
	FLASH_OPCODE_HPM     = 21,
	FLASH_OPCODE_CRMR    = 22,
	FLASH_OPCODE_CRMR2   = 23,
} FLASH_OPCODE;

typedef struct
{
    uint32_t flash_id;
    uint8_t  sr_size;
    uint8_t  line_mode;
    uint8_t  cmp_post;
    uint8_t  protect_post;
    uint8_t  protect_mask;
    uint16_t protect_all;
    uint16_t protect_none;
    uint16_t protect_half;
    uint16_t unprotect_last_block;
    uint8_t  qe_bit_post;
    uint8_t  qe_bit;
    uint8_t  m_value;
    uint8_t  mode_sel;
    uint8_t  flash_clk;
    uint16_t flash_size; ///unit:sector(4Kbytes)
} flash_config_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize flash driver.
 ****************************************************************************************
 */
void flash_init(void);

/**
 ****************************************************************************************
 * @brief   Identify the flash device.
 *
 * This function is used to read the flash device ID.
 * 
 *
 ****************************************************************************************
 */
UINT32 flash_get_id(void);

/**
 ****************************************************************************************
 * @brief   Write a flash section.
 *
 * This function is used to write a part of the flash memory.
 * 
 * Note: callback parameter is not used
 *
 * @param[in]    flash_type  Flash type
 * @param[in]    offset      Starting offset from the beginning of the flash device
 * @param[in]    length      Size of the portion of flash to write
 * @param[in]    buffer      Pointer on data to write
 * @param[in]    callback    Callback for end of write
 * @return       status      0 if operation can start successfully
 ****************************************************************************************
 */
UINT32 flash_write(char *user_buf, UINT32 count, UINT32 address);

/**
 ****************************************************************************************
 * @brief   Read a flash section.
 *
 * This function is used to read a part of the flash memory.
 * 
 * Note: callback parameter is not used
 *
 * @param[in]    flash_type  Flash type
 * @param[in]    offset      Starting offset from the beginning of the flash device
 * @param[in]    length      Size of the portion of flash to read
 * @param[out]   buffer      Pointer on data to read
 * @param[in]    callback    Callback for end of read
 * @return       status      0 if operation can start successfully
 ****************************************************************************************
 */
UINT32 flash_read(char *user_buf, UINT32 count, UINT32 address);

UINT32 flash_open(UINT32 op_flag);

UINT32 flash_close(void);




void flash_clk_conf(uint8_t clk_sel,uint8_t clk_src,uint8_t div);
void flash_set_line_mode(uint8_t mode);
void flash_read_data (UINT8 *buffer, UINT32 address, UINT32 len);
void flash_write_data (UINT8 *buffer, UINT32 address, UINT32 len);
void flash_set_dual_mode(void);
UINT32 flash_ctrl(UINT32 cmd, void *parm);

#endif // FLASH_H_

