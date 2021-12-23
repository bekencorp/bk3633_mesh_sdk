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
//#include <stdint.h>               // standard integer functions
#define AHB_FLASH_BASE                   0x00802000
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
#define SET_MODE_DUAL                   (0x01 & 0x1F) << BIT_MODE_SEL
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

#define addFLASH_Reg0x5                                         *((volatile unsigned long *) (0x00802000+0x5*4))
#define set_FLASH_Reg0x5_pw_write(val)                          addFLASH_Reg0x5 = ((addFLASH_Reg0x5 & (~0x40000000)) | ((val) << 30))

#define addFLASH_Reg0x9                                         *((volatile unsigned long *) (0x00802000+0x9*4))
#define posFLASH_Reg0x9_mem_addr_clr                            31
#define bitFLASH_Reg0x9_mem_addr_clr                            0x80000000
#define set_FLASH_Reg0x9_mem_addr_clr(val)                      addFLASH_Reg0x9 = ((addFLASH_Reg0x9 & (~0x80000000)) | ((val) << 31))
#define setf_FLASH_Reg0x9_mem_addr_clr                          addFLASH_Reg0x9 |= 0x80000000
#define clrf_FLASH_Reg0x9_mem_addr_clr                          addFLASH_Reg0x9 &= ~0x80000000
#define get_FLASH_Reg0x9_mem_addr_clr                           ((addFLASH_Reg0x9 & 0x80000000) >> 31)

#define posFLASH_Reg0x9_mem_data                                0
#define bitFLASH_Reg0x9_mem_data                                0xFF
#define set_FLASH_Reg0x9_mem_data(val)                          addFLASH_Reg0x9 = ((addFLASH_Reg0x9 & (~0xFF)) | ((val) << 0))
#define get_FLASH_Reg0x9_mem_data                               (addFLASH_Reg0x9 & 0xFF)

/// Common for EMBED BEKEN FLASH Family

#define FLASH_PAGE_SIZE                                    (256)
#define FLASH_PAGE_MASK                                   (FLASH_PAGE_SIZE - 1)
#define FLASH_ERASE_SECTOR_CNT                              (12)
#define FLASH_ERASE_SECTOR_SIZE                           (4096)
#define FLASH_ERASE_SECTOR_SIZE_MASK                      (FLASH_ERASE_SECTOR_SIZE - 1)
#define UPDATE_CHUNK_SIZE                                 (32)
#define GD_FLASH_1   0XC84013
#define GD_MD25D40   0x514013
#define GD_GD25WD40  0xc86413
#define MX_FLASH_1  0XC22314
#define XTX_FLASH_1 0X1C3113
#define BY25Q80     0xe04014
#define PN25f04     0xe04013
#define MX_FLASH_4M 0XC22313
#define P25Q40U     0X856013

#define FLASH_ADDR_FIX  0X7D000

#define BACK_BUFF_SIZE          16
/// flash operation command type(decimal)
typedef enum
{
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

typedef enum
{
    FLASH_SEC_ERASE_HEAD = 1,
    FLASH_SEC_ERASE_TAIL,
}FLASH_SEC_ERASE_DIRECT;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

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
uint8_t flash_erase(uint32_t offset, uint32_t size);
void flash_erase_one_block(uint32_t address);
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
uint8_t flash_write(uint8_t *buffer,uint32_t address, uint32_t len);
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
uint8_t flash_read(uint8_t *buffer,uint32_t offset, uint32_t length);
void flash_wp_none( void );
void flash_wp_ALL( void );
void set_flash_clk(unsigned char clk_conf) ;
void flash_write_sr(unsigned char bytes,  unsigned short val);
void flash_read_data (uint8_t *buffer, uint32_t address, uint32_t len);
void flash_write_data (uint8_t *buffer, uint32_t address, uint32_t len);
void flash_set_dual_mode(void);
void bim_flash_init(void);
#endif // FLASH_H_
