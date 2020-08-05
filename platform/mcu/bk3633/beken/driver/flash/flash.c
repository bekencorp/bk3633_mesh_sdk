#include "include.h"
#include "arm_arch.h"
#include "sys_config.h"

#include "flash.h"
#include "sys_ctrl.h"
#include "flash_pub.h"

#include "drv_model_pub.h"

#include <string.h>
#include "uart_pub.h"
#include "sys_ctrl_pub.h"
#include "mcu_ps_pub.h"
#include "ll.h"


#define MODE_STD     0
#define MODE_DUAL    1
#define MODE_QUAD    2

#define FLASH_ADDR_FIX  0X7D000

extern void  _app_data_flash_end;

static UINT32 flash_id;
static DD_OPERATIONS flash_op =
{
    NULL,
    NULL,
    flash_read,
    flash_write,
    flash_ctrl
};

uint8_t flash_enable_write_flag1;
uint8_t flash_enable_write_flag2;
uint8_t flash_enable_write_flag3;
uint8_t flash_enable_write_flag4;
uint8_t flash_enable_erase_flag1;
uint8_t flash_enable_erase_flag2;

uint32_t flash_mid = 0;

static const flash_config_t flash_config[] =
{
	/// for BK3435
	{0x1C3113, 1, 2, 16, 2, 0x0F, 0x0F, 0x00, 0x0B, 0x0D | (0 << 8), 16, 0, 0xA0, 0x01, 0x1, 0x080}, //XTX_FLASH_1
	{0xC84013, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0B, 0x13 | (1 << 8),  9, 1, 0xA0, 0x01, 0x1, 0x080}, //GD_25Q40
	{0x514013, 1, 2, 16, 2, 0x07, 0x07, 0x00, 0x06, 0x02 | (0 << 8), 16, 0, 0xA0, 0x01, 0x2, 0x080}, //GD_MD25D40D
	{0x514014, 1, 2, 16, 2, 0x07, 0x07, 0x00, 0x00, 0x02 | (0 << 8), 16, 0, 0xA0, 0x01, 0x1, 0x100}, //GD_MD25D80D
	{0x000000, 1, 2, 16, 2, 0x0F, 0x0F, 0x00, 0x00, 0x00 | (0 << 8), 16, 0, 0xA0, 0x01, 0x2, 0x000}, //default

	/*/// for BK7231
    {0x1C7016, 1, 2,  0, 2, 0x1F, 0x1F, 0x00, 0x16, 0x01B, 0, 0, 0xA5, 0x01}, //en_25qh32b
    {0x0B4014, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0C, 0x101, 9, 1, 0xA0, 0x01}, //xtx_25f08b
    {0x0B4015, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0D, 0x101, 9, 1, 0xA0, 0x01}, //xtx_25f16b
    {0x0B4016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //xtx_25f32b
    {0x0E4016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //xtx_FT25H32
    {0xC84015, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0D, 0x101, 9, 1, 0xA0, 0x01}, //gd_25q16c
    {0xC84016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //gd_25q32c
    {0xEF4016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x00, 0x101, 9, 1, 0xA0, 0x01}, //w_25q32(bfj)
    {0x204016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //xmc_25qh32b
    {0xC22315, 1, 2,  0, 2, 0x0F, 0x0F, 0x00, 0x05, 0x005, 6, 1, 0xA5, 0x01}, //mx_25v16b
    {0x000000, 2, 2,  0, 2, 0x1F, 0x00, 0x00, 0x00, 0x000, 0, 0, 0x00, 0x01}, //default
    */
};

static const flash_config_t *flash_current_config = NULL;

static void flash_get_current_flash_config(void)
{
    int i;
    for(i = 0; i < (sizeof(flash_config) / sizeof(flash_config_t) - 1); i++)
    {
        if(flash_id == flash_config[i].flash_id)
        {
            flash_current_config = &flash_config[i];
            uint16_t size = flash_current_config->flash_size*4;
            os_printf("flash #%d, /%d, size:%dKB\r\n", i, flash_current_config->flash_clk, size);
            break;
        }
    }
    if(i == (sizeof(flash_config) / sizeof(flash_config_t) - 1))
    {
        flash_current_config = &flash_config[i];
		os_printf("flash id : 0x%x\r\n", flash_id);
        os_printf("don't config this flash, choose default config\r\n");
    }
    os_printf("code area end addr:0x%X\r\n", &_app_data_flash_end);
}

static void flash_set_clk(UINT8 clk_conf)
{
    	//note :>16M don't use la for flash debug
    unsigned int temp0;
    temp0 = REG_FLASH_CONF;
    REG_FLASH_CONF = (  (clk_conf << BIT_FLASH_CLK_CONF)
                      | (temp0    &  SET_MODE_SEL)
                      | (temp0    &  SET_FWREN_FLASH_CPU)
                      | (temp0    &  SET_WRSR_DATA)
                      | (temp0    &  SET_CRC_EN));
	while(REG_FLASH_OPERATE_SW & 0x80000000){;}
}

uint8_t flash_identify(uint32_t* id, void (*callback)(void))
{
    unsigned int temp0;

	while(REG_FLASH_OPERATE_SW & 0x80000000);
	
	REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
								| (0x1				 << BIT_OP_SW));
	while(REG_FLASH_OPERATE_SW & 0x80000000){;}

    for (temp0=0; temp0<8; temp0++)
            REG_FLASH_DATA_SW_FLASH = 0xffffffff;
    
    *id =  REG_FLASH_RDID_DATA_FLASH ;

     printf("identify:0x%x\r\n",*id);
     return 0;
}

uint32_t flash_get_id(void)
{
    unsigned int temp0;

	while(REG_FLASH_OPERATE_SW & 0x80000000);
	
	REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
								| (0x1				 << BIT_OP_SW));
	while(REG_FLASH_OPERATE_SW & 0x80000000);

    for (temp0=0; temp0<8; temp0++)
            REG_FLASH_DATA_SW_FLASH = 0xffffffff;

	return REG_FLASH_RDID_DATA_FLASH ;
}

static UINT16 flash_read_sr(UINT8 sr_width)
{
	UINT16 temp;
/*
	while(REG_FLASH_OPERATE_SW & 0x80000000);
	temp0 = REG_FLASH_OPERATE_SW;
	REG_FLASH_OPERATE_SW = (  (temp0             &  SET_ADDRESS_SW)
	                        | (FLASH_OPCODE_RDSR2 << BIT_OP_TYPE_SW)
	                        | (0x1               << BIT_OP_SW)
	                        | (temp0             &  SET_WP_VALUE));
	while(REG_FLASH_OPERATE_SW & 0x80000000);

	temp=(REG_FLASH_SR_DATA_CRC_CNT&0xff)<<8;
*/
	 while(REG_FLASH_OPERATE_SW & 0x80000000);
	 
	 REG_FLASH_OPERATE_SW = (  FLASH_ADDR_FIX
							 | (FLASH_OPCODE_RDSR << BIT_OP_TYPE_SW)
							 | (0x1 			  << BIT_OP_SW));

	while(REG_FLASH_OPERATE_SW & 0x80000000);
    temp = (REG_FLASH_SR_DATA_CRC_CNT & 0xff);

	return temp;
}

void flash_write_sr( uint8_t bytes,  uint16_t val )
{
    if(flash_mid != flash_get_id())
		return;

	switch(flash_mid)
	{
		case MX_FLASH_4M:
		case MX_FLASH_1:
			REG_FLASH_CONF &= 0xffdf0fff;
		break;
        
		case GD_FLASH_1:
		case BY25Q80:
		case PN25f04:
			REG_FLASH_CONF &= 0xfefe0fff;
		break;
        case XTX_FLASH_1:
		case GD_MD25D40:
        case GD_GD25WD40:
        default:
			REG_FLASH_CONF &= 0xffff0fff;
		break;
	}

    if(val==0||bytes>2)	
        return;
    REG_FLASH_CONF |= (val << BIT_WRSR_DATA)|SET_FWREN_FLASH_CPU;
    while(REG_FLASH_OPERATE_SW & 0x80000000);
	if(flash_mid != flash_get_id())
		return;
    if( bytes == 1 ) 
    {
        REG_FLASH_OPERATE_SW = (FLASH_ADDR_FIX|(FLASH_OPCODE_WRSR << BIT_OP_TYPE_SW)
	                           | (0x1<< BIT_OP_SW)
	                           | (0x1<< BIT_WP_VALUE));
        
    }
    else if(bytes == 2 )
    {
        REG_FLASH_OPERATE_SW = (FLASH_ADDR_FIX|(FLASH_OPCODE_WRSR2 << BIT_OP_TYPE_SW)
	                           | (0x1<< BIT_OP_SW)
	                           | (0x1<< BIT_WP_VALUE));       
    }
        
    while(REG_FLASH_OPERATE_SW & 0x80000000);


    REG_FLASH_OPERATE_SW = FLASH_ADDR_FIX; 

    while(REG_FLASH_OPERATE_SW & 0x80000000);
}

void flash_set_qe(void)
{
	uint32_t temp0;
	while(REG_FLASH_OPERATE_SW & 0x80000000){;}

	temp0 = REG_FLASH_CONF;

	if(flash_mid == XTX_FLASH_1)  // wanghong
		return;
	if((flash_mid == MX_FLASH_1) || (flash_mid == MX_FLASH_4M))  // wanghong
	{
		//WRSR QE=1
		REG_FLASH_CONF = ((temp0 &  SET_FLASH_CLK_CONF)
						| (temp0 &  SET_MODE_SEL)
						| (temp0 &  SET_FWREN_FLASH_CPU)
						| (temp0 & SET_WRSR_DATA)
						| (0x1 << 16) // SET QE=1,quad enable
						| (temp0 &  SET_CRC_EN));

		//Start WRSR
		
		REG_FLASH_OPERATE_SW = (  FLASH_ADDR_FIX
						     | (FLASH_OPCODE_WRSR2 << BIT_OP_TYPE_SW)
						     | (0x1				  << BIT_OP_SW)); 
	}
	else
	{
		REG_FLASH_CONF = (	(temp0 &  SET_FLASH_CLK_CONF)
						  | (temp0 &  SET_MODE_SEL)
						  | (temp0 &  SET_FWREN_FLASH_CPU)
						  |	(temp0 & SET_WRSR_DATA)
						  |	(0x01 << 19)
						  | (temp0 &  SET_CRC_EN));

		//Start WRSR
		
		REG_FLASH_OPERATE_SW = (  FLASH_ADDR_FIX
						        | (FLASH_OPCODE_WRSR2 << BIT_OP_TYPE_SW)
						        | (0x1				  << BIT_OP_SW)); 
	}
	while(REG_FLASH_OPERATE_SW & 0x80000000);
}

static void flash_clr_qwfr(void)
{
        uint32_t temp0,mod_sel;	
	
    temp0 = REG_FLASH_CONF;
    while(REG_FLASH_OPERATE_SW & 0x80000000){;}
    mod_sel = temp0 & (0xC << BIT_MODE_SEL); //??3ymode_sel?D
    mod_sel |= (0x1 << BIT_MODE_SEL);
    REG_FLASH_CONF = (  (temp0 &  SET_FLASH_CLK_CONF)
                        | mod_sel
                        | (temp0 &  SET_FWREN_FLASH_CPU)
                        | (temp0 &  SET_WRSR_DATA)
                        | (temp0 &  SET_CRC_EN));
    //reset flash
    if(flash_mid == XTX_FLASH_1)
    {
        REG_FLASH_OPERATE_SW = (  (FLASH_ADDR_FIX << BIT_ADDRESS_SW)
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

UINT8 flash_get_line_mode(void)
{
    return flash_current_config->line_mode;
}

void flash_set_line_mode(UINT8 mode)
{

}

static UINT32 flash_read_mid(void)
{
    unsigned int temp0;

	while(REG_FLASH_OPERATE_SW & 0x80000000);
	
	REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
								| (0x1				 << BIT_OP_SW));
	while(REG_FLASH_OPERATE_SW & 0x80000000);

    for (temp0 = 0; temp0 < 8; temp0++)
    {
        REG_FLASH_DATA_SW_FLASH = 0xffffffff;   
    }

	return REG_FLASH_RDID_DATA_FLASH ;
}

static void flash_erase_sector(UINT32 address)
{
    GLOBAL_INT_DISABLE();
    flash_set_line_mode(1);
    if(flash_enable_erase_flag1==FLASH_ERASE_ENABLE1&&flash_enable_erase_flag2==FLASH_ERASE_ENABLE2)    
    {
        flash_wp_256k();
        
        while(REG_FLASH_OPERATE_SW & 0x80000000);

        REG_FLASH_OPERATE_SW = (  (address << BIT_ADDRESS_SW)
                                                   | (FLASH_OPCODE_SE<< BIT_OP_TYPE_SW)
                                                   | (0x1             << BIT_OP_SW));

        while(REG_FLASH_OPERATE_SW & 0x80000000);
        flash_set_line_mode(4);
        flash_wp_ALL();
        printf("flash_erase_sector 0x%x", address);
    }
    GLOBAL_INT_RESTORE();
}

void flash_read_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    uint32_t i, j, k;
    uint32_t addr = address;
    uint32_t buf[8];
    k=0;
    if (len == 0)
        return;

    GLOBAL_INT_DISABLE();
    while(REG_FLASH_OPERATE_SW & 0x80000000);

    for(j=0; j < ((len-1)/32+1); j++)
    {
        REG_FLASH_OPERATE_SW = (  (addr << BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_READ << BIT_OP_TYPE_SW)
                                | (0x1 << BIT_OP_SW));
        while(REG_FLASH_OPERATE_SW & 0x80000000);
        addr+=32;
        for (i=0; i<8; i++)
        {
            buf[i] = REG_FLASH_DATA_FLASH_SW;
        }

        if(len > 32 * (j + 1))
            memcpy(&buffer[k],buf,32);
        else
        {
            memcpy(&buffer[k],buf,len-k);
        }
        k += 32;
    }
    REG_FLASH_OPERATE_SW = FLASH_ADDR_FIX ;
    for (i=0; i<8; i++)
        REG_FLASH_DATA_SW_FLASH = 0xffffffff;
    GLOBAL_INT_RESTORE();
}

void flash_wp_256k( void)
{
    switch(flash_mid)
    {
        case MX_FLASH_4M:
        case MX_FLASH_1:			   //MG xx
            flash_write_sr( 2, 0x088C );
            break;
        case XTX_FLASH_1:			   //XTX xx
            flash_write_sr( 1, 0xAC );
            break;   

        case GD_FLASH_1:			  //QD xx ,
        case BY25Q80:
        case PN25f04:
            flash_write_sr( 2, 0x00ac );
            break;
        case GD_MD25D40:
        case GD_GD25WD40:    
        default:
            flash_write_sr( 1, 0x98 );
            break;
    }
}

void flash_wp_ALL( void )
{
   //  return;
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
        case GD_MD25D40:
        case GD_GD25WD40:    
        default:
            flash_write_sr( 1, 0x9c );
            break;    
    }
}

void flash_write_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    uint32_t i, j,k;
    uint32_t addr = address;
    uint32_t buf[8];
    k=0;
    if (len == 0)
        return;
    if (address<0x40000)
        return;
    GLOBAL_INT_DISABLE();
	flash_set_line_mode(1);

    while(REG_FLASH_OPERATE_SW & 0x80000000);
    
    flash_enable_write_flag3=FLASH_WRITE_ENABLE3; 
    flash_wp_256k();
    for(j=0;j<((len-1)/32+1);j++)
    {
        if(len>32*(j+1))
            memcpy(buf,&buffer[k],32);
        else
        {
         	for(i=0;i<8;i++)
                buf[i]=0xffffffff;
            memcpy(buf,&buffer[k],len-k);
        }
        k += 32;
        flash_enable_write_flag4=FLASH_WRITE_ENABLE4; 
        for (i=0; i<8; i++)
            REG_FLASH_DATA_SW_FLASH = buf[i];


        if(flash_enable_write_flag1==FLASH_WRITE_ENABLE1 && flash_enable_write_flag2==FLASH_WRITE_ENABLE2 )
        {
            while(REG_FLASH_OPERATE_SW & 0x80000000);
            
            if(flash_enable_write_flag3==FLASH_WRITE_ENABLE3 && flash_enable_write_flag4==FLASH_WRITE_ENABLE4)
            {
                if(addr<0x40000)
                    return;
                REG_FLASH_OPERATE_SW = (  (addr << BIT_ADDRESS_SW)
                                    | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                                    | (0x1 << BIT_OP_SW));
            }
            while(REG_FLASH_OPERATE_SW & 0x80000000);
        }
        addr+=32;
    }
    flash_wp_ALL();
    REG_FLASH_OPERATE_SW=FLASH_ADDR_FIX ;
    flash_enable_write_flag3=0;
    flash_enable_write_flag4=0;
    for (i=0; i<8; i++)
        REG_FLASH_DATA_SW_FLASH = 0xffffffff;
	flash_set_line_mode(4);
	GLOBAL_INT_RESTORE();
}

void clr_flash_qwfr(void)
{
    uint32_t temp0,mod_sel;	
	
    temp0 = REG_FLASH_CONF;
    while(REG_FLASH_OPERATE_SW & (unsigned long)0x80000000){;}
    mod_sel = temp0 & (0xC << BIT_MODE_SEL); //??3ymode_sel?D
    mod_sel |= (0x1 << BIT_MODE_SEL);
    REG_FLASH_CONF = (  (temp0 &  SET_FLASH_CLK_CONF)
                        | mod_sel
                        | (temp0 &  SET_FWREN_FLASH_CPU)
                        | (temp0 &  SET_WRSR_DATA)
                        | (temp0 &  SET_CRC_EN));
    //reset flash
    if(flash_mid == XTX_FLASH_1)
    {
        REG_FLASH_OPERATE_SW = (  (FLASH_ADDR_FIX << BIT_ADDRESS_SW)
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

void flash_set_dual_mode(void)
{
    clr_flash_qwfr();
    REG_FLASH_CONF &= (~(7<<BIT_MODE_SEL));
    REG_FLASH_CONF |= (1<<BIT_MODE_SEL);
    while(REG_FLASH_OPERATE_SW & 0x80000000);
}

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
	while(REG_FLASH_OPERATE_SW & 0x80000000){;}
}

void flash_init(void)
{
    flash_set_dual_mode();
    set_flash_clk(0x08);

    ddev_register_dev(FLASH_DEV_NAME, &flash_op);
    
    os_printf("flash_init end\r\n");
}

void flash_exit(void)
{
    ddev_unregister_dev(FLASH_DEV_NAME);
}

UINT32 flash_read(char *user_buf, UINT32 count, UINT32 address)
{
    uint32_t pre_address;
    uint32_t post_address;
    uint32_t pre_len;
    uint32_t post_len;
    uint32_t page0;
    uint32_t page1;
    page0 = address &(~FLASH_PAGE_MASK);
    page1 = (address + count) &(~FLASH_PAGE_MASK);
    if(page0 != page1)
    {
        pre_address = address;
        pre_len = page1 - address;
        flash_read_data(user_buf, pre_address, pre_len);
        post_address = page1;
        post_len = address + count - page1;
        flash_read_data((user_buf + pre_len), post_address, post_len);
    }
    else
    {
        flash_read_data(user_buf, address, count);
    }

    return FLASH_SUCCESS;
}

UINT32 flash_write(char *user_buf, UINT32 count, UINT32 address)
{
        uint32_t pre_address;
    uint32_t post_address;
    uint32_t pre_len;
    uint32_t post_len;
    uint32_t page0;
    uint32_t page1;
    
    if(flash_mid != flash_get_id())
    {
        printf("Wrong flash ID = 0x%x\r\n", flash_get_id());
        return FLASH_FAILURE;
    }

    flash_enable_write_flag1=FLASH_WRITE_ENABLE1; 
    
    page0 = address &(~FLASH_PAGE_MASK);
    page1 = (address + count) &(~FLASH_PAGE_MASK);
     
    if(page0 != page1)
    {
        pre_address = address;
        pre_len = page1 - address;
        flash_enable_write_flag2=FLASH_WRITE_ENABLE2;
        flash_write_data(user_buf, pre_address, pre_len);
        
        post_address = page1;
        post_len = address + count - page1;
        flash_write_data((user_buf + pre_len), post_address, post_len);

    }
    else
    {
        flash_enable_write_flag2=FLASH_WRITE_ENABLE2;
        flash_write_data(user_buf, address, count);

    }
    flash_enable_write_flag1=0; 
    flash_enable_write_flag2=0;	
    return FLASH_SUCCESS;
}

UINT32 flash_ctrl(UINT32 cmd, void *parm)
{
    UINT8 clk;
    UINT16 wsr;
    UINT32 address;
    UINT32 reg;
    UINT32 ret = FLASH_SUCCESS;
    
    switch(cmd)
    {
    case CMD_FLASH_SET_CLK:
        clk = (*(UINT8 *)parm);
        flash_set_clk(clk);
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
        (*(UINT16 *)parm) = flash_read_sr(2);
        break;

    case CMD_FLASH_WRITE_SR:
        flash_write_sr(*(unsigned long *)parm & 0x00FF, ((*(unsigned long *)parm) >> 8) & 0x00FFFF);
        break;

    case CMD_FLASH_READ_QE:
        break;

    case CMD_FLASH_SET_QE:
        if(flash_current_config->qe_bit)
        {
            flash_set_qe();
        }
        break;

    case CMD_FLASH_SET_QWFR:
        break;

    case CMD_FLASH_CLR_QWFR:
        flash_clr_qwfr();
        break;

    case CMD_FLASH_SET_WSR:
        wsr = (*(UINT16 *)parm);
        break;

    case CMD_FLASH_GET_ID:
        (*(UINT32 *)parm) = flash_get_id();
        break;

    case CMD_FLASH_READ_MID:
        (*(UINT32 *)parm) = flash_read_mid();
        break;

    case CMD_FLASH_ERASE_SECTOR:
        address = (*(UINT32 *)parm);
        flash_erase_sector(address);
        break;

    case CMD_FLASH_SET_HPM:
        break;
	case CMD_FLASH_SET_PROTECT:
		reg =  (*(UINT32 *)parm);
		// flash_protection_op(FLASH_XTX_16M_SR_WRITE_DISABLE, reg);
		break;
    default:
        ret = FLASH_FAILURE;
        break;
    }

    return ret;
}
// eof

