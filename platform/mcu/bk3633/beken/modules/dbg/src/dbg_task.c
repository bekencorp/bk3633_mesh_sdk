/**
 ****************************************************************************************
 *
 * @file dbg_task.c
 *
 * @brief DBG task source file
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup DBGTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"         // stack configuration
#if (RW_DEBUG && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
#include <string.h>               // for mem* functions
#include "co_error.h"
#include "co_utils.h"
#include "arch.h"                 // arch definition
#include "dbg.h"                  // debug block definition
#include "rwip.h"                 // RF API definitions
#include "ke.h"                   // kernel definitions
#include "ke_mem.h"               // kernel memory management
#include "ke_task.h"              // kernel task management

#include "sch_plan.h"             // scheduling planner

#if (RW_DEBUG_FLASH)
#include "flash.h"                // flash functions and macros
#endif //RW_DEBUG_FLASH

#if (RW_WLAN_COEX_TEST || RW_MWS_COEX_TEST)
#include "dbg_mwsgen.h"
#endif //(RW_WLAN_COEX_TEST || RW_MWS_COEX_TEST)

#if BLE_IQ_GEN
#include "dbg_iqgen.h"
#endif //BLE_IQ_GEN

#if (HCI_PRESENT)
#include "hci.h"
#endif //(HCI_PRESENT)


/*
 * DEFINES
 ****************************************************************************************
 */

/// 8 bit access types
#define _8_Bit                              8
/// 16 bit access types
#define _16_Bit                             16
/// 32 bit access types
#define _32_Bit                             32

/// PLATFORM RESET REASON: Reset and load FW from flash
#define PLATFORM_RESET_TO_FW    (0)
/// PLATFORM RESET REASON: Reset and stay in ROM code
#define PLATFORM_RESET_TO_ROM   (1)

/// Maximum number of instances of the DBG task
#define DBG_IDX_MAX 1


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read from memory.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_mem_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_rd_mem_cmd const *param,
                       ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    #if (RW_DEBUG_MEM)
    uint8_t length = 0;
    uint32_t init_addr = 0;
    uint32_t value = 0;
    uint32_t i = 0;
    #endif //RW_DEBUG_MEM

    // structure type for the complete command event
    struct hci_dbg_basic_rd_data_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_basic_rd_data_cmd_cmp_evt);

    #if (RW_DEBUG_MEM)
    init_addr = param->start_addr;
    length = param->length;


    /* Check that data length is not null or too big before reading */
    if ((length == 0)||
        (length > (sizeof(struct buffer_tag)- sizeof(uint8_t))))
    {
        event->status = CO_ERROR_INVALID_HCI_PARAM;
    }
    else
    {
        /* Check type of data to be read */
        if (param->type == _8_Bit)
        {
            /* Read bytes */
            for (i = 0; i < length; i++)
            {
                /* Read value at @ set in Param1+i */
                event->buf.data[i] = *(volatile uint8_t *)(init_addr+i);
            }
        }
        else if (param->type == _16_Bit)
        {
            for (i = 0; i < length; i += 2)
            {
                /* Read value at @ set in Param1+i */
                value = (*(volatile uint16_t *)(init_addr+i));

                /* store in the buffer */
                event->buf.data[i]   = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+1] = (uint8_t) value;
             }
        }
        else if (param->type == _32_Bit)
        {
            /* Read 32 bit word */
            for (i = 0; i < length; i += 4)
            {
                value = (*(volatile uint32_t *)(init_addr+i));

                /* store in the buffer */
                event->buf.data[i]   = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+1] = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+2] = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+3] = (uint8_t) value;
            }
        }
        event->buf.length = length;
        event->status = CO_ERROR_NO_ERROR;
    }
    #else //RW_DEBUG_MEM
    event->status = CO_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_MEM

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write to memory.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_mem_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_wr_mem_cmd const *param,
                       ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    #if (RW_DEBUG_MEM)
    uint32_t value = 0;
    uint32_t length = 0;
    uint32_t init_addr = 0;
    uint8_t *data_buf;
    uint32_t i = 0;
    #endif //RW_DEBUG_MEM

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_WR_MEM_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    #if (RW_DEBUG_MEM)
    length    = param->buf.length;
    data_buf  = (uint8_t*)&param->buf.data[0];
    init_addr = param->start_addr;

    /* Check that data length is not null or too big before reading */
    if ((length == 0)|| (length > (sizeof(struct buffer_tag)- sizeof(uint8_t))))
    {
        event->status = CO_ERROR_INVALID_HCI_PARAM;
    }
    else
    {
        /* Check type of data to be written */
        if (param->type == _8_Bit)
        {
            /* Write bytes */
            for (i = 0; i < length; i++)
            {
                /* Set value type at @ Param1 */
                *(volatile uint8_t *)(init_addr+i) = data_buf[i];
            }
        }
        else if (param->type == _16_Bit)
        {
            /* Write 16 bits word */
            for (i = 0; i < length; i += 2)
            {
                /* Set value type at @ Param1 */
                value = ((uint32_t)data_buf[i+1]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+0]);
                *(volatile uint16_t *)(init_addr+i) = value;
            }
        }
        else if(param->type == _32_Bit)
        {
            /* Write 32 bit word */
            for (i = 0; i < length; i += 4)
            {
                /* Set value at @ Param1 */
                value  = ((uint32_t)data_buf[i+3]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+2]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+1]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+0]);
                *(volatile uint32_t *)(init_addr+i) = value;
            }
        }
        event->status = CO_ERROR_NO_ERROR;
    }
    #else //RW_DEBUG_MEM
    event->status = CO_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_MEM

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command delete param.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_del_par_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_del_param_cmd const *param,
                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_DEL_PAR_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    rwip_param.del(param->param_tag);
    event->status = CO_ERROR_NO_ERROR;

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command id flash.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_id_flash_cmd_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_id_flash_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_id_flash_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    flash_identify(&event->flash_id, NULL);
    event->status = (event->flash_id != FLASH_TYPE_UNKNOWN)?CO_ERROR_NO_ERROR : CO_ERROR_HARDWARE_FAILURE;
    #else //RW_DEBUG_FLASH
    event->status = CO_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command flash erase.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_er_flash_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_er_flash_cmd const *param,
                            ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_basic_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    event->status = flash_erase(param->flashtype, param->startoffset, param->size, NULL);
    #else //RW_DEBUG_FLASH
    event->status = CO_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write flash.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_flash_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_wr_flash_cmd const *param,
                            ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_basic_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    event->status = flash_write(param->flashtype, param->startoffset, param->buf.length, (uint8_t*)&param->buf.data[0], NULL);
    #else //RW_DEBUG_FLASH
    event->status = CO_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read flash.
 *
 * Read the requested number of byte from flash.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_flash_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_rd_flash_cmd const *param,
                           ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_basic_rd_data_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_basic_rd_data_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    event->buf.length = param->size;
    event->status     = flash_read(param->flashtype, param->startoffset, param->size, (uint8_t*)&event->buf.data[0], NULL);
    #else //RW_DEBUG_FLASH
    event->status = CO_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read
 *        parameter request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_par_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_rd_par_cmd const *param,
                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_basic_rd_data_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_basic_rd_data_cmd_cmp_evt);
    event->buf.length = sizeof(event->buf.data);
    rwip_param.get(param->param_tag, (uint8_t*)&event->buf.length, (uint8_t*)&event->buf.data[0]);
    event->status = CO_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write
 *        parameter request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_par_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_wr_par_cmd const *param,
                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_basic_cmd_cmp_evt);

    rwip_param.set(param->param_tag, param->buf.length, (uint8_t*)&param->buf.data[0]);
    event->status = CO_ERROR_NO_ERROR;

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Read Kernel Stats
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_ke_stats_cmd_handler(ke_msg_id_t const msgid, void const *param,
                              ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_rd_ke_stats_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, 0, HCI_DBG_RD_KE_STATS_CMD_OPCODE, hci_dbg_rd_ke_stats_cmd_cmp_evt);

    #if (KE_PROFILING)
    if(ke_stats_get(&event->max_msg_sent, &event->max_msg_saved, &event->max_timer_used, &event->max_heap_used) == KE_SUCCESS)
    {
        event->status = CO_ERROR_NO_ERROR;
    }
    else
    {
        event->status = CO_ERROR_UNSUPPORTED;
        event->max_msg_sent   = 0;
        event->max_msg_saved  = 0;
        event->max_timer_used = 0;
        event->max_heap_used  = 0;
    }
    #if (RW_DEBUG_STACK_PROF)
    event->max_stack_used = get_stack_usage();
    #else //RW_DEBUG_STACK_PROF
    event->max_stack_used = 0;
    #endif //RW_DEBUG_STACK_PROF
    #else //KE_PROFILING
    event->status         = CO_ERROR_UNSUPPORTED;
    event->max_msg_sent   = 0;
    event->max_msg_saved  = 0;
    event->max_timer_used = 0;
    event->max_heap_used  = 0;
    event->max_stack_used = 0;
    #endif //KE_PROFILING

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Read Memory info
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int hci_dbg_rd_mem_info_cmd_handler(ke_msg_id_t const msgid, void const *param,
                              ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct hci_dbg_rd_mem_info_cmd_cmp_evt * meminfo_msg;
    #if (KE_PROFILING)
    uint8_t cursor;
    // First remove command message in order to be sure it's not taken in account.
    ke_msg_free(ke_param2msg(param));

    /// memory size currently used into each heaps.
    uint16_t mem_used[KE_MEM_BLOCK_MAX];
    /// peak of memory usage measured
    uint32_t max_mem_used;


    // Retrieve kernel information and put it into a cache
    for(cursor = 0; cursor < KE_MEM_BLOCK_MAX ; cursor++)
    {
        mem_used[cursor] = ke_get_mem_usage(cursor);
    }
    max_mem_used = ke_get_max_mem_usage();

    // Finally send indication to application that request memory information
    meminfo_msg = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RD_MEM_INFO_CMD_OPCODE, hci_dbg_rd_mem_info_cmd_cmp_evt);

    for(cursor = 0; cursor < KE_MEM_BLOCK_MAX ; cursor++)
    {
        meminfo_msg->mem_used[cursor] = mem_used[cursor];
    }
    meminfo_msg->max_mem_used = max_mem_used;
    meminfo_msg->status = CO_ERROR_NO_ERROR;
    #else
    // Finally send indication to application that request memory information
    meminfo_msg = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RD_MEM_INFO_CMD_OPCODE, hci_dbg_rd_mem_info_cmd_cmp_evt);
    meminfo_msg->status = CO_ERROR_UNSUPPORTED;
    #endif // (KE_PROFILING)

    // sends the message
    hci_send_2_host(meminfo_msg);

    // source message already free
    return (KE_MSG_NO_FREE);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Read RF Register value.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rf_reg_rd_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_rf_reg_rd_cmd const *param,
                          ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_rf_reg_rd_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RF_REG_RD_CMD_OPCODE, hci_dbg_rf_reg_rd_cmd_cmp_evt);

    // Read RF register
    event->value   = rwip_rf.reg_rd(param->addr);
    event->addr    = param->addr;
    event->status  = CO_ERROR_NO_ERROR;

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Write RF Register value.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rf_reg_wr_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_rf_reg_wr_cmd const *param,
                          ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_rf_reg_wr_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RF_REG_WR_CMD_OPCODE, hci_dbg_rf_reg_wr_cmd_cmp_evt);

    // Write RF register
    rwip_rf.reg_wr(param->addr, param->value);

    event->addr    = param->addr;
    event->status  = CO_ERROR_NO_ERROR;

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command platform reset.
 * This command reset FW and return to ROM code.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_plf_reset_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_plf_reset_cmd const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint32_t error_code = RESET_NO_ERROR;

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_PLF_RESET_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    switch(param->reason)
    {
        case PLATFORM_RESET_TO_FW: error_code = RESET_AND_LOAD_FW; break;
        case PLATFORM_RESET_TO_ROM: error_code = RESET_TO_ROM; break;
        default: break;
    }

    if(error_code != RESET_NO_ERROR)
    {
#if	(ROM_REGISTER_CALLBACK)
			 // Initialize sleep manager 
			if(rom_env.platform_reset != NULL)
			{
				rom_env.platform_reset(error_code);
			}			
#else		
			
       // Perform platform Reset (return to ROM code)
       platform_reset(error_code);
#endif //(ROM_REGISTER_CALLBACK)
		
    }

    // If reason is not valid, returns a CC event with bad status
    event->status = CO_ERROR_INVALID_HCI_PARAM;

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

#if (BT_EMB_PRESENT)
/// Handle HCI_DBG_Set_Local_Clock command
static int
hci_dbg_set_local_clock_cmd_handler(ke_msg_id_t const msgid,
                         struct hci_dbg_set_local_clock_cmd const *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event;

    // Set clock
    rwip_time_set(param->clock);

    // Sends the CC event
    event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_SET_LOCAL_CLOCK_CMD_OPCODE, hci_basic_cmd_cmp_evt);
    event->status = CO_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}
#endif // (BT_EMB_PRESENT)

#if (RW_WLAN_COEX)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command wlan coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wlan_coex_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_wlan_coex_cmd const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_WLAN_COEX_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    rwip_wlcoex_set(param->state);

    // If reason is not valid, returns a CC event with bad status
    event->status = CO_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

#if (RW_WLAN_COEX_TEST)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command wlan coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wlan_coextst_scen_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_wlan_coextst_scen_cmd const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_WLAN_COEXTST_SCEN_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    event->status = dbg_wlcoex_scen_set(param->scenario);

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}
#endif //RW_WLAN_COEX_TEST
#endif //RW_WLAN_COEX

#if (RW_MWS_COEX)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command mws coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_mws_coex_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_mws_coex_cmd const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_MWS_COEX_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    rwip_mwscoex_set(param->state);

    // If reason is not valid, returns a CC event with bad status
    event->status = CO_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

#if (RW_MWS_COEX_TEST)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command mws coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_mws_coextst_scen_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_mws_coextst_scen_cmd const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_MWS_COEXTST_SCEN_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    event->status = dbg_mwscoex_scen_set(param->scenario);

    // sends the message
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}
#endif //RW_MWS_COEX_TEST
#endif //RW_MWS_COEX

#if (SCH_PLAN_UT)
/// Activities used for testing scheduling planner
#define SCH_PLAN_ACT_NB   3
struct sch_plan_elt_tag dbg_sch_plan_act[SCH_PLAN_ACT_NB];
uint16_t dbg_sch_plan_move_id;

/// Callback function for activity move
static void dbg_sch_plan_move (uint16_t id)
{
    dbg_sch_plan_move_id = id;
}

/// Handle HCI_DBG_Test_Sch_Plan_Set command
static int
hci_dbg_test_sch_plan_set_cmd_handler(ke_msg_id_t const msgid,
                         struct hci_dbg_test_sch_plan_set_cmd const *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id)
{
    struct hci_dbg_test_sch_plan_set_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_TEST_SCH_PLAN_SET_CMD_OPCODE, hci_dbg_test_sch_plan_set_cmd_cmp_evt);;

    if ((param->id > 0) && (param->id <= SCH_PLAN_ACT_NB))
    {
        dbg_sch_plan_move_id = 0;

        // Set activity into the planner
        dbg_sch_plan_act[param->id-1].conhdl       = param->id;
        dbg_sch_plan_act[param->id-1].interval     = param->interval;
        dbg_sch_plan_act[param->id-1].offset       = param->offset;
        dbg_sch_plan_act[param->id-1].duration_min = param->duration_min;
        dbg_sch_plan_act[param->id-1].duration_max = param->duration_max;
        dbg_sch_plan_act[param->id-1].cb_move      = &dbg_sch_plan_move;
        dbg_sch_plan_act[param->id-1].mobility     = param->mobility_level;
        sch_plan_set(&dbg_sch_plan_act[param->id-1]);

        event->status   = CO_ERROR_NO_ERROR;
        event->moved_id = dbg_sch_plan_move_id;
    }
    else
    {
        event->status   = CO_ERROR_UNKNOWN_CONNECTION_ID;
    }

    // Sends the CC event
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/// Handle HCI_DBG_Test_Sch_Plan_Rem command
static int
hci_dbg_test_sch_plan_rem_cmd_handler(ke_msg_id_t const msgid,
                         struct hci_dbg_test_sch_plan_rem_cmd const *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id)
{
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_TEST_SCH_PLAN_REM_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    if ((param->id > 0) && (param->id <= SCH_PLAN_ACT_NB))
    {
        // Set activity into the planner
        sch_plan_rem(&dbg_sch_plan_act[param->id-1]);

        event->status   = CO_ERROR_NO_ERROR;
    }
    else
    {
        event->status   = CO_ERROR_UNKNOWN_CONNECTION_ID;
    }

    // Sends the CC event
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/// Handle HCI_DBG_Test_Sch_Plan_Chk command
static int
hci_dbg_test_sch_plan_chk_cmd_handler(ke_msg_id_t const msgid,
                         struct hci_dbg_test_sch_plan_chk_cmd const *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id)
{
    struct sch_plan_chk_param chk_param;
    uint8_t status;
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_TEST_SCH_PLAN_CHK_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    // Check parameters to the planner
    chk_param.conhdl       = param->id;
    chk_param.interval     = param->interval;
    chk_param.offset       = param->offset;
    chk_param.duration_min = param->duration_min;
    status = sch_plan_chk(&chk_param);

    // Sends the CC event
    event->status   = status;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/// Handle HCI_DBG_Test_Sch_Plan_Req command
static int
hci_dbg_test_sch_plan_req_cmd_handler(ke_msg_id_t const msgid,
                         struct hci_dbg_test_sch_plan_req_cmd const *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id)
{
    struct sch_plan_req_param req_param;
    uint8_t status;
    struct hci_dbg_test_sch_plan_req_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_TEST_SCH_PLAN_REQ_CMD_OPCODE, hci_dbg_test_sch_plan_req_cmd_cmp_evt);;

    // Request parameters to the planner
    req_param.conhdl       = param->id;
    req_param.interval_min = param->interval_min;
    req_param.interval_max = param->interval_max;
    req_param.duration_min = param->duration_min;
    req_param.duration_max = param->duration_max;
    req_param.pref_period  = param->period;
    req_param.odd_offset   = false;
    status = sch_plan_req(&req_param);

    // Sends the CC event
    event->status       = status;
    event->interval     = req_param.interval;
    event->offset_min   = req_param.offset_min;
    event->offset_max   = req_param.offset_max;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}
#endif // (SCH_PLAN_UT)

#if BLE_IQ_GEN
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command iqgen config.
 * Configures the I&Q sample generator.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_iqgen_cfg_cmd_handler(ke_msg_id_t const msgid, struct hci_dbg_iqgen_cfg_cmd const *param,
        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_DBG_IQGEN_CFG_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    event->status = dbg_iqgen_config(param);

    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}
#endif //BLE_IQ_GEN

/// The message handlers for HCI command complete events
static const struct ke_msg_handler dbg_hci_command_handler_tab[] =
{
    {HCI_DBG_RD_MEM_CMD_OPCODE,         (ke_msg_func_t)hci_dbg_rd_mem_cmd_handler},
    {HCI_DBG_WR_MEM_CMD_OPCODE,         (ke_msg_func_t)hci_dbg_wr_mem_cmd_handler},
    {HCI_DBG_DEL_PAR_CMD_OPCODE,        (ke_msg_func_t)hci_dbg_del_par_cmd_handler},
    {HCI_DBG_ID_FLASH_CMD_OPCODE,       (ke_msg_func_t)hci_dbg_id_flash_cmd_handler},
    {HCI_DBG_ER_FLASH_CMD_OPCODE,       (ke_msg_func_t)hci_dbg_er_flash_cmd_handler},
    {HCI_DBG_WR_FLASH_CMD_OPCODE,       (ke_msg_func_t)hci_dbg_wr_flash_cmd_handler},
    {HCI_DBG_RD_FLASH_CMD_OPCODE,       (ke_msg_func_t)hci_dbg_rd_flash_cmd_handler},
    {HCI_DBG_RD_PAR_CMD_OPCODE,         (ke_msg_func_t)hci_dbg_rd_par_cmd_handler},
    {HCI_DBG_WR_PAR_CMD_OPCODE,         (ke_msg_func_t)hci_dbg_wr_par_cmd_handler},
    {HCI_DBG_RD_KE_STATS_CMD_OPCODE,    (ke_msg_func_t)hci_dbg_rd_ke_stats_cmd_handler},
    {HCI_DBG_RF_REG_RD_CMD_OPCODE,      (ke_msg_func_t)hci_dbg_rf_reg_rd_cmd_handler},
    {HCI_DBG_RF_REG_WR_CMD_OPCODE,      (ke_msg_func_t)hci_dbg_rf_reg_wr_cmd_handler},
    {HCI_DBG_PLF_RESET_CMD_OPCODE,      (ke_msg_func_t)hci_dbg_plf_reset_cmd_handler},
    #if (RW_WLAN_COEX)
    {HCI_DBG_WLAN_COEX_CMD_OPCODE,      (ke_msg_func_t)hci_dbg_wlan_coex_cmd_handler},
    #if (RW_WLAN_COEX_TEST)
    {HCI_DBG_WLAN_COEXTST_SCEN_CMD_OPCODE, (ke_msg_func_t)hci_dbg_wlan_coextst_scen_cmd_handler},
    #endif //RW_WLAN_COEX_TEST
    #endif //RW_WLAN_COEX
    #if (RW_MWS_COEX)
    {HCI_DBG_MWS_COEX_CMD_OPCODE,      (ke_msg_func_t)hci_dbg_mws_coex_cmd_handler},
    #if (RW_MWS_COEX_TEST)
    {HCI_DBG_MWS_COEXTST_SCEN_CMD_OPCODE, (ke_msg_func_t)hci_dbg_mws_coextst_scen_cmd_handler},
    #endif //RW_MWS_COEX_TEST
    #endif //RW_MWS_COEX
    {HCI_DBG_RD_MEM_INFO_CMD_OPCODE,    (ke_msg_func_t)hci_dbg_rd_mem_info_cmd_handler},
    #if (BT_EMB_PRESENT)
    {HCI_DBG_SET_LOCAL_CLOCK_CMD_OPCODE, (ke_msg_func_t)hci_dbg_set_local_clock_cmd_handler},
    #endif // (BT_EMB_PRESENT)
    #if (SCH_PLAN_UT)
    {HCI_DBG_TEST_SCH_PLAN_SET_CMD_OPCODE, (ke_msg_func_t)hci_dbg_test_sch_plan_set_cmd_handler},
    {HCI_DBG_TEST_SCH_PLAN_REM_CMD_OPCODE, (ke_msg_func_t)hci_dbg_test_sch_plan_rem_cmd_handler},
    {HCI_DBG_TEST_SCH_PLAN_CHK_CMD_OPCODE, (ke_msg_func_t)hci_dbg_test_sch_plan_chk_cmd_handler},
    {HCI_DBG_TEST_SCH_PLAN_REQ_CMD_OPCODE, (ke_msg_func_t)hci_dbg_test_sch_plan_req_cmd_handler},
    #endif // (SCH_PLAN_UT)
    #if BLE_IQ_GEN
    {HCI_DBG_IQGEN_CFG_CMD_OPCODE, (ke_msg_func_t)hci_dbg_iqgen_cfg_cmd_handler},
    #endif //BLE_IQ_GEN
};

/**
 ****************************************************************************************
 * @brief Handles any HCI command
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_command_handler(ke_msg_id_t const msgid,
        void const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    int return_status = KE_MSG_CONSUMED;

    // Check if there is a handler corresponding to the original command opcode
    for(int i = 0; i < (sizeof(dbg_hci_command_handler_tab)/sizeof(dbg_hci_command_handler_tab[0])); i++)
    {
        // Check if opcode matches
        if(dbg_hci_command_handler_tab[i].id == src_id)
        {
            // Check if there is a handler function
            if(dbg_hci_command_handler_tab[i].func != NULL)
            {
                // Call handler
                return_status = dbg_hci_command_handler_tab[i].func(src_id, param, dest_id, src_id);
            }
            break;
        }
    }

    return return_status;
}

/*
 * TASK DESCRIPTOR DEFINITIONS
 ****************************************************************************************
 */

/// Message handlers table
KE_MSG_HANDLER_TAB(dbg)
{
    {HCI_COMMAND,         (ke_msg_func_t)hci_command_handler},
};

/// Defines the placeholder for the states of all the task instances.
ke_state_t dbg_state[DBG_IDX_MAX];

/// DEBUG task descriptor
const struct ke_task_desc TASK_DESC_DBG = {dbg_msg_handler_tab, dbg_state, DBG_IDX_MAX, ARRAY_LEN(dbg_msg_handler_tab)};

#endif // (RW_DEBUG && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
/// @} DBGTASK
