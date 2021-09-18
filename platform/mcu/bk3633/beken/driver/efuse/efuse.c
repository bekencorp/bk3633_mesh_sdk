#include <stddef.h>     // standard definition
#include <stdarg.h>
#include <stdio.h>
#include "user_config.h"
#include "efuse.h"
#include "BK3633_RegList.h"


uint8_t efuse_read(uint8_t addr)
{
	uint32_t tempdata;
	EFUSE_REG0X0_CN = (1<<POS_EFUSE_REG0X0_VDD25_EN)|
					  (addr<<POS_EFUSE_REG0X0_OPRT_ADDR)|
					  (1<<POS_EFUSE_REG0X0_OPRT_EN);

	
	while(EFUSE_REG0X0_CN&(1<<POS_EFUSE_REG0X0_OPRT_EN));
	
	while(1)
	{
		if(EFUSE_REG0X1_CN&(1<<POS_EFUSE_REG0X1_READ_VALID))
		{
		
			tempdata = EFUSE_REG0X1_CN;
			break;
		}
	}
 
	return tempdata&0xff;

}

void efuse_write(uint8_t addr,uint8_t data)
{

	if(addr>0x3f || addr<0x20)
	{
		return;
	}
	  
	EFUSE_REG0X0_CN = (1<<POS_EFUSE_REG0X0_VDD25_EN)|
					  (addr<<POS_EFUSE_REG0X0_OPRT_ADDR)|
					  (data<<POS_EFUSE_REG0X0_OPRT_WDATA)|
					  (1<<POS_EFUSE_REG0X0_OPRT_DIR)|
					  (1<<POS_EFUSE_REG0X0_OPRT_EN);

	
	while(EFUSE_REG0X0_CN&(1<<POS_EFUSE_REG0X0_OPRT_EN));
	
	
}

