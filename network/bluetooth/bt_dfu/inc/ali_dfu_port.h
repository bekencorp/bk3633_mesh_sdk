#ifndef _ALI_DFU_PORT_H
#define _ALI_DFU_PORT_H
#include <stdbool.h>

int u_check_checksum(int image_id);
void unlock_flash_all();
void flash_lock();


#endif  
