/**
 ****************************************************************************************
 *
 * @file appm_task.c
 *
 * @brief RW APP Task implementation
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"          // SW configuration

#if (BLE_APP_PRESENT)
#include <string.h>
#include "app_task.h"              // Application Manager Task API
#include "app.h"                      // Application Manager Definition
#include "arch.h"                    // Platform Definitions

#include "co_utils.h"
#include "ke_timer.h"             // Kernel timer


#include "uart_pub.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t app_get_handler(const struct app_subtask_handlers *handler_list_desc,
                               ke_msg_id_t msgid,
                               void *param,
                               ke_task_id_t src_id)
{
    // Counter
    uint8_t counter;

    // Get the message handler function by parsing the message table
    for (counter = handler_list_desc->msg_cnt; 0 < counter; counter--)
    {
        struct ke_msg_handler handler
                = (struct ke_msg_handler)(*(handler_list_desc->p_msg_handler_tab + counter - 1));

        if ((handler.id == msgid) ||
            (handler.id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERR(handler.func);

            return (uint8_t)(handler.func(msgid, param, TASK_APP, src_id));
        }
    }

    // If we are here no handler has been found, drop the message
    return (KE_MSG_CONSUMED);
}


/*******************************************************************************
 * Function: app_period_timer_handler
 * Description: app period timer process
 * Input: msgid -Id of the message received.
 *		  param -Pointer to the parameters of the message.
 *		  dest_id -ID of the receiving task instance (TASK_GAP).
 *		  ID of the sending task instance.
 * Return: If the message was consumed or not.
 * Others: void
*******************************************************************************/
extern uint32_t prog_timer_cnt;
static int app_period_timer_handler(ke_msg_id_t const msgid,
                                          void *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    printf("%s\r\n", __func__);
    #if(STORE_DBG_MSG)
   	printf("%s\r\n", __func__);
//    CHARLES_PRINTF("<func>%s\r\n",__func__);
    #endif
    //DEBUG_MSG(0xEF);
    #if SAVE_INFO
    if (m_tb_state_get_prov_state() == M_TB_STATE_PROV_STATE_PROV)
    {
        extern uint8_t gneed_to_save_to_nvds;
        if(gneed_to_save_to_nvds == 1)
        {
            m_api_set_store_time_var(20000);
            m_api_storage_config(m_api_get_store_time_var());
        }
        else
        {
            //1 :M_LAY_PROXY_CTL_ADV_START_NODE 2 : M_LAY_PROXY_CTL_ADV_START_NET
            m_api_proxy_ctrl(M_LAY_PROXY_CTL);
        }
    }
    #endif

    ke_time_func.ke_timer_clear(APP_PERIOD_TIMER,TASK_APP);
	ke_time_func.ke_timer_set(APP_PERIOD_TIMER,TASK_APP,200);
    return KE_MSG_CONSUMED;
}


/**
 ****************************************************************************************
 * @brief Handles reception of all messages sent from the lower layers to the application
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int appm_msg_handler(ke_msg_id_t const msgid,
                            void *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    // Retrieve identifier of the task from received message
    ke_task_id_t src_task_id = MSG_T(msgid);
    // Message policy
    uint8_t msg_pol          = KE_MSG_CONSUMED;


#if 0
    switch (src_task_id)
    {
        case (TASK_ID_GAPC):
        {
            //chengyi change to PTS_TEST_FLAG
            //from
            //#if(PTS_WITHOUT_MESH)
            //to
            #if(MESH_PROFILE_PTS_TEST)
            //end
            if ((msgid >= GAPC_BOND_CMD) &&
                (msgid <= GAPC_SECURITY_IND))//PTS test GAP layer
            {
                // Call the Security Module
                msg_pol = appm_get_handler(&app_sec_table_handler, msgid, param, src_id);
            }
            #endif
            // else drop the message
        } break;
            // else drop the message
      

        case (TASK_ID_GATTC):
        {
            // Service Changed - Drop
        } break;

        case (TASK_ID_FFF0S):
        {
            // Call the Health Thermometer Module
            msg_pol = appm_get_handler(&app_fff0_table_handler, msgid, param, src_id);
        } break;
				
        case (TASK_ID_DISS):
        {
            // Call the Device Information Module
            msg_pol = appm_get_handler(&app_dis_table_handler, msgid, param, src_id);
        } break;

        case (TASK_ID_BASS):
        {
            // Call the Battery Module
            msg_pol = appm_get_handler(&app_batt_table_handler, msgid, param, src_id);
        } break;
		
        case (TASK_ID_OADS):
        {
            // Call the Health Thermometer Module
            msg_pol = appm_get_handler(&app_oads_table_handler, msgid, param, src_id);
        } break;
				
				
		case (TASK_ID_MESH):
        {
            // Call the Health Thermometer Module
            msg_pol = appm_get_handler(&app_mesh_table_handler, msgid, param, src_id);
        } break;
        default:
        {
        } break;
    }
#endif

    return (msg_pol);
}

#if 0
/*******************************************************************************
 * Function: gapc_update_conn_param_req_ind_handler
 * Description: Update request command processing from slaver connection parameters
 * Input: msgid   -Id of the message received.
 *		  param   -Pointer to the parameters of the message.
 *		  dest_id -ID of the receiving task instance
 *		  src_id  -ID of the sending task instance.
 * Return: If the message was consumed or not.
 * Others: void
*******************************************************************************/
static int gapc_update_conn_param_req_ind_handler (ke_msg_id_t const msgid, 
									const struct gapc_param_update_req_ind  *param,
                 					ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

	UART_PRINTF("slave send param_update_req\r\n");
	struct gapc_conn_param  up_param;
	
	up_param.intv_min   = BLE_UAPDATA_MIN_INTVALUE;
	up_param.intv_max   = BLE_UAPDATA_MAX_INTVALUE; 
	up_param.latency    = BLE_UAPDATA_LATENCY;  
	up_param.time_out   = BLE_UAPDATA_TIMEOUT; 
//	CHARLES_PRINTF("<func>%s,intv_min :D'%d,intv_max:D'%d,TO:D'%d\r\n",__func__,up_param.intv_min,up_param.intv_max,up_param.time_out);
	appm_update_param(&up_param);
	
	return KE_MSG_CONSUMED;
}

 
/*******************************************************************************
 * Function: gapc_le_pkt_size_ind_handler
 * Description: GAPC_LE_PKT_SIZE_IND
 * Input: msgid   -Id of the message received.
 *		  param   -Pointer to the parameters of the message.
 *		  dest_id -ID of the receiving task instance
 *		  src_id  -ID of the sending task instance.
 * Return: If the message was consumed or not.
 * Others: void
*******************************************************************************/
static int gapc_le_pkt_size_ind_handler (ke_msg_id_t const msgid, 
									const struct gapc_le_pkt_size_ind  *param,
                 					ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
   	UART_PRINTF("%s \r\n", __func__);
	UART_PRINTF("1max_rx_octets = %d\r\n",param->max_rx_octets);
	UART_PRINTF("1max_rx_time = %d\r\n",param->max_rx_time);
	UART_PRINTF("1max_tx_octets = %d\r\n",param->max_tx_octets);
	UART_PRINTF("1max_tx_time = %d\r\n",param->max_tx_time);
	
	return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief  GAPC_PARAM_UPDATED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_updated_ind_handler (ke_msg_id_t const msgid, 
									const struct gapc_param_updated_ind  *param,
                 					ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    UART_PRINTF("%s \r\n", __func__);
	UART_PRINTF("con_interval = %d\r\n",param->con_interval);
	UART_PRINTF("con_latency = %d\r\n",param->con_latency);
	UART_PRINTF("sup_to = %d\r\n",param->sup_to);
    //CHARLES_PRINTF("con_interval = %d\r\n",param->con_interval);
    //CHARLES_PRINTF("sup_to = %d\r\n",param->sup_to);
	DEBUG_MSG(0xAC);
    //m_tb_store_config(100);//parameter update finished,set 10 sec to start store timer
	return KE_MSG_CONSUMED;
}


/**
 ****************************************************************************************
 * @brief  GATTC_MTU_CHANGED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_mtu_changed_ind_handler(ke_msg_id_t const msgid,
                                     struct gattc_mtu_changed_ind const *ind,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
	UART_PRINTF("%s \r\n",__func__);
	UART_PRINTF("ind->mtu = %d,seq = %d\r\n",ind->mtu,ind->seq_num);
	
 	return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief   GAPC_PARAM_UPDATE_REQ_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_update_req_ind_handler(ke_msg_id_t const msgid,
                                struct gapc_param_update_req_ind const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
	UART_PRINTF("%s \r\n", __func__);
	// Prepare the GAPC_PARAM_UPDATE_CFM message
    struct gapc_param_update_cfm *cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                                             src_id, dest_id,
                                             gapc_param_update_cfm);
	 
	cfm->ce_len_max = 0xffff;
	cfm->ce_len_min = 0xffff;
	cfm->accept = true; 

	// Send message
    ke_msg_send(cfm);
	 
	return (KE_MSG_CONSUMED);
}
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

#if 0
static int hci_ahi_msg_process_handler(ke_msg_id_t const msgid,
                                struct hci_ahi_msg_pro_t *param,ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
	uint8_t states = 0;
	
	static uint8_t wait_cnt = 0;
	UART_PRINTF("hci_ahi_msg_process_handler start\r\n");
	uint8_t (*callback) (void*, uint8_t) = NULL;
    void* data =NULL;

	callback = param->callback;
	data = param->dummy;
	if(callback != NULL)
	{
		// Call handler
		UART_PRINTF("callback 0x%08x\r\n",callback);
//		if(wait_cnt > 3)
//		{
//			states = callback(data, RWIP_EIF_STATUS_ERROR);
//			wait_cnt = 0;
//		}else
		{
			states = callback(data, RWIP_EIF_STATUS_OK);
		}
	  	
	}
//	if(states == 1)
//	{
//		//ke_timer_set(HCI_AHI_MSG_PRO_TMR,dest_id ,100);
//		wait_cnt++;
//		UART_PRINTF("SAVED hci_ahi_msg_process_handler end,states : %d,wait_cnt:%d\r\n",states,wait_cnt);
//		return (KE_MSG_SAVED);
//	}
	wait_cnt = 0;
	UART_PRINTF("CONSUMED hci_ahi_msg_process_handler end,states : %d,wait_cnt:%d\r\n",states,wait_cnt);
	return (KE_MSG_CONSUMED);
}

static int hci_ahi_msg_timer_handler(ke_msg_id_t const msgid,
								void *param,ke_task_id_t const dest_id,
								ke_task_id_t const src_id)
{
	UART_PRINTF("hci_ahi_msg_timer_handler\r\n");	
	 
	ke_state_t states = ke_state_get(dest_id);
	ke_state_set(dest_id, APPM_PRO_HCI_AHI_MSG);
	ke_state_set(dest_id, states);
	
	return (KE_MSG_CONSUMED);
}
#endif

static int hci_no_command_handler(ke_msg_id_t const msgid,
								void *param,ke_task_id_t const dest_id,
								ke_task_id_t const src_id)
{
	//UART_PRINTF("%s\r\n", __func__);

	hci_event_recv_process(NULL);

	return (KE_MSG_CONSUMED);
}

/*static int app_hci_send_handler(ke_msg_id_t const msgid,
								struct app_hci_send_data *param, ke_task_id_t const dest_id,
								ke_task_id_t const src_id)
{
	//UART_PRINTF("%s\r\n", __func__);

	appm_hci_send(param->param);

	return (KE_MSG_CONSUMED);
}*/

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(appm)
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,    	(ke_msg_func_t)appm_msg_handler},
#if 0
    {GAPM_DEVICE_READY_IND,     	(ke_msg_func_t)gapm_device_ready_ind_handler},
    {GAPM_CMP_EVT,             		(ke_msg_func_t)gapm_cmp_evt_handler},
    {GAPC_GET_DEV_INFO_REQ_IND, 	(ke_msg_func_t)gapc_get_dev_info_req_ind_handler},
    {GAPC_SET_DEV_INFO_REQ_IND, 	(ke_msg_func_t)gapc_set_dev_info_req_ind_handler},
    {GAPC_CONNECTION_REQ_IND,   	(ke_msg_func_t)gapc_connection_req_ind_handler},
    {GAPC_CMP_EVT,             		(ke_msg_func_t)gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       	(ke_msg_func_t)gapc_disconnect_ind_handler},
    {GAPM_PROFILE_ADDED_IND,    	(ke_msg_func_t)gapm_profile_added_ind_handler},
    {GAPC_LE_PKT_SIZE_IND,			(ke_msg_func_t)gapc_le_pkt_size_ind_handler},
    {GAPC_PARAM_UPDATED_IND,		(ke_msg_func_t)gapc_param_updated_ind_handler},
    {GATTC_MTU_CHANGED_IND,			(ke_msg_func_t)gattc_mtu_changed_ind_handler},	
    {GAPC_PARAM_UPDATE_REQ_IND, 	(ke_msg_func_t)gapc_param_update_req_ind_handler},
    {APP_PARAM_UPDATE_REQ_IND, 		(ke_msg_func_t)gapc_update_conn_param_req_ind_handler},
		
	{HCI_AHI_MSG_PRO_CMD,			(ke_msg_func_t)hci_ahi_msg_process_handler},
		
	{HCI_AHI_MSG_PRO_TMR,			(ke_msg_func_t)hci_ahi_msg_timer_handler},
#endif
    {APP_PERIOD_TIMER,				(ke_msg_func_t)app_period_timer_handler},

    //{APP_HCI_SEND_HANDLER,			(ke_msg_func_t)app_hci_send_handler},

	{APP_HCI_NO_CMD_HANDLER,		(ke_msg_func_t)hci_no_command_handler},
};

/* Defines the place holder for the states of all the task instances. */
ke_state_t appm_state[APP_IDX_MAX];

// Application task descriptor
const struct ke_task_desc TASK_DESC_APP = {appm_msg_handler_tab, appm_state, APP_IDX_MAX, ARRAY_LEN(appm_msg_handler_tab)};

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
