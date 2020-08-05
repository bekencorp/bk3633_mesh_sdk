/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)
#include <string.h>
//#include "rwapp_config.h"
#include "app_task.h"                // Application task Definition
#include "app.h"                     // Application Definition
//#include "gap.h"                     // GAP Definition
//#include "gapm_task.h"               // GAP Manager Task API
//#include "gapc_task.h"               // GAP Controller Task API

#include "co_bt.h"                   // Common BT Definition
#include "co_math.h"                 // Common Maths Definition
#include "co_list.h"
#include "ke_timer.h"
//#include "app_fff0.h"                 // Application security Definition
//#include "app_dis.h"                 // Device Information Service Application Definitions
//#include "app_batt.h"                // Battery Application Definitions
//#include "app_oads.h"                 // Application oads Definition
//#include "app_mesh.h"

//#include "nvds.h"                    // NVDS Definitions
//#include "rf.h"
#include "uart.h"
//#include "adc.h"
//#include "gpio.h"
#include "wdt.h"

//#include "em_buf.h"
#include "hci.h"
#include "app_hci.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define APP_DEVICE_NAME_LENGTH_MAX      (18)


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*appm_add_svc_func_t)(void);

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of service to add in the database
enum appm_svc_list
{
    //APPM_SVC_FFF0,
    //APPM_SVC_DIS,
    //APPM_SVC_BATT,
	//APPM_SVC_OADS,
	//APPM_SVC_MESH,
    APPM_SVC_LIST_STOP ,
};

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
extern const struct ke_task_desc TASK_DESC_APP;

/// List of functions used to create the database

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Environment Structure
struct app_env_tag  app_env;

static hci_recv_cb_t  hci_event_recv = NULL;


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */ 
//uint8_t appm_set_dev_name(char * name,uint8_t len);
void appm_init(void)
{
    uint8_t key_len = KEY_LEN;
 
    // Reset the application manager environment
    memset(&app_env, 0, sizeof(app_env));
    printf("********* %s *********\n", __func__);
    // Create APP task
    ke_task_func.ke_task_create(TASK_APP, &TASK_DESC_APP);

    // Initialize Task state
    ke_task_func.ke_state_set(TASK_APP, APPM_INIT);
}

void hci_event_recv_process(void *recv_buf)
{
	if(hci_event_recv) {
		(*hci_event_recv)((app_hci_buf_t *)recv_buf);
	}
	else {
		UART_PRINTF("hci_event_recv is NULL!\r\n");
	}
}

void hci_event_process(uint8_t type, uint8_t *buf, uint16_t len)
{
	if(type == HCI_EVT_MSG_TYPE) {
		struct hci_evt_hdr *evt_hdr = buf;
		HCI_DBG("evt rx, event:0x%x, p_len:%d, len:%d\r\n",
								evt_hdr->code, evt_hdr->parlen, len);
	}
	else if(type == HCI_ACL_MSG_TYPE) {
		struct hci_acl_hdr *acl_hdr = buf;
		HCI_DBG("acl rx, conhdl:0x%x, p_len:%d, len:%d\r\n",
								acl_hdr->hdl_flags, acl_hdr->datalen, len);
	}

	if(!type) {
		HCI_DBG("%s, unknown type %d !\r\n", __func__, type);
		return;
	}

	if(len > HCI_RECV_BUF_LEN) {
		HCI_DBG("receive data length %d > rx buffer size %d\r\n", len, HCI_RECV_BUF_LEN);
		return;
	}

	///fill receive event buffer
	app_hci_buf_t *hci_recv = krhino_mm_alloc(sizeof(app_hci_buf_t));
	hci_recv->cont = 0;
	hci_recv->type = type;
	hci_recv->buf_len = len;
	memcpy(hci_recv->buf, buf, len);

    ///run hci event receive call back
    hci_event_recv_process(hci_recv);

    krhino_mm_free(hci_recv);
}

uint16_t appm_hci_recv_reg_callback(hci_recv_cb_t callback)
{
	if(!callback) {
		HCI_DBG("%s, callback is NULL!\r\n", __func__, callback);
	    return -1;
	}

	///setting hci event receive call back
    hci_event_recv = callback;
    return 0;
}

uint16_t appm_hci_send_to_controller(app_hci_buf_t *hci_send)
{
	uint16_t err=0;
    uint8_t  conidx  = 0;

    ///check send command type
	switch(hci_send->type)
	{
    case HCI_CMD_MSG_TYPE:
    	HCI_INFO("send CMD opcode:0x%x,m:%d\r\n", hci_send->cmd.opcode, ke_task_func.ke_state_get(TASK_LLM));

    	///create send command message
    	void *cmd_data = ke_msg_func.ke_msg_alloc(HCI_COMMAND, conidx, hci_send->cmd.opcode, hci_send->cmd.para_len);
    	if(cmd_data) {
			memcpy(cmd_data, hci_send->cmd.para, hci_send->cmd.para_len);

			///send command to controller
			err = hci_func.hci_send_2_controller(cmd_data);
    	}
    	else {
    		UART_PRINTF("create send command message error!\r\n");
    		err = -1;
    	}
        break;

    case HCI_ACL_MSG_TYPE:
    	HCI_INFO("send ACL handle:0x%x\r\n", hci_send->acl.handle);

    	///get hci acl data buffer point
    	uint8_t *acl_buf = hci_func.hci_acl_tx_data_alloc(hci_send->acl.handle, hci_send->acl.data_len);
    	if(acl_buf) {
			memcpy(acl_buf, hci_send->acl.data, hci_send->acl.data_len);

			///send acl to controller
			hci_func.hci_acl_tx_data_received(hci_send->acl.handle, hci_send->acl.data_len, acl_buf);
    	}
    	else {
    		UART_PRINTF("get hci acl data buffer error!\r\n");
    		err = -1;
    	}
    	break;

    default:
    	UART_PRINTF("%s no support %d type\r\n", __func__, hci_send->type);
    	err = -1;
    	break;
	}

	return err;
}

#endif //(BLE_APP_PRESENT)

/// @} APP


