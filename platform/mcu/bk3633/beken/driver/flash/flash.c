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

static DD_OPERATIONS flash_op =
{
    flash_open,
    flash_close,
    flash_read,
    flash_write,
    flash_ctrl
};

uint32_t flash_mid = 0;

// extern bool bt_ready_count;

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
        if(flash_mid == flash_config[i].flash_id)
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
		os_printf("flash id : 0x%x\r\n", flash_mid);
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
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
}

uint8_t flash_identify(uint32_t* id, void (*callback)(void))
{
    unsigned int temp0;

	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
	
	REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
								| (0x1				 << BIT_OP_SW));
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    for (temp0=0; temp0<8; temp0++)
            REG_FLASH_DATA_SW_FLASH = 0xffffffff;
    
    *id =  REG_FLASH_RDID_DATA_FLASH ;

    printf("identify:0x%x\r\n",*id);
    return 0;
}

uint32_t flash_get_id(void)
{
    unsigned int temp0;

	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
	
	REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
								| (0x1				 << BIT_OP_SW));
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    for (temp0=0; temp0<8; temp0++)
            REG_FLASH_DATA_SW_FLASH = 0xffffffff;

	return REG_FLASH_RDID_DATA_FLASH ;
}

static UINT16 flash_read_sr(UINT8 sr_width)
{
	UINT16 temp;
/*
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
	temp0 = REG_FLASH_OPERATE_SW;
	REG_FLASH_OPERATE_SW = (  (temp0             &  SET_ADDRESS_SW)
	                        | (FLASH_OPCODE_RDSR2 << BIT_OP_TYPE_SW)
	                        | (0x1               << BIT_OP_SW)
	                        | (temp0             &  SET_WP_VALUE));
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

	temp=(REG_FLASH_SR_DATA_CRC_CNT&0xff)<<8;
*/
	 while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
	 
	 REG_FLASH_OPERATE_SW = (  FLASH_ADDR_FIX
							 | (FLASH_OPCODE_RDSR << BIT_OP_TYPE_SW)
							 | (0x1 			  << BIT_OP_SW));

	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
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

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

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
        
    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    REG_FLASH_OPERATE_SW = FLASH_ADDR_FIX; 

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
}

void flash_set_qe(void)
{
	uint32_t temp0;
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

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
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
}

UINT8 flash_get_line_mode(void)
{
    return flash_current_config->line_mode;
}

static void flash_set_qwfr(void)
{
    UINT32 value;

    value = REG_FLASH_CONF;
    value &= ~(MODEL_SEL_MASK << BIT_MODE_SEL);
    value |= (flash_current_config->mode_sel << BIT_MODE_SEL);
    REG_FLASH_CONF = value;
}

void flash_set_line_mode(UINT8 mode)
{
    uint32_t value;
    
    if(1 == mode)
    {
        clr_flash_qwfr();
    }
    if(2 == mode)
    {
        clr_flash_qwfr();
        value = REG_FLASH_CONF;
        value &= ~(MODEL_SEL_MASK << BIT_MODE_SEL);
        value |= ((MODE_DUAL & MODEL_SEL_MASK) << BIT_MODE_SEL);
        REG_FLASH_CONF = value;

    }
    else if(4 == mode)
    {
        clr_flash_qwfr();
        value = REG_FLASH_SR_DATA_CRC_CNT;
        value &= ~(M_VALUE_MASK << M_VALUE_POST);
        value |= (flash_current_config->m_value<< M_VALUE_POST);
        
        REG_FLASH_SR_DATA_CRC_CNT = value;

        if(1 == flash_current_config->qe_bit)
        {
            flash_set_qe();
        }

        flash_set_qwfr();
    }
}

static UINT32 flash_read_mid(void)
{
    unsigned int temp0;

	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
	
	REG_FLASH_OPERATE_SW = (       FLASH_ADDR_FIX
								| (FLASH_OPCODE_RDID << BIT_OP_TYPE_SW)
								| (0x1				 << BIT_OP_SW));
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    for (temp0 = 0; temp0 < 8; temp0++)
    {
        REG_FLASH_DATA_SW_FLASH = 0xffffffff;   
    }

	return REG_FLASH_RDID_DATA_FLASH ;
}

static void flash_erase(UINT32 cmd, UINT32 address)
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

    // if (bt_ready_count) {
    //     bt_mesh_scan_disable();
    // }
    GLOBAL_INT_DISABLE();
    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    value = REG_FLASH_OPERATE_SW;
    value = ((address << BIT_ADDRESS_SW)
             | (type << BIT_OP_TYPE_SW)
             | (0x1 << BIT_OP_SW));
    REG_FLASH_OPERATE_SW = value;

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
    GLOBAL_INT_RESTORE();
    // if (bt_ready_count) {
    //     bt_mesh_scan_enable();
    // }
}

void flash_read_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    uint32_t i, reg_value;
    uint32_t addr = address & (~0x1F);
    uint32_t buf[8];
    uint8_t *pb = (uint8_t *)&buf[0];

    if(len == 0)
    {
        return;
    }

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    while(len)
    {
        reg_value = REG_FLASH_OPERATE_SW;
        reg_value = (  (addr << BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_READ << BIT_OP_TYPE_SW)
                                | (0x1 << BIT_OP_SW)
                                | (reg_value & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = reg_value;

        while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

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
/* 
    REG_FLASH_OPERATE_SW = FLASH_ADDR_FIX ;
    for (i = 0; i < 8; i++)
        REG_FLASH_DATA_SW_FLASH = 0xffffffff; */
        
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
    uint32_t i, reg_value;
    uint32_t addr = address & (~0x1F);
    uint32_t buf[8];
    uint8_t *pb = (uint8_t *)&buf[0];


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

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

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

        while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

        reg_value = REG_FLASH_OPERATE_SW;
        reg_value = ((addr << BIT_ADDRESS_SW)
                     | (FLASH_OPCODE_PP << BIT_OP_TYPE_SW)
                     | (0x1 << BIT_OP_SW)
                     | (reg_value & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = reg_value;

        while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
        addr += 32;
        memset(pb, 0xFF, 32);
    }

/*     REG_FLASH_OPERATE_SW=FLASH_ADDR_FIX;

    for (i=0; i<8; i++)
        REG_FLASH_DATA_SW_FLASH = 0xffffffff; */

}

void clr_flash_qwfr(void)
{
    uint32_t temp0;	

	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);

    temp0 = REG_FLASH_CONF;
    temp0 &= ~(MODEL_SEL_MASK << BIT_MODE_SEL);
    REG_FLASH_CONF = temp0;

    //reset flash
    if(flash_mid == XTX_FLASH_1)
    {
        temp0 = REG_FLASH_OPERATE_SW;
        temp0 = (  (FLASH_ADDR_FIX<< BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_CRMR << BIT_OP_TYPE_SW)
                                | (0x1               << BIT_OP_SW)
                                | (temp0 & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = temp0;
    }
    else
    {
        temp0 = REG_FLASH_OPERATE_SW;
        temp0 = (  (FLASH_ADDR_FIX<< BIT_ADDRESS_SW)
                                | (FLASH_OPCODE_CRMR2 << BIT_OP_TYPE_SW)
                                | (0x1               << BIT_OP_SW)
                                | (temp0 & SET_WP_VALUE));
        REG_FLASH_OPERATE_SW = temp0;
    }

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
}

void flash_set_dual_mode(void)
{
    UINT32 value;

    clr_flash_qwfr();

    value = REG_FLASH_CONF;
    value &= (~(MODEL_SEL_MASK<<BIT_MODE_SEL));
    value |= ((MODE_DUAL & MODEL_SEL_MASK)<<BIT_MODE_SEL);
    REG_FLASH_CONF = value;

    while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
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
	while(REG_FLASH_OPERATE_SW & FLASH_BUSY_BIT);
}

void flash_init(void)
{
    flash_mid = flash_get_id();
    flash_get_current_flash_config();

    flash_set_dual_mode();
    set_flash_clk(0x01);

    ddev_register_dev(FLASH_DEV_NAME, &flash_op);
    
    os_printf("flash_init end, id 0x%x\r\n", flash_mid);
}

void flash_exit(void)
{
    ddev_unregister_dev(FLASH_DEV_NAME);
}

UINT32 flash_open(UINT32 op_flag)
{
    flash_wp_256k();
}

UINT32 flash_close(void)
{
    flash_wp_ALL();
}

UINT32 flash_read(char *user_buf, UINT32 count, UINT32 address)
{
    // if (bt_ready_count) {
    //     bt_mesh_scan_disable();
    // }
    GLOBAL_INT_DISABLE();

    flash_read_data(user_buf, address, count);

    GLOBAL_INT_RESTORE();
    // if (bt_ready_count) {
    //     bt_mesh_scan_enable();
    // }
    return FLASH_SUCCESS;
}

UINT32 flash_write(char *user_buf, UINT32 count, UINT32 address)
{
    // if(bt_ready_count) {
    //     bt_mesh_scan_disable();
    // }
    GLOBAL_INT_DISABLE();

    if(4 == flash_current_config->line_mode)
    {
        flash_set_line_mode(LINE_MODE_TWO);
        //os_printf("change line mode 2\r\n");
    }

    flash_write_data((UINT8 *)user_buf, address, count);

    if(4 == flash_current_config->line_mode)
    {
        flash_set_line_mode(LINE_MODE_FOUR);
        //os_printf("change line mode 4\r\n");
    }

    GLOBAL_INT_RESTORE();
    // if (bt_ready_count) {
    //     bt_mesh_scan_enable();
    // }
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
        clr_flash_qwfr();
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
    case CMD_FLASH_ERASE_BLOCK1:
    case CMD_FLASH_ERASE_BLOCK2:
        address = (*(UINT32 *)parm);
        flash_erase(cmd, address);
        break;

    case CMD_FLASH_SET_HPM:
        break;
	case CMD_FLASH_SET_PROTECT:
		reg =  (*(UINT32 *)parm);
		//flash_protection_op(FLASH_XTX_16M_SR_WRITE_DISABLE, reg);
		break;
    default:
        ret = FLASH_FAILURE;
        break;
    }

    return ret;
}
// eof

