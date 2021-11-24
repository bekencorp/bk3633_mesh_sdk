#ifndef __JX_VENDOR_H
#define __JX_VENDOR_H

#include "common/log.h"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <bluetooth.h>
#include <soc.h>
#include <api/mesh.h>
#include "genie_app.h"
#include "gpio_pub.h"

#include "light_board.h"

typedef struct
{
	u8 send_flag;
	u8 elem_num;
	u8 op;
	u8 tid;

	u8 send_cnt;
	u32 send_time_ms;//发送间隔时间

	u8 len;
	u8 buffer[10];
}JX_vendor_struct;

extern JX_vendor_struct JX_vendor_flag;

void JX_vendor_init(void);

void JX_onoff_send(u8 elem_num, u8 onoff_value);
void JX_vendor_send(u8 elem_num, u8 op, u8 *pBuffer, u8 len);

void JX_vendor_loop(void);
u8 JX_vendor_auto_send(u8 elem_num, u8 op, u8 *pBuffer, u8 len, u8 send_cnt);
void JX_vendor_rec(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);



void JX_vendor_onoff(u8 elem_num, u8 op, u8 tid);
void JX_vendor_rsp_set(u8 elem_num, u8 tid);

void JX_vendor_version(u8 op, u8 tid);
void JX_vendor_auto_version(u8 send_cnt);

void JX_vendor_hard_reset(u8 retry_cnt);



#endif

