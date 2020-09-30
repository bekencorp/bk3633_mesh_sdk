/**
****************************************************************************************
*
* @file rwip.c
*
* @brief RW IP SW main module
*
* Copyright (C) RivieraWaves 2009-2015
*
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup RW IP SW main module
 * @ingroup ROOT
 * @brief The RW IP SW main module.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // RW SW configuration

#include <string.h>          // for mem* functions
#include <stdio.h>
#include "arch.h"            // Platform architecture definition
#include "compiler.h"
#include "co_version.h"      // version information
#include "co_utils.h"

#include "rwip.h"            // RW definitions
#include "rwip_int.h"        // RW internal definitions

#if (NVDS_SUPPORT)
#include "nvds.h"         // NVDS definitions
#endif // NVDS_SUPPORT

#if (BT_EMB_PRESENT)
#include "rwbt.h"            // rwbt definitions
#endif //BT_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#include "rwble.h"           // rwble definitions
#endif //BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)
//#include "rwble_hl.h"        // BLE HL definitions
//#include "gapc.h"
//#include "gapm.h"
//#include "gattc.h"
//#include "l2cc.h"
#endif //BLE_HOST_PRESENT

#if (BLE_APP_PRESENT)
#include "app.h"             // Application definitions
#endif //BLE_APP_PRESENT

//#include "led.h"             // led definitions

#if (BT_EMB_PRESENT)
#include "ld.h"
#endif //BT_EMB_PRESENT

#if (DISPLAY_SUPPORT)
#include "display.h"         // display definitions
#include "co_utils.h"        // toolbox
#include "plf.h"             // platform definition
#if (BT_EMB_PRESENT)
#include "reg_btcore.h"
#endif // (BT_EMB_PRESENT)
#if (BLE_EMB_PRESENT)
#include "reg_blecore.h"
#endif // (BLE_EMB_PRESENT)
#if (BT_DUAL_MODE)
#include "reg_ipcore.h"
#endif // (BT_DUAL_MODE)
#endif //DISPLAY_SUPPORT
#include "reg_ipcore.h"

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#include "sch_arb.h"            // Scheduling Arbiter
#include "sch_prog.h"           // Scheduling Programmer
#include "sch_plan.h"           // Scheduling Planner
#include "sch_slice.h"          // Scheduling Slicer
#include "sch_alarm.h"          // Scheduling Alarm
#endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#include "rf.h"              // RF definitions
#endif //BT_EMB_PRESENT || BLE_EMB_PRESENT

#if (H4TL_SUPPORT)
#include "h4tl.h"
#endif //H4TL_SUPPORT

#if (AHI_TL_SUPPORT)
#include "ahi.h"
#endif //AHI_TL_SUPPORT

#if (HCI_PRESENT)
#include "hci.h"             // HCI definition
#endif //HCI_PRESENT

#include "ke.h"              // kernel definition
#include "ke_event.h"        // kernel event
#include "ke_timer.h"        // definitions for timer
#include "ke_mem.h"          // kernel memory manager

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#include "ecc_p256.h"        // ECC P256 library
#endif // (BT_EMB_PRESENT || BLE_EMB_PRESENT)

#include "aes.h"             // For AES functions

#if (BLE_EMB_PRESENT)
#include "reg_blecore.h"        // ble core registers
#endif //BLE_EMB_PRESENT

#if (BT_EMB_PRESENT)
#include "reg_btcore.h"         // bt core registers
#endif //BT_EMB_PRESENT

#include "dbg.h"             // debug definition
//#include "icu.h" 
#include "user_config.h"
#include "debug_uart.h"
/*
 * DEFINES
 ****************************************************************************************
 */

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /// Sleep Duration Value in periodic wake-up mode in Half slots
#define MAX_SLEEP_DURATION_PERIODIC_WAKEUP      (0x0640 * 3)  // 1.5s
    
    /// Sleep Duration Value in external wake-up mode
#define MAX_SLEEP_DURATION_EXTERNAL_WAKEUP      0x7D00  //10s
    
    
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

__attribute__((section("STACK_FUNC")))
const struct rwip_func_tag rwip_func = {rwip_isr, rwip_init, rwip_schedule, rwip_set_bd_address};

#if (DISPLAY_SUPPORT)
///Table of HW image names for display
static const char* plf_type[4] =
{
        "BK",
        "LE",
        "BT",
        "DM"
};
static const char* rf_type[6] =
{
         "NONE",
         "RPLE",
         "EXRC",
         "ICY1",
         "ICY2",
         "BTPT"
 };

/// FW type display line
#if (BT_EMB_PRESENT && BLE_EMB_PRESENT)
#define FW_TYPE_DISPLAY   "FW: BTDM split emb"
#elif (BT_EMB_PRESENT)
#define FW_TYPE_DISPLAY   "FW: BT split emb"
#elif (BLE_EMB_PRESENT && BLE_HOST_PRESENT)
#define FW_TYPE_DISPLAY   "FW: BLE full"
#elif (BLE_EMB_PRESENT)
#define FW_TYPE_DISPLAY   "FW: BLE split emb"
#else
#define FW_TYPE_DISPLAY   "FW: ROM"
#endif // BT_EMB_PRESENT / BLE_EMB_PRESENT / BLE_HOST_PRESENT
#endif //DISPLAY_SUPPORT

 // Heap header size is 12 bytes
 #define RWIP_HEAP_HEADER             (12 / sizeof(uint32_t))
 // ceil(len/sizeof(uint32_t)) + RWIP_HEAP_HEADER
 #define RWIP_CALC_HEAP_LEN(len)      ((((len) + (sizeof(uint32_t) - 1)) / sizeof(uint32_t)) + RWIP_HEAP_HEADER)
 // compute size of the heap block in bytes
 #define RWIP_CALC_HEAP_LEN_IN_BYTES(len)  (RWIP_CALC_HEAP_LEN(len) * sizeof(uint32_t))


/*
 * STRUCT DEFINITIONS
 ****************************************************************************************
 */

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
#if (RW_WLAN_COEX || RW_MWS_COEX)
const uint8_t rwip_coex_cfg[RWIP_COEX_CFG_MAX]={
    #if (BT_EMB_PRESENT)
    [RWIP_COEX_MSSWITCH_IDX] = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_SNIFFATT_IDX] = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_PAGE_IDX]     = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTEN << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_PSCAN_IDX]    = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_INQ_IDX]      = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_INQRES_IDX]   = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_SCORSVD_IDX]  = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_BCAST_IDX]    = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_DIS << RWIP_SAMEN_POS)),
    [RWIP_COEX_CONNECT_IDX]  = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) | (RWIP_SAM_EN << RWIP_SAMEN_POS)),
    #endif // #if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    [RWIP_COEX_CON_IDX]     = (uint8_t)((RWIP_PTI_TXDIS << RWIP_TXBSY_POS)  | (RWIP_PTI_RXDIS << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_CON_DATA_IDX]= (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_ADV_IDX]     = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXDIS << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_SCAN_IDX]    = (uint8_t)((RWIP_PTI_TXDIS << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_INIT_IDX]    = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    #endif // #if (BLE_EMB_PRESENT)
};
#endif //(RW_WLAN_COEX || RW_MWS_COEX)
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
#if 0
#define KE_HEAP                    __attribute__((section("ke_heap"),zero_init))
#else
#define KE_HEAP 
#endif
/// Heap definitions - use uint32 to ensure that memory blocks are 32bits aligned.

/// Memory allocated for environment variables
//uint32_t KE_HEAP rwip_heap_env[RWIP_CALC_HEAP_LEN(RWIP_HEAP_ENV_SIZE)];
#if (BLE_HOST_PRESENT)
/// Memory allocated for Attribute database
// uint32_t KE_HEAP rwip_heap_db[RWIP_CALC_HEAP_LEN(RWIP_HEAP_DB_SIZE)];
#endif // (BLE_HOST_PRESENT)
/// Memory allocated for kernel messages
// uint32_t  rwip_heap_msg[RWIP_CALC_HEAP_LEN(RWIP_HEAP_MSG_SIZE)];
/// Non Retention memory block
// uint32_t KE_HEAP rwip_heap_non_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_NON_RET_SIZE)];
/// IP reset state variable
uint8_t rwip_rst_state;

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if (!NVDS_SUPPORT)
__STATIC uint8_t rwip_param_dummy_get(uint8_t param_id, uint8_t * lengthPtr, uint8_t *buf)
{
    //uart_printf("rwip_param_dummy_get\r\n");
    return (PARAM_FAIL);
}
__STATIC uint8_t rwip_param_dummy_set(uint8_t param_id, uint8_t length, uint8_t *buf)
{
    return (PARAM_FAIL);
}
__STATIC uint8_t rwip_param_dummy_del(uint8_t param_id)
{
    return (PARAM_FAIL);
}
#endif // (!NVDS_SUPPORT)

#if (DISPLAY_SUPPORT)
/**
 ****************************************************************************************
 * @brief Display device configuration
 *
 * This function adds graphical display
 ****************************************************************************************
 */
__STATIC void display_add_config(void)
{
    struct plf_version plfversion;
    struct plf_version plfversion_unkn;

    // Get platform version, date, time ...
    plf_read_version(&plfversion);

    /************************************************************************/
    /*                              FW TYPE                                 */
    /************************************************************************/
    {
        char scr_fw_type[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        memset(&plfversion_unkn, 0, sizeof(plfversion_unkn));

        if((plfversion.ip_type == 0) || (memcmp(&plfversion, &plfversion_unkn, sizeof(plfversion_unkn)) == 0))
            sprintf(scr_fw_type,"HW Unknown");
        else if(plfversion.plf_type == 0)
            sprintf(scr_fw_type,"HW: Backup Image");
        else if(plfversion.ip_type == 1)
            sprintf(scr_fw_type,"HW: %s40 %s %2x",plf_type[plfversion.plf_type],rf_type[plfversion.rf_type],plfversion.version);
        else if(plfversion.ip_type == 7)
            sprintf(scr_fw_type,"HW: %s41 %s %2x",plf_type[plfversion.plf_type],rf_type[plfversion.rf_type],plfversion.version);
        else if(plfversion.ip_type == 8)
            sprintf(scr_fw_type,"HW: %s42 %s %2x",plf_type[plfversion.plf_type],rf_type[plfversion.rf_type],plfversion.version);
        else if(plfversion.ip_type == 9)
            sprintf(scr_fw_type,"HW: %s50 %s %2x",plf_type[plfversion.plf_type],rf_type[plfversion.rf_type],plfversion.version);
        else if(plfversion.ip_type == 10)
            sprintf(scr_fw_type,"HW: %s51 %s %2x",plf_type[plfversion.plf_type],rf_type[plfversion.rf_type],plfversion.version);
        else
            sprintf(scr_fw_type,"HW Unknown");

        display_screen_set(scr_id, NULL, scr_fw_type , FW_TYPE_DISPLAY);
    }

    /************************************************************************/
    /*                             FW VERSION                               */
    /************************************************************************/
    {
        uint8_t byte;
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);
        byte = RWBT_SW_VERSION_MAJOR;
        co_bytes_to_string(&line[0], &byte, 1);
        line[2] = '.';
        byte = RWBT_SW_VERSION_MINOR;
        co_bytes_to_string(&line[3], &byte, 1);
        line[5] = '.';
        byte = RWBT_SW_VERSION_BUILD;
        co_bytes_to_string(&line[6], &byte, 1);
        line[8] = '.';
        byte = RWBT_SW_VERSION_SUB_BUILD;
        co_bytes_to_string(&line[9], &byte, 1);
        line[11] = '\0';
        display_screen_set(scr_id, NULL, "FW version:", line);
    }

    /************************************************************************/
    /*                              FW TIME                                 */
    /************************************************************************/
    {
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        /* Build the FW type screen with:
         *  - type
         *  - build date "Mmm dd yyyy"
         *  - build time "hh:mm:ss"
         */
        strncpy(line, __DATE__, 7);
        strncpy(line+7, __TIME__, 8);
        line[DISPLAY_LINE_SIZE] = '0';
        display_screen_set(scr_id, NULL, "FW date:", line);
    }

    /************************************************************************/
    /*                            FPGA VERSION                              */
    /* The HW FPGA version w.xy.0z.v is coded is coded as below :           */
    /*	w : IP SIG release  )(09 for 5.0)                                   */
    /*   x : processor type(0: Cortus, 1 : RISCV)                           */
    /*   y : RW IP(1 :BLE, 2 : BT, 3 : BTDM)                                */
    /*   z : RF type(1: Ripple, 3 : Icytrx_v1, 4 : Icytrx_v2, 5 : btipt)    */
    /*   v : synthesis build version                                        */
    /************************************************************************/
    {
        char line[DISPLAY_LINE_SIZE + 1];
        uint8_t xy = (plfversion.cpu_type << 4) + plfversion.plf_type;
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);
        co_bytes_to_string(&line[0], &plfversion.ip_type, 1);
        line[2] = '.';
        co_bytes_to_string(&line[3], &xy, 1);
        line[5] = '.';
        co_bytes_to_string(&line[6], &plfversion.rf_type, 1);
        line[8] = '.';
        co_bytes_to_string(&line[9], &plfversion.version, 1);
        line[11] = '\0';
        display_screen_set(scr_id, NULL, "FPGA version:", line);
    }

    /************************************************************************/
    /*                           FPGA DATE/TIME                             */
    /************************************************************************/
    {
        char* ptr;
        uint8_t value;
        uint8_t digit;
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        ptr = line;
        // Month
        value = plfversion.month;
        digit = (value/10) + 48;
        *(ptr++) = (char)digit;
        digit = (value - 10*(value/10)) + 48;
        *(ptr++) = (char)digit;
        *(ptr++) = '_';
        // Day
        value = plfversion.day;
        digit = (value/10) + 48;
        *(ptr++) = (char)digit;
        digit = (value - 10*(value/10)) + 48;
        *(ptr++) = (char)digit;
        *(ptr++) = ' ';
        // Hours
        value = plfversion.hour;
        digit = (value/10) + 48;
        *(ptr++) = (char)digit;
        digit = (value - 10*(value/10)) + 48;
        *(ptr++) = (char)digit;
        *(ptr++) = ':';
        // Minutes
        value = plfversion.minute;
        digit = (value/10) + 48;
        *(ptr++) = (char)digit;
        digit = (value - 10*(value/10)) + 48;
        *(ptr++) = (char)digit;
        *(ptr++) = ':';
        // Seconds
        value = plfversion.second;
        digit = (value/10) + 48;
        *(ptr++) = (char)digit;
        digit = (value - 10*(value/10)) + 48;
        *(ptr++) = (char)digit;
        *(ptr++) = '\0';
        display_screen_set(scr_id, NULL, "FPGA Date:", line);
    }

    #if (BT_EMB_PRESENT)
    /************************************************************************/
    /*                           BT HW VERSION                              */
    /************************************************************************/
    {
        uint8_t byte;
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);
        byte = bt_version_typ_getf();
        co_bytes_to_string(&line[0], &byte, 1);
        line[2] = '.';
        byte = bt_version_rel_getf();
        co_bytes_to_string(&line[3], &byte, 1);
        line[5] = '.';
        byte = bt_version_upg_getf();
        co_bytes_to_string(&line[6], &byte, 1);
        line[8] = '.';
        byte = bt_version_build_getf();
        co_bytes_to_string(&line[9], &byte, 1);
        line[11] = '\0';
        display_screen_set(scr_id, NULL, "BT HW version:", line);
    }
    #endif // (BT_EMB_PRESENT)

    #if (BLE_EMB_PRESENT)
    /************************************************************************/
    /*                           BLE HW VERSION                              */
    /************************************************************************/
    {
        uint8_t byte;
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);
        byte = ble_version_typ_getf();
        co_bytes_to_string(&line[0], &byte, 1);
        line[2] = '.';
        byte = ble_version_rel_getf();
        co_bytes_to_string(&line[3], &byte, 1);
        line[5] = '.';
        byte = ble_version_upg_getf();
        co_bytes_to_string(&line[6], &byte, 1);
        line[8] = '.';
        byte = ble_version_build_getf();
        co_bytes_to_string(&line[9], &byte, 1);
        line[11] = '\0';
        display_screen_set(scr_id, NULL, "BLE HW version:", line);
    }
    #endif // (BLE_EMB_PRESENT)

    #if (BT_DUAL_MODE)
    /************************************************************************/
    /*                           DM HW VERSION                              */
    /************************************************************************/
    {
        uint8_t byte;
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);
        byte = ip_version_typ_getf();
        co_bytes_to_string(&line[0], &byte, 1);
        line[2] = '.';
        byte = ip_version_rel_getf();
        co_bytes_to_string(&line[3], &byte, 1);
        line[5] = '.';
        byte = ip_version_upg_getf();
        co_bytes_to_string(&line[6], &byte, 1);
        line[8] = '.';
        byte = ip_version_build_getf();
        co_bytes_to_string(&line[9], &byte, 1);
        line[11] = '\0';
        display_screen_set(scr_id, NULL, "DM HW version:", line);
    }
    #endif // (BT_DUAL_MODE)

    /************************************************************************/
    /*                            DEVICE NAME                               */
    /************************************************************************/
    {
        uint8_t dev_name_length = PARAM_LEN_DEVICE_NAME;
        uint8_t dev_name_data[PARAM_LEN_DEVICE_NAME];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        if(rwip_param.get(PARAM_ID_DEVICE_NAME, &dev_name_length, dev_name_data) == PARAM_OK)
        {
            // Put end of line
            if(dev_name_length > 16)
            {
                dev_name_length = 16;
            }
            dev_name_data[dev_name_length] = '\0';
        }
        else
        {
            dev_name_data[0] = '\0';
        }
        display_screen_set(scr_id, NULL, "Device name:", (char*)dev_name_data);
    }

    /************************************************************************/
    /*                              BD ADDRESS                              */
    /************************************************************************/
    {
        char scr_bd_ad[DISPLAY_LINE_SIZE+1];
        uint8_t bd_ad_length = PARAM_LEN_BD_ADDRESS;
        uint8_t bd_ad_data[PARAM_LEN_BD_ADDRESS];
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        strcpy(scr_bd_ad, "0x");
        if(rwip_param.get(PARAM_ID_BD_ADDRESS, &bd_ad_length, bd_ad_data) == PARAM_OK)
        {
            // Encode to ASCII
            for(int i = 0; i < PARAM_LEN_BD_ADDRESS; i++)
            {
                uint8_t digit = bd_ad_data[PARAM_LEN_BD_ADDRESS-1-i]>>4;
                digit += (digit < 10) ? 48:55;
                *(scr_bd_ad+2+2*i) = (char)digit;
                digit = bd_ad_data[PARAM_LEN_BD_ADDRESS-1-i]&0xF;
                digit += (digit < 10) ? 48:55;
                *(scr_bd_ad+2+2*i+1) = (char)digit;
            }
        }
        scr_bd_ad[14] = '\0';
        display_screen_set(scr_id, NULL, "BD Address:", scr_bd_ad);
    }

    #if (PLF_UART)
    /************************************************************************/
    /*                            UART BAUDRATE                             */
    /************************************************************************/
    {
        int i = 0;
        char line[DISPLAY_LINE_SIZE+1];
        uint8_t uart_length = PARAM_LEN_UART_BAUDRATE;
        uint32_t baudrate = 921600;
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        // Get UART baudrate from NVDS
        if(rwip_param.get(PARAM_ID_UART_BAUDRATE, &uart_length, (uint8_t*) &baudrate) == PARAM_OK)
        {
            if(baudrate > 3500000 || baudrate < 9600)
            {
                baudrate = 921600;
            }
        }
        else
        {
            baudrate = 921600;
        }

        // Display UART baudrate on screen
        strcpy(line, "        bps");
        while (baudrate > 0)
        {
            line[6-i++] = (baudrate - (10*(baudrate / 10))) + 48;
            baudrate = baudrate / 10;
        }
        display_screen_set(scr_id, NULL, "UART baudrate:", line);
    }
    #endif //PLF_UART

    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    /************************************************************************/
    /*                               RF BOARD                               */
    /************************************************************************/
    {
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        #if defined(CFG_RF_RIPPLE)
        {
            char line[DISPLAY_LINE_SIZE+1];
            // Read board ID in platform
            uint16_t rf_id = plf_read_rf_board_id();
            // Add screen to display RF board type
            strcpy(line, "Ripple #");
            line[8] = (rf_id/10) + 48;
            line[9] = (rf_id - (10*(rf_id/10))) + 48;
            line[10] = '\0';
            display_screen_set(scr_id, NULL, "RF board:", line);
        }
        #elif defined(CFG_RF_ATLAS)
        if(plfversion.rf_type == 3)
            display_screen_set(scr_id, NULL, "RF board:", "IcyTRx V1");
        else if(plfversion.rf_type == 4)
            display_screen_set(scr_id, NULL, "RF board:", "IcyTRx V2");
        #elif defined(CFG_RF_BTIPT)
            display_screen_set(scr_id, NULL, "RF board:", "BTIPT");
        #else
            display_screen_set(scr_id, NULL, "RF board:", "Unknown");
        #endif //CFG_RF
    }
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    /************************************************************************/
    /*                               CPU TYPE                               */
    /************************************************************************/
    {
        uint8_t scr_id = display_screen_alloc();
        display_screen_insert(scr_id, 0);

        if (plfversion.cpu_type == 0)
            display_screen_set(scr_id, NULL, "CPU type:", "Cortus APS3");
        else if (plfversion.cpu_type == 1)
            display_screen_set(scr_id, NULL, "CPU type:", "RISC-V");
        else
            display_screen_set(scr_id, NULL, "CPU type:", "Unknown");
    }

    // Start with FW type screen
    display_start(0);
}
#endif //DISPLAY_SUPPORT

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void rwip_init(uint32_t error)
{
    UART_PRINTF("%s\r\n", __func__);
    // IP power up
    SET_RWBT_POWER_UP;
    // IP initialization
    rwip_rst_state = RWIP_INIT;
    #if (NVDS_SUPPORT)
    // Point to NVDS for parameters get/set
    rwip_param.get = nvds_get;
    rwip_param.set = nvds_put;
    rwip_param.del = nvds_del;
    #else // (NVDS_SUPPORT)
    // !! Need to point to some parameter configuration system
    ASSERT_WARN(0, 0, 0);
    rwip_param.get = rwip_param_dummy_get;
    rwip_param.set = rwip_param_dummy_set;
    rwip_param.del = rwip_param_dummy_del;
    #endif // (NVDS_SUPPORT)

    // Initialize kernel
    ke_init();
#if (KERNEL_MEM_RW)
    // Initialize memory heap used by kernel.
    // Memory allocated for environment variables
    // ke_mem_init(KE_MEM_ENV,           (uint8_t*)rwip_heap_env,     RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_ENV_SIZE));
    #if (0) //(BLE_HOST_PRESENT)
    // Memory allocated for Attribute database
    ke_mem_init(KE_MEM_ATT_DB,        (uint8_t*)rwip_heap_db,      RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_DB_SIZE));
    #endif // (BLE_HOST_PRESENT)
    // Memory allocated for kernel messages
    // ke_mem_init(KE_MEM_KE_MSG,        (uint8_t*)rwip_heap_msg,     RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_MSG_SIZE));
    ke_mem_init(KE_MEM_NON_RETENTION, (uint8_t *)(REG_EM_ET_BASE_ADDR + EM_BLE_END + 1), 16 * 1024 -  EM_BLE_END - 4);
    UART_PRINTF("++++ EM_BLE_END 0x%x, len %d ++++, end 0x%x\n", 
               EM_BLE_END, 16 * 1024 -  EM_BLE_END - 4,
               REG_EM_ET_BASE_ADDR + (16 * 1024 -  4));
    // Non Retention memory block
    // ke_mem_init(KE_MEM_NON_RETENTION, (uint8_t*)rwip_heap_non_ret, RWIP_CALC_HEAP_LEN_IN_BYTES(RWIP_HEAP_NON_RET_SIZE));
#endif // KERNEL_MEM_RW
    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    #if (RW_DEBUG)
    // Initialize the debug process
    dbg_init(false);
    #endif //(RW_DEBUG)
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

    // Initialize RF
    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
	rom_env.rf_init(&rwip_rf);
    #endif //BT_EMB_PRESENT || BLE_EMB_PRESENT

    #if (DISPLAY_SUPPORT)
    // Initialize display module
    display_init();

    // Add some configuration information to display
    display_add_config();
    #endif //DISPLAY_SUPPORT

#if (CFG_ECC_P256_ACC)
    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    // Initialize Diffie Hellman Elliptic Curve Algorithm
    ecc_init(false);
    #endif // (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#endif // CFG_ECC_P256_ACC
    // Initialize H4TL
    #if (H4TL_SUPPORT)
    #if (BLE_HOST_PRESENT)
    #if (BLE_EMB_PRESENT) // FULL HOST

    #if (H4TL_NB_CHANNEL > 1)
    h4tl_init(1, rwip_eif_get(1));
    UART_PRINTF("h4tl_init ok\r\n");
    #endif // (H4TL_NB_CHANNEL > 1)
    h4tl_init(0, rwip_eif_get(0));
    UART_PRINTF("h4tl_init-1 ok\r\n");
    #endif // (BLE_HOST_PRESENT)
    #endif //(H4TL_SUPPORT)
    #endif //(H4TL_SUPPORT)

    #if (HCI_PRESENT)
    // Initialize the HCI
    hci_init(false);
    UART_PRINTF("hci_init ok\r\n");
    #endif //HCI_PRESENT

    #if (AHI_TL_SUPPORT)
    // Initialize the Application Host Interface
    ahi_init();
    UART_PRINTF("ahi_init ok\r\n");
    #endif //AHI_TL_SUPPORT

    #if (BLE_HOST_PRESENT)
    // Initialize BLE Host stack
    //rwble_hl_init();
    UART_PRINTF("rwble_hl_init ok\r\n");
    #endif //BLE_HOST_PRESENT

    #if (BT_EMB_PRESENT)
    // Initialize BT
    rwbt_init();
    uart_printf("rwbt_init ok\r\n");
    #endif //BT_EMB_PRESENT

    #if (BLE_EMB_PRESENT)
    // Initialize BLE
    rwble_init(false);
    UART_PRINTF("rwble_init ok\r\n");
    #endif //BLE_EMB_PRESENT

    // Initialize AES
    #if (BT_DUAL_MODE || BLE_STD_MODE)
    //aes_init(false);
    UART_PRINTF("aes_init ok\r\n");
    #endif //(BT_DUAL_MODE || BLE_STD_MODE)

    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    // Initialize Scheduling blocks
    sch_arb_init(false);
    sch_prog_init(false);
    sch_plan_init(false);
    sch_alarm_init(false);
    sch_slice_init(false);
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

    // Initialize IP core driver
    rwip_driver_init(false);
    UART_PRINTF("rwip_driver_init ok\r\n");

    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    #if (RW_WLAN_COEX)
    rwip_wlcoex_set(1);
    UART_PRINTF("rwip_wlcoex_set ok\r\n");
    #endif //(RW_WLAN_COEX)
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)
    #if (BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
    // If FW initializes due to FW reset, send the message to Host
    if(error != RESET_NO_ERROR)
    {
        uart_printf("@@error(%x)\r\n",error);
        if(error == RESET_TO_ROM || error == RESET_AND_LOAD_FW)
        {
            // Send platform reset command complete if requested by user

        }
        else
        {
            // Allocate a message structure for hardware error event
            struct hci_hw_err_evt *evt = KE_MSG_ALLOC(HCI_EVENT, 0, HCI_HW_ERR_EVT_CODE, hci_hw_err_evt);

            // Fill the HW error code
            switch(error)
            {
                case RESET_MEM_ALLOC_FAIL: evt->hw_code = CO_ERROR_HW_MEM_ALLOC_FAIL; break;
                default: ASSERT_INFO(0, error, 0); break;
            }

            // Send the message
            hci_send_2_host(evt);
        }
    }
    #endif //(BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))

    /*
     ************************************************************************************
     * Application initialization
     ************************************************************************************
     */
    #if (BLE_APP_PRESENT)
    // Initialize APP
    if(rom_env.appm_init)
    	rom_env.appm_init();
    UART_PRINTF("appm_init ok\r\n");
    #endif //BLE_APP_PRESENT
}

void rwip_reset(void)
{
    // Disable interrupts until reset procedure is completed
    GLOBAL_INT_DISABLE();

    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    #if (RW_DEBUG)
    // Reset dbg
    dbg_init(true);
    #endif //(RW_DEBUG)
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

    //Clear all message and timer pending
    ke_flush();

    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#if (CFG_ECC_P256_ACC)
    // Reset Diffie Hellman Elliptic Curve Algorithm
    ecc_init(true);
#endif
   #endif // (BT_EMB_PRESENT || BLE_EMB_PRESENT)

    #if (HCI_PRESENT)
    // Reset the HCI
    hci_init(true);
    #endif //HCI_PRESENT

    #if 0 //(BLE_HOST_PRESENT)
    // Initialize BLE Host stack
    rwble_hl_init(true);
    #endif //BLE_HOST_PRESENT

    #if (BT_EMB_PRESENT)
    // Reset BT
    rwbt_reset();
    #endif //BT_EMB_PRESENT

    #if (BLE_EMB_PRESENT)
    // Reset BLE
    rwble_init(true);
    #endif //BLE_EMB_PRESENT
#if (BT_DUAL_MODE || BLE_STD_MODE)
    // Reset AES
    //aes_init(true);
#endif //(BT_DUAL_MODE || BLE_STD_MODE) 
    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    // Reset Scheduling blocks
    sch_arb_init(true);
    sch_prog_init(true);
    sch_plan_init(true);
    sch_alarm_init(true);
    sch_slice_init(true);
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    // Initialize IP core driver
    rwip_driver_init(true);
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    #if (RW_WLAN_COEX)
    rwip_wlcoex_set(1);
    #endif //(RW_WLAN_COEX)

    // Reset the RF
    rwip_rf.reset();
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)


    #if (DISPLAY_SUPPORT)
    // Restart display module
    display_resume();
    #endif //DISPLAY_SUPPORT

    // Restore interrupts once reset procedure is completed
    GLOBAL_INT_RESTORE();
}

void rwip_schedule(void)
{
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    // If system is waking up, delay the handling up to the Bluetooth clock is available and corrected
    if ((rwip_env.prevent_sleep & RW_WAKE_UP_ONGOING) == 0)
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    {
        // schedule all pending events
        ke_event_schedule();
    }
}

#if (BT_EMB_PRESENT)
#if PCA_SUPPORT
bool rwip_pca_clock_dragging_only(void)
{
#if (BLE_EMB_PRESENT)
    return rwble_activity_ongoing_check();
#else
    return false;
#endif // BLE_EMB_PRESENT
}
#endif // PCA_SUPPORT
#endif // BT_EMB_PRESENT

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#if (RW_MWS_COEX)
void rwip_mwscoex_set(bool state)
{
#if (BT_EMB_PRESENT)
    if (state)
    {
        bt_coexifcntl0_mwswci_en_setf(0);
        bt_coexifcntl0_mwscoex_en_setf(1);
    }
    else
    {
        bt_coexifcntl0_mwswci_en_setf(0);
        bt_coexifcntl0_mwscoex_en_setf(0);
    }
#endif // BT_EMB_PRESENT
}
#endif // RW_MWS_COEX
#if (RW_WLAN_COEX)
void rwip_wlcoex_set(bool state)
{
#if (BLE_EMB_PRESENT)
    if (state)
    {
        ble_coexifcntl0_syncgen_en_setf(1);
        ble_coexifcntl0_wlancoex_en_setf(1);
    }
    else
    {
        ble_coexifcntl0_syncgen_en_setf(0);
        ble_coexifcntl0_wlancoex_en_setf(0);
    }
#endif // BLE_EMB_PRESENT
}
#endif // RW_WLAN_COEX
#endif // (BT_EMB_PRESENT || BLE_EMB_PRESENT)

#if RW_DEBUG
void rwip_assert(const char * file, int line, int param0, int param1, uint8_t type)
{
    #if (BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
    struct hci_dbg_assert_evt *evt = KE_MSG_ALLOC_DYN(HCI_DBG_EVT, 0, 0, hci_dbg_assert_evt, sizeof(struct hci_dbg_assert_evt) + strlen(file));
    evt->subcode = HCI_DBG_ASSERT_EVT_SUBCODE;
    evt->type = type;
    evt->line = line;
    evt->param0 = param0;
    evt->param1 = param1;
    strcpy((char *) evt->file, file);
    hci_send_2_host(evt);
    #endif //(BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
}
#endif //RW_DEBUG

void rwip_isr(void)
{
    DBG_SWDIAG(ISR, RWIP, 1);

    // Check interrupt status and call the appropriate handlers
    uint32_t irq_stat      = ip_intstat1_get();

    //UART_PRINTF("%s, irq_stat(0x%x) \r\n\r\n", __func__, irq_stat );
    // General purpose timer interrupt - half slot accuracy
    if (irq_stat & IP_FINETGTINTSTAT_BIT)
    {
        DBG_SWDIAG(IP_ISR, FINETGTINT, 1);
        // Clear the interrupt
        ip_intack1_finetgtintack_clearf(1);

        // handles half slot timer target
        rwip_timer_hs_handler();

        DBG_SWDIAG(IP_ISR, FINETGTINT, 0);
    }

    // General purpose timer interrupt - half us accuracy
    if (irq_stat & IP_TIMESTAMPTGT1INTSTAT_BIT)
    {
        DBG_SWDIAG(IP_ISR, TIMESTAMPINT, 1);
        // Clear the interrupt
        ip_intack1_timestamptgt1intack_clearf(1);

        // handles half slot timer target
        rwip_timer_hus_handler();

        DBG_SWDIAG(IP_ISR, TIMESTAMPINT, 0);
    }
#if 0
    // Clock
    if (irq_stat & IP_CLKNINTSTAT_BIT) // clock interrupt
    {
        DBG_SWDIAG(IP_ISR, CLKNINT, 1);

        // Ack clock interrupt
        ip_intack1_clknintack_clearf(1);

        if(rwip_env.prevent_sleep & RW_WAKE_UP_ONGOING)
        {
            // Handle end of wake-up
            rwip_wakeup_end();
        }
        #if (BT_EMB_PRESENT)
        else // BT uses clock IRQ to program ACL frames
        {
            // Call Scheduling Programmer
            sch_prog_clk_isr();
        }
        #endif //BT_EMB_PRESENT

        DBG_SWDIAG(IP_ISR, CLKNINT, 0);
    }
#endif
    // FIFO
    if (irq_stat & IP_FIFOINTSTAT_BIT) // FIFO interrupt
    {
        DBG_SWDIAG(IP_ISR, FIFOINT, 1);

        // Call scheduling programmer
        sch_prog_fifo_isr();

        // Ack FIFO interrupt
        ip_intack1_fifointack_clearf(1);

        DBG_SWDIAG(IP_ISR, FIFOINT, 0);
    }

    if (irq_stat & IP_SLPINTSTAT_BIT)
    {
        DBG_SWDIAG(IP_ISR, SLPINT, 1);

        // ack Sleep wakeup interrupt
        ip_intack1_slpintack_clearf(1);

        // Handle wake-up
        rwip_wakeup();
        rwip_prevent_sleep_clear(RW_BLE_ACTIVE_MODE);
        DBG_SWDIAG(IP_ISR, SLPINT, 0);
    }

    // General purpose timer interrupt
    if (irq_stat & IP_TIMESTAMPTGT2INTSTAT_BIT)
    {
        DBG_SWDIAG(IP_ISR, GROSSTGTINT, 1);

        // Clear the interrupt
        ip_intack1_timestamptgt2intack_clearf(1);

        // handles 10 ms timer target
        rwip_timer_10ms_handler();

        DBG_SWDIAG(IP_ISR, GROSSTGTINT, 0);
    }

    // Encryption interrupt
    if (irq_stat & IP_CRYPTINTSTAT_BIT)
    {
        DBG_SWDIAG(IP_ISR, CRYPTINT, 1);

        ip_intack1_cryptintack_clearf(1);

        // call the crypto ISR handler
        rwip_crypt_isr_handler();

        DBG_SWDIAG(IP_ISR, CRYPTINT, 0);
    }

    // SW interrupt
    if (irq_stat & IP_SWINTSTAT_BIT)
    {
        DBG_SWDIAG(IP_ISR, SWINT, 1);
        // Clear the interrupt
        ip_intack1_swintack_clearf(1);

        // call SW interrupt handler
        rwip_sw_int_handler();

        DBG_SWDIAG(IP_ISR, SWINT, 0);
    }

    DBG_SWDIAG(ISR, RWIP, 0);
}

#if 0
uint8_t rwip_sleep(void)
{
    uint8_t sleep_res = RWIP_ACTIVE;
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    int32_t sleep_duration;
//    int32_t sleep_duration1;
    rwip_time_t current_time;
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    DBG_SWDIAG(SLEEP, FUNC, 1);

    DBG_SWDIAG(SLEEP, ALGO, 0);

    do
    {
        /************************************************************************
         **************            CHECK KERNEL EVENTS             **************
         ************************************************************************/
        // Check if some kernel processing is ongoing (during wakeup, kernel events are not processed)
        if (((rwip_env.prevent_sleep & RW_WAKE_UP_ONGOING) == 0) && !ke_sleep_check())
            break;
      //  uart_printf("%x:%x\r\n",rwip_env.prevent_sleep,ke_sleep_check());
        // Processor sleep can be enabled
        sleep_res = RWIP_CPU_SLEEP;

        DBG_SWDIAG(SLEEP, ALGO, 1);

        /************************************************************************
         **************              CHECK RW FLAGS                **************
         ************************************************************************/
        // First check if no pending procedure prevent from going to sleep
        if (rwip_env.prevent_sleep != 0)
            break;

        #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
        DBG_SWDIAG(SLEEP, ALGO, 2);

        /************************************************************************
         **************           Retrieve Current time            **************
         ************************************************************************/
        current_time = rwip_time_get();
        // remove the on-going slot for clock correction
        current_time.hs += 1;
        // Remove 1 more slot because next slot will be started at end of function
        if((HALF_SLOT_INV(current_time.hus)) < rwip_env.sleep_algo_dur)
        {
            current_time.hs += 1;
        }
        // Be sure that we don't exceed the clock wrapping time
        current_time.hs &= RWIP_MAX_CLOCK_TIME;

        /************************************************************************
         ******* COMPUTE SLEEP TIME ACCORDING TO 10 MS AND HALF SLOT TIMER ******
         ************************************************************************/

        // put sleep duration to maximum value
        sleep_duration = (rwip_env.ext_wakeup_enable) ? MAX_SLEEP_DURATION_EXTERNAL_WAKEUP : MAX_SLEEP_DURATION_PERIODIC_WAKEUP;

        // check if 10ms timer is active
        if(rwip_env.timer_10ms_target != RWIP_INVALID_TARGET_TIME)
        {
            int32_t duration = CLK_DIFF(current_time.hs, rwip_env.timer_10ms_target);
            // update sleep duration to minimum value
            sleep_duration = co_min_s(sleep_duration, duration);
        }

        // check if Half slot timer is active
        if(rwip_env.timer_hs_target != RWIP_INVALID_TARGET_TIME)
        {
            int32_t duration = CLK_DIFF(current_time.hs, rwip_env.timer_hs_target);
            // update sleep duration to minimum value
            sleep_duration = co_min_s(sleep_duration, duration);
        }

        // check if Half us timer is active
        if(rwip_env.timer_hus_target != RWIP_INVALID_TARGET_TIME)
        {
            int32_t duration = CLK_DIFF(current_time.hs, rwip_env.timer_hus_target);
            // update sleep duration to minimum value
            sleep_duration = co_min_s(sleep_duration, duration);
        }

        // A timer ISR is not yet handled or will be raised soon
        // note the sleep duration could be negative, that's why it's useful to check if a minimum requirement is ok
        // at least one half slot.
        if(sleep_duration <= RWIP_MINIMUM_SLEEP_TIME)
        {
            break;
        }

        DBG_SWDIAG(SLEEP, ALGO, 3);

        /************************************************************************
         **************           CHECK SLEEP TIME                 **************
         ************************************************************************/
//        sleep_duration1 = sleep_duration;
        sleep_duration -= RWIP_MINIMUM_SLEEP_TIME;
        sleep_duration = rwip_slot_2_lpcycles(sleep_duration);

        // check if sleep duration is sufficient according to wake-up delay
        // HW issue, if sleep duration = max(twosc,twext) + 1 the HW never wakes up, so we have to ensure that at least
        // sleep duration > max(twosc,twext) + 1 (all in lp clk cycle)
        if(sleep_duration < rwip_env.lp_cycle_wakeup_delay * 2)
        {
            break;
        }

        DBG_SWDIAG(SLEEP, ALGO, 4);

        #if (H4TL_SUPPORT)
        /************************************************************************
         **************                 CHECK TL                   **************
         ************************************************************************/
        // Try to switch off TL
        if (!h4tl_stop())
        {
            sleep_res = RWIP_ACTIVE;
            break;
        }
        #endif //H4TL_SUPPORT

        DBG_SWDIAG(SLEEP, FUNC, 0);
        sleep_res = RWIP_DEEP_SLEEP;

        TRC_REQ_SLEEP();

        /************************************************************************
         **************          PROGRAM CORE DEEP SLEEP           **************
         ************************************************************************/
        #if (BT_EMB_PRESENT)
        // Put BT core into deep sleep
        rwbt_sleep_enter();
        #endif // (BT_EMB_PRESENT)

        #if (BLE_EMB_PRESENT)
        // Put BLE core into deep sleep
        rwble_sleep_enter();
        #endif // (BLE_EMB_PRESENT)

        // Program wake-up time
        ip_deepslwkup_set(sleep_duration);

        // Mask all interrupts except sleep IRQ
        ip_intcntl1_set(IP_SLPINTMSK_BIT);

        // Clear possible pending IRQs
        ip_intack1_clear(0xFFFFFFFF);
      //  uart_printf("sleep:%d:%d\r\n",sleep_duration1,current_time.hs);


        if(!rwip_env.ext_wakeup_enable)
        {
            ip_deepslcntl_extwkupdsb_setf(1);
        }


        DBG_SWDIAG(SLEEP, SLEEP, 1);

        /************************************************************************
         **************               SWITCH OFF RF                **************
         ************************************************************************/
        rwip_rf.sleep();
        
        rwip_prevent_sleep_set(RW_BLE_ACTIVE_MODE) ;
        #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    } while(0);

    if(sleep_res != RWIP_DEEP_SLEEP)
    {
        DBG_SWDIAG(SLEEP, FUNC, 0);
    }
    return sleep_res;
}
#endif

void rwip_set_bd_address(struct bd_addr *bd_addr)
{
	llm_set_public_addr(bd_addr);
	//lld_util_set_bd_address(bd_addr, ADDR_PUBLIC);
}

///@} RW

