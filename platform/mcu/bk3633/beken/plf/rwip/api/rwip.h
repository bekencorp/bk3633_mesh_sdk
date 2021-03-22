/**
****************************************************************************************
*
* @file rwip.h
*
* @brief RW IP SW main module
*
* Copyright (C) RivieraWaves 2009-2015
*
*
****************************************************************************************
*/
#ifndef _RWIP_H_
#define _RWIP_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @brief Entry points of the RW IP stacks/modules
 *
 * This module contains the primitives that allow an application accessing and running the
 * RW IP protocol stacks / modules.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"          // stack configuration

#include <stdint.h>               // standard integer definitions
#include <stdbool.h>              // standard boolean definitions
#include "ke_msg.h" 
#include "RomCallFlash.h"
//#include "prf.h" 

#define GPIO_TRIGER_SCAN_EVT_END    0x33
#define GPIO_TRIGER_SCAN_EVT_START  0x13
#define GPIO_TRIGER_CONN_EVT_START  0x30
#define GPIO_TRIGER_CONN_EVT_END    0x34
#define GPIO_TRIGER_ADV_EVT_START   0x31
#define GPIO_TRIGER_ADV_EVT_END     0x32
/*
 * DEFINES
 ****************************************************************************************
 */
/// Maximum value of a Bluetooth clock (in 312.5us half slots)
#define RWIP_MAX_CLOCK_TIME              ((1L<<28) - 1)
/// Maximum value of a 10ms Bluetooth clock
#define RWIP_MAX_10MS_TIME               ((1L<<23) - 1)
/// retrieve 10ms time according to clock time
#define RWIP_CLOCK_TO_10MS_TIME(clock)   ((clock) >> 5)
/// retrieve clock time according to 10ms time
#define RWIP_10MS_TIME_TO_CLOCK(time)    ((time)  << 5)
/// Invalid target time
#define RWIP_INVALID_TARGET_TIME         (0xFFFFFFFFL)

#define SYS_REG0X3_POWER                   *((volatile unsigned long *) (0x00800000+0x3*4))
#define POS_SYS_REG0X3_RWBT_PWD            17
#define SET_RWBT_POWER_UP                  SYS_REG0X3_POWER &= ~(1<<POS_SYS_REG0X3_RWBT_PWD)

/**
 * Inverse an intra-half-slot value (in half-us), from/to following formats:
 *   - A: elapsed time from the previous half-slot (in half-us)
 *   - B: remaining time to the next half-slot (in half-us)
 * The function from A to B or from B to A.
 *  ____________________________________________________________________________________________________
 *     Half-slot N-1            |             Half-slot N              |             Half-slot N+1
 *  ____________________________|______________________________________|________________________________
 *                              |<---------- A ---------->|<---- B --->|
 *  ____________________________|______________________________________|________________________________
 */
#define HALF_SLOT_INV(x)  (HALF_SLOT_SIZE - x - 1)

/// result of sleep state.
enum rwip_sleep_state
{
    /// Some activity pending, can not enter in sleep state
    RWIP_ACTIVE    = 0,
    /// CPU can be put in sleep state
    RWIP_CPU_SLEEP,
    /// IP could enter in deep sleep
    RWIP_DEEP_SLEEP,
};


/// Definition of the bits preventing the system from sleeping
enum prevent_sleep
{
    /// Flag indicating that the wake up process is ongoing
    RW_WAKE_UP_ONGOING                 = 0x0001,
    /// Flag indicating that an TX transfer is ongoing on Transport Layer
    RW_TL_TX_ONGOING                   = 0x0002,
    /// Flag indicating that an RX transfer is ongoing on Transport Layer
    RW_TL_RX_ONGOING                   = 0x0004,
    /// Flag indicating HCI timeout is ongoing
    RW_AHI_TIMEOUT                     = 0x0008,
    /// Flag indicating that an encryption is ongoing
    RW_CRYPT_ONGOING                   = 0x0010,
    /// Flag indicating that controller shall not sleep due to not CSB LPO_Allowed
    RW_CSB_NOT_LPO_ALLOWED             = 0x0040,
    /// Flag indicating the MWS/WLAN Event Generator is in operation
    RW_MWS_WLAN_EVENT_GENERATOR_ACTIVE = 0x0080,
    /// Flag to indicate that platform does not support deep sleep
    RW_PLF_DEEP_SLEEP_DISABLED         = 0x0100,
    /// Flag to indicate that a baseband frame is ongoing
    RW_BB_FRAME_ONGOING                = 0x0200,
    /// Flag to indicate that BLE Hopping computation on-going
    RW_HOP_CALC_ONGOING                = 0x0400,
    /// Flag to indicate that BT is in active mode (ACL, SCO)
    RW_BT_ACTIVE_MODE                  = 0x0800,
    /// Flag to indicate that BLE requires active mode
    RW_BLE_ACTIVE_MODE                 = 0x1000,
};

/// Parameters - Possible Returned Status
enum PARAM_STATUS
{
    /// PARAM status OK
    PARAM_OK,
    /// generic PARAM status KO
    PARAM_FAIL,
    /// PARAM ID unrecognized
    PARAM_ID_NOT_DEFINED,
    /// No space for PARAM
    PARAM_NO_SPACE_AVAILABLE,
    /// Length violation
    PARAM_LENGTH_OUT_OF_RANGE,
    /// PARAM parameter locked
    PARAM_PARAM_LOCKED,
    /// PARAM corrupted
    PARAM_CORRUPT
};

/**
 * External interface type types.
 */
enum rwip_eif_types
{
    /// Host Controller Interface - Controller part
    RWIP_EIF_HCIC,

    /// Host Controller Interface - Host part
    RWIP_EIF_HCIH,

    /// Application Host interface
    RWIP_EIF_AHI,
};


/// Enumeration of External Interface status codes
enum rwip_eif_status
{
    /// EIF status OK
    RWIP_EIF_STATUS_OK,
    /// EIF status KO
    RWIP_EIF_STATUS_ERROR,

#if (BLE_EMB_PRESENT == 0)
    /// External interface detached
    RWIP_EIF_STATUS_DETACHED,
    /// External interface attached
    RWIP_EIF_STATUS_ATTACHED,
#endif // (BLE_EMB_PRESENT == 0)
};

/// Enumeration of RF modulations
enum rwip_rf_mod
{
    MOD_GFSK  = 0x01,
    MOD_DQPSK = 0x02,
    MOD_8DPSK = 0x03,
};

#if RW_DEBUG
/// Assert type
/*@TRACE*/
enum assert_type
{
    ASSERT_TYPE_WARNING = 0,
    ASSERT_TYPE_ERROR   = 1,
};
#endif //RW_DEBUG

/// IP reset state.
enum rwip_init_type
{
    /// IP initialization
    RWIP_INIT    = 0,
    /// IP first reset
    RWIP_1ST_RST,
    /// Normal IP reset
    RWIP_RST,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Time information
/*@TRACE*/
typedef struct
{
    /// Integer part of the time (in half-slot)
    uint32_t hs;
    /// Fractional part of the time (in half-us) (range: 0-624)
    uint32_t hus;
} rwip_time_t;

/// API functions of the RF driver that are used by the BLE or BT software
struct rwip_rf_api
{
    /// Function called upon HCI reset command reception
    void (*reset)(void);
    /// Function called to enable/disable force AGC mechanism (true: en / false : dis)
    void (*force_agc_enable)(bool);
    /// Function called when TX power has to be decreased for a specific link id
    bool (*txpwr_dec)(uint8_t);
    /// Function called when TX power has to be increased for a specific link id
    bool (*txpwr_inc)(uint8_t);
    /// Function called when TX power has to be set to max for a specific link id
    void (*txpwr_max_set)(uint8_t);
    /// Function called to convert a TX power CS power field into the corresponding value in dBm
    uint8_t (*txpwr_dbm_get)(uint8_t, uint8_t);
    /// Function called to convert a power in dBm into a control structure tx power field
    uint8_t (*txpwr_cs_get)(int8_t, bool);
    /// Function called to convert the RSSI read from the control structure into a real RSSI
    int8_t (*rssi_convert)(uint8_t);
    /// Function used to read a RF register
    uint32_t (*reg_rd)(uint32_t);
    /// Function used to write a RF register
    void (*reg_wr)(uint32_t, uint32_t);
    /// Function called to put the RF in deep sleep mode
    void (*sleep)(void);
    /// Index of minimum TX power
    uint8_t txpwr_min;
    /// Index of maximum TX power
    uint8_t txpwr_max;
    /// RSSI high threshold ('real' signed value in dBm)
    int8_t rssi_high_thr;
    /// RSSI low threshold ('real' signed value in dBm)
    int8_t rssi_low_thr;
    /// interferer threshold ('real' signed value in dBm)
    int8_t rssi_interf_thr;
    /// RF wakeup delay (in slots)
    //uint8_t wakeup_delay;
};

/// API functions of the parameters that are used by the BLE or BT software
struct rwip_param_api
{
    /**
     * Get a parameter value
     * @param[in]      param_id     Parameter identifier
     * @param[in/out]  lengthPtr    Pointer to the length of the parameter (input: contain max length, output contain the effective param length, in bytes)
     * @param[out]     buf          Pointer to the buffer be filled with the parameter value
     * @return  status              0: success | >0 : error
     */
    uint8_t (*get) (uint8_t param_id, uint8_t * lengthPtr, uint8_t *buf);

    /**
     * Set a parameter value
     * @param[in]      param_id     Parameter identifier
     * @param[in/out]  length       Length of the parameter (in bytes)
     * @param[out]     buf          Pointer to the buffer containing the parameter value
     * @return  status              0: success | >0 : error
     */
    uint8_t (*set) (uint8_t param_id, uint8_t length, uint8_t *buf);

    /**
     * Delete a parameter
     * @param[in]      param_id     Parameter identifier
     * @return  status              0: success | >0 : error
     */
    uint8_t (*del) (uint8_t param_id);
};

/// Internal API for priority
struct rwip_prio
{
    ///value
    uint8_t value;
    ///Increment
    uint8_t increment;
};
/**
 ****************************************************************************************
 * @brief Function called when packet transmission/reception is finished.

 * @param[in]  dummy  Dummy data pointer returned to callback when operation is over.
 * @param[in]  status Ok if action correctly performed, else reason status code.
 *****************************************************************************************
 */
typedef void (*rwip_eif_callback) (void*, uint8_t);

/**
 * Transport layer communication interface.
 */
struct rwip_eif_api
{
    /**
     *************************************************************************************
     * @brief Starts a data reception.
     *
     * @param[out] bufptr      Pointer to the RX buffer
     * @param[in]  size        Size of the expected reception
     * @param[in]  callback    Pointer to the function called back when transfer finished
     * @param[in]  dummy       Dummy data pointer returned to callback when reception is finished
     *************************************************************************************
     */
    void (*read) (uint8_t *bufptr, uint32_t size, rwip_eif_callback callback, void* dummy);

    /**
     *************************************************************************************
     * @brief Starts a data transmission.
     *
     * @param[in]  bufptr      Pointer to the TX buffer
     * @param[in]  size        Size of the transmission
     * @param[in]  callback    Pointer to the function called back when transfer finished
     * @param[in]  dummy       Dummy data pointer returned to callback when transmission is finished
     *************************************************************************************
     */
    void (*write)(uint8_t *bufptr, uint32_t size, rwip_eif_callback callback, void* dummy);

    /**
     *************************************************************************************
     * @brief Enable Interface flow.
     *************************************************************************************
     */
    void (*flow_on)(void);

    /**
     *************************************************************************************
     * @brief Disable Interface flow.
     *
     * @return True if flow has been disabled, False else.
     *************************************************************************************
     */
    bool (*flow_off)(void);
};

struct rwip_func_tag
{
	__BLEIRQ void (*rwip_isr)(void);
	void (*rwip_init)(uint32_t error);
	void (*rwip_schedule)(void);
	void (*rwip_set_bd_address)(struct bd_addr *bd_addr);
    uint8_t (*rwip_sleep)(int32_t * dur);
    uint16_t (*rwip_sleep_flag)(void);
};
extern const struct rwip_func_tag rwip_func;


/*
 * VARIABLE DECLARATION
*****************************************************************************************
 */

/// API for RF driver
extern struct rwip_rf_api rwip_rf;
/// API for parameters
extern struct rwip_param_api rwip_param;

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// API for dual mode priority
extern struct rwip_prio rwip_priority[RWIP_PRIO_IDX_MAX];
#endif //#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// IP reset state variable
extern uint8_t rwip_rst_state;

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#if (RW_WLAN_COEX || RW_MWS_COEX)
/// API for COEX configuration
extern const uint8_t rwip_coex_cfg[RWIP_COEX_CFG_MAX];
#endif //(RW_WLAN_COEX || RW_MWS_COEX)
#endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

/*
 * MACROS
 ****************************************************************************************
 */

/// Get Event status flag
#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#if (RW_WLAN_COEX || RW_MWS_COEX)
#define RWIP_COEX_GET(coex_cfg_idx, bit_field) \
                (uint8_t)(((rwip_coex_cfg[RWIP_COEX_ ## coex_cfg_idx ##_IDX]) >> RWIP_ ## bit_field ## _POS ) & RWIP_COEX_BIT_MASK)
#else //!(RW_WLAN_COEX || RW_MWS_COEX)
#define RWIP_COEX_GET(coex_cfg_idx, bit_field) 0
#endif //(RW_WLAN_COEX || RW_MWS_COEX)
#endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

/*
 * FUNCTION DECLARATION
*****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the RW BT SW.
 *
 ****************************************************************************************
 */
void rwip_init(uint32_t error);

/**
 ****************************************************************************************
 * @brief Reset the RW BT SW.
 *
 ****************************************************************************************
 */
void rwip_reset(void);



#if (BT_EMB_PRESENT)

#if PCA_SUPPORT
/**
 ****************************************************************************************
 * @brief Check if clock dragging limitation
 *
 * @return    true if clock dragging must be used
 ****************************************************************************************
 */
bool rwip_pca_clock_dragging_only(void);
#endif //PCA_SUPPORT
#endif // (BT_EMB_PRESENT)

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#if (RW_MWS_COEX)
/**
 ****************************************************************************************
 * @brief Enable/Disable the MWS coexistence interface.
 * 
 * @param[in]   CoexSetting     Coexistence value
 *
 ****************************************************************************************
 */
void rwip_mwscoex_set(bool state);
#endif //RW_MWS_COEX

#if (RW_WLAN_COEX)
/**
 ****************************************************************************************
 * @brief Enable/Disable the Wireless LAN coexistence interface.
 * 
 * @param[in]   CoexSetting     Coexistence value
 *
 ****************************************************************************************
 */
void rwip_wlcoex_set(bool state);
#endif //RW_WLAN_COEX
#endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Function to implement in platform in order to retrieve each external interface API
 *
 * @param[in] idx External interface index
 *
 * @return External interface api structure
 ****************************************************************************************
 */
extern const struct rwip_eif_api* rwip_eif_get(uint8_t idx);

#if RW_DEBUG
/**
 ****************************************************************************************
 * @brief Raises an assertion message to the control interface (if present)
 *
 * @param[in] file    File name
 * @param[in] line    Line number
 * @param[in] param0  Parameter 0 (custom value given by the assert instruction)
 * @param[in] param1  Parameter 1 (custom value given by the assert instruction)
 * @param[in] type    0: warning / 1: error
 ****************************************************************************************
 */
void rwip_assert(const char * file, int line, int param0, int param1, uint8_t type);
#endif //RW_DEBUG


/* **************************************************************************************
 * Driver functions
 * **************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieved sampled time
 *
 * @note Shall be called within a critical section
 *
 * @return current time sampled (@see rwip_time_t)
 ****************************************************************************************
 */
rwip_time_t rwip_time_get(void);


#if (BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Set current time
 *
 * @param clock value in half-slots
 ****************************************************************************************
 */
void rwip_time_set(uint32_t clock);
#endif // (BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Set the a 10 ms target timer
 *
 * @note if target is RWIP_INVALID_TARGET_TIME, not timer are programmed
 *
 * @param[in] target       10ms Timer target value
 ****************************************************************************************
 */
void rwip_timer_10ms_set(uint32_t target);

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Set the a half slot target timer
 *
 * @note if target is RWIP_INVALID_TARGET_TIME, not timer are programmed
 *
 * @param[in] target      Half Slot Timer target value
 ****************************************************************************************
 */
void rwip_timer_hs_set(uint32_t target);

/**
 ****************************************************************************************
 * @brief Set the a half slot target timer
 *
 * @note if target is RWIP_INVALID_TARGET_TIME, not timer are programmed
 *
 * @param[in] target         Half Slot Timer target value
 * @param[in] half_us_delay  Half us timer delay in corresponding half slot (range [0:624])
 ****************************************************************************************
 */
void rwip_timer_hus_set(uint32_t target, uint32_t half_us_delay);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Start AES encryption
 *
 * The exchange memory must be filled before calling this function.
 * This function expect to be called from a BLE Module
 *
 * @param[in] key           AES Encryption key must be 16 bytes
 * @param[in] val           16 bytes value array to encrypt using AES
 ****************************************************************************************
 */
void rwip_aes_encrypt(const uint8_t *key, const uint8_t* val);

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Request a Software interrupt to be triggered
 ****************************************************************************************
 */
void rwip_sw_int_req(void);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Invoke the sleep function.
 *
 * @return  sleep status (@see enum rwip_sleep_state)
 ****************************************************************************************
 */
uint8_t rwip_sleep(int32_t * dur);
uint16_t rwip_sleep_flag(void);
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Handle the common core interrupts.
 ****************************************************************************************
 */
void rwip_isr(void);

#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Set a bit in the prevent sleep bit field, in order to prevent the system from
 *        going to sleep
 *
 * @param[in] prv_slp_bit   Bit to be set in the prevent sleep bit field
 ****************************************************************************************
 */
void rwip_prevent_sleep_set(uint16_t prv_slp_bit);

/**
 ****************************************************************************************
 * @brief Clears a bit in the prevent sleep bit field, in order to allow the system
 *        going to sleep
 *
 * @param[in] prv_slp_bit   Bit to be cleared in the prevent sleep bit field
 ****************************************************************************************
 */
void rwip_prevent_sleep_clear(uint16_t prv_slp_bit);

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Check if the system is permanently active (e.g. for BT ACL connection)
 *
 * @return  false: system might sleep | true: system stays active
 ****************************************************************************************
 */
bool rwip_active_check(void);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Schedule all pending events.
 *
 ****************************************************************************************
 */
void rwip_schedule(void);
void rwip_timer_hs_handler(void);
void rwip_timer_hus_handler(void);
void rwip_wakeup(void);
void rwip_wakeup_end(void);
void rwip_timer_10ms_handler(void);
void rwip_crypt_isr_handler(void);
void rwip_sw_int_handler(void);
int32_t rwip_slot_2_lpcycles(int32_t hs_cnt);

void rwip_set_bd_address(struct bd_addr *bd_addr);

///@} ROOT

#endif // _RWIP_H_
