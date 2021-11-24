/* main.c - light demo */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include <hal/soc/gpio.h>
#include <hal/soc/pwm.h>
#include "inc/time_scene_server.h"
#ifdef BOARD_BK3633DEVKIT
#include "gpio_pub.h"
#endif

#include "mesh_model/inc/ble_mesh.h"
#include <mesh/access.h>


#include "light_board.h"
#include "uart_test_cmd.h"

#include "JX_vendor.h"
#include "JX_app.h"


JX_vendor_struct JX_vendor_flag;

void JX_vendor_init(void)
{
	memset(&JX_vendor_flag, 0, sizeof(JX_vendor_flag));
	JX_vendor_flag.tid =1 +rand() %20;
}

void JX_tid_add(void)
{
	JX_vendor_flag.tid ++;
	if(JX_vendor_flag.tid ==0)
	{
		JX_vendor_flag.tid ++;
	}
}

//publish onoff
void JX_onoff_send(u8 elem_num, u8 onoff_value)
{
	static u8 tid =0x11;
	struct bt_mesh_elem *elem;
	struct bt_mesh_model *model;

	u8_t status;

	u16_t addr = bt_mesh_primary_addr() +elem_num;

	LIGHT_DBG("addr:%x num:%d onoff_value:%d", addr, elem_num, onoff_value);

	elem = bt_mesh_elem_find(addr);

	if(elem == NULL)
	{
		LIGHT_DBG("elem is not exit !");
		return ;
	}

	model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);

	if(model == NULL)
	{
		LIGHT_DBG("model is not exit !");
		return;
	}
	else
	{
		int err;

		struct bt_mesh_msg_ctx ctx;

		struct net_buf_simple *p_msg = NET_BUF_SIMPLE(3 + 17 + 4);

		bt_mesh_model_msg_init(p_msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
		net_buf_simple_add_u8(p_msg, onoff_value);

		ctx.addr = 0xF000;
		//app key ��index
		ctx.app_idx = 0;
		//net key ��index
		ctx.net_idx = 0;
		//����ttl
		ctx.send_ttl = 5;

//		LIGHT_DBG("%s", bt_hex(p_msg, 3));
		//��������
		err = bt_mesh_model_send(model, &ctx, p_msg, NULL, NULL);

		if (err)
		{
			BT_ERR("bt_mesh_model_publish err %d\n", err);
			return;
		}
	}
}

void JX_vendor_send(u8 elem_num, u8 op, u8 *pBuffer, u8 len)
{
	 struct bt_mesh_elem *elem;
	 struct bt_mesh_model *model;

	 u8_t status;
	 u16_t addr = bt_mesh_primary_addr() +elem_num;

//	 u8 pos =0;
//	 u8 payload[20] = {0};

	 elem = bt_mesh_elem_find(addr);
	 LIGHT_DBG("addr=%x %x elem=%x\r\n", addr, elem_num);

	 if(elem == NULL)
	 {
		LIGHT_DBG("elem is not exit !");
		return ;
	 }

	if(len >= 10)
	{
	   printf("********** %s, len is too large %d ***********\n", len);
	   krhino_backtrace_now();
	   while(1);
	   return ;
	}

	model = bt_mesh_model_find_vnd(elem, CONFIG_MESH_VENDOR_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV);

	if(model == NULL)
	{
		LIGHT_DBG("model is not exit !");
		return;
	}
	else
	{
		int err;

		struct bt_mesh_msg_ctx ctx;

		struct net_buf_simple *p_msg_buf = NET_BUF_SIMPLE(3 + 17 + 4);//op:3 par ????17

		bt_mesh_model_msg_init(p_msg_buf, BT_MESH_MODEL_OP_3(op, CONFIG_MESH_VENDOR_COMPANY_ID));


//		payload[len++] = 0x01;
//		payload[len++] = tid;
		net_buf_simple_add_mem(p_msg_buf, pBuffer, len);
		LIGHT_DBG("op:%x len:%d: %s", op, len, bt_hex(pBuffer, len));

		ctx.addr = 0xF000;
		ctx.app_idx = 0;
		ctx.net_idx = 0;
		ctx.send_ttl = 5;

		err = bt_mesh_model_send(model, &ctx, p_msg_buf, NULL, NULL);

		if (err)
		{
			BT_ERR("bt_mesh_model_publish err %d\n", err);
			return;
		}
	}
}

void JX_vendor_queue(void)
{
	if(!JX_vendor_flag.send_flag && JX_vendor_flag.wait_size)
	{
		u32 current_time =JX_current_time();
		u8 cur_num =JX_vendor_flag.cur_num;
		JX_send_vendor_struct *pVendor_send =&JX_vendor_flag.wait_struct[cur_num];

		u8 elem_num =pVendor_send->elem_num;
		u8 op =pVendor_send->op;

		u8 send_cnt =pVendor_send->cnt;
		u8 len =pVendor_send->len;
		u8 *pBuffer =pVendor_send->buffer;

		JX_vendor_flag.wait_size--;
		JX_vendor_flag.cur_num =(JX_vendor_flag.wait_size +1) %JX_VENDOR_SIZE;

		JX_tid_add();

		JX_vendor_flag.send_flag =1;
		JX_vendor_flag.send_struct.elem_num =elem_num;
		JX_vendor_flag.send_struct.op =op;
		JX_vendor_flag.send_struct.cnt =send_cnt;
		JX_vendor_flag.send_cnt =send_cnt;
		JX_vendor_flag.send_time_ms =current_time;

		JX_vendor_flag.send_struct.len =len;
		JX_vendor_flag.send_struct.buffer[0] =JX_vendor_flag.tid;
		memcpy(JX_vendor_flag.send_struct.buffer+1, pBuffer, len);
        if (len >= 10) {
			printf("********** %s, Length (%d) Invalid ************\n", __func__, len);
			krhino_backtrace_now();
			while(1);
		}
		JX_vendor_send(elem_num, op, JX_vendor_flag.send_struct.buffer, len+1);
		LIGHT_DBG("line:%d elem_num:%d op:%x send_cnt:%d len:%d  %s", __LINE__, JX_vendor_flag.send_struct.elem_num,
			JX_vendor_flag.send_struct.op, JX_vendor_flag.send_cnt, len, bt_hex(JX_vendor_flag.send_struct.buffer, len+1));
	}
}

void JX_vendor_loop(void)
{
	if(!JX_vendor_flag.send_flag)
	{
		JX_vendor_queue();
		return;
	}

	if(JX_vendor_flag.send_cnt ==0)
	{
		JX_vendor_flag.send_flag =0;
		return;
	}

	JX_vendor_flag.send_cnt --;

	u32 current_time =JX_current_time();

	if(JX_vendor_flag.send_time_ms +200 <= current_time)
	{
		LIGHT_DBG("line:%d op:%x send_cnt:%d len:%d  %s", __LINE__,
			JX_vendor_flag.send_struct.op, JX_vendor_flag.send_cnt, JX_vendor_flag.send_struct.len, bt_hex(JX_vendor_flag.send_struct.buffer, JX_vendor_flag.send_struct.len));
		JX_vendor_flag.send_time_ms =current_time;
		JX_vendor_send(JX_vendor_flag.send_struct.elem_num, JX_vendor_flag.send_struct.op, JX_vendor_flag.send_struct.buffer, JX_vendor_flag.send_struct.len);
	}
}

u8 JX_vendor_auto_send(u8 elem_num, u8 op, u8 *pBuffer, u8 len, u8 send_cnt)
{
	if(!bt_mesh_is_provisioned())
	{
		return 1;
	}

	if(JX_vendor_flag.send_flag ==1 || JX_vendor_flag.wait_size >0)
	{
		if(JX_vendor_flag.wait_size < JX_VENDOR_SIZE)
		{
			u8 store_num =(JX_vendor_flag.cur_num + JX_vendor_flag.wait_size) %JX_VENDOR_SIZE;
			JX_send_vendor_struct *pVendor_store;

			pVendor_store =&JX_vendor_flag.wait_struct[store_num];

			pVendor_store->elem_num =elem_num;
			pVendor_store->op =op;
			pVendor_store->cnt =send_cnt;

			pVendor_store->len =len;
			memcpy(pVendor_store->buffer, pBuffer, len);
            if (len > 10) {
				printf("************** %s, Invlaid len(%d)\n", __func__, len);
				krhino_backtrace_now();
				while(1);
			}
			JX_vendor_flag.wait_size ++;
		}

		return 1;
	}

	u32 current_time =JX_current_time();

	JX_tid_add();

	JX_vendor_flag.send_flag =1;
	JX_vendor_flag.send_struct.elem_num =elem_num;
	JX_vendor_flag.send_struct.op =op;
	JX_vendor_flag.send_struct.cnt =send_cnt;
	JX_vendor_flag.send_cnt =send_cnt;
	JX_vendor_flag.send_time_ms =current_time;

	JX_vendor_flag.send_struct.len =len;
	JX_vendor_flag.send_struct.buffer[0] =JX_vendor_flag.tid;
	memcpy(JX_vendor_flag.send_struct.buffer+1, pBuffer, len);

	JX_vendor_send(elem_num, op, JX_vendor_flag.send_struct.buffer, len+1);
	LIGHT_DBG("line:%d elem_num:%d op:%x send_cnt:%d len:%d  %s", __LINE__, JX_vendor_flag.send_struct.elem_num,
 		JX_vendor_flag.send_struct.op, JX_vendor_flag.send_cnt, len, bt_hex(JX_vendor_flag.send_struct.buffer, len+1));

	return 0;
}

void JX_vendor_rec(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    u8 tid = net_buf_simple_pull_u8(buf);
	if(tid == JX_vendor_flag.send_struct.buffer[0])
	{
		JX_vendor_flag.send_flag = 0;
		JX_vendor_flag.send_cnt = 0;
	}
}

void JX_vendor_onoff(u8 elem_num, u8 op, u8 tid)
{
	u8 pos =0;
	u8 pBuffer[10];

	pBuffer[pos++] = tid;
	pBuffer[pos++] = (u8)ONOFF_T;
	pBuffer[pos++] = (u8)(ONOFF_T>>8);
	pBuffer[pos++] = switch_para.single_switch[0];	//

	JX_vendor_send(elem_num, op, pBuffer, pos);
}

//�ظ�APP ��������
void JX_vendor_rsp_set(u8 elem_num, u8 tid)
{
	u8 pos =0;
	u8 pBuffer[10];

	LIGHT_DBG("tid:%d", tid);
	pBuffer[pos++] = tid;

	JX_vendor_send(elem_num, VD_HW_ATTR_SET_STATUS, pBuffer, pos);
}

//APP��ѯ �汾��
void JX_vendor_version(u8 op, u8 tid)
{
	u8 pos =0;
	u8 pBuffer[10];

	pBuffer[pos++] = tid;
	pBuffer[pos++] = (u8)ATTR_VERSION;
	pBuffer[pos++] = (u8)(ATTR_VERSION>>8);
	pBuffer[pos++] = 0;//MCU�̼��汾��
	pBuffer[pos++] = 0;//
	pBuffer[pos++] = (u8)PROJECT_SW_VERSION;
	pBuffer[pos++] = (u8)(PROJECT_SW_VERSION>>8);
	pBuffer[pos++] = 1;//Ӳ���汾��

	JX_vendor_send(0, op, pBuffer, pos);
}

//�����ϱ��汾��
void JX_vendor_auto_version(u8 send_cnt)
{
	u8 pos =0;
	u8 pBuffer[10];

	LIGHT_DBG("send_cnt:%d", send_cnt);
	pBuffer[pos++] = (u8)ATTR_VERSION;
	pBuffer[pos++] = (u8)(ATTR_VERSION>>8);
	pBuffer[pos++] = 0;//MCU�̼��汾��
	pBuffer[pos++] = 0;//
	pBuffer[pos++] = (u8)PROJECT_SW_VERSION;
	pBuffer[pos++] = (u8)(PROJECT_SW_VERSION>>8);
	pBuffer[pos++] = 1;//Ӳ���汾��

	JX_vendor_auto_send(0, VD_HW_ATTR_NOTIFY, pBuffer, pos, send_cnt);
}

//�����ϱ�Ӳ����λ
void JX_vendor_hard_reset(u8 send_cnt)
{
	u8 pos =0;
	u8 pBuffer[10];

	LIGHT_DBG("send_cnt:%d", send_cnt);
	pBuffer[pos++] = (u8)ATTR_EVENT_RESET;
	pBuffer[pos++] = (u8)(ATTR_EVENT_RESET>>8);

	JX_vendor_auto_send(0, VD_HW_ATTR_NOTIFY, pBuffer, pos, send_cnt);
}


