#include <zephyr.h>
#include <misc/byteorder.h>
#include "ais_btmesh_service.h"
#include <net/buf.h>
#include <bluetooth.h>
#include <conn.h>
#include <gatt.h>
#include <api/mesh.h>
#include <errno.h>
#include "mesh_def.h"

#include "ais_common.h"
#include "bt_mesh_custom_log.h"
#include "common/log.h"
#include "ali_dfu_port.h"
#include "ali_dfu.h"
#include <stdbool.h>
#include "ais_btmesh.h"

ais_hal_port_t g_ais_port;

void ais_port_init()
{
	g_ais_port.flash_id = 0x0;
	g_ais_port.erase_size = 4096;
	g_ais_port.ota_tmp_size = 256*1024;
	g_ais_port.write_size = 256;
}

#if 1
static ota_breeze_rec_t* ais_btmesh_queue = NULL;
static unsigned char ais_btmesh_queue_ptr_in = 0;
static unsigned char ais_btmesh_queue_ptr_out = 0;

static unsigned int ais_btmesh_creat_receive_buf()
{
    if(ais_btmesh_queue != NULL) {
        return OTA_BREEZE_ERROR_INVALID_PARAM;
    }
    ais_btmesh_queue = ota_malloc(OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    if(ais_btmesh_queue == NULL) {
        return OTA_BREEZE_ERROR_NULL;
    }
    memset(ais_btmesh_queue, 0x00, OTA_BREEZE_RECEIVE_BUF_NUMB * sizeof(ota_breeze_rec_t));
    return OTA_BREEZE_SUCCESS;
}

static void ais_btmesh_destroy_receive_buf()
{
    if(ais_btmesh_queue != NULL) {
        ota_free(ais_btmesh_queue);
    }
    ais_btmesh_queue = NULL;
}

static unsigned int ais_btmesh_init_receive_buf()
{
    unsigned int error_code = 0;
    ais_btmesh_destroy_receive_buf();
    error_code = ais_btmesh_creat_receive_buf();
    if(error_code != OTA_BREEZE_SUCCESS) {
        return error_code;
    }
    ais_btmesh_queue_ptr_in = 0;
    ais_btmesh_queue_ptr_out = 0;
    return OTA_BREEZE_SUCCESS;
}

static void ais_btmesh_receive_data_product(unsigned char ota_cmd, unsigned char num_frame, unsigned char *buffer, unsigned int length)
{
    if(ais_btmesh_queue == NULL) {
        return;
    }
    if(length > OTA_BREEZE_REC_PER_FRAME_LEN) {
        return;
    }

    ais_btmesh_queue[ais_btmesh_queue_ptr_in].cmd = ota_cmd;
    ais_btmesh_queue[ais_btmesh_queue_ptr_in].length = length;
    ais_btmesh_queue[ais_btmesh_queue_ptr_in].num_frames = num_frame;
    if(buffer != NULL) {
        memcpy(ais_btmesh_queue[ais_btmesh_queue_ptr_in].rec_buf, buffer, length);
    }
    ais_btmesh_queue_ptr_in++;
    ais_btmesh_queue_ptr_in = ais_btmesh_queue_ptr_in % OTA_BREEZE_RECEIVE_BUF_NUMB;
}

static unsigned int ais_btmesh_receive_data_consume(ota_breeze_rec_t* out_queue)
{
    if(ais_btmesh_queue == NULL) {
        return OTA_BREEZE_ERROR_NULL;
    }
    if(ais_btmesh_queue_ptr_out != ais_btmesh_queue_ptr_out) {
        out_queue->cmd = ais_btmesh_queue[ais_btmesh_queue_ptr_out].cmd;
        out_queue->length = ais_btmesh_queue[ais_btmesh_queue_ptr_out].length;
        out_queue->num_frames = ais_btmesh_queue[ais_btmesh_queue_ptr_out].num_frames;
        memcpy(out_queue->rec_buf, ais_btmesh_queue[ais_btmesh_queue_ptr_out].rec_buf, out_queue->length);
        ais_btmesh_queue_ptr_out++;
        ais_btmesh_queue_ptr_out = ais_btmesh_queue_ptr_out % OTA_BREEZE_RECEIVE_BUF_NUMB;
    }
    else {
        return OTA_BREEZE_ERROR_NULL;
    }
    return OTA_BREEZE_SUCCESS;
}

void ota_write_flash_start()
{
	ota_breeze_rec_t tmp_queue;
	ota_breeze_state_t cur_breeze_status;
	ota_breeze_version_t* tmp_verion = NULL;
	unsigned int error_code = 0;
	unsigned char send_err = false;

	while(1) {
		if(ais_btmesh_receive_data_consume(&tmp_queue) == OTA_BREEZE_SUCCESS) {
			if ((tmp_queue.cmd & OTA_BREEZE_CMD_TYPE_MASK) != OTA_BREEZE_CMD_TYPE_FW_UPGRADE) {
				OTA_LOG_E("cmd err\r\n");
			}
		}
		ota_msleep(1);
	}
OTA_BREEZE_OVER:
	OTA_LOG_I("task over!\r\n");
	ais_btmesh_destroy_receive_buf();
	return 0;

}
#endif
