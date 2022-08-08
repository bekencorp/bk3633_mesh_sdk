#include <stdint.h>        // standard integer definition
#include <string.h>        // string manipulation
#include "bim_flash.h"         // flash definition
#include "bim_uart2.h"
#include "bim_updataImage.h"

uint32_t flash_mid = 0;

#define GLOBAL_INT_DISABLE(); \
do { \
    uint32_t  fiq_tmp; \
    uint32_t  irq_tmp; \
    fiq_tmp = __disable_fiq(); \
    irq_tmp = __disable_irq();
#define GLOBAL_INT_RESTORE(); \
    if(!fiq_tmp)           \
    {                      \
        __enable_fiq();    \
    }                      \
    if(!irq_tmp)           \
    {                      \
        __enable_irq();    \
    }                      \
} while(0) ;

void set_flash_clk(unsigned char clk_conf)
{
    //note :>16M don't use la for flash debug
    unsigned int temp0;
    temp0 = REG_FLASH_CONF;
    REG_FLASH_CONF = (  (clk_conf << BIT_FLASH_CLK_CONF)
                        | (temp0    &  SET_MODE_SEL)
                        | (temp0    &  SET_FWREN_FLASH_CPU)
                        | (temp0    &  SET_WRSR_DATA)
                        | (temp0    &  SET_CRC_EN));
    while(REG_FLASH_OPERATE_SW & 0x80000000);
}

uint32_t get_flash_ID(void)
{
    unsigned int temp0;
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
                                   | (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
                                   | (0x1               << BIT_OP_SW));
    while(REG_FLASH_OPERATE_SW & 0x80000000)
    {
        ;
    }
    for (temp0=0; temp0<8; temp0++)
    {
        REG_FLASH_DATA_SW_FLASH = 0xffffffff;
    }
    return REG_FLASH_RDID_DATA_FLASH ;
}

#if 0
uint32_t flash_read_sr( )
{
    uint16_t temp;

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    REG_FLASH_OPERATE_SW = (  FLASH_ADDR_FIX
                              | (FLASH_OPCODE_RDSR << BIT_OP_TYPE_SW)
                              | (0x1               << BIT_OP_SW));

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    temp = (REG_FLASH_SR_DATA_CRC_CNT&0xff);

    return temp ;
}
#else
uint32_t flash_read_sr(void)
{
	uint32_t temp = 0;

	while(REG_FLASH_OPERATE_SW & 0x80000000);

	REG_FLASH_OPERATE_SW = ( FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDSR << BIT_OP_TYPE_SW)
								| (0x1 							<< BIT_OP_SW));

	while(REG_FLASH_OPERATE_SW & 0x80000000);

	temp = (REG_FLASH_SR_DATA_CRC_CNT&0xff);

	REG_FLASH_OPERATE_SW = (	FLASH_ADDR_FIX
															| (FLASH_OPCODE_RDSR2 << BIT_OP_TYPE_SW)
															| (0x1 								 << BIT_OP_SW));

	while(REG_FLASH_OPERATE_SW & 0x80000000);

	temp |= (REG_FLASH_SR_DATA_CRC_CNT&0xff) << 8;


	return temp ;
}
#endif

void flash_write_sr_temp( uint8_t bytes,  uint16_t val )
{

	REG_FLASH_CONF &= 0xffff0fff;

    if(bytes==0||bytes>2)
    {
        return;
    }

    REG_FLASH_CONF |= (val << BIT_WRSR_DATA)|SET_FWREN_FLASH_CPU;

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    if(flash_mid != get_flash_ID())
    {
        return;
    }

    if( bytes == 1 )
    {
        REG_FLASH_OPERATE_SW = (FLASH_ADDR_FIX | (FLASH_OPCODE_WRSR << BIT_OP_TYPE_SW)
                                | (0x1<< BIT_OP_SW)
                                | (0x1<< BIT_WP_VALUE));
    }
    else if(bytes == 2)
    {
        REG_FLASH_OPERATE_SW = (FLASH_ADDR_FIX | (FLASH_OPCODE_WRSR2 << BIT_OP_TYPE_SW)
                                | (0x1<< BIT_OP_SW)
                                | (0x1<< BIT_WP_VALUE));
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    REG_FLASH_OPERATE_SW = FLASH_ADDR_FIX;

    while(REG_FLASH_OPERATE_SW & 0x80000000);

}


void flash_write_sr( uint8_t bytes,  uint16_t val )
{
	if(flash_read_sr() == val) 
	{
		return; 
	}
	
	flash_write_sr_temp(bytes, val);

	while (flash_read_sr() != val)
	{
	    bim_printf("++bim flash write: error repeat  val(%X, %X)\r\n", val, flash_read_sr() );
		flash_write_sr_temp(bytes, val);
	}
}

void flash_wp_none( void )
{

    flash_write_sr( 1, 0x00 );

}

void flash_wp_ALL( void )
{

	switch(flash_mid)
    {
        case MX_FLASH_4M:
        case MX_FLASH_1:			   //MG xx
			flash_write_sr( 2, 0x00bc );
            break;
        case XTX_FLASH_1:			   //XTX xx
			flash_write_sr( 1, 0xBC );
            break;  
        case GD_FLASH_1:			  //QD xx ,
        case BY25Q80:
        case PN25f04:
			flash_write_sr( 2, 0x0094 );
            break;
        case P25Q40U:
			flash_write_sr( 2, 0x4000 );
            break;    
        case GD_MD25D40:
        case GD_GD25WD40:    
        default:
			flash_write_sr( 1, 0x1c );
            break;    
    }

}

void clr_flash_qwfr(void);
void flash_set_dual_mode(void)
{
    clr_flash_qwfr();

    REG_FLASH_CONF &= (~(7<<BIT_MODE_SEL));
    REG_FLASH_CONF |= (1<<BIT_MODE_SEL);

    while(REG_FLASH_OPERATE_SW & 0x80000000);
}

void bim_flash_init(void)
{
    flash_mid = get_flash_ID();

    flash_set_dual_mode();
    flash_wp_ALL();
    //set_flash_clk(0x08);
}

void flash_erase_sector(uint32_t address)
{
    if (address<PARTITION_STACK_CPU_ADDR)
    {
        return;
    }

    GLOBAL_INT_DISABLE();

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    REG_FLASH_OPERATE_SW = (  (address << BIT_ADDRESS_SW)
                                  | (FLASH_OPCODE_SE<< BIT_OP_TYPE_SW)
                                  | (0x1             << BIT_OP_SW));

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    GLOBAL_INT_RESTORE();

}
void flash_erase_one_block(uint32_t address)
{

    if (address<0x2000)
    {
        return;
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    REG_FLASH_OPERATE_SW = (  (address << BIT_ADDRESS_SW)
                                  | (FLASH_OPCODE_BE2 << BIT_OP_TYPE_SW)
                                  | (0x1             << BIT_OP_SW));

    while(REG_FLASH_OPERATE_SW & 0x80000000);

}

void flash_read_data(uint8_t *buffer, uint32_t address, uint32_t len)
{
    uint32_t i, reg_value;
    uint32_t addr = address & (~0x1F);
    uint32_t buf[8];
    uint8_t *pb = (uint8_t *)&buf[0];

    if(len == 0)
    {
        return;
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    while(len)
    {
        reg_value = REG_FLASH_OPERATE_SW;
        reg_value = (  (addr << BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_READ << BIT_OP_TYPE_SW)
                                | (0x1 << BIT_OP_SW)
                                | (reg_value & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = reg_value;

        while(REG_FLASH_OPERATE_SW & 0x80000000);

        addr += 32;

        for (i = 0; i < 8; i++)
        {
            buf[i] = REG_FLASH_DATA_FLASH_SW;
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

void flash_write_data(uint8_t *buffer, uint32_t address, uint32_t len)
{
    uint32_t i, reg_value;
    uint32_t addr = address & (~0x1F);
    uint32_t buf[8];
    uint8_t *pb = (uint8_t *)&buf[0];

    if (len == 0)
        return;

    if(address % 32)
    {
        flash_read_data(pb, addr, 32);
    }
    else
    {
        memset(pb, 0xFF, 32);
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);

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
            REG_FLASH_DATA_SW_FLASH = buf[i];
        }

        while(REG_FLASH_OPERATE_SW & 0x80000000);

        reg_value = REG_FLASH_OPERATE_SW;
        reg_value = ((addr << BIT_ADDRESS_SW)
                     | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                     | (0x1 << BIT_OP_SW)
                     | (reg_value & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = reg_value;

        while(REG_FLASH_OPERATE_SW & 0x80000000);
        addr += 32;
        memset(pb, 0xFF, 32);
    }

}

void flash_write_page_data(uint8_t *buffer, uint32_t address, uint32_t len)
{
    uint32_t i, reg_value;
    uint32_t addr = address & (~0xFF);
    uint8_t buf[256];
    uint8_t *pb = (uint8_t *)&buf[0];

    if (len == 0)
        return;

    if(address % 256)
    {
        flash_read_data(pb, addr, 256);
    }
    else
    {
        memset(pb, 0xFF, 256);
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);

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
            set_FLASH_Reg0x9_mem_data(buf[i]);
        }

        while(REG_FLASH_OPERATE_SW & 0x80000000);

        reg_value = REG_FLASH_OPERATE_SW;
        reg_value = ((addr << BIT_ADDRESS_SW)
                     | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                     | (0x1 << BIT_OP_SW)
                     | (reg_value & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = reg_value;

        while(REG_FLASH_OPERATE_SW & 0x80000000);
        addr += 256;
        memset(pb, 0xFF, 256);
    }
    set_FLASH_Reg0x5_pw_write(0);

}

void clr_flash_qwfr(void)
{
    uint32_t temp0,mod_sel;
    temp0 = REG_FLASH_CONF;

    while(REG_FLASH_OPERATE_SW & 0x80000000);

    mod_sel = temp0 & (0xC << BIT_MODE_SEL);
    mod_sel |= (0x1 << BIT_MODE_SEL);
    REG_FLASH_CONF = (  (temp0 &  SET_FLASH_CLK_CONF)
                                    | mod_sel
                                    | (temp0 &  SET_FWREN_FLASH_CPU)
                                    | (temp0 &  SET_WRSR_DATA)
                                    | (temp0 &  SET_CRC_EN));
    //reset flash
    if(flash_mid == XTX_FLASH_1)
    {
        REG_FLASH_OPERATE_SW = (  (FLASH_ADDR_FIX<< BIT_ADDRESS_SW)
                                                  | (FLASH_OPCODE_CRMR << BIT_OP_TYPE_SW)
                                                  | (0x1               << BIT_OP_SW));
    }
    else
    {
        REG_FLASH_OPERATE_SW = (  (FLASH_ADDR_FIX<< BIT_ADDRESS_SW)
                                                  | (FLASH_OPCODE_CRMR2 << BIT_OP_TYPE_SW)
                                                  | (0x1               << BIT_OP_SW));
    }

    while(REG_FLASH_OPERATE_SW & 0x80000000);
}

uint8_t flash_read( uint8_t *buffer,uint32_t address, uint32_t len)
{
    GLOBAL_INT_DISABLE();
    flash_read_data(buffer, address, len);
    GLOBAL_INT_RESTORE();
    return 0;
}

uint8_t flash_write(uint8_t *buffer,uint32_t address, uint32_t len)
{
    GLOBAL_INT_DISABLE();
    flash_write_data(buffer, address, len);
    //flash_write_page_data(buffer, address, len);
    GLOBAL_INT_RESTORE();
    return 0;
}

uint8_t bim_flash_erase_part_sector( uint32_t address, uint32_t len, FLASH_SEC_ERASE_DIRECT erase_direct)
{
    uint32_t backup_addr, tmp_back_addr;
    uint8_t tmp[BACK_BUFF_SIZE] = {0};
    uint32_t backup_len, rest_data, readsize;
    uint32_t read_addr, tmp_addr;

    switch(erase_direct)
    {
        case FLASH_SEC_ERASE_HEAD:
            backup_addr = address + FLASH_ERASE_SECTOR_SIZE;
            read_addr = address;
            backup_len = len;
            break;
        case FLASH_SEC_ERASE_TAIL:
            backup_addr = address - FLASH_ERASE_SECTOR_SIZE;
            read_addr = address + len;
            backup_len = FLASH_ERASE_SECTOR_SIZE - len;
            break;
        default:
            return 1;
    }

    flash_erase_sector(backup_addr);

    rest_data = backup_len;
    tmp_addr = read_addr;
    tmp_back_addr = backup_addr;

    while(rest_data)
    {
        readsize = rest_data>BACK_BUFF_SIZE?BACK_BUFF_SIZE:rest_data;
        flash_read(tmp, tmp_addr, readsize);
        flash_write(tmp, tmp_back_addr, readsize);
        tmp_addr += readsize;
        tmp_back_addr += readsize;
        rest_data -= readsize;
    }

    flash_erase_sector(address);

    rest_data = backup_len;
    tmp_addr = read_addr;
    tmp_back_addr = backup_addr;

    while(rest_data)
    {
        readsize = rest_data>BACK_BUFF_SIZE ? BACK_BUFF_SIZE : rest_data;
        flash_read(tmp, tmp_back_addr, readsize);
        flash_write(tmp, tmp_addr, readsize);
        tmp_addr += readsize;
        tmp_back_addr += readsize;
        rest_data -= readsize;
    }

    flash_erase_sector(backup_addr);

    return 0;
}

uint8_t flash_erase( uint32_t address, uint32_t len)
{
    /* assume: the worst span is four sectors*/
    uint32_t erase_addr;
    uint32_t head_len, tail_len;
    int32_t erase_whole_sector_cnt;
    uint32_t i;

    erase_addr = address & (~FLASH_ERASE_SECTOR_SIZE_MASK);
    head_len = address & FLASH_ERASE_SECTOR_SIZE_MASK;
    tail_len = (head_len + len) & FLASH_ERASE_SECTOR_SIZE_MASK;
    erase_whole_sector_cnt = ((len + head_len - tail_len)>>FLASH_ERASE_SECTOR_CNT) - (head_len>0?1:0);

    if(erase_whole_sector_cnt <= 0)
    {
        goto exit;
    }
bim_printf(" 0x%x 0x%x 0x%x 0x%x \r\n", address, len, ((len + head_len - tail_len)>>FLASH_ERASE_SECTOR_CNT), (len + head_len - tail_len));
bim_printf(" 0x%x 0x%x 0x%x 0x%x\r\n", erase_addr, head_len, tail_len, erase_whole_sector_cnt);


    if(head_len)
    {
        bim_flash_erase_part_sector(erase_addr, head_len, FLASH_SEC_ERASE_HEAD);
        erase_addr += FLASH_ERASE_SECTOR_SIZE;
    }

    for(i = 0; i < erase_whole_sector_cnt; i ++)
    {
        flash_erase_sector(erase_addr);
        erase_addr += FLASH_ERASE_SECTOR_SIZE;
    }

    if(tail_len)
    {
        bim_flash_erase_part_sector(erase_addr, tail_len, FLASH_SEC_ERASE_TAIL);
    }

exit:

    return 0;
}
