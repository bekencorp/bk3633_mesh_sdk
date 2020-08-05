#if 0
#include "ali_dfu_port.h"
#include <stdio.h>

unsigned char dfu_check_checksum(short image_id)
{
	return 1;
}

void unlock_flash_all()
{
}

void flash_lock()
{
}

void lock_flash()
{
}

void dfu_reboot()
{
	ota_breeze_set_boot();
}

int ali_dfu_image_update_frame(short signature, int offset, int length,
                                          int/*void*/ *p_void)
{
#if 0 //todo vincent ota ali_dfu_image_update_frame
    unsigned int err_code = 0;
	err_code = ota_breeze_on_fw_data((unsigned char *)p_void, length, tmp_queue.num_frames);
	return err_code;
#endif
return 0;
}
										  
int ali_dfu_image_update(short signature, int offset, int length,
                                          int/*void*/ *p_void)
{
#if 0 //todo vincent ota ali_dfu_image_update_frame
	if (ota_breeze_write(&offset,length, (unsigned char *)p_void) != 0) {
		return	15;
	}
#endif
return 0;

}
#endif