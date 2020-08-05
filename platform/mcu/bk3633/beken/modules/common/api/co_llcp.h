/**
 ****************************************************************************************
 *
 * @file co_llcp.h
 *
 * @brief This file contains the LLCP Bluetooth defines, enumerations and structures
 *        definitions for use by all modules in RW stack.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 ****************************************************************************************
 */

#ifndef CO_LLCP_H_
#define CO_LLCP_H_

/**
 ****************************************************************************************
 * @addtogroup CO_BT
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_bt.h"
/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Control packet op_code
/*@TRACE*/
enum co_llcp_op_code
{
    LL_CONNECTION_UPDATE_IND_OPCODE = 0x00,
    LL_CHANNEL_MAP_IND_OPCODE       = 0x01,
    LL_TERMINATE_IND_OPCODE         = 0x02,
    LL_ENC_REQ_OPCODE               = 0x03,
    LL_ENC_RSP_OPCODE               = 0x04,
    LL_START_ENC_REQ_OPCODE         = 0x05,
    LL_START_ENC_RSP_OPCODE         = 0x06,
    LL_UNKNOWN_RSP_OPCODE           = 0x07,
    LL_FEATURE_REQ_OPCODE           = 0x08,
    LL_FEATURE_RSP_OPCODE           = 0x09,
    LL_PAUSE_ENC_REQ_OPCODE         = 0x0A,
    LL_PAUSE_ENC_RSP_OPCODE         = 0x0B,
    LL_VERSION_IND_OPCODE           = 0x0C,
    LL_REJECT_IND_OPCODE            = 0x0D,
    LL_SLAVE_FEATURE_REQ_OPCODE     = 0x0E,
    LL_CONNECTION_PARAM_REQ_OPCODE  = 0x0F,
    LL_CONNECTION_PARAM_RSP_OPCODE  = 0x10,
    LL_REJECT_EXT_IND_OPCODE        = 0x11,
    LL_PING_REQ_OPCODE              = 0x12,
    LL_PING_RSP_OPCODE              = 0x13,
    LL_LENGTH_REQ_OPCODE            = 0x14,
    LL_LENGTH_RSP_OPCODE            = 0x15,
    LL_PHY_REQ_OPCODE               = 0x16,
    LL_PHY_RSP_OPCODE               = 0x17,
    LL_PHY_UPDATE_IND_OPCODE        = 0x18,
    LL_MIN_USED_CHANNELS_IND_OPCODE = 0x19,
    LL_CTE_REQ_OPCODE               = 0x1A,
    LL_CTE_RSP_OPCODE               = 0x1B,
    LL_PER_SYNC_IND_OPCODE          = 0x1C,
    LL_CLK_ACC_REQ_OPCODE           = 0x1D,
    LL_CLK_ACC_RSP_OPCODE           = 0x1E,

    /// Request creation of ICO
    LL_ICO_REQ_OPCODE = 0x1F,
    /// Response to ICO Request with Slave parameters
    LL_ICO_RSP_OPCODE,
    /// Issued by the Master to establish the ICO with a Slave.
    LL_ICO_IND_OPCODE,
    /// Terminate the ICO channel associated with the Stream_ID and Channel_ID fields
    LL_ICO_TERMINATE_IND_OPCODE,

    /// Opcode length
    LL_OPCODE_MAX_OPCODE,
    LL_OPCODE_DEBUG = 0xFF,
};

/// LLCP PDU lengths (including op_code)
enum co_llcp_length
{
    LL_CONNECTION_UPDATE_IND_LEN     = 12,
    LL_CHANNEL_MAP_IND_LEN           = 8,
    LL_TERMINATE_IND_LEN             = 2,
    LL_ENC_REQ_LEN                   = 23,
    LL_ENC_RSP_LEN                   = 13,
    LL_START_ENC_REQ_LEN             = 1,
    LL_START_ENC_RSP_LEN             = 1,
    LL_UNKNOWN_RSP_LEN               = 2,
    LL_FEATURE_REQ_LEN               = 9,
    LL_FEATURE_RSP_LEN               = 9,
    LL_PAUSE_ENC_REQ_LEN             = 1,
    LL_PAUSE_ENC_RSP_LEN             = 1,
    LL_VERSION_IND_LEN               = 6,
    LL_REJECT_IND_LEN                = 2,
    LL_SLAVE_FEATURE_REQ_LEN         = 9,
    LL_REJECT_EXT_IND_LEN            = 3,
    LL_CONNECTION_PARAM_REQ_LEN      = 24,
    LL_CONNECTION_PARAM_RSP_LEN      = 24,
    LL_PING_REQ_LEN                  = 1,
    LL_PING_RSP_LEN                  = 1,
    LL_LENGTH_REQ_LEN                = 9,
    LL_LENGTH_RSP_LEN                = 9,
    LL_PHY_REQ_LEN                   = 3,
    LL_PHY_RSP_LEN                   = 3,
    LL_PHY_UPDATE_IND_LEN            = 5,
    LL_MIN_USED_CHANNELS_IND_LEN     = 3,
    LL_CTE_REQ_LEN                   = 2,
    LL_CTE_RSP_LEN                   = 1,
    LL_PER_SYNC_IND_LEN              = 35,
    LL_CLK_ACC_REQ_LEN               = 2,
    LL_CLK_ACC_RSP_LEN               = 2,

    LL_ICO_REQ_LEN                   = 24,
    LL_ICO_RSP_LEN                   = 9,
    LL_ICO_IND_LEN                   = 16,
    LL_ICO_TERMINATE_IND_LEN         = 4,

    LL_PDU_LENGTH_MAX                = 33
};


/// ICL Control packet op_code
/*@TRACE*/
enum co_icl_op_code
{
    /// Channel map update
    ICL_CHANNEL_MAP_IND_OPCODE,
    /// Termination indication
    ICL_TERMINATE_IND_OPCODE,

    /// Opcode length
    ICL_OPCODE_MAX_OPCODE,
};

/// ICL PDU lengths (including op_code)
enum co_icl_length
{
    ICL_CHANNEL_MAP_IND_LEN           = 8,
    ICL_TERMINATE_IND_LEN             = 2,

    ICL_PDU_LENGTH_MAX                = 8
};


/// PDU lengths
enum co_pdu_length
{
    PDU_SCAN_REQ_LEN          = 12,
    PDU_CON_REQ_LEN           = 34,
    PDU_CON_RSP_LEN           = 14,
};

/// Bit field definitions for BN field sent over the air
///    7    6    5    4    3    2    1    0
/// +----+----+----+----+----+----+----+----+
/// |     BN_S_TO_M     |     BN_M_TO_S     |
/// +----+----+----+----+----+----+----+----+
enum co_llcp_ico_bn_field
{
    /// BN Master to Slave
    LLCP_ICO_BN_M_TO_S_LSB     = 0,
    LLCP_ICO_BN_M_TO_S_MASK    = (0x0F),

    /// BN Slave to Master
    LLCP_ICO_BN_S_TO_M_LSB     = 4,
    LLCP_ICO_BN_S_TO_M_MASK    = (0xF0),
};

/// Bit field definitions for combination of advertising Set ID, advertiser address type and sleep clock accuracy in LL_PER_SYNC_IND
///    7    6    5     4     3    2    1    0
/// +----+----+----+-------+----+----+----+----+
/// |      SCA     | AType |        SID        |
/// +----+----+----+-------+----+----+----+----+
enum co_llcp_sid_atype_sca
{
    /// Advertising SID
    LLCP_PER_SYNC_IND_SID_LSB     = 0,
    LLCP_PER_SYNC_IND_SID_MASK    = (0x0F),

    /// Advertiser address type
    LLCP_PER_SYNC_IND_ATYPE_POS   = 4,
    LLCP_PER_SYNC_IND_ATYPE_BIT  = (0x10),

    /// Sleep clock accuracy (@see enum SCA)
    LLCP_PER_SYNC_IND_SCA_LSB     = 5,
    LLCP_PER_SYNC_IND_SCA_MASK    = (0xE0),
};

/*
 * MESSAGES
 ****************************************************************************************
 */

/// LL_DEFAULT structure (for packet with no parameter)
/*@TRACE
 * @NO_PAD
*/
struct  ll_default
{
    /// op_code
    uint8_t             op_code;
};

/// LL_CONNECTION_UPDATE_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_connection_update_ind
{
    /// op_code
    uint8_t         op_code;
    /// window size (units of 1.25 ms)
    uint8_t         win_size;
    /// window offset (units of 1.25 ms)
    uint16_t        win_off;
    /// interval (units of 1.25 ms)
    uint16_t        interv;
    /// connection latency (unit of connection event)
    uint16_t        latency;
    /// link supervision timeout (unit of 10 ms)
    uint16_t        timeout;
    /// instant (unit of connection event)
    uint16_t        instant;
};

/// LL_CHANNEL_MAP_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_channel_map_ind
{
    /// op_code
    uint8_t            op_code;
    /// channel mapping
    struct le_chnl_map ch_map;
    /// instant
    uint16_t           instant;
};

/// LL_TERMINATE_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_terminate_ind
{
    /// op_code
    uint8_t         op_code;
    /// termination code
    uint8_t         err_code;
};

/// LL_ENC_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_enc_req
{
    /// op_code
    uint8_t               op_code;
    /// random value
    struct rand_nb        rand;
    /// ediv
    uint16_t              ediv;
    /// skdm
    struct sess_k_div_x   skdm;
    /// ivm
    struct init_vect      ivm;
};

/// LL_ENC_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_enc_rsp
{
    /// op_code
    uint8_t             op_code;
    /// skds
    struct sess_k_div_x   skds;
    /// ivs
    struct init_vect    ivs;
};

/// LL_START_ENC_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_start_enc_req
{
    /// op_code
    uint8_t             op_code;
};

/// LL_START_ENC_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_start_enc_rsp
{
    /// op_code
    uint8_t             op_code;
};

/// LL_UNKNOWN_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_unknown_rsp
{
    /// op_code
    uint8_t         op_code;
    /// unknown type
    uint8_t         unk_type;
};

/// LL_FEATURE_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_feature_req
{
    /// op_code
    uint8_t             op_code;
    /// le features
    struct le_features  feats;
};


/// LL_FEATURE_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_feature_rsp
{
    /// op_code
    uint8_t             op_code;
    /// le features
    struct le_features  feats;
};

/// LL_PAUSE_ENC_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_pause_enc_req
{
    /// op_code
    uint8_t             op_code;
};

/// LL_PAUSE_ENC_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_pause_enc_rsp
{
    /// op_code
    uint8_t             op_code;
};

/// LL_VERSION_IND structure
/*@TRACE
 * @NO_PAD
*/
struct ll_version_ind
{
    /// op_code
    uint8_t     op_code;
    /// version
    uint8_t     vers;
    /// company id
    uint16_t    compid;
    /// sub version
    uint16_t    subvers;
};

/// LL_REJECT_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_reject_ind
{
    /// op_code
    uint8_t         op_code;
    /// reject reason
    uint8_t         err_code;
};

/// LL_SLAVE_FEATURE_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_slave_feature_req
{
    /// op_code
    uint8_t             op_code;
    /// le features
    struct le_features  feats;
};

/// LL_CONNECTION_PARAM_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_connection_param_req
{
    /// op_code
    uint8_t         op_code;
    /// minimum value of connInterval (units of 1.25 ms)
    uint16_t        interval_min;
    /// maximum value of connInterval (units of 1.25 ms)
    uint16_t        interval_max;
    /// connSlaveLatency value (unit of connection event)
    uint16_t        latency;
    /// connSupervisionTimeout value (unit of 10 ms)
    uint16_t        timeout;
    /// preferred periodicity (units of 1.25 ms)
    uint8_t         pref_period;
    /// ReferenceConnEventCount (unit of connection event)
    uint16_t        ref_con_event_count;
    /// Offset0 (units of 1.25 ms)
    uint16_t        offset0;
    /// Offset1 (units of 1.25 ms)
    uint16_t        offset1;
    /// Offset2 (units of 1.25 ms)
    uint16_t        offset2;
    /// Offset3 (units of 1.25 ms)
    uint16_t        offset3;
    /// Offset4 (units of 1.25 ms)
    uint16_t        offset4;
    /// Offset5 (units of 1.25 ms)
    uint16_t        offset5;
};

/// LL_CONNECTION_PARAM_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_connection_param_rsp
{
    /// op_code
    uint8_t         op_code;
    /// minimum value of connInterval (units of 1.25 ms)
    uint16_t        interval_min;
    /// maximum value of connInterval (units of 1.25 ms)
    uint16_t        interval_max;
    /// connSlaveLatency value (unit of connection event)
    uint16_t        latency;
    /// connSupervisionTimeout value (unit of 10 ms)
    uint16_t        timeout;
    /// preferred periodicity (units of 1.25 ms)
    uint8_t         pref_period;
    /// ReferenceConnEventCount (unit of connection event)
    uint16_t        ref_con_event_count;
    /// Offset0 (units of 1.25 ms)
    uint16_t        offset0;
    /// Offset1 (units of 1.25 ms)
    uint16_t        offset1;
    /// Offset2 (units of 1.25 ms)
    uint16_t        offset2;
    /// Offset3 (units of 1.25 ms)
    uint16_t        offset3;
    /// Offset4 (units of 1.25 ms)
    uint16_t        offset4;
    /// Offset5 (units of 1.25 ms)
    uint16_t        offset5;
};

/// LL_REJECT_EXT_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_reject_ext_ind
{
    /// op_code
    uint8_t         op_code;
    /// rejected op_code
    uint8_t         rej_op_code;
    /// reject reason
    uint8_t         err_code;
};

/// LL_PING_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_ping_req
{
    /// op_code
    uint8_t         op_code;
};

/// LL_PING_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_ping_rsp
{
    /// op_code
    uint8_t         op_code;
};

/// LL_LENGTH_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_length_req
{
    /// op_code
    uint8_t     op_code;
    /// The max size in reception (unit of byte)
    uint16_t    max_rx_octets;
    /// The max time in reception (unit of microsecond)
    uint16_t    max_rx_time;
    /// The max size in transmission (unit of byte)
    uint16_t    max_tx_octets;
    /// The max time in transmission (unit of microsecond)
    uint16_t    max_tx_time;
};

/// LL_LENGTH_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_length_rsp
{
    /// op_code
    uint8_t     op_code;
    /// The max size in reception (unit of byte)
    uint16_t    max_rx_octets;
    /// The max time in reception (unit of microsecond)
    uint16_t    max_rx_time;
    /// The max size in transmission (unit of byte)
    uint16_t    max_tx_octets;
    /// The max time in transmission (unit of microsecond)
    uint16_t    max_tx_time;
};
/// LL_PHY_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_phy_req
{
    /// op_code
    uint8_t    op_code;
    /// Tx phy selection
    uint8_t    tx_phys;
    /// Rx phy selection
    uint8_t    rx_phys;
};

/// LL_PHY_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_phy_rsp
{
    /// op_code
    uint8_t    op_code;
    /// Tx phy selection
    uint8_t    tx_phys;
    /// Rx phy selection
    uint8_t    rx_phys;
};

/// LL_PHY_UPDATE_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_phy_update_ind
{
    /// op_code
    uint8_t    op_code;
    /// master to slave phy selected
    uint8_t    m_to_s_phy;
    /// slave to master phy selected
    uint8_t    s_to_m_phy;
    /// Instant
    uint16_t   instant;
};

/// LL_MIN_USED_CHANNELS_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_min_used_channels_ind
{
    /// op_code
    uint8_t    op_code;
    /// PHY(s) for which the slave has a minimum number of used channels requirement
    uint8_t    phys;
    /// minimum number of channels to be used on the specified PHY
    uint8_t    min_used_ch;
};

/**
 * CTE length and type bit field definition
 *
 * |     5 bits     |  1b |   2 bits    |
 * |  MinCTELenReq  | RFU | CTETypeReq  |
 *
 *   - MinCTELenReq: minimum CTE length (x 8us)
 *   - CTETypeReq: 0-AoA, 1-AoD_1us, 2-AoD_2us, 3-RFU
 */
enum cte_len_type
{
    CTE_TYPE_REQ_MASK       = 0xC0,
    CTE_TYPE_REQ_LSB        = 6,

    MIN_CTE_LEN_REQ_MASK    = 0x1F,
    MIN_CTE_LEN_REQ_LSB     = 0,
};

/// LL_CTE_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct ll_cte_req
{
    /// op_code
    uint8_t    op_code;
    /// CTE length and type (@see enum cte_len_type)
    uint8_t    cte_len_type;
};

/// LL_PER_SYNC_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct ll_per_sync_ind
{
    /// op_code
    uint8_t        op_code;
    /// ID (provided by the Host)
    uint16_t       id;
    /// Sync Info (TODO ref definition or spec)
    uint8_t        sync_info[18];
    /// Connection event counter
    uint16_t       con_evt_cnt;
    /// Last periodic advertising event counter
    uint16_t       last_pa_evt_cnt;
    /// Advertising Set ID, advertiser address type and sleep clock accuracy
    uint8_t        sid_atype_sca;
    /// Periodic advertising PHY (@see enum le_phy_mask)
    uint8_t        phy;
    /// Periodic advertiser address
    struct bd_addr adva;
    /// Sync Connection event counter
    uint16_t       sync_con_evt_cnt;
};

/// LL_CLK_ACC_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct ll_clk_acc_req
{
    /// op_code
    uint8_t    op_code;
    /// SCA (@see enum SCA)
    uint8_t    sca;
};

/// LL_CLK_ACC_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct ll_clk_acc_rsp
{
    /// op_code
    uint8_t    op_code;
    /// SCA (@see enum SCA)
    uint8_t    sca;
};

/// LL_ICO_REQ structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_ico_req
{
    /// op_code
    uint8_t         op_code;
    /// Stream identifier of the ICO stream
    uint8_t         ico_stream_id;
    /// Identifier of the ICO.
    uint8_t         ico_id;
    /// Physical Channel Info Master to Slave PHY
    uint8_t         phy_m_to_s;
    /// Physical Channel Info Slave to Master PHY
    uint8_t         phy_s_to_m;
    /// Max Payload Size: Master to Slave
    uint8_t         max_pld_size_m_to_s;
    /// Max Payload Size: Slave to Master
    uint8_t         max_pld_size_s_to_m;
    /// Number of Sub Event
    uint8_t         nse;
    /// Sub-event interval in microseconds (Range 400us - Connection Interval)
    uint32_t        sub_evt_interval;
    /// Burst number: Number of new packet per channel interval
    /// bit[0:3] : Master to Slave Burst number The range shall be 0 to 15
    /// bit[4:7] : Slave to Master Burst number The range shall be 0 to 15
    uint8_t         bn;
    /// Master to Slave Flush timeout
    uint8_t         ft_m_to_s;
    /// Slave to Master Flush timeout
    uint8_t         ft_s_to_m;
    ///Ttime between two consecutive ICO events. The ICO Interval field shall be in multiple of 1.25ms and the value
    /// shall be between 5ms and 4s
    uint16_t        chan_interval;
    /// Minimum time in microseconds between the start of the connection event when connEventCount will be transmitted
    /// to the first ICO anchor point. The ICO_Offset_Min field shall have a value between 300µs and a value less than
    /// the Connection Interval of the connection between the Master and the Slave .
    uint32_t        ico_offset_min;
    /// maximum time in microseconds between the start of the connection event when connEventCount will be transmitted
    /// to the first ICO anchor point. The ICO_Offset_Max field shall have a value between ICO_Offset_Min and a value
    /// less than Connection Interval of the connection between the Master and the Slave.
    uint32_t        ico_offset_max;
    /// Event counter of the ACL event from which the Ch_Offset_Min and the Ch_Offset_Max are referenced
    uint16_t        conn_event_cnt;
};

/// LL_ICO_RSP structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_ico_rsp
{
    /// op_code
    uint8_t         op_code;
    /// Minimum time in microseconds between the start of the connection event when connEventCount will be transmitted
    /// to the first ICO anchor point. The ICO_Offset_Min field shall have a value between 300µs and a value less than
    /// the Connection Interval of the connection between the Master and the Slave .
    uint32_t        ico_offset_min;
    /// maximum time in microseconds between the start of the connection event when connEventCount will be transmitted
    /// to the first ICO anchor point. The ICO_Offset_Max field shall have a value between ICO_Offset_Min and a value
    /// less than Connection Interval of the connection between the Master and the Slave.
    uint32_t        ico_offset_max;
    /// Event counter of the ACL event from which the Ch_Offset_Min and the Ch_Offset_Max are referenced
    uint16_t        conn_event_cnt;
};

/// LL_ICO_UPDATE_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_ico_ind
{
    /// op_code
    uint8_t         op_code;
    /// Access Address field is the number used to identify the ICO channel.
    uint32_t        access_addr;
    /// Time in microseconds from the start of the connection event referenced by the connEventCount to the first ICO Anchor Point
    uint32_t        ico_offset;
    /// time in microseconds from the ICO anchor point of the first ICO in the ICO stream to the end of the last subevent of the last
    /// ICO within the ICO stream in the same ICO stream interval.
    uint32_t        ico_stream_delay;
    /// Time in microseconds from the ICO anchor point of the ICO to the end of the last subevent of the last ICO within the ICO
    /// stream in the same ICO stream interval
    uint32_t        ico_trans_delay;
    /// Indicates the point from which the Channel_Offset is added to give the time of the first ICO Channel Anchor Point
    uint16_t        conn_event_cnt;
};

/// LL_ICO_TERMINATE_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  ll_ico_terminate_ind
{
    /// op_code
    uint8_t         op_code;
    ///  Stream identifier of the ICO stream.
    uint8_t         ico_stream_id;
    /// Channel identifier of the ICO channel.
    uint8_t         ico_id;
    /// Reason for ICO channel termination
    uint8_t         err_code;
};


/// LLCP pdu format
/*@TRACE
 @trc_ref co_llcp_op_code
 */
union llcp_pdu
{
    /// op_code
    uint8_t  op_code;

    //@trc_union op_code == LL_CONNECTION_UPDATE_IND_OPCODE
    struct ll_connection_update_ind     con_update_ind;

    //@trc_union op_code == LL_CHANNEL_MAP_IND_OPCODE
    struct ll_channel_map_ind           channel_map_ind;

    //@trc_union op_code == LL_TERMINATE_IND_OPCODE
    struct ll_terminate_ind             terminate_ind;

    //@trc_union op_code == LL_ENC_REQ_OPCODE
    struct ll_enc_req                   enc_req;

    //@trc_union op_code == LL_ENC_RSP_OPCODE
    struct ll_enc_rsp                   enc_rsp;

    //@trc_union op_code == LL_START_ENC_REQ_OPCODE
    struct ll_start_enc_req             start_enc_req;

    //@trc_union op_code == LL_START_ENC_RSP_OPCODE
    struct ll_start_enc_rsp             start_enc_rsp;

    //@trc_union op_code == LL_UNKNOWN_RSP_OPCODE
    struct ll_unknown_rsp               unknown_rsp;

    //@trc_union op_code == LL_FEATURE_REQ_OPCODE
    struct ll_feature_req               feats_req;

    //@trc_union op_code == LL_FEATURE_RSP_OPCODE
    struct ll_feature_rsp               feats_rsp;

    //@trc_union op_code == LL_PAUSE_ENC_REQ_OPCODE
    struct ll_pause_enc_req             pause_enc_req;

    //@trc_union op_code == LL_PAUSE_ENC_RSP_OPCODE
    struct ll_pause_enc_rsp             pause_enc_rsp;

    //@trc_union op_code == LL_VERSION_IND_OPCODE
    struct ll_version_ind               vers_ind;

    //@trc_union op_code == LL_REJECT_IND_OPCODE
    struct ll_reject_ind                reject_ind;

    //@trc_union op_code == LL_SLAVE_FEATURE_REQ_OPCODE
    struct ll_slave_feature_req         slave_feature_req;

    //@trc_union op_code == LL_CONNECTION_PARAM_REQ_OPCODE
    struct ll_connection_param_req      con_param_req;

    //@trc_union op_code == LL_CONNECTION_PARAM_RSP_OPCODE
    struct ll_connection_param_rsp      con_param_rsp;

    //@trc_union op_code == LL_REJECT_EXT_IND_OPCODE
    struct ll_reject_ext_ind            reject_ind_ext;

    //@trc_union op_code == LL_PING_REQ_OPCODE
    struct ll_ping_req                  ping_req;

    //@trc_union op_code == LL_PING_RSP_OPCODE
    struct ll_ping_rsp                  ping_rsp;

    //@trc_union op_code == LL_LENGTH_REQ_OPCODE
    struct ll_length_req                length_req;

    //@trc_union op_code == LL_LENGTH_RSP_OPCODE
    struct ll_length_rsp                length_rsp;

    //@trc_union op_code == LL_PHY_REQ_OPCODE
    struct ll_phy_req                   phy_req;

    //@trc_union op_code == LL_PHY_RSP_OPCODE
    struct ll_phy_rsp                   phy_rsp;

    //@trc_union op_code == LL_PHY_UPDATE_IND_OPCODE
    struct ll_phy_update_ind            phy_upd_ind;

    //@trc_union op_code == LL_MIN_USED_CHANNELS_IND_OPCODE
    struct ll_min_used_channels_ind     min_used_channels_ind;

    //@trc_union op_code == LL_CLK_ACC_REQ_OPCODE
    struct ll_clk_acc_req        clk_acc_req;

    //@trc_union op_code == LL_CLK_ACC_RSP_OPCODE
    struct ll_clk_acc_rsp        clk_acc_rsp;

    //@trc_union op_code == LL_PER_SYNC_IND_OPCODE
    struct ll_per_sync_ind       per_sync_ind;

    //@trc_union op_code == LL_ICO_REQ_OPCODE
    struct  ll_ico_req                  ico_req;

    //@trc_union op_code == LL_ICO_RSP_OPCODE
    struct  ll_ico_rsp                  ico_rsp;

    //@trc_union op_code == LL_ICO_IND_OPCODE
    struct  ll_ico_ind                  ico_ind;

    //@trc_union op_code == LL_ICO_TERMINATE_IND_OPCODE
    struct  ll_ico_terminate_ind        ico_terminate_ind;
};


/// ICL_CHANNEL_MAP_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  icl_channel_map_ind
{
    /// op_code
    uint8_t            op_code;
    /// channel mapping
    struct le_chnl_map ch_map;
    /// ICL Event Count
    uint16_t           evt_cnt;
};

/// ICL_TERMINATE_IND structure.
/*@TRACE
 * @NO_PAD
*/
struct  icl_terminate_ind
{
    /// op_code
    uint8_t         op_code;
    /// termination code
    uint8_t         err_code;
};


/// ICL PDU format
/*@TRACE
 @trc_ref co_icl_op_code
 */
union icl_pdu
{
    /// op_code
    uint8_t  op_code;

    //@trc_union op_code == ICL_CHANNEL_MAP_IND_OPCODE
    struct icl_channel_map_ind channel_map_ind;

    //@trc_union op_code == ICL_TERMINATE_IND_OPCODE
    struct icl_terminate_ind   terminate_ind;
};

/// @} CO_BT
#endif // CO_LLCP_H_
