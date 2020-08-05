/**
 ****************************************************************************************
 *
 * @file flash.h
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>               // standard integer functions

#define AHB_FLASH_BASE			         0x00802000
#define REG_FLASH_OPERATE_SW            (*((volatile unsigned long *)   (AHB_FLASH_BASE+0*4)))
#define REG_FLASH_DATA_SW_FLASH         (*((volatile unsigned long *)   (AHB_FLASH_BASE+1*4)))
#define REG_FLASH_DATA_FLASH_SW         (*((volatile unsigned long *)   (AHB_FLASH_BASE+2*4)))
#define REG_FLASH_RDID_DATA_FLASH       (*((volatile unsigned long *)   (AHB_FLASH_BASE+4*4)))
#define REG_FLASH_SR_DATA_CRC_CNT       (*((volatile unsigned long *)   (AHB_FLASH_BASE+5*4)))
#define REG_FLASH_CONF                  (*((volatile unsigned long *)   (AHB_FLASH_BASE+7*4)))
#define flash_200k_ADDR                 (*((volatile unsigned long *)    (0x00032000)))
#define BIT_ADDRESS_SW                  0
#define BIT_OP_TYPE_SW                  24
#define BIT_OP_SW                       29
#define BIT_WP_VALUE                    30
#define BIT_BUSY_SW                     31

#define SET_ADDRESS_SW                  0xFFFFFF << BIT_ADDRESS_SW
#define SET_OP_TYPE_SW                  0x1F     << BIT_OP_TYPE_SW
#define SET_OP_SW                       0x1      << BIT_OP_SW
#define SET_WP_VALUE                    0x1      << BIT_WP_VALUE
#define SET_BUSY_SW                     0x1      << BIT_BUSY_SW

#define BIT_FLASH_CLK_CONF              0
#define BIT_MODE_SEL                    4
#define BIT_FWREN_FLASH_CPU             9
#define BIT_WRSR_DATA                   10
#define BIT_CRC_EN                      26

#define SET_FLASH_CLK_CONF              0xF      << BIT_FLASH_CLK_CONF
#define SET_MODE_SEL                    0x1F     << BIT_MODE_SEL
#define SET_FWREN_FLASH_CPU             0x1      << BIT_FWREN_FLASH_CPU
#define SET_WRSR_DATA                   0xFFFF   << BIT_WRSR_DATA
#define SET_CRC_EN                      0x1      << BIT_CRC_EN

#define BIT_SR_DATA_FLASH               0
#define BIT_CRC_ERR_COUNTER             8
#define BIT_DATA_FLASH_SW_SEL           16
#define BIT_DATA_SW_FLASH_SEL           19

#define SET_SR_DATA_FLASH               0xFF     << BIT_SR_DATA_FLASH
#define SET_CRC_ERR_COUNTER             0xFF     << BIT_CRC_ERR_COUNTER
#define SET_DATA_FLASH_SW_SEL           0x7      << BIT_DATA_FLASH_SW_SEL
#define SET_DATA_SW_FLASH_SEL           0x7      << BIT_DATA_SW_FLASH_SEL


///Flash type code used to select the correct erasing and programming algorithm
#define FLASH_TYPE_UNKNOWN             0
#define FLASH_SPACE_TYPE_MAIN          0x3435
#define FLASH_TYPE_EMBEN_BK3435        FLASH_SPACE_TYPE_MAIN


///Base address of Flash on system bus
#define FLASH_MAIN_BASE_ADDR         0x00000000



#define FLASH_MAIN_SIZE              0x00040000

/// Common for EMBED BEKEN FLASH Family

#define FLASH_SECT_SIZE2         0x00001000  //  4 KByte
#define FLASH_BOUNDARY256B       0x00000100  // 256 Byte

#define FLASH_PAGE_SIZE                                    (256)
#define FLASH_PAGE_MASK                                   (FLASH_PAGE_SIZE - 1)
#define FLASH_ERASE_SECTOR_SIZE_RSL_BIT_CNT               (12)
#define FLASH_ERASE_SECTOR_SIZE                           (4096)
#define FLASH_ERASE_SECTOR_SIZE_MASK                      (FLASH_ERASE_SECTOR_SIZE - 1)
#define UPDATE_CHUNK_SIZE                                 (32)

#define GD_FLASH_1	 0XC84013
#define GD_MD25D40   0x514013
#define GD_GD25WD40  0xc86413


#define MX_FLASH_1	0XC22314
#define XTX_FLASH_1	0X1C3113

#define BY25Q80		0xe04014
#define PN25f04		0xe04013
#define MX_FLASH_4M 0XC22313

#define P25Q40U     0X856013




#define FLASH_LINE_1  1
#define FLASH_LINE_2  2
#define FLASH_LINE_4  4

#define DEFAULT_LINE_MODE  FLASH_LINE_4

#define FLASH_ADDR_FIX  0X7D000
#define FLASH_WRITE_ENABLE1  0XA6
#define FLASH_WRITE_ENABLE2  0XB3
#define FLASH_WRITE_ENABLE3  0XC2
#define FLASH_WRITE_ENABLE4  0XD1
#define FLASH_ERASE_ENABLE1  0XAB
#define FLASH_ERASE_ENABLE2  0XBC


#define MAX(x, y)                  (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                  (((x) < (y)) ? (x) : (y))
#define max(x, y)                  (((x) > (y)) ? (x) : (y))
#define min(x, y)                  (((x) < (y)) ? (x) : (y))

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
extern struct flash_env_tag flash_env;

void flash_advance_init(void);

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
 * Note: callback parameter is not used
 *
 * @param[out]   id          Pointer to id location
 * @param[in]    callback    Callback for end of identification
 * @return       status      0 if operation can start successfully
 ****************************************************************************************
 */
uint8_t flash_identify(uint32_t* id, void (*callback)(void));

/**
 ****************************************************************************************
 * @brief   Erase a flash section.
 *
 * This function is used to erase a part of the flash memory.
 * 
 * Note: callback parameter is not used
 *
 * @param[in]    flash_type  Flash type
 * @param[in]    offset      Starting offset from the beginning of the flash device
 * @param[in]    size        Size of the portion of flash to erase
 * @param[in]    callback    Callback for end of erase
 * @return       status      0 if operation can start successfully
 ****************************************************************************************
 */
uint8_t flash_erase(uint32_t flash_id, uint32_t offset, uint32_t size, void (*callback)(void));

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




void flash_clk_conf(uint8_t clk_sel,uint8_t clk_src,uint8_t div);

void flash_set_qe(void);
void flash_write_sr(unsigned char bytes,  unsigned short val);
void flash_test(void);
void flash_set_line_mode(uint8_t mode);
void flash_read_data (UINT8 *buffer, UINT32 address, UINT32 len);
void flash_write_data (UINT8 *buffer, UINT32 address, UINT32 len);
void flash_set_dual_mode(void);
UINT32 flash_ctrl(UINT32 cmd, void *parm);
void flash_wp_256k( void);

#endif // FLASH_H_

