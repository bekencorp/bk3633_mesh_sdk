/**
 ****************************************************************************************
 *
 * @file flash.c
 *
 * @brief Flash driver interface
 *
 * Copyright (C) Beken Leonardo 2009-2021
 *
 *
 ****************************************************************************************
 */

#include "include.h"
#include "arm_arch.h"
#include "sys_config.h"

#include "flash.h"

#include "flash_pub.h"

#include "drv_model_pub.h"

#include <string.h>
#include "uart_pub.h"

#include "mcu_ps_pub.h"
#include "ll.h"


#define MODE_STD     0
#define MODE_DUAL    1
#define MODE_QUAD    2

#define FLASH_ADDR_FIX  0X7D000

extern void  _app_data_flash_end;

static DD_OPERATIONS flash_op =
{
    flash_open,
    flash_close,
    flash_read,
    flash_write,
    flash_ctrl
};

UINT32 flash_mid = 0;

static const flash_config_t flash_config[] =
{
	/// for BK3435
	{0x1C3113, 1, 2, 16, 2, 0x0F, 0x0F, 0x00, 0x0B, 0x0D | (0 << 8), 16, 0, 0xA0, 0x01, 0x1, 0x080}, //XTX_FLASH_1
	{0xC84013, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0B, 0x13 | (1 << 8),  9, 1, 0xA0, 0x01, 0x1, 0x080}, //GD_25Q40
	{0x514013, 1, 2, 16, 2, 0x07, 0x07, 0x00, 0x06, 0x02 | (0 << 8), 16, 0, 0xA0, 0x01, 0x2, 0x080}, //GD_MD25D40D
	{0x514014, 1, 2, 16, 2, 0x07, 0x07, 0x00, 0x00, 0x02 | (0 << 8), 16, 0, 0xA0, 0x01, 0x1, 0x100}, //GD_MD25D80D
	{0x000000, 1, 2, 16, 2, 0x0F, 0x0F, 0x00, 0x00, 0x00 | (0 << 8), 16, 0, 0xA0, 0x01, 0x2, 0x000}, //default

};

static const flash_config_t *flash_current_config = NULL;

static UINT8 flash_sw_busy(void)
{
    UINT32 value;

    value = REG_READ(REG_FLASH_OPERATE_SW);

    return (value & FLASH_BUSY_BIT)?1:0;
}

static void flash_get_current_flash_config(void)
{
    int i;
    for(i = 0; i < (sizeof(flash_config) / sizeof(flash_config_t) - 1); i++)
    {
        if(flash_mid == flash_config[i].flash_id)
        {
            flash_current_config = &flash_config[i];
            UINT16 size = flash_current_config->flash_size*4;
            os_printf("flash #%d, /%d, size:%dKB\r\n", i, flash_current_config->flash_clk, size);
            break;
        }
    }
    if(i == (sizeof(flash_config) / sizeof(flash_config_t) - 1))
    {
        flash_current_config = &flash_config[i];
		os_printf("flash id : 0x%x\r\n", flash_mid);
        os_printf("don't config this flash, choose default config\r\n");
    }
    os_printf("code area end addr:0x%X\r\n", &_app_data_flash_end);
}

UINT32 flash_get_id(void)
{
    UINT32 value, i;

	while(flash_sw_busy());

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value &= (~SET_OP_TYPE_SW);
    value |= (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW) | SET_OP_SW;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
	
	while(flash_sw_busy());

    for (i = 0; i < 8; i++)
    {
        REG_WRITE(REG_FLASH_DATA_SW_FLASH, 0xffffffff);
    }

    value = REG_READ(REG_FLASH_RDID_DATA_FLASH);

	return value ;
}

static UINT16 flash_read_sr(void)
{
	UINT32 value;

	while(flash_sw_busy());

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value &= (~SET_OP_TYPE_SW);
    value |= (FLASH_OPCODE_RDSR << BIT_OP_TYPE_SW) | SET_OP_SW;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

	while(flash_sw_busy());

    value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);
    value &= MASK_SR_DATA_FLASH;

	return value;
}

void flash_write_sr(UINT16 val)
{
    UINT32 value;

    value = REG_READ(REG_FLASH_CONF);
    value &= (~WRSR_DATA_MASK);
    value |= (val << BIT_WRSR_DATA)|SET_FWREN_FLASH_CPU;
    REG_WRITE(REG_FLASH_CONF, value);

    while(flash_sw_busy());

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value &= (~SET_OP_TYPE_SW);

	if(flash_mid == GD_MD25D40)
    {
	    value |= (FLASH_OPCODE_WRSR << BIT_OP_TYPE_SW)
                | SET_OP_SW | DISABLE_WP_VALUE;
	}
	else 
	{
		value |= (FLASH_OPCODE_WRSR2 << BIT_OP_TYPE_SW)
                | SET_OP_SW | DISABLE_WP_VALUE;
	}

    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(flash_sw_busy());

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = FLASH_ADDR_FIX;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(flash_sw_busy());
}

UINT8 flash_get_line_mode(void)
{
    return flash_current_config->line_mode;
}

static void flash_set_qwfr(void)
{
    UINT32 value;

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(MODEL_SEL_MASK << BIT_MODE_SEL);
    value |= (flash_current_config->mode_sel << BIT_MODE_SEL);
    REG_WRITE(REG_FLASH_CONF, value);
}

void flash_set_line_mode(UINT8 mode)
{
    UINT32 value;
    
    if(1 == mode)
    {
        clr_flash_qwfr();
    }
    if(2 == mode)
    {
        clr_flash_qwfr();
        value = REG_READ(REG_FLASH_CONF);
        value &= ~(MODEL_SEL_MASK << BIT_MODE_SEL);
        value |= (MODE_DUAL << BIT_MODE_SEL);
        REG_WRITE(REG_FLASH_CONF, value);

    }
    else if(4 == mode)
    {
        clr_flash_qwfr();
        value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);
        value &= ~M_VALUE_MASK;
        value |= (flash_current_config->m_value<< M_VALUE_POST);
        REG_WRITE(REG_FLASH_SR_DATA_CRC_CNT, value);

        flash_set_qwfr();
    }
}

void flash_erase(UINT32 cmd, UINT32 address)
{
    UINT32 value;
    UINT32 type;

    switch(cmd)
    {
        case CMD_FLASH_ERASE_SECTOR:
            type = FLASH_OPCODE_SE;
            break;
        case CMD_FLASH_ERASE_BLOCK1:
            type = FLASH_OPCODE_BE1;
            break;
        case CMD_FLASH_ERASE_BLOCK2:
            type = FLASH_OPCODE_BE2;
            break;
        default:
            return;
    }

    GLOBAL_INT_DISABLE();
    while(flash_sw_busy());

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value &= (~SET_ADDRESS_SW);
    value &= (~SET_OP_TYPE_SW);
    value = (address << BIT_ADDRESS_SW)
             | (type << BIT_OP_TYPE_SW)
             | SET_OP_SW;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(flash_sw_busy());
    GLOBAL_INT_RESTORE();

}

void flash_read_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, value;
    UINT32 addr = address & (~0x1F);
    UINT32 buf[8];
    UINT8 *pb = (UINT8 *)&buf[0];

    if(len == 0)
    {
        return;
    }

    while(flash_sw_busy());

    while(len)
    {
        value = REG_READ(REG_FLASH_OPERATE_SW);
        value &= (~SET_ADDRESS_SW);
        value &= (~SET_OP_TYPE_SW);
        value = (addr << BIT_ADDRESS_SW)
                    | (FLASH_OPCODE_READ << BIT_OP_TYPE_SW)
                    | SET_OP_SW;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);

        while(flash_sw_busy());

        addr += 32;

        for (i = 0; i < 8; i++)
        {
            buf[i] = REG_READ(REG_FLASH_DATA_FLASH_SW);
        }

        for(i = address % 32; i < 32; i++)
        {
            *buffer++ = pb[i];
            address++;
            len--;
            if(len == 0)
            {
                break;
            }
        }
    }
}

void flash_write_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, value;
    UINT32 addr = address & (~0x1F);
    UINT32 buf[8];
    UINT8 *pb = (UINT8 *)&buf[0];


    if (len == 0)
        return;
/*     if (address<0x40000)
        return; */

    if(address % 32)
    {
        flash_read_data(pb, addr, 32);
    }
    else
    {
        memset(pb, 0xFF, 32);
    }

    while(flash_sw_busy());

    while(len)
    {
        if(len < 32)
        {
            flash_read_data(pb, addr, 32);
        }

        for (i = address % 32; i < 32; i++)
        {
            pb[i] = *buffer++;
            address++;
            len--;
            if (len == 0)
                break;
        }

        for (i = 0; i < 8; i++)
        {
            REG_WRITE(REG_FLASH_DATA_SW_FLASH, buf[i]);
        }

        while(flash_sw_busy());

        value = REG_READ(REG_FLASH_OPERATE_SW);
        value &= (~SET_ADDRESS_SW);
        value &= (~SET_OP_TYPE_SW);
        value = (addr << BIT_ADDRESS_SW)
                     | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                     | SET_OP_SW;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);

        while(flash_sw_busy());
        addr += 32;
        memset(pb, 0xFF, 32);
    }
}

void flash_write_page_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, value;
    UINT32 addr = address & (~0xFF);

    //printf("%s, addr 0x%x, address 0x%x\n", __func__, addr, address);
    if (len == 0)
        return;

    UINT8 *buf = os_malloc(256);
    if (!buf) {
        return;
    }

    UINT8 *pb = (UINT8 *)&buf[0];

    if(address % 256)
    {
        flash_read_data(pb, addr, 256);
    }
    else
    {
        memset(pb, 0xFF, 256);
    }

    while(flash_sw_busy());

    set_FLASH_Reg0x5_pw_write(1);
    while(len)
    {
        if(len < 256)
        {
            flash_read_data(pb, addr, 256);
        }

        for (i = address % 256; i < 256; i++)
        {
            pb[i] = *buffer++;
            address++;
            len--;
            if (len == 0)
                break;
        }

        // Clear the data before write the flash.
        set_FLASH_Reg0x9_mem_addr_clr(1);
        for (i = 0; i < 256; i++)
        {
            REG_FLASH_DATA_WRITE_MEM(buf[i]);
        }

        while(flash_sw_busy());

        value = REG_READ(REG_FLASH_OPERATE_SW);
        value &= (~SET_ADDRESS_SW);
        value &= (~SET_OP_TYPE_SW);
        value = (addr << BIT_ADDRESS_SW)
                     | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                     | SET_OP_SW;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);

        while(flash_sw_busy());
        addr += 256;
        memset(pb, 0xFF, 256);
    }
    set_FLASH_Reg0x5_pw_write(0);

    os_free(buf);
}

void clr_flash_qwfr(void)
{
    UINT32 value;
    UINT32 temp0;

	while(flash_sw_busy());

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(MODEL_SEL_MASK << BIT_MODE_SEL);
    //value |= (0xD << BIT_MODE_SEL);
    REG_WRITE(REG_FLASH_CONF, value);

    //reset flash
    value = REG_READ(REG_FLASH_OPERATE_SW);
    value &= (~SET_OP_TYPE_SW);
    value = (FLASH_OPCODE_CRMR2 << BIT_OP_TYPE_SW) | SET_OP_SW;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(flash_sw_busy());

}

void flash_set_dual_mode(void)
{
    UINT32 value;

    clr_flash_qwfr();

    value = REG_READ(REG_FLASH_CONF);
    value &= (~(MODEL_SEL_MASK<<BIT_MODE_SEL));
    value |= ((MODE_DUAL & MODEL_SEL_MASK)<<BIT_MODE_SEL);
    REG_WRITE(REG_FLASH_CONF, value);

    while(flash_sw_busy());
}

void set_flash_clk(unsigned char clk_conf) 
{
	//note :>16M don't use la for flash debug
    GLOBAL_INT_DISABLE();
    UINT32 value;
    value = REG_READ(REG_FLASH_CONF);
    value &= (~FLASH_CLK_MASK);
    value |= (clk_conf << BIT_FLASH_CLK_CONF);
    REG_WRITE(REG_FLASH_CONF, value);
	while(flash_sw_busy());
    GLOBAL_INT_RESTORE();
}

static void flash_protection_op_GD_MD25D40(PROTECT_TYPE type)
{
    GLOBAL_INT_DISABLE();
    UINT8 bp2, bp1, bp0;
    if(FLASH_PROTECT_SEC_112 <= type)
    {
        bp2 = 1;
    }
    else
    {
        bp2 = 0;
    }

    if(type == FLASH_PROTECT_ALL || type == FLASH_PROTECT_SEC_64
            || type == FLASH_PROTECT_SEC_120 || type == FLASH_PROTECT_SEC_124)
    {
        bp1 = 1;
    }
    else
    {
        bp1 = 0;
    }

    if(type == FLASH_PROTECT_ALL || type == FLASH_PROTECT_SEC_96
            || type == FLASH_PROTECT_SEC_120 || type == FLASH_PROTECT_SEC_126)
    {
        bp0 = 1;
    }
    else
    {
        bp0 = 0;
    }

    flash_write_sr((1 << 6) | (1 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2));
    GLOBAL_INT_RESTORE();
}

static void flash_protection_op_P25Q40U(PROTECT_TYPE type)
{
    GLOBAL_INT_DISABLE();
    UINT8 bp;
    switch(type)
    {
        case FLASH_PROTECT_SEC_126:            //504K     0x7DFFF
            bp = 0x12;
            break;
        case FLASH_PROTECT_SEC_124:            //496K     0x7BFFF
            bp = 0x13;
            break;
        case FLASH_PROTECT_SEC_120:            //480K     0x77FFF
            bp = 0x16;
            break;
        case FLASH_PROTECT_SEC_112:            //448K     0x6FFFF
            bp = 0x01;
            break;
        case FLASH_PROTECT_SEC_96:             //384K     0x5FFFF
            bp = 0x02;
            break;
        case FLASH_PROTECT_SEC_64:            //256K      0x3FFFF
            bp = 0x03;
            break;
        case FLASH_PROTECT_ALL:               //512K      0x7FFFF
            bp = 0;
            break;
        default:
            return;
    }

    flash_write_sr((1 << 14) | (bp << 2));
    GLOBAL_INT_RESTORE();
}

static void flash_protection_op(PROTECT_TYPE type)
{
	if(GD_MD25D40 == flash_mid)
	{
	    flash_protection_op_GD_MD25D40(type);
	}
	else if(P25Q40U == flash_mid)
	{
	    flash_protection_op_P25Q40U(type);	
	}

    return;
}

void flash_init(void)
{
    flash_mid = flash_get_id();
    flash_get_current_flash_config();

    flash_set_dual_mode();
    set_flash_clk(0x01);

    ddev_register_dev(FLASH_DEV_NAME, &flash_op);

    set_flash_clk_xtal16M();
    os_printf("flash_init end, id 0x%x\r\n", flash_mid);
}

void flash_exit(void)
{
    ddev_unregister_dev(FLASH_DEV_NAME);
}

UINT32 flash_open(UINT32 op_flag)
{

}

UINT32 flash_close(void)
{

}

UINT32 flash_read(char *user_buf, UINT32 count, UINT32 address)
{
    GLOBAL_INT_DISABLE();

    flash_read_data(user_buf, address, count);

    GLOBAL_INT_RESTORE();

    return DRIV_SUCCESS;
}

UINT32 flash_write(char *user_buf, UINT32 count, UINT32 address)
{
    GLOBAL_INT_DISABLE();
    flash_write_data((UINT8 *)user_buf, address, count);
    //flash_write_page_data((UINT8 *)user_buf, address, count);
    GLOBAL_INT_RESTORE();

    return DRIV_SUCCESS;
}

UINT32 flash_write_normal(char *user_buf, UINT32 count, UINT32 address)
{
    GLOBAL_INT_DISABLE();
    flash_write_data((UINT8 *)user_buf, address, count);
    //flash_write_page_data((UINT8 *)user_buf, address, count)
    GLOBAL_INT_RESTORE();

    return DRIV_SUCCESS;
}

UINT32 flash_ctrl(UINT32 cmd, void *parm)
{
    UINT8 clk;
    UINT16 wsr;
    UINT32 address;
    PROTECT_TYPE type;
    UINT32 ret = DRIV_SUCCESS;
    
    switch(cmd)
    {
        case CMD_FLASH_SET_CLK:
            clk = (*(UINT8 *)parm);
            set_flash_clk(clk);
            break;

        case CMD_FLASH_SET_DPLL:
            break;

        case CMD_FLASH_SET_DCO:
            break;

        case CMD_FLASH_WRITE_ENABLE:
            //flash_write_enable();
            break;

        case CMD_FLASH_WRITE_DISABLE:
            // flash_write_disable();
            break;

        case CMD_FLASH_READ_SR:
            (*(UINT16 *)parm) = flash_read_sr();
            break;

        case CMD_FLASH_WRITE_SR:
            flash_write_sr(((*(unsigned long *)parm) >> 8) & 0x00FFFF);
            break;

        case CMD_FLASH_SET_QWFR:
            break;

        case CMD_FLASH_CLR_QWFR:
            clr_flash_qwfr();
            break;

        case CMD_FLASH_SET_WSR:
            wsr = (*(UINT16 *)parm);
            break;

        case CMD_FLASH_GET_ID:
            (*(UINT32 *)parm) = flash_get_id();
            break;

        case CMD_FLASH_ERASE_SECTOR:
        case CMD_FLASH_ERASE_BLOCK1:
        case CMD_FLASH_ERASE_BLOCK2:
            address = (*(UINT32 *)parm);
            flash_erase(cmd, address);
            break;

        case CMD_FLASH_SET_HPM:
            break;
        case CMD_FLASH_SET_PROTECT:
            type = (*(PROTECT_TYPE *)parm);
            flash_protection_op(type);
            break;
        default:
            ret = DRIV_FAIL;
            break;
    }

    return ret;
}

#define AHB_FLASH_BASE			         0x00802000
#define REG_FLASH_CONF_1                  (*((volatile unsigned long *)   (AHB_FLASH_BASE+7*4)))
#define REG_FLASH_OPERATE_SW_1            (*((volatile unsigned long *)   (AHB_FLASH_BASE+0*4)))
#define REG_FLASH_CON_1                  (*((volatile unsigned long *)   (AHB_FLASH_BASE+7*4)))
#define BIT_FLASH_CLK_CONF_1              0
#define BIT_MODE_SEL_1                    4
#define BIT_FWREN_FLASH_CPU_1             9
#define BIT_WRSR_DATA_1                   10
#define BIT_CRC_EN_1                      26

#define SET_FLASH_CLK_CONF_1              0xF      << BIT_FLASH_CLK_CONF
#define SET_MODE_SEL_1                    0x1F     << BIT_MODE_SEL
#define SET_FWREN_FLASH_CPU_1             0x1      << BIT_FWREN_FLASH_CPU
#define SET_WRSR_DATA_1                   0xFFFF   << BIT_WRSR_DATA
#define SET_CRC_EN_1                      0x1      << BIT_CRC_EN

void set_flash_clk_xtal16M(void)
{
    GLOBAL_INT_DISABLE();
    //note :>16M don't use la for flash debug
    unsigned int temp0;
    unsigned char clk_conf = 0;

    clk_conf = 0x08;
    temp0 = REG_FLASH_CONF_1;
    REG_FLASH_CONF_1 = (  (clk_conf << BIT_FLASH_CLK_CONF_1)
                      | (temp0    &  SET_MODE_SEL_1)
                      | (temp0    &  SET_FWREN_FLASH_CPU_1)
                      | (temp0    &  SET_WRSR_DATA_1)
                      | (temp0    &  SET_CRC_EN_1));
    while(REG_FLASH_OPERATE_SW_1 & 0x80000000){;}
    GLOBAL_INT_RESTORE();
}
// eof

