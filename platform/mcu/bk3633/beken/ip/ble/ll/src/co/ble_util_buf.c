/**
 ****************************************************************************************
 *
 * @file ble_util_buf.c
 *
 * @brief BLE EM buffers
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup BLE_UTIL_BUF
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"       // SW configuration

#if (BT_DUAL_MODE || BLE_STD_MODE)       
#include <string.h>
#include "arch.h"
#include "co_bt.h"
#include "co_utils.h"
#include "co_math.h"
#include "ble_util_buf.h"
#include "em_map.h"
#include "ke_mem.h"

#include "em_map_ble.h"
/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * STRUCTURE DEFINITION
 ****************************************************************************************
 */

/// BLE EM buffer management environment structure
struct ble_util_buf_env_tag
{
    /// List of free LLCP RX buffers
    struct co_list                llcp_tx_free;
    /// List of free RX buffers
    struct co_list                rx_free;
    /// List of free ACL TX buffers
    struct co_list                acl_tx_free;
    /// List of free advertising data TX buffers
    struct co_list                adv_tx_free;

    #if (BLE_ISO_PRESENT)
    /// List of free isochronous data TX/RX buffers
    struct co_list                iso_free;
    #endif // (BLE_ISO_PRESENT)

    /// Pool of LLCP TX buffers (common for all links)
    struct ble_em_llcp_buf_elt    llcp_tx_pool[EM_BLE_LLCPTXBUF_NB];

    /// Pool of RX buffers (common for all activities) (use the format of ACL packets)
    struct ble_em_acl_buf_elt     rx_pool[EM_BLE_DATARXBUF_NB];

    /// Pool of ACL TX buffers (common for all links)
    struct ble_em_acl_buf_elt     acl_tx_pool[EM_BLE_ACLTXBUF_NB];

    /// Pool of advertising data TX buffers (common for all advertising sets)
    struct ble_em_adv_buf_elt     adv_tx_pool[EM_BLE_ADVDATATXBUF_NB];

    #if (BLE_ISO_PRESENT)
    /// Pool of isochronous data TX/RX buffers (common for all isochronous channels)
    struct ble_em_iso_buf_elt     iso_pool[EM_BLE_ISO_BUFFER_NB];
    #endif // (BLE_ISO_PRESENT)
};

#if (BLE_ISO_PRESENT)
/// EM ISO Descriptor
struct ble_em_isodesc_elt
{
    /// List header
    struct co_list_hdr hdr;
    /// Descriptor index
    uint8_t desc_idx;
};


/// BLE EM ISO descriptor management environment structure
struct ble_util_isodesc_env_tag
{
    /// List of free ISO descriptors
    struct co_list            isodesc_free;

    /// Pool of ISO descriptors (common for all isochronous channels)
    struct ble_em_isodesc_elt isodesc_pool[EM_BLE_ISO_DESC_NB];
};
#endif // (BLE_ISO_PRESENT)

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// BLE EM buffer management environment
static struct ble_util_buf_env_tag ble_util_buf_env;

#if (BLE_ISO_PRESENT)
/// BLE EM ISO descriptor management environment structure
static struct ble_util_isodesc_env_tag ble_util_isodesc_env;
#endif // (BLE_ISO_PRESENT)

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */
////uint8_t _tmpValK = 0;
void ble_util_buf_init(bool reset)
{
    int16_t i;
    #if defined EM_SIZE_LIMIT
    ASSERT_INFO(EM_BLE_END <= EM_SIZE_LIMIT, EM_BLE_END/1024, EM_SIZE_LIMIT/1024);
    #endif // defined EM_SIZE_LIMIT
    if(!reset)
    {
        // Clear environment memory
        memset(&ble_util_buf_env, 0, sizeof(ble_util_buf_env));

        // Point each pool element to its associated buffer in EM
////		_tmpValK = EM_BLE_LLCPTXBUF_NB-1;	////
        for(i = (EM_BLE_LLCPTXBUF_NB-1) ; i >= 0 ; i--)
        {
            ble_util_buf_env.llcp_tx_pool[i].buf_ptr = EM_BLE_LLCPTXBUF_OFFSET + i * EM_BLE_LLCPTXBUF_SIZE;
        }

        // Point each pool element to its associated buffer in EM
////		_tmpValK = EM_BLE_DATARXBUF_NB-1;	////
        for(i = (EM_BLE_DATARXBUF_NB-1) ; i >= 0 ; i--)
        {
            ble_util_buf_env.rx_pool[i].buf_ptr = EM_BLE_DATARXBUF_OFFSET + i * EM_BLE_DATARXBUF_SIZE + BLE_ACL_RX_BUF_HEADER_SPACE;
        }

        // Point each pool element to its associated buffer in EM
////		_tmpValK = EM_BLE_ACLTXBUF_NB-1;		////
        for(i = (EM_BLE_ACLTXBUF_NB-1) ; i >= 0 ; i--)
        {
            ble_util_buf_env.acl_tx_pool[i].buf_ptr = EM_BLE_ACLTXBUF_OFFSET + i * EM_BLE_ACLTXBUF_SIZE;
        }

        // Point each pool element to its associated buffer in EM
////		_tmpValK = EM_BLE_ADVDATATXBUF_NB-1;	////
        for(i = (EM_BLE_ADVDATATXBUF_NB-1) ; i >= 0 ; i--)
        {
            ble_util_buf_env.adv_tx_pool[i].buf_ptr = EM_BLE_ADVDATATXBUF_OFFSET + i * EM_BLE_ADVDATATXBUF_SIZE;
        }

        #if (BLE_ISO_PRESENT)
        // Point each pool element to its associated buffer in EM
////		_tmpValK = EM_BLE_ISO_BUFFER_NB-1;		////
        for(i = (EM_BLE_ISO_BUFFER_NB-1) ; i >= 0 ; i--)
        {
            ble_util_buf_env.iso_pool[i].buf_ptr = EM_BLE_ISO_BUF_OFFSET + i * EM_BLE_ISO_BUFFER_SIZE;
            ble_util_buf_env.iso_pool[i].buf_idx = i;
        }
        #endif // (BLE_ISO_PRESENT)
    }

    // Initialize the list of free LLCP TX buffers
    co_list_pool_init(&ble_util_buf_env.llcp_tx_free,
                      &ble_util_buf_env.llcp_tx_pool[0],
                      sizeof(struct ble_em_llcp_buf_elt),
                      EM_BLE_LLCPTXBUF_NB);

    // Initialize the list of free ACL RX buffers
    co_list_pool_init(&ble_util_buf_env.rx_free,
                      &ble_util_buf_env.rx_pool[0],
                      sizeof(struct ble_em_acl_buf_elt),
                      EM_BLE_DATARXBUF_NB);

    // Initialize the list of free ACL TX buffers
    co_list_pool_init(&ble_util_buf_env.acl_tx_free,
                      &ble_util_buf_env.acl_tx_pool[0],
                      sizeof(struct ble_em_acl_buf_elt),
                      EM_BLE_ACLTXBUF_NB);

    // Initialize the list of free advertising data TX buffers
    co_list_pool_init(&ble_util_buf_env.adv_tx_free,
                      &ble_util_buf_env.adv_tx_pool[0],
                      sizeof(struct ble_em_adv_buf_elt),
                      EM_BLE_ADVDATATXBUF_NB);

    #if (BLE_ISO_PRESENT)
    // Initialize the list of free isochronous data TX buffers
    co_list_pool_init(&ble_util_buf_env.iso_free,
                      &ble_util_buf_env.iso_pool[0],
                      sizeof(struct ble_em_iso_buf_elt),
                      EM_BLE_ISO_BUFFER_NB);
    #endif // (BLE_ISO_PRESENT)
}

struct ble_em_llcp_buf_elt* ble_util_buf_llcp_tx_alloc(void)
{
   struct ble_em_llcp_buf_elt* elt;
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    elt = (struct ble_em_llcp_buf_elt*) co_list_pop_front(&ble_util_buf_env.llcp_tx_free);
    GLOBAL_INT_RESTORE();
    return elt;
}

void ble_util_buf_llcp_tx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_LLCPTXBUF_OFFSET) / EM_BLE_LLCPTXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_LLCPTXBUF_NB, index, buf);
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Push to free list
    co_list_push_back(&ble_util_buf_env.llcp_tx_free, &ble_util_buf_env.llcp_tx_pool[index].hdr);
    GLOBAL_INT_RESTORE();
}

uint16_t ble_util_buf_rx_alloc(void)
{
    uint16_t buf_ptr = 0;
    struct ble_em_acl_buf_elt* elt;

    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    elt = (struct ble_em_acl_buf_elt*) co_list_pop_front(&ble_util_buf_env.rx_free);
    GLOBAL_INT_RESTORE();

    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }


    return buf_ptr;
}

void ble_util_buf_rx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - BLE_ACL_RX_BUF_HEADER_SPACE - EM_BLE_DATARXBUF_OFFSET) / EM_BLE_DATARXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_DATARXBUF_NB, index, buf);

    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Push to free list
    co_list_push_back(&ble_util_buf_env.rx_free, &ble_util_buf_env.rx_pool[index].hdr);
    GLOBAL_INT_RESTORE();
}

uint16_t ble_util_buf_acl_tx_alloc(void)
{
    uint16_t buf_ptr = 0;
    struct ble_em_acl_buf_elt* elt;
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
     elt = (struct ble_em_acl_buf_elt*) co_list_pop_front(&ble_util_buf_env.acl_tx_free);
    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }
    GLOBAL_INT_RESTORE();
    return buf_ptr;
}

uint16_t ble_util_buf_acl_tx_size(void)
{
    return co_list_size(&ble_util_buf_env.acl_tx_free);
}

struct ble_em_acl_buf_elt* ble_util_buf_acl_tx_elt_get(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_ACLTXBUF_OFFSET) / EM_BLE_ACLTXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_ACLTXBUF_NB, index, buf);

    return &(ble_util_buf_env.acl_tx_pool[index]);
}


void ble_util_buf_acl_tx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_ACLTXBUF_OFFSET) / EM_BLE_ACLTXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_ACLTXBUF_NB, index, buf);
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Push to free list
    co_list_push_back(&ble_util_buf_env.acl_tx_free, &ble_util_buf_env.acl_tx_pool[index].hdr);
    GLOBAL_INT_RESTORE();
    
}

uint16_t ble_util_buf_adv_tx_alloc(void)
{
    uint16_t buf_ptr = 0;
    struct ble_em_adv_buf_elt* elt;
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    elt = (struct ble_em_adv_buf_elt*) co_list_pop_front(&ble_util_buf_env.adv_tx_free);
    if(elt != NULL)
    {
        buf_ptr = elt->buf_ptr;
    }
    GLOBAL_INT_RESTORE();
    return buf_ptr;
}

void ble_util_buf_adv_tx_free(uint16_t buf)
{
    // Find associated pool element index
    uint8_t index = (buf - EM_BLE_ADVDATATXBUF_OFFSET) / EM_BLE_ADVDATATXBUF_SIZE;

    // Check that the buffer address is in the buffer section
    ASSERT_INFO(index < EM_BLE_ADVDATATXBUF_NB, index, buf);
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Push to free list
    co_list_push_back(&ble_util_buf_env.adv_tx_free, &ble_util_buf_env.adv_tx_pool[index].hdr);
    GLOBAL_INT_RESTORE();
}

#if (BLE_ISO_PRESENT)
uint16_t ble_util_buf_iso_alloc(uint8_t* index)
{
    uint16_t em_ptr = 0;
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    struct ble_em_iso_buf_elt* elt = (struct ble_em_iso_buf_elt*) co_list_pop_front(&ble_util_buf_env.iso_free);

    if(elt != NULL)
    {
        em_ptr = elt->buf_ptr;
        *index = elt->buf_idx;
    }
    GLOBAL_INT_RESTORE();
    return em_ptr;
}

void ble_util_buf_iso_free(uint8_t index)
{
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    ASSERT_INFO(index < EM_BLE_ISO_BUFFER_NB, index, 0);
    // Push to free list
    co_list_push_back(&ble_util_buf_env.iso_free, &ble_util_buf_env.iso_pool[index].hdr);
    GLOBAL_INT_RESTORE();
}

void ble_util_isodesc_init(bool reset)
{
    if (!reset)
    {
        int16_t i;

        // Clear environment memory
        memset(&ble_util_isodesc_env, 0, sizeof(ble_util_isodesc_env));

        // Point each pool element to its associated ISO descriptor in EM
        for (i = (EM_BLE_ISO_DESC_NB - 1) ; i >= 0 ; i--)
        {
            ble_util_isodesc_env.isodesc_pool[i].desc_idx = i;
        }
    }
    // Initialize the list of free ISO descriptors
    co_list_pool_init(&ble_util_isodesc_env.isodesc_free,
                      &ble_util_isodesc_env.isodesc_pool[0],
                      sizeof(struct ble_em_isodesc_elt),
                      EM_BLE_ISO_DESC_NB);
}

uint8_t ble_util_isodesc_alloc(void)
{
    uint8_t desc_idx = BLE_UTIL_ISODESC_INVALID;
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    // Get free element from free list
    struct ble_em_isodesc_elt *p_elt = (struct ble_em_isodesc_elt *)co_list_pop_front(&ble_util_isodesc_env.isodesc_free);

    if(p_elt)
    {
        desc_idx = p_elt->desc_idx;
    }
    else
    {
        ASSERT_ERR(0);
    }
    
    GLOBAL_INT_RESTORE();
    return (desc_idx);
}


void ble_util_isodesc_free(uint8_t desc_idx)
{
    // This value must be protected because can be accessed within interrupt context
    GLOBAL_INT_DISABLE();
    if(desc_idx < EM_BLE_ISO_DESC_NB)
    {
        // Push to free list
        co_list_push_back(&ble_util_isodesc_env.isodesc_free, &ble_util_isodesc_env.isodesc_pool[desc_idx].hdr);
    }
    else
    {
        ASSERT_INFO(0, desc_idx, 0);
    }

    GLOBAL_INT_RESTORE();
}

#endif // (BLE_ISO_PRESENT)

#endif //(BT_DUAL_MODE || BLE_STD_MODE)  
/// @} BLE_UTIL_BUF
