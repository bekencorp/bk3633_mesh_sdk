/**
 ****************************************************************************************
 *
 * @file h4tl.c
 *
 * @brief H4 UART Transport Layer source code.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup H4TL
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"      // stack configuration

#if (H4TL_SUPPORT)

#include <string.h>           // in order to use memset
#include "co_endian.h"        // endian-ness definition
#include "co_utils.h"         // stack common utility definitions
#include "co_error.h"         // error definition
#include "co_hci.h"           // HCI definitions
#include "h4tl.h"             // hci External Interface definition
#include "ke_mem.h"           // kernel memory
#include "ke_msg.h"           // kernel event
#include "ke_event.h"         // kernel event definition
#include "rwip.h"             // rw bt core interrupt

#if (HCI_TL_SUPPORT)
#include "hci.h"              // hci definition
#endif //(HCI_TL_SUPPORT))

#if (AHI_TL_SUPPORT)
#include "ahi.h"              // Application Host Interface definition
#include "gapm_task.h"        // GAP Manager Task
#include "gapm.h"             // GAP Manager Native API
#endif // (AHI_TL_SUPPORT)

#if (BLE_ISOOHCI)
#include "isoohci.h"
#endif //(BLE_ISOOHCI)

#include "dbg.h"
//#include "uart.h"
/*
 * DEFINES
 ****************************************************************************************
 */

/// Size of the RX Buffer that can both receive the header and valid packet to exit from out of sync mode
#if (AHI_TL_SUPPORT)
#define RX_TMP_BUFF_SIZE     AHI_RESET_MSG_LEN
#elif (HCI_TL_SUPPORT)
#define RX_TMP_BUFF_SIZE     HCI_RESET_MSG_LEN
#endif // (AHI_TL_SUPPORT) or (HCI_TL_SUPPORT)


/*
 * ENUMERATIONS DEFINTION
 ****************************************************************************************
 */

///H4TL RX states
enum H4TL_STATE_RX
{
    ///H4TL RX Start State - receive message type
    H4TL_STATE_RX_START,
    ///H4TL RX Header State - receive message header
    H4TL_STATE_RX_HDR,
    ///H4TL RX Header State - receive (rest of) message payload
    H4TL_STATE_RX_PAYL,
    ///H4TL RX Out Of Sync state - receive message type
    H4TL_STATE_RX_OUT_OF_SYNC
};


///H4TL queue type
enum H4TL_QUEUE
{
    /// HCI/AHI Tx Queue
    H4TL_TX_QUEUE_MSG,

    #if (BLE_ISOOHCI)
    /// ISO Tx Queue
    H4TL_TX_QUEUE_ISO,
    #endif // (BLE_ISOOHCI)

    #if (TRACER_PRESENT)
    /// Tracer Message Queue
    H4TL_TX_QUEUE_TRACER,
    #endif // (TRACER_PRESENT)

    /// Maximum number of TX queue
    H4TL_TX_QUEUE_MAX,

    /// Queue in Idle mode
    H4TL_TX_QUEUE_IDLE  = 0xFF,
};



/*
 * STRUCTURES DEFINITION
 ****************************************************************************************
 */

/// Information about message under transmission
struct h4tl_tx_info
{
    /// message buffer
    uint8_t *buf;
    /// Tx callback
    void (*callback)(void);
    /// buffer length
    uint16_t buf_len;
    /// Priority
    uint8_t prio;
};

/// H4TL Environment context structure
struct h4tl_env_tag
{
    /// pointer to External interface api
    const struct rwip_eif_api* ext_if;
    ///Pointer to space reserved for received payload.
    uint8_t *curr_payl_buff;
    /// Ensure that array is 32bits aligned
    /// Latest received message header, or used to receive a message allowing to exit from out of sync
    uint8_t rx_buf[RX_TMP_BUFF_SIZE];

    ///Rx state - can be receiving message type, header, payload or error
    uint8_t rx_state;
    ///Latest received message type: CMD/EVT/ACL.
    uint8_t rx_type;
    /// Transport layer interface
    uint8_t tl_itf;

    /// Maximum Tx queue
    struct h4tl_tx_info tx_queue[H4TL_TX_QUEUE_MAX];

    /// Information about on-going transmission
    uint8_t tx_active_queue;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

///HCI table for correspondence between External Interface message type and header length.
static const uint8_t h4tl_msgtype2hdrlen[]=
{
    #if (HCI_TL_SUPPORT)
    [HCI_CMD_MSG_TYPE]     = HCI_CMD_HDR_LEN,
    [HCI_ACL_MSG_TYPE]     = HCI_ACL_HDR_LEN,
    #if BT_EMB_PRESENT
    #if (VOICE_OVER_HCI)
    [HCI_SYNC_MSG_TYPE]    = HCI_SYNC_HDR_LEN,
    #endif // BT_EMB_PRESENT
    #endif // (VOICE_OVER_HCI)
    [HCI_EVT_MSG_TYPE]     = HCI_EVT_HDR_LEN,
    #endif // (HCI_TL_SUPPORT)
    #if (AHI_TL_SUPPORT)
    [AHI_KE_MSG_TYPE]      = AHI_KE_MSG_HDR_LEN,
    #endif // (AHI_TL_SUPPORT)
    #if (BLE_ISOOHCI)
    [ISOOHCI_ISO_MSG_TYPE] = ISOOHCI_HEADER_LEN,
    #endif //(BLE_ISOOHCI)
    #if (TRACER_PRESENT)
    [TRC_MSG_TYPE]         = TRC_MSG_HDR_LEN,
    #endif // (TRACER_PRESENT)
};

/// H4TL environment context
static struct h4tl_env_tag h4tl_env[H4TL_NB_CHANNEL];

/*
 * LOCAL FUNCTION DECLARATION
 ****************************************************************************************
 */

#if (HCI_TL_SUPPORT)
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
static void h4tl_rx_cmd_hdr_extract(struct h4tl_env_tag* env, struct hci_cmd_hdr * p_hdr);
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)
#if (HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
static void h4tl_rx_acl_hdr_extract(struct h4tl_env_tag* env, struct hci_acl_hdr * p_hdr);
#endif // (HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
#if ((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
static void h4tl_rx_evt_hdr_extract(struct h4tl_env_tag* env, struct hci_evt_hdr * p_hdr);
#endif //((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
static void h4tl_cmd_hdr_rx_evt_handler(void);
static void h4tl_cmd_pld_rx_evt_handler(void);
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)
#endif // (HCI_TL_SUPPORT)

static void h4tl_read_start(struct h4tl_env_tag* env);
static void h4tl_read_hdr(struct h4tl_env_tag* env, uint8_t len);
static void h4tl_read_payl(struct h4tl_env_tag* env, uint16_t len);
static void h4tl_read_next_out_of_sync(struct h4tl_env_tag* env);
static void h4tl_out_of_sync(struct h4tl_env_tag* env);
static bool h4tl_out_of_sync_check(struct h4tl_env_tag* env);
static void h4tl_tx_done(struct h4tl_env_tag* env, uint8_t status);
static void h4tl_rx_done(struct h4tl_env_tag* env, uint8_t status);
static void h4tl_tx_evt_handler(void);

static void h4tl_acl_hdr_rx_evt_handler(void);				////


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if (HCI_TL_SUPPORT)
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
****************************************************************************************
* @brief Local function : extracts command header components
*
* @param[in] env Environment of transport layer
* @param[out]   p_hdr   Pointer to command header structure
*****************************************************************************************
*/
static void h4tl_rx_cmd_hdr_extract(struct h4tl_env_tag* env, struct hci_cmd_hdr * p_hdr)
{
    //extract command header:opcode, parameter length
    p_hdr->opcode = co_btohs(co_read16p(&(env->rx_buf[0])));
    p_hdr->parlen = env->rx_buf[HCI_CMD_OPCODE_LEN];
}
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)

#if (HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
/**
****************************************************************************************
* @brief Local function : extracts ACL header components
*
* @param[in] env Environment of transport layer
* @param[out]   p_hdr   Pointer to ACL header structure
*****************************************************************************************
*/
static void h4tl_rx_acl_hdr_extract(struct h4tl_env_tag* env, struct hci_acl_hdr * p_hdr)
{
    // Extract ACL header: data length, connection handle and flags
    p_hdr->datalen = co_btohs(co_read16p(env->rx_buf + HCI_ACL_HDR_HDL_FLAGS_LEN));
    p_hdr->hdl_flags = co_btohs(co_read16p(env->rx_buf));
}
#endif //(HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)

#if (BT_EMB_PRESENT)
#if (VOICE_OVER_HCI)
/**
****************************************************************************************
* @brief Local function : extracts synchronous header components
*
* @param[in] env Environment of transport layer
* @param[out]   p_hdr   Pointer to synchronous header structure
*****************************************************************************************
*/
static void h4tl_rx_sync_hdr_extract(struct h4tl_env_tag* env, struct hci_sync_hdr * p_hdr)
{
    // Extract ACL header: data length, connection handle and flags
    p_hdr->data_total_len = *(env->rx_buf + HCI_SYNC_HDR_HDL_FLAGS_LEN);
    p_hdr->conhdl_flags = co_btohs(co_read16p(env->rx_buf));
}
#endif // (VOICE_OVER_HCI)
#endif // (BT_EMB_PRESENT)

#if ((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
/**
****************************************************************************************
* @brief Local function : extracts event header components
*
* @param[in] env Environment of transport layer
* @param[out]   p_hdr   Pointer to event header structure
*****************************************************************************************
*/
static void h4tl_rx_evt_hdr_extract(struct h4tl_env_tag* env, struct hci_evt_hdr * p_hdr)
{
    //extract event header:code, parameter length
    p_hdr->code   = env->rx_buf[0];
    p_hdr->parlen = env->rx_buf[HCI_EVT_CODE_LEN];
}
#endif //((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))

#endif // (HCI_TL_SUPPORT)

/**
******************************************************************************************
* @brief Local function : places H4TL in RX_START state and sets the External Interface environment.
*
* @param[in] env Environment of transport layer
******************************************************************************************
*/
static void h4tl_read_start(struct h4tl_env_tag* env)
{
    //uart_printf("%s\r\n",__func__);
    //Initialize External Interface in reception mode state
    env->rx_state = H4TL_STATE_RX_START;

    //Set the External Interface environment to message type 1 byte reception
    env->ext_if->read(&(env->rx_buf[RX_TMP_BUFF_SIZE-1]), H4TL_LOGICAL_CHANNEL_LEN, (rwip_eif_callback) &h4tl_rx_done, env);

    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    // No HCI reception is ongoing, so allow going to sleep
    rwip_prevent_sleep_clear(RW_TL_RX_ONGOING);
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
}


/**
****************************************************************************************
* @brief Local function : places H4TL in RX header state and sets the External Interface env.
*
* @param[in] env Environment of transport layer
* @param[in] len Length of header to be received in the currently set buffer.
*****************************************************************************************
*/
static void h4tl_read_hdr(struct h4tl_env_tag* env, uint8_t len)
{
  //  uart_printf("%s\r\n",__func__);
    //change Rx state - wait for header next
    env->rx_state = H4TL_STATE_RX_HDR;

    //set External Interface environment to header reception of len bytes
    env->ext_if->read(&env->rx_buf[0], len, (rwip_eif_callback) &h4tl_rx_done, env);

    // An HCI reception is ongoing
    rwip_prevent_sleep_set(RW_TL_RX_ONGOING);
}


/**
******************************************************************************************
* @brief Local function : places H4TL in RX payload state and request the External IF
*
* @param[in] env Environment of transport layer
* @param[in] buf Buffer for payload reception
* @param[in] len Length of payload to be received in the currently set buffer.
******************************************************************************************
*/
static void h4tl_read_payl(struct h4tl_env_tag* env, uint16_t len)
{
   // uart_printf("%s\r\n",__func__);
    //change rx state to payload reception
    env->rx_state = H4TL_STATE_RX_PAYL;

    //set External Interface environment to payload reception of len bytes
    env->ext_if->read(env->curr_payl_buff, len, (rwip_eif_callback) &h4tl_rx_done, env);
}

/**
******************************************************************************************
* @brief Local function : places H4TL in RX_START_OUT_OF_SYNC state.
*
* @param[in] env Environment of transport layer
******************************************************************************************
*/
static void h4tl_read_next_out_of_sync(struct h4tl_env_tag* env)
{
    //Set External Interface reception state to H4TL_STATE_RX_START_OUT_OF_SYNC
    env->rx_state = H4TL_STATE_RX_OUT_OF_SYNC;

    //Set the External Interface environment to 1 byte reception (at end of rx buffer)
    env->ext_if->read(&(env->rx_buf[RX_TMP_BUFF_SIZE-1]), H4TL_LOGICAL_CHANNEL_LEN, (rwip_eif_callback) &h4tl_rx_done, env);
}

/**
 *****************************************************************************************
 *@brief Static function handling External Interface out of synchronization detection.
 *
 * At External Interface reception, when packet indicator opcode of a command is not
 * recognized.
 *
* @param[in] env Environment of transport layer
 *****************************************************************************************
 */
static void h4tl_out_of_sync(struct h4tl_env_tag* env)
{
    #if (HCI_TL_SUPPORT && !BLE_HOST_PRESENT)
    // Send HCI hardware error event
    struct hci_hw_err_evt *evt = KE_MSG_ALLOC(HCI_EVENT, 0, HCI_HW_ERR_EVT_CODE, hci_hw_err_evt);
    evt->hw_code = CO_ERROR_HW_UART_OUT_OF_SYNC;
    hci_send_2_host(evt);
    #endif // (HCI_TL_SUPPORT && !BLE_HOST_PRESENT)

    // Initialize receive buffer
    memset(&(env->rx_buf[0]), 0 , RX_TMP_BUFF_SIZE-2);
    // keep last octet received
    env->rx_buf[RX_TMP_BUFF_SIZE-2] = env->rx_buf[RX_TMP_BUFF_SIZE-1];
    // Start reception of new packet ID
    h4tl_read_next_out_of_sync(env);

    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    // No HCI reception is ongoing, so allow going to sleep
    rwip_prevent_sleep_clear(RW_TL_RX_ONGOING);
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
}


/**
 ****************************************************************************************
 * @brief Check received byte in out of sync state
 *
 * This function is the algorithm to check that received byte stream in out of sync state
 * corresponds to HCI_reset command.
 *
 * Level of reception is incremented when bytes of HCI_reset_cmd are detected.
 *
 * @param[in] env Environment of transport layer
 *****************************************************************************************
 */
static bool h4tl_out_of_sync_check(struct h4tl_env_tag* env)
{
    bool sync_ok = false;

    #if (HCI_TL_SUPPORT)
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    const uint8_t hci_reset_msg[HCI_RESET_MSG_LEN] = HCI_RESET_MSG_BUF;

    // check if valid HCI reset has been received
    if(memcmp(&(hci_reset_msg[0]), &(env->rx_buf[RX_TMP_BUFF_SIZE-HCI_RESET_MSG_LEN]) , HCI_RESET_MSG_LEN) == 0)
    {
        // HCI processes the command
        hci_cmd_received(HCI_RESET_CMD_OPCODE, 0, NULL);
        sync_ok = true;
    }
    #endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)
    #endif // (HCI_TL_SUPPORT)

    #if (AHI_TL_SUPPORT)
    // Check reset message
    if(!sync_ok)
    {
        const uint8_t ahi_reset_msg[AHI_RESET_MSG_LEN] = AHI_RESET_MSG_BUF;

        // check if valid HCI reset has been received
        if(memcmp(&(ahi_reset_msg[0]), &(env->rx_buf[RX_TMP_BUFF_SIZE-AHI_RESET_MSG_LEN]) , AHI_RESET_MSG_LEN) == 0)
        {
            // send reset message to application
            struct gapm_reset_cmd* reset = KE_MSG_ALLOC(GAPM_RESET, TASK_GAPM, APP_MAIN_TASK, gapm_reset_cmd);
            reset->operation = GAPM_RESET;
            ke_msg_send(reset);
            sync_ok = true;
        }
    }
    #endif // (AHI_TL_SUPPORT)

    // sync not found, ensure that a packet will be received
    if(!sync_ok)
    {
        uint8_t i;

        // An HCI reception is ongoing
        rwip_prevent_sleep_set(RW_TL_RX_ONGOING);

        // shift received bytes left into rx buffer
        for(i = 0 ; i < (RX_TMP_BUFF_SIZE-1); i++)
        {
            env->rx_buf[i] = env->rx_buf[i+1];
        }
    }

    return sync_ok;
}

/**
 ****************************************************************************************
 * @brief Callback for TL to indicate the end of TX
 *
 * @param[in] env    Environment of transport layer
 * @param[in] status External Interface Tx status: ok or error.
 *****************************************************************************************
 */
static void h4tl_tx_done(struct h4tl_env_tag* env, uint8_t status)
{
    
 
    // Sanity check: Transmission should always work
    ASSERT_ERR(status == RWIP_EIF_STATUS_OK);

    // mark transmission done
    env->tx_queue[env->tx_active_queue].buf = NULL;

    // Defer the processing to ensure that it is done in background
  #if 0		////
    ke_event_set(KE_EVENT_H4TL_TX);
  #else
	h4tl_tx_evt_handler();
  #endif
}

/**
 ****************************************************************************************
 * @brief Actions after External Interface TX.
 *****************************************************************************************
 */
static void h4tl_tx_evt_handler(void)
{
    uint8_t i;
  //   uart_printf("%s\r\n",__func__);
    // Clear the event
    ke_event_clear(KE_EVENT_H4TL_TX);

    bool on_going_tx = false;

    for (i = 0 ; i < H4TL_NB_CHANNEL ; i++)
    {
        struct h4tl_env_tag* env = &(h4tl_env[i]);
        uint8_t queue = env->tx_active_queue;
        struct h4tl_tx_info *tx_info = NULL;

        if(queue != H4TL_TX_QUEUE_IDLE)
        {
            // Inform that tx is done
            tx_info = &(env->tx_queue[queue]);

            if(tx_info->buf == NULL)
            {
                uint8_t cursor;
                uint8_t prio = 0;

                if (tx_info->callback != NULL)
                {
                    tx_info->callback();
                }

                // by default nothing to do
                queue = H4TL_TX_QUEUE_IDLE;

                // search new transmission to perform
                tx_info = NULL;
                for(cursor = 0 ; cursor < H4TL_TX_QUEUE_MAX; cursor++)
                {
                    tx_info = &(env->tx_queue[cursor]);
                    if((tx_info->buf != NULL) && (tx_info->prio > prio))
                    {
                        queue = cursor;
                        prio = tx_info->prio;
                    }
                }

                env->tx_active_queue = queue;
                if(queue != H4TL_TX_QUEUE_IDLE)
                {
                    tx_info = &(env->tx_queue[queue]);
                    env->ext_if->write(tx_info->buf, tx_info->buf_len + HCI_TRANSPORT_HDR_LEN, (rwip_eif_callback)&h4tl_tx_done, env);
                    on_going_tx = true;
                }
            }
            else
            {
                on_going_tx = true;
            }
        }
    }

    // Deep Sleep management
    if(!on_going_tx)
    {
        #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
        // No HCI reception is ongoing, so allow going to sleep [VLE] TODO should be done also in HCI mode
        rwip_prevent_sleep_clear(RW_TL_TX_ONGOING);
        #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    }
}

/**
 ****************************************************************************************
 * @brief Function called at each RX interrupt.
 *
 * @param[in] env    Environment of transport layer
 * @param[in] status External Interface RX status: ok or error
 *****************************************************************************************
 */
static void h4tl_rx_done(struct h4tl_env_tag* env, uint8_t status)
{
    
  //  uart_printf("%s\r\n",__func__);
    // detect that an event occurs on interface
    if (status != RWIP_EIF_STATUS_OK)
    {
        //detect External Interface RX error and handle accordingly
        if ((status == RWIP_EIF_STATUS_ERROR) || (env->rx_state != H4TL_STATE_RX_START))
        {
            // External Interface RX error -> enter in out of sync
            h4tl_out_of_sync(env);
        }
        else
        {
            // restart logical channel reception
            h4tl_read_start(env);
        }
    }
    else
    {
        
        //check HCI state to see what was received
        switch(env->rx_state)
        {
            /* RECEIVE MESSAGE TYPE STATE*/
            case H4TL_STATE_RX_START:
            {   
                // extract RX type
                env->rx_type = env->rx_buf[RX_TMP_BUFF_SIZE-1];
               // uart_printf("RX_START type:%x\r\n",env->rx_type);
                // Check received packet indicator
                switch(env->rx_type)
                {
                    #if (HCI_TL_SUPPORT)
                    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
                    case HCI_CMD_MSG_TYPE:
                    #endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)
                    case HCI_ACL_MSG_TYPE:
                    #if BT_EMB_PRESENT
                    #if (VOICE_OVER_HCI)
                    case HCI_SYNC_MSG_TYPE:
                    #endif // (VOICE_OVER_HCI)
                    #endif //BT_EMB_PRESENT
                    #if BLE_HOST_PRESENT
                    case HCI_EVT_MSG_TYPE:
                    #endif //BLE_HOST_PRESENT
                    {
                        //change state to header reception
                        h4tl_read_hdr(env, h4tl_msgtype2hdrlen[env->rx_type]);
                    }
                    break;
                    #endif // (HCI_TL_SUPPORT)

                    #if (BLE_ISOOHCI)
                    case ISOOHCI_ISO_MSG_TYPE:
                    {
                        //change state to header reception
                        h4tl_read_hdr(env, h4tl_msgtype2hdrlen[env->rx_type]);
                    }
                    break;
                    #endif //(BLE_ISOOHCI)

                    #if (AHI_TL_SUPPORT)
                    case AHI_KE_MSG_TYPE:
                    {
                        //change state to header reception
                        h4tl_read_hdr(env, h4tl_msgtype2hdrlen[env->rx_type]);
                    }
                    break;
                    #endif // (AHI_TL_SUPPORT)

                    #if (TRACER_PRESENT)
                    case TRC_MSG_TYPE:
                    {
                        //change state to header reception
                        h4tl_read_hdr(env, h4tl_msgtype2hdrlen[env->rx_type]);
                    }
                    break;
                    #endif /*(TRACER_PRESENT)*/

                    default:
                    {
                        // Incorrect packet indicator -> enter in out of sync
                        h4tl_out_of_sync(env);
                    }
                    break;
                }
            }
            break;
            /* RECEIVE MESSAGE TYPE STATE END*/
                
            /* RECEIVE HEADER STATE*/
            case H4TL_STATE_RX_HDR:
            {
                // uart_printf("RX_HDR type:%x\r\n",env->rx_type);
                switch (env->rx_type)
                {
                    #if (HCI_TL_SUPPORT)
                    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
                    //Command Header reception
                    case HCI_CMD_MSG_TYPE:
                    {
                    #if (0)		////
                        // Defer the processing to ensure that it is done in background
                        ke_event_set(KE_EVENT_H4TL_CMD_HDR_RX);
					#else
						h4tl_cmd_hdr_rx_evt_handler();
						////h4tl_read_payl(env, env.rx_buf[2]);
					#endif
                    }
                    break;
                    #endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)

                    #if (HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
                    case HCI_ACL_MSG_TYPE:
                    {
                    #if (0)		////
                        // Defer the processing to ensure that it is done in background
                        ke_event_set(KE_EVENT_H4TL_ACL_HDR_RX);
					#else
						h4tl_acl_hdr_rx_evt_handler();
					#endif
                    }
                    break;
                    #endif //(HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)

                    #if (BT_EMB_PRESENT)
                    #if (VOICE_OVER_HCI)
                    case HCI_SYNC_MSG_TYPE:
                    {
                        struct hci_sync_hdr* p_hdr = (struct hci_sync_hdr*) env->rx_buf;

                        // Extract the Synchronous header components
                        h4tl_rx_sync_hdr_extract(env, p_hdr);

                        // HCI allocate a buffer for data reception
                        env->curr_payl_buff = hci_sync_tx_data_alloc(p_hdr->conhdl_flags, p_hdr->data_total_len);

                        // Check data length
                        if (env->curr_payl_buff == NULL)
                        {
                            // Incorrect payload size -> enter in out of sync
                            h4tl_out_of_sync(env);
                        }
                        else
                        {
                            //change HCI rx state to payload reception
                            h4tl_read_payl(env, p_hdr->data_total_len);
                        }
                    }
                    break;
                    #endif // (VOICE_OVER_HCI)
                    #endif // (BT_EMB_PRESENT)

                    #if ((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
                    //Event Header reception
                    case HCI_EVT_MSG_TYPE:
                    {
                        struct hci_evt_hdr* p_hdr = (struct hci_evt_hdr*) env->rx_buf;

                        // Extract the event header components
                        h4tl_rx_evt_hdr_extract(env, p_hdr);

                        // Check if the event has parameters
                        if(p_hdr->parlen == 0)
                        {
                            // HCI processes the event
                            hci_evt_received(p_hdr->code, p_hdr->parlen, NULL);

                            //change hci rx state to message type reception
                            h4tl_read_start(env);
                        }
                        else
                        {
                            // Allocate memory buffer for receiving the payload
                            env->curr_payl_buff = (uint8_t*) ke_malloc(p_hdr->parlen, KE_MEM_KE_MSG);

                            if (env->curr_payl_buff != NULL)
                            {
                                //change HCI rx state to payload reception
                                h4tl_read_payl(env, p_hdr->parlen);
                            }
                            else
                            {
                                // Problem in the RX buffer allocation -> enter in out of sync
                                h4tl_out_of_sync(env);
                            }
                        }
                    }
                    break;
                    #endif //((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
                    #endif // (HCI_TL_SUPPORT)

                    #if (AHI_TL_SUPPORT)
                    case AHI_KE_MSG_TYPE:
                    {
                        struct ahi_kemsghdr * p_msg_hdr = (struct ahi_kemsghdr *) (&env->rx_buf[0]);
                        struct ke_msg* rx_msg;

                        // retrieve destination task
                        ke_task_id_t dest = gapm_get_task_from_id(p_msg_hdr->dest_id);

                        // destination task is unknown
                        if(KE_TYPE_GET(dest) == TASK_NONE)
                        {
                            // Allocate the kernel message to GAPM with source identifier = destination task ID
                            rx_msg = ke_param2msg(ke_msg_alloc(GAPM_UNKNOWN_TASK_MSG, TASK_GAPM,
                                                               p_msg_hdr->dest_id,
                                                               p_msg_hdr->param_len));

                            // Store identifier in parameter length field
                            rx_msg->param_len = p_msg_hdr->id;
                        }
                        // destination task is known
                        else
                        {
                            // Update source and dest task identifier with task numbers
                            p_msg_hdr->src_id  = gapm_get_task_from_id(p_msg_hdr->src_id);

                            // Allocate the kernel message
                            rx_msg = ke_param2msg(ke_msg_alloc(p_msg_hdr->id, dest,
                                                               p_msg_hdr->src_id,
                                                               p_msg_hdr->param_len));
                        }

                        //no params
                        if (p_msg_hdr->param_len == 0)
                        {
                            // Send message directly
                            ke_msg_send(ke_msg2param(rx_msg));

                            // Restart a new packet reception
                            h4tl_read_start(env);
                        }
                        else
                        {
                            env->curr_payl_buff = (uint8_t*) &(rx_msg->param[0]);
                            // Start payload reception
                            h4tl_read_payl(env, p_msg_hdr->param_len);
                        }
                    }
                    break;
                    #endif // (AHI_TL_SUPPORT)

                    #if (BLE_ISOOHCI)
                    case ISOOHCI_ISO_MSG_TYPE:
                    {
                        struct isoohci_pkt_hdr* p_hdr = (struct isoohci_pkt_hdr*) env->rx_buf;

                        // Allocate a buffer for data reception
                        env->curr_payl_buff = isoohci_in_buf_alloc(p_hdr);

                        // Check data length
                        if (env->curr_payl_buff == NULL)
                        {
                            // Incorrect payload size -> enter in out of sync
                            h4tl_out_of_sync(env);
                        }
                        else
                        {
                            //change HCI rx state to payload reception
                            h4tl_read_payl(env, p_hdr->length);
                        }
                    }break;
                    #endif //(BLE_ISOOHCI)

                    #if (TRACER_PRESENT)
                    case TRC_MSG_TYPE:
                    {
                        //change rx state to payload reception
                        env->curr_payl_buff = dbg_trc_pay_buff_get();
                        h4tl_read_payl(env, co_read16p(env->rx_buf + 1));
                    }
                    break;
                    #endif /*(TRACER_PRESENT)*/

                    default:
                    {
                        ASSERT_INFO(0, env->rx_type, env->tl_itf);
                    }
                    break;
                }//end switch
            }
            break;
            /* RECEIVE HEADER STATE END*/

            /* RECEIVE PAYLOAD STATE */
            case H4TL_STATE_RX_PAYL:
            {
                // uart_printf("RX_PAYL type:%x\r\n",env->rx_type);
                switch (env->rx_type)
                {
                    #if (HCI_TL_SUPPORT)
                    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
                    case HCI_CMD_MSG_TYPE:
                    {
                    #if(0)
                        // Defer the processing to ensure that it is done in background
                        ke_event_set(KE_EVENT_H4TL_CMD_PLD_RX);
					#else
						h4tl_cmd_pld_rx_evt_handler();
					#endif
                    }
                    break;
                    #endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)

                    case HCI_ACL_MSG_TYPE:
                    {

                        #if (BT_EMB_PRESENT || (BLE_EMB_PRESENT && HCI_BLE_CON_SUPPORT))
                        struct hci_acl_hdr* p_hdr = (struct hci_acl_hdr *) env->rx_buf;
                        // HCI processes the data
                        hci_acl_tx_data_received(p_hdr->hdl_flags, p_hdr->datalen, env->curr_payl_buff);
                        #elif (BLE_HOST_PRESENT && HCI_BLE_CON_SUPPORT)
                        struct hci_acl_hdr* p_hdr = (struct hci_acl_hdr *) env->rx_buf;
                        // HCI processes the data
                        hci_acl_rx_data_received(p_hdr->hdl_flags, p_hdr->datalen, env->curr_payl_buff);
                        #else
                        ASSERT_ERR(0);
                        #endif //BLE_EMB_PRESENT/BLE_HOST_PRESENT/BT_EMB_PRESENT

                        //change hci rx state to message type reception - common to all types
                        h4tl_read_start(env);
                    }
                    break;

                    #if (BT_EMB_PRESENT)
                    #if (VOICE_OVER_HCI)
                    case HCI_SYNC_MSG_TYPE:
                    {
                        struct hci_sync_hdr* p_hdr = (struct hci_sync_hdr *) env->rx_buf;
                        // HCI processes the data
                        hci_sync_tx_data_received(p_hdr->conhdl_flags, p_hdr->data_total_len, env->curr_payl_buff);

                        //change hci rx state to message type reception - common to all types
                        h4tl_read_start(env);
                    }
                    break;
                    #endif // (VOICE_OVER_HCI)
                    #endif // (BT_EMB_PRESENT)

                    #if ((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
                    case HCI_EVT_MSG_TYPE:
                    {
                        struct hci_evt_hdr* p_hdr = (struct hci_evt_hdr *) env->rx_buf;

                        // HCI processes the event
                        hci_evt_received(p_hdr->code, p_hdr->parlen, env->curr_payl_buff);

                        if (env->curr_payl_buff != NULL)
                        {
                            // Free payload buffer
                            ke_free(env->curr_payl_buff);

                            // Clear current payload buffer pointer
                            env->curr_payl_buff = NULL;
                        }

                        //change hci rx state to message type reception - common to all types
                        h4tl_read_start(env);
                    }
                    break;
                    #endif //((BLE_HOST_PRESENT) && (!BLE_EMB_PRESENT))
                    #endif // (HCI_TL_SUPPORT)
                    #if (AHI_TL_SUPPORT)
                    case AHI_KE_MSG_TYPE:
                    {
                        // Send the kernel message
                        ke_msg_send(env->curr_payl_buff);

                        // Restart a new packet reception
                        h4tl_read_start(env);
                    }
                    break;
                    #endif // (AHI_TL_SUPPORT)
                    #if (BLE_ISOOHCI)
                    case ISOOHCI_ISO_MSG_TYPE:
                    {
                        struct isoohci_pkt_hdr* p_hdr = (struct isoohci_pkt_hdr*) env->rx_buf;

                        // Send the kernel message
                        isoohci_in_buf_send(p_hdr, env->curr_payl_buff);

                        // Restart a new packet reception
                        h4tl_read_start(env);
                    }
                    break;
                    #endif //(BLE_ISOOHCI)
                    #if (TRACER_PRESENT)
                    case TRC_MSG_TYPE:
                    {
                        // Alert the tracer that the configuration packet has been fully received
                        dbg_trc_cfg_received();

                        // Restart a new packet reception
                        h4tl_read_start(env);
                    }
                    break;
                    #endif /*(TRACER_PRESENT)*/
                    default:
                    {
                        ASSERT_INFO(0, env->rx_type, env->tl_itf);
                    }
                    break;
                }
            }
            break;
            /* RECEIVE PAYLOAD STATE END*/

            /* RX OUT OF SYNC STATE */
            case H4TL_STATE_RX_OUT_OF_SYNC:
            {
                // Check received byte
                if(h4tl_out_of_sync_check(env))
                {
                    // sync found, return to normal mode
                    h4tl_read_start(env);
                }
                else
                {
                    // Start a new byte reception
                    h4tl_read_next_out_of_sync(env);
                }
            }
            break;
            /* RX OUT OF SYNC STATE END*/

            /* DEFAULT STATE */
            default:
            {
                ASSERT_ERR(0);
            }
            break;
            /* DEFAULT END*/

        }
        /* STATE SWITCH END */
    }
}

#if (HCI_TL_SUPPORT)
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Actions after reception of HCI command header
 *****************************************************************************************
 */
static void h4tl_cmd_hdr_rx_evt_handler(void)
{
    struct h4tl_env_tag* env  = &(h4tl_env[0]);
    struct hci_cmd_hdr* p_hdr = (struct hci_cmd_hdr*) env->rx_buf;

    // Clear the event
    ke_event_clear(KE_EVENT_H4TL_CMD_HDR_RX);

    // Extract the command header components
    h4tl_rx_cmd_hdr_extract(env, p_hdr);

    // Check if the command has parameters
    if(p_hdr->parlen == 0)
    {
        // HCI processes the command
        hci_cmd_received(p_hdr->opcode, p_hdr->parlen, NULL);

        //change hci rx state to message type reception
        h4tl_read_start(env);
    }
    else
    {
        // Check received parameter size
        if(p_hdr->parlen > hci_cmd_get_max_param_size(p_hdr->opcode))
        {
            // Incorrect header -> enter in out of sync
            h4tl_out_of_sync(env);
        }
        else
        {
            // Allocate memory buffer for receiving the payload
            env->curr_payl_buff = (uint8_t*) ke_malloc(p_hdr->parlen, KE_MEM_KE_MSG);

            if (env->curr_payl_buff != NULL)
            {
                //change HCI rx state to payload reception
                h4tl_read_payl(env, p_hdr->parlen);
            }
            else
            {
                // Problem in the RX buffer allocation -> enter in out of sync
                h4tl_out_of_sync(env);
            }
        }
    }
}

/**
 ****************************************************************************************
 * @brief Actions after reception of HCI command payload
 *****************************************************************************************
 */
static void h4tl_cmd_pld_rx_evt_handler(void)
{
    struct h4tl_env_tag* env = &(h4tl_env[0]);
    struct hci_cmd_hdr* p_hdr = (struct hci_cmd_hdr*) env->rx_buf;
    //uart_printf("h4tl_cmd_pld_rx_evt_handler\r\n");
    // Clear the event
    ke_event_clear(KE_EVENT_H4TL_CMD_PLD_RX);

    // HCI processes the command
    hci_cmd_received(p_hdr->opcode, p_hdr->parlen, env->curr_payl_buff);

    if (env->curr_payl_buff != NULL)
    {
        // Free payload buffer
        ke_free(env->curr_payl_buff);

        // Clear current payload buffer pointer
        env->curr_payl_buff = NULL;
    }

    //change hci rx state to message type reception - common to all types
    h4tl_read_start(env);
}
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)

#if (HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Actions after reception of HCI ACL header
 *****************************************************************************************
 */
static void h4tl_acl_hdr_rx_evt_handler(void)
{
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    struct h4tl_env_tag* env  = &(h4tl_env[0]);
    #elif (BLE_HOST_PRESENT)
    struct h4tl_env_tag* env  = &(h4tl_env[1]);
    #else
    #error "Not Supported"
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    struct hci_acl_hdr* p_hdr = (struct hci_acl_hdr*) env->rx_buf;

    // Clear the event
    ke_event_clear(KE_EVENT_H4TL_ACL_HDR_RX);

    // Extract the ACL header components
    h4tl_rx_acl_hdr_extract(env, p_hdr);

    if(p_hdr->datalen > 0)
    {
        #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
        // HCI allocate a buffer for data reception (HL rx or LL tx)
        env->curr_payl_buff = hci_acl_tx_data_alloc(p_hdr->hdl_flags, p_hdr->datalen);
        #elif BLE_HOST_PRESENT
        // HCI allocate a buffer for data reception (HL rx or LL tx)
        env->curr_payl_buff = hci_acl_rx_data_alloc(p_hdr->hdl_flags, p_hdr->datalen);
        #else
        ASSERT_ERR(0);
        #endif //BLE_EMB_PRESENT/BLE_HOST_PRESENT

        // Check data length
        if (env->curr_payl_buff == NULL)
        {
            // Incorrect payload size -> enter in out of sync
            h4tl_out_of_sync(env);
        }
        else
        {
            //change HCI rx state to payload reception
            h4tl_read_payl(env, p_hdr->datalen);
        }
    }
    else
    {
        // HCI processes the data
        hci_acl_tx_data_received(p_hdr->hdl_flags, 0, NULL);

        // Restart reception of new packet
        h4tl_read_start(env);
    }
}
#endif //(HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
#endif //(HCI_TL_SUPPORT)


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void h4tl_init(uint8_t tl_itf, const struct rwip_eif_api* eif)
{
    uint8_t cursor;

    struct h4tl_env_tag* env = &(h4tl_env[tl_itf]);
    // Store external interface API
    env->ext_if              = eif;
    //initialize tx state
    env->tx_active_queue     = H4TL_TX_QUEUE_IDLE;
    env->tl_itf              = tl_itf;
    // Initialize buffers
    for(cursor = 0 ; cursor < H4TL_TX_QUEUE_MAX; cursor ++)
    {
        env->tx_queue[cursor].buf  = NULL;

        // TODO priority mechanism to be changed
        env->tx_queue[cursor].prio = 0xFF - cursor;
    }

    // Enable External Interface
    env->ext_if->flow_on();

    // Create HCI TX kernel event
    ke_event_callback_set(KE_EVENT_H4TL_TX        , &h4tl_tx_evt_handler        );

    #if (HCI_TL_SUPPORT)
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    ke_event_callback_set(KE_EVENT_H4TL_CMD_HDR_RX, &h4tl_cmd_hdr_rx_evt_handler);
    ke_event_callback_set(KE_EVENT_H4TL_CMD_PLD_RX, &h4tl_cmd_pld_rx_evt_handler);
    #endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)
    #if (HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
    ke_event_callback_set(KE_EVENT_H4TL_ACL_HDR_RX, &h4tl_acl_hdr_rx_evt_handler);
    #endif //(HCI_BLE_CON_SUPPORT || BT_EMB_PRESENT)
    #endif // (HCI_TL_SUPPORT)

    //start External Interface reception
    h4tl_read_start(env);
}


void h4tl_write(uint8_t type, uint8_t *buf, uint16_t len, void (*tx_callback)(void))
{
    uint8_t tl_itf   = H4TL_NB_CHANNEL;
    uint8_t tx_queue = H4TL_TX_QUEUE_IDLE;

    // An HCI transmission is ongoing - The bit has to be set prior to call to write
    // as this function may call h4tl_tx_done immediately
    rwip_prevent_sleep_set(RW_TL_TX_ONGOING);

    //pack event type message (External Interface header)
    buf -= HCI_TRANSPORT_HDR_LEN;
    *buf = type;

    // retrieve Transport Layer Type
    switch(type)
    {
        #if (AHI_TL_SUPPORT)
        case AHI_KE_MSG_TYPE:
        {
            tl_itf = 0;
            tx_queue = H4TL_TX_QUEUE_MSG;
        }
        break;
        #endif // (AHI_TL_SUPPORT)

        #if (BLE_ISOOHCI)
        case ISOOHCI_ISO_MSG_TYPE:
        {
            tl_itf = 0;
            tx_queue = H4TL_TX_QUEUE_ISO;
        }
        break;
        #endif //(BLE_ISOOHCI)

        #if (HCI_TL_SUPPORT)
        case HCI_CMD_MSG_TYPE:
        case HCI_ACL_MSG_TYPE:
        case HCI_SYNC_MSG_TYPE:
        case HCI_EVT_MSG_TYPE:
        {
            #if (!BLE_EMB_PRESENT && !BT_EMB_PRESENT && BLE_HOST_PRESENT)
            tl_itf = H4TL_NB_CHANNEL-1;
            #else // !(!BLE_EMB_PRESENT && !BT_EMB_PRESENT && BLE_HOST_PRESENT)
            tl_itf = 0;
            #endif // (!BLE_EMB_PRESENT && !BT_EMB_PRESENT && BLE_HOST_PRESENT)
            tx_queue = H4TL_TX_QUEUE_MSG;
        }
        break;
        #endif // (HCI_TL_SUPPORT)

        #if (TRACER_PRESENT)
        case TRC_MSG_TYPE:
        {
            #if (!BLE_EMB_PRESENT && !BT_EMB_PRESENT && BLE_HOST_PRESENT)
            tl_itf = 0;
            #else // !(!BLE_EMB_PRESENT && !BT_EMB_PRESENT && BLE_HOST_PRESENT)
            tl_itf = H4TL_NB_CHANNEL-1;
            #endif // (!BLE_EMB_PRESENT && !BT_EMB_PRESENT && BLE_HOST_PRESENT)
            tx_queue = H4TL_TX_QUEUE_TRACER;
        }
        break;
        #endif /*(TRACER_PRESENT)*/

        default:
        {
            ASSERT_INFO(0, type, tl_itf);
        }
        break;
    }

    // Sanity check
    if((tx_queue != H4TL_TX_QUEUE_IDLE) && (tl_itf < H4TL_NB_CHANNEL))
    {
        // fill queued message info
        struct h4tl_env_tag* env = &(h4tl_env[tl_itf]);
        struct h4tl_tx_info *tx_info = &(env->tx_queue[tx_queue]);
        tx_info->buf      = buf;
        tx_info->buf_len  = len;
        tx_info->callback = tx_callback;

        //send only if there is no pending messages
        if(env->tx_active_queue == H4TL_TX_QUEUE_IDLE)
        {
            env->tx_active_queue = tx_queue;
            env->ext_if->write(buf, len + HCI_TRANSPORT_HDR_LEN, (rwip_eif_callback)&h4tl_tx_done, env);
        }
    }
    else
    {
        ASSERT_INFO(0, type, tl_itf);
    }
}

void h4tl_start(void)
{
    uint8_t i;

    for (i = 0 ; i < H4TL_NB_CHANNEL ; i++)
    {
        // Enable External Interface flow
        h4tl_env[i].ext_if->flow_on();
    }
}

bool h4tl_stop(void)
{
    bool res = true;
    uint8_t i;
    for (i = 0 ; (i < H4TL_NB_CHANNEL) && (res); i++)
    {
        // Disable External Interface flow
        res = h4tl_env[i].ext_if->flow_off();
    }

    // If Flow cannot be stopped on all interfaces, restart flow on interfaces stopped
    if(!res)
    {
        uint8_t j;
        for (j = 0 ; (j < i) ; j++)
        {
            // Enable External Interface flow
            h4tl_env[j].ext_if->flow_on();
        }
    }

    return (res);
}

#endif //(H4TL_SUPPORT)

/// @} H4TL
