/**
 ****************************************************************************************
 *
 * @file rwble_config.h
 *
 * @brief Configuration of the BLE lower layer stack
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

#ifndef RWBLE_CONFIG_H_
#define RWBLE_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 * @name BLE LL stack configuration
 * @{
 ****************************************************************************************
 */
/******************************************************************************************/
/* --------------------------   FEATURE SETUP       --------------------------------------*/
/******************************************************************************************/

/// Features byte 0
#define BLE_FEATURES_BYTE0  (  (1 << BLE_FEAT_ENC                   ) \
                             | (1 << BLE_FEAT_CON_PARAM_REQ_PROC    ) \
                             | (1 << BLE_FEAT_EXT_REJ_IND           ) \
                             | (1 << BLE_FEAT_SLAVE_INIT_FEAT_EXCHG ) \
                             | (1 << BLE_FEAT_PING                  ) \
                             | (1 << BLE_FEAT_DATA_PKT_LEN_EXT      ) \
                             | (1 << BLE_FEAT_LL_PRIVACY            ) \
                             | (1 << BLE_FEAT_EXT_SCAN_FILT_POLICY  ) )

/// Features byte 1
#define BLE_FEATURES_BYTE1  (  (BLE_PHY_2MBPS_SUPPORT         << (BLE_FEAT_2M_PHY            - 8) ) \
                             | (BLE_STABLE_MOD_IDX_TX_SUPPORT << (BLE_FEAT_STABLE_MOD_IDX_TX - 8) ) \
                             | (BLE_STABLE_MOD_IDX_RX_SUPPORT << (BLE_FEAT_STABLE_MOD_IDX_RX - 8) ) \
                             | (BLE_PHY_CODED_SUPPORT         << (BLE_FEAT_CODED_PHY         - 8) ) \
                             | (1                             << (BLE_FEAT_EXT_ADV           - 8) ) \
                             | (1                             << (BLE_FEAT_PER_ADV           - 8) ) \
                             | (1                             << (BLE_FEAT_CHAN_SEL_ALGO_2   - 8) ) \
                             | (1                             << (BLE_FEAT_PWR_CLASS_1       - 8) ) )

/// Features byte 2
#define BLE_FEATURES_BYTE2  (  (1                                      << (BLE_FEAT_MIN_NUM_USED_CHAN_PROC - 16) ) \
                             | (BLE_CON_CTE_REQ                        << (BLE_FEAT_CON_CTE_REQ            - 16) ) \
                             | (BLE_CON_CTE_RSP                        << (BLE_FEAT_CON_CTE_RSP            - 16) ) \
                             | (BLE_CONLESS_CTE_TX                     << (BLE_FEAT_CONLESS_CTE_TX         - 16) ) \
                             | (BLE_CONLESS_CTE_RX                     << (BLE_FEAT_CONLESS_CTE_RX         - 16) ) \
                             | (BLE_AOD                                << (BLE_FEAT_AOD                    - 16) ) \
                             | (BLE_AOA                                << (BLE_FEAT_AOA                    - 16) ) \
                             | ((BLE_CON_CTE_REQ | BLE_CONLESS_CTE_RX) << (BLE_FEAT_CTE_RX                 - 16) ) )

/// Features byte 3
#define BLE_FEATURES_BYTE3  (  (1 << (BLE_FEAT_PER_ADV_SYNC_TRANSF_TX - 24) ) \
                             | (1 << (BLE_FEAT_PER_ADV_SYNC_TRANSF_RX - 24) ) \
                             | (1 << (BLE_FEAT_SLEEP_CLK_ACC_UPD - 24) ) )

/// Features byte 4
#define BLE_FEATURES_BYTE4  (0)

/// Features byte 5
#define BLE_FEATURES_BYTE5  (0)

/// Features byte 6
#define BLE_FEATURES_BYTE6  (0)

/// Features byte 7
#define BLE_FEATURES_BYTE7  (0)

/// States byte 0
#define BLE_STATES_BYTE0    ( BLE_NON_CON_ADV_STATE | BLE_DISC_ADV_STATE\
                            | BLE_CON_ADV_STATE | BLE_HDC_DIRECT_ADV_STATE\
                            | BLE_PASS_SCAN_STATE | BLE_ACTIV_SCAN_STATE\
                            | BLE_INIT_MASTER_STATE | BLE_CON_SLAVE_STATE)

/// States byte 1
#define BLE_STATES_BYTE1    ( BLE_NON_CON_ADV_PASS_SCAN_STATE | BLE_DISC_ADV_PASS_SCAN_STATE\
                            | BLE_CON_ADV_PASS_SCAN_STATE | BLE_HDC_DIRECT_ADV_PASS_SCAN_STATE\
                            | BLE_NON_CON_ADV_ACTIV_SCAN_STATE | BLE_DISC_ADV_ACTIV_SCAN_STATE\
                            | BLE_CON_ADV_ACTIV_SCAN_STATE | BLE_HDC_DIRECT_ADV_ACTIV_SCAN_STATE)

/// States byte 2
#define BLE_STATES_BYTE2    ( BLE_NON_CON_ADV_INIT_STATE | BLE_DISC_ADV_INIT_STATE\
                            | BLE_NON_CON_ADV_MASTER_STATE | BLE_DISC_ADV_MASTER_STATE\
                            | BLE_NON_CON_ADV_SLAVE_STATE | BLE_DISC_ADV_SLAVE_STATE\
                            | BLE_PASS_SCAN_INIT_STATE | BLE_ACTIV_SCAN_INIT_STATE)

/// States byte 3
#define BLE_STATES_BYTE3    ( BLE_PASS_SCAN_MASTER_STATE | BLE_ACTIV_SCAN_MASTER_STATE\
                            | BLE_PASS_SCAN_SLAVE_STATE | BLE_ACTIV_SCAN_SLAVE_STATE\
                            | BLE_INIT_MASTER_MASTER_STATE | BLE_LDC_DIRECT_ADV_STATE\
                            | BLE_LDC_DIRECT_ADV_PASS_SCAN_STATE | BLE_LDC_DIRECT_ADV_ACTIV_SCAN_STATE)

/// States byte 4
#define BLE_STATES_BYTE4    ( BLE_CON_ADV_INIT_MASTER_SLAVE_STATE | BLE_HDC_DIRECT_ADV_INIT_MASTER_SLAVE_STATE\
                            | BLE_LDC_DIRECT_ADV_INIT_MASTER_SLAVE_STATE | BLE_CON_ADV_MASTER_SLAVE_STATE\
                            | BLE_HDC_DIRECT_ADV_MASTER_SLAVE_STATE | BLE_LDC_DIRECT_ADV_MASTER_SLAVE_STATE\
                            | BLE_CON_ADV_SLAVE_SLAVE_STATE | BLE_HDC_DIRECT_ADV_SLAVE_SLAVE_STATE)

/// States byte 5
#define BLE_STATES_BYTE5    ( BLE_LDC_DIRECT_ADV_SLAVE_SLAVE_STATE |  BLE_INIT_MASTER_SLAVE_STATE)

/// States byte 6
#define BLE_STATES_BYTE6    0x0
/// States byte 7
#define BLE_STATES_BYTE7    0x0


/******************************************************************************************/
/* --------------------------   GENERAL SETUP       --------------------------------------*/
/******************************************************************************************/

/************************************
 * KE message heap                  *
 ************************************
 *
 * LLD->LLC/LLM messages:
 *      - connection      < 6
 *      - advertising     adv end 56
 *      - scan            max adv chain 1650
 *      - initiating      init end 56
 *      - per adv tx      2
 *      - per adv rx      max adv chain 1650
 *      - encryption      16
 *      => 1650 + 16 + (N-1) x 56
 *
 * HCI params stored by LLM:
 *      - advertising     26
 *      - scan            16
 *      - initiating      58
 *      - per adv tx      8
 *      - per adv rx      14
 *      => 58 + (N-1) x 26
 *
 * LLCP TX queue:
 *      - Max 3 LLCP in the queue per link
 *      - Average 10 bytes per PDU
 *      - N x 3 x (12 + 10) = N x 66
 *
 * HCI commands reception:
 *      - N commands of max 255 bytes
 *      - N x 255
 *
 * HCI events:
 *      - Max advertising reports in TX queue: N x 255
 *      - + 1 100-bytes event per activity: N x 100
 *
 * Kernel timers:
 *     - 1 timer per connection
 *     - N x 12
 *
 * Due to high probability of fragmentation, and sizes of the Kernel message headers, the total of above additions is
 * doubled.
 */

#define BLE_HEAP_MSG_SIZE  (   1650*BLE_OBSERVER                                               \
                             + 2 * (  (16 + (BLE_ACTIVITY_MAX-1) * 56)                         \
                                    + (58 + (BLE_ACTIVITY_MAX-1) * 26)                         \
                                    + (BLE_ACTIVITY_MAX * 66)                                  \
                                    + (HCI_NB_CMD_PKTS * 255)                                  \
                                    + (BLE_OBSERVER * BLE_MAX_NB_ADV_REP_FRAG * 255 + BLE_ACTIVITY_MAX * 100) \
                                    + (BLE_ACTIVITY_MAX * 12)                                  \
                                   ) )

/************************************
 * Environments heap                *
 ************************************
 *
 * Max env usage per activity:
 *    - connection     84 (LLC) + 144          228
 *    - advertising    140                     140
 *    - scan           32 + 2 x 108 + 16       264
 *    - initiating     84 + 3 x 80             324
 *    - per adv tx     80                      80
 *    - per adv rx     96                      96
 *
 * It is possible to have one initiating + 1 scan + (N-2) other activities.
 * Add 2 activities to consider Heap fragmentation.
 *
 * Heap size = 330 + 270 + (N-2 x 230) + 2 x 230 = 600 + N x 230
 */
#define BLE_HEAP_ENV_SIZE               (330*BLE_CENTRAL + 270*BLE_OBSERVER + BLE_ACTIVITY_MAX * 230)

/// Normal Rx window size (in us)
#define BLE_NORMAL_WIN_SIZE              14
/// Normal Rx window size for coded phy (in us)
// In Long Range, the symbol for the Access Address is 8 times longer than the symbol for 1Mbps.
#define BLE_PHY_CODED_NORMAL_WIN_SIZE   (BLE_NORMAL_WIN_SIZE * 8)


/// Number of devices in the white list
#define BLE_WHITELIST_MAX           (BLE_ACTIVITY_MAX + 2)

/// Number of devices in the Resolution Address List
/// This have to be tuned according to the core frequency. Worst case is having in scan mode
/// all IRK and valid in resolving list and device receive a Direct Adv Report that contains
/// RPAs for InitA and AdvA
#define BLE_RESOL_ADDR_LIST_MAX     (BLE_RAL_MAX)

/// Number of RX data buffers (common for all activities)
#define BLE_DATA_BUF_NB_RX           (BLE_RX_DESC_NB)
/// Guard space reserved for HCI ACL header in BLE RX buffers
#define BLE_ACL_RX_BUF_HEADER_SPACE  (HCI_ACL_HDR_LEN + 1)

/// Number of TX ACL data buffers
#define BLE_ACL_BUF_NB_TX            (BLE_ACTIVITY_MAX + 2)

/// Number of advertising data buffers
#define BLE_ADV_BUF_NB_TX            (BLE_ACTIVITY_MAX + 2) // sean add 2 2020.02.28
/// Number of advertising or scan response data fragments in extended advertising PDU chain
#define BLE_ADV_FRAG_NB_TX           (1)
/// Size of advertising or scan response data fragments in extended advertising PDU chain
#define BLE_ADV_FRAG_SIZE_TX         (254)
/// Maximum advertising data length
#define BLE_MAX_ADV_DATA_LEN         (BLE_ADV_FRAG_NB_TX * BLE_ADV_FRAG_SIZE_TX)

/// Data packet transmission size and duration
/// These values represent what the device supports
#define BLE_MIN_OCTETS  (27) // number of octets
#define BLE_MIN_TIME    (328) // in us
#define BLE_MAX_OCTETS  (251) // number of octets
#define BLE_MAX_TIME    (17040) // in us

/// Number of devices capacity for the scan duplicate filtering
#if (BLE_OBSERVER)
#define BLE_DUPLICATE_FILTER_MAX    (10)
#endif //(BLE_OBSERVER)

/// Number of TX descriptors per BLE connection [2|3|4]
#define BLE_NB_TX_DESC_PER_CON          2

/// Number of TX descriptors per advertising set
#define BLE_NB_TX_DESC_PER_ADV          (2+BLE_ADV_FRAG_NB_TX)

/// Number of TX descriptors per BLE activity (maximum needed for connection or advertising)
#if (BLE_NB_TX_DESC_PER_CON > BLE_NB_TX_DESC_PER_ADV)
#define BLE_NB_TX_DESC_PER_ACT          (BLE_NB_TX_DESC_PER_CON)
#else //(BLE_NB_TX_DESC_PER_CON > BLE_NB_TX_DESC_PER_ADV)
#define BLE_NB_TX_DESC_PER_ACT          (BLE_NB_TX_DESC_PER_ADV)
#endif //(BLE_NB_TX_DESC_PER_CON > BLE_NB_TX_DESC_PER_ADV)

/// Number of RX descriptors
#define BLE_RX_DESC_NB               (6)
/// Number of TX descriptors
#define BLE_TX_DESC_NB               (BLE_NB_TX_DESC_PER_ACT * BLE_ACTIVITY_MAX)

/// Legacy advertising HCI interface
#define BLE_ADV_LEGACY_ITF           (HCI_TL_SUPPORT)

/// Low energy mask (should be a 4.0 value if device is on controller only)
#if ((BLE_EMB_PRESENT) && (!BLE_HOST_PRESENT))
#define BLE_EVT_MASK                {0x1C, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#else // (!(BLE_EMB_PRESENT) || (BLE_HOST_PRESENT))
#define BLE_EVT_MASK                {0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00}
#endif // ((BLE_EMB_PRESENT) && (!BLE_HOST_PRESENT))

/******************************************************************************************/
/* --------------------------   DEBUG SETUP       ----------------------------------------*/
/******************************************************************************************/

#define BLE_TEST_MODE_SUPPORT       (1)

/******************************************************************************************/
/* --------------------        CHANNEL ASSESSMENT SETUP         --------------------------*/
/******************************************************************************************/

/// RSSI threshold for interference detection (in dBm)
#define BLE_CH_ASSESS_RSSI_INTERF_THR   (-70)

/// Channel map update period (in sec)
#define BLE_CH_MAP_UPDATE_PERIOD         4

/// Maximum duration without receiving a packet before considering a channel quality unknown  (in slots) (2 secs)
#define BLE_CH_ASSESS_VALID_TO       0x1900

/// Maximum duration without receiving a packet before reintroducing a channel to the map (in slots) (20 secs)
#define BLE_CH_REASSESS_TO           0x7D00

/// Maximum counter value for channel assessment
#define BLE_CH_ASSESS_COUNT_MAX      4
/// Counter threshold to consider a channel good
#define BLE_CH_ASSESS_COUNT_THR_GOOD 2
/// Counter threshold to consider a channel bad
#define BLE_CH_ASSESS_COUNT_THR_BAD  -2
/// Minimum counter value for channel assessment
#define BLE_CH_ASSESS_COUNT_MIN      -4


/******************************************************************************************/
/* --------------------------   Direction finding (AoA/AoD)      -------------------------*/
/******************************************************************************************/

/// Number of antennae used for constant tone extension transmission or reception
#define BLE_ANTENNA_NB         4

/// Number of RX descriptors used for constant tone extension reception
#define BLE_RX_CTE_DESC_NB     BLE_RX_DESC_NB


/******************************************************************************************/
/* -----------------------   SUPPORTED HCI COMMANDS       --------------------------------*/
/******************************************************************************************/

//byte0
#define BLE_CMDS_BYTE0      BLE_DISC_CMD
//byte2
#define BLE_CMDS_BYTE2      BLE_RD_REM_VERS_CMD
//byte5
#define BLE_CMDS_BYTE5      (BLE_SET_EVT_MSK_CMD | BLE_RESET_CMD)
//byte10
#define BLE_CMDS_BYTE10     (BLE_HL_NB_CMP_PKT_CMD | BLE_RD_TX_PWR_CMD\
                            |BLE_HL_BUF_SIZE_CMD | BLE_SET_CTRL_TO_HL_FCTRL_CMD)
//byte14
#define BLE_CMDS_BYTE14     (BLE_RD_LOC_VERS_CMD | BLE_RD_LOC_SUP_FEAT_CMD)
//byte15
#define BLE_CMDS_BYTE15     (BLE_RD_BD_ADDR_CMD | BLE_RD_RSSI_CMD)
//byte22
#define BLE_CMDS_BYTE22     (BLE_SET_EVT_MSK_PG2_CMD)
//byte25
#define BLE_CMDS_BYTE25     (BLE_LE_SET_EVT_MSK_CMD | BLE_LE_RD_BUF_SIZE_CMD\
                            |BLE_LE_RD_LOC_SUP_FEAT_CMD | BLE_LE_SET_RAND_ADDR_CMD\
                            |BLE_LE_SET_ADV_PARAM_CMD | BLE_LE_RD_ADV_TX_PWR_CMD\
                            |BLE_LE_SET_ADV_DATA_CMD)
//byte26
#define BLE_CMDS_BYTE26     (BLE_LE_SET_SC_RSP_DATA_CMD | BLE_LE_SET_ADV_EN_CMD\
                            |BLE_LE_SET_SC_PARAM_CMD | BLE_LE_SET_SC_EN_CMD\
                            |BLE_LE_CREAT_CNX_CMD | BLE_LE_CREAT_CNX_CNL_CMD\
                            |BLE_LE_RD_WL_SIZE_CMD | BLE_LE_CLEAR_WL_CMD)
//byte27
#define BLE_CMDS_BYTE27     (BLE_LE_ADD_DEV_WL_CMD | BLE_LE_REM_DEV_WL_CMD\
                            |BLE_LE_CNX_UPDATE_CMD | BLE_LE_SET_HL_CH_CLASS_CMD\
                            |BLE_LE_RD_CH_MAP_CMD | BLE_LE_RD_REM_FEAT_CMD\
                            |BLE_LE_ENCRYPT_CMD | BLE_LE_RAND_CMD)
//byte28
#define BLE_CMDS_BYTE28     (BLE_LE_START_ENC_CMD | BLE_LE_LTK_REQ_RPLY_CMD\
                            |BLE_LE_LTK_REQ_NEG_RPLY_CMD | BLE_LE_RD_SUPP_STATES_CMD\
                            |BLE_LE_RX_TEST_V1_CMD | BLE_LE_TX_TEST_V1_CMD\
                            |BLE_LE_STOP_TEST_CMD)
//byte32
#define BLE_CMDS_BYTE32     (BLE_RD_AUTH_PAYL_TO_CMD | BLE_WR_AUTH_PAYL_TO_CMD)
//byte33
#define BLE_CMDS_BYTE33     (BLE_LE_REM_CON_PARA_REQ_RPLY_CMD | BLE_LE_REM_CON_PARA_REQ_NEG_RPLY_CMD\
                            | BLE_LE_SET_DATA_LEN_CMD | BLE_LE_RD_SUGGTED_DFT_DATA_LEN_CMD)
//byte34
#define BLE_CMDS_BYTE34     (  BLE_LE_WR_SUGGTED_DFT_DATA_LEN_CMD   \
                             | BLE_LE_RD_LOC_P256_PUB_KEY_CMD       \
                             | BLE_LE_GEN_DHKEY_V1_CMD              \
                             | BLE_LE_ADD_DEV_TO_RESOLV_LIST_CMD    \
                             | BLE_LE_REM_DEV_FROM_RESOLV_LIST_CMD  \
                             | BLE_LE_CLEAR_RESOLV_LIST_CMD         \
                             | BLE_LE_RD_RESOLV_LIST_SIZE_CMD       \
                             | BLE_LE_RD_PEER_RESOLV_ADDR_CMD        )
//byte35
#define BLE_CMDS_BYTE35     (  BLE_LE_RD_LOCAL_RESOLV_ADDR_CMD      \
                             | BLE_LE_SET_ADDR_RESOL_CMD            \
                             | BLE_LE_SET_RESOLV_PRIV_ADDR_TO_CMD   \
                             | BLE_LE_RD_MAX_DATA_LEN_CMD           \
                             | BLE_LE_RD_PHY_CMD                    \
                             | BLE_LE_SET_DFT_PHY_CMD               \
                             | BLE_LE_SET_PHY_CMD                   \
                             | BLE_LE_RX_TEST_V2_CMD                 )
//byte36
#define BLE_CMDS_BYTE36     (  BLE_LE_TX_TEST_V2_CMD                \
                             | BLE_LE_SET_ADV_SET_RAND_ADDR_CMD     \
                             | BLE_LE_SET_EXT_ADV_PARAM_CMD         \
                             | BLE_LE_SET_EXT_ADV_DATA_CMD          \
                             | BLE_LE_SET_EXT_SCAN_RSP_DATA_CMD     \
                             | BLE_LE_SET_EXT_ADV_EN_CMD            \
                             | BLE_LE_RD_MAX_ADV_DATA_LEN_CMD       \
                             | BLE_LE_RD_NB_SUPP_ADV_SETS_CMD        )
//byte37
#define BLE_CMDS_BYTE37     (  BLE_LE_RMV_ADV_SET_CMD               \
                             | BLE_LE_CLEAR_ADV_SETS_CMD            \
                             | BLE_LE_SET_PER_ADV_PARAM_CMD         \
                             | BLE_LE_SET_PER_ADV_DATA_CMD          \
                             | BLE_LE_SET_PER_ADV_EN_CMD            \
                             | BLE_LE_SET_EXT_SCAN_PARAM_CMD        \
                             | BLE_LE_SET_EXT_SCAN_EN_CMD           \
                             | BLE_LE_EXT_CREATE_CON_CMD             )
//byte38
#define BLE_CMDS_BYTE38     (  BLE_LE_PER_ADV_CREATE_SYNC_CMD        \
                             | BLE_LE_PER_ADV_CREATE_SYNC_CANCEL_CMD \
                             | BLE_LE_PER_ADV_TERM_SYNC_CMD          \
                             | BLE_LE_ADD_DEV_TO_PER_ADV_LIST_CMD    \
                             | BLE_LE_RMV_DEV_FROM_PER_ADV_LIST_CMD  \
                             | BLE_LE_CLEAR_PER_ADV_LIST_CMD         \
                             | BLE_LE_RD_PER_ADV_LIST_SIZE_CMD       \
                             | BLE_LE_RD_TX_PWR_CMD                   )
//byte39
#define BLE_CMDS_BYTE39     (  BLE_LE_RD_RF_PATH_COMP_CMD            \
                             | BLE_LE_WR_RF_PATH_COMP_CMD            \
                             | BLE_LE_SET_PRIV_MODE_CMD              \
                             | BLE_LE_RX_TEST_V3_CMD                 \
                             | BLE_LE_TX_TEST_V3_CMD                 \
                             | BLE_LE_SET_CONLESS_CTE_TX_PARAM_CMD   \
                             | BLE_LE_SET_CONLESS_CTE_TX_EN_CMD      \
                             | BLE_LE_SET_CONLESS_IQ_SAMPL_EN_CMD     )
//byte40
#define BLE_CMDS_BYTE40     (  BLE_LE_SET_CON_CTE_RX_PARAM_CMD       \
                             | BLE_LE_SET_CON_CTE_TX_PARAM_CMD       \
                             | BLE_LE_CON_CTE_REQ_EN_CMD             \
                             | BLE_LE_CON_CTE_RSP_EN_CMD             \
                             | BLE_LE_RD_ANTENNA_INF_CMD             \
                             | BLE_LE_SET_PER_ADV_RX_EN_CMD          \
                             | BLE_LE_PER_ADV_SYNC_TRANSF_CMD        \
                             | BLE_LE_PER_ADV_SET_INFO_TRANSF_CMD     )
//byte41
#define BLE_CMDS_BYTE41     (  BLE_LE_SET_PER_ADV_SYNC_TRANSF_PARAM_CMD        \
                             | BLE_LE_SET_DFT_PER_ADV_SYNC_TRANSF_PARAM_CMD    \
                             | BLE_LE_GEN_DHKEY_V2_CMD                         \
                             | BLE_LE_MOD_SLP_CLK_ACC_CMD                       )


/******************************************************************************************/
/* ------------------------   CONNECTION HANDLE   ----------------------------------------*/
/******************************************************************************************/

/// Bit set for BLE connection handles
#define BLE_CONHDL_MIN                 (0)
#define BLE_CONHDL_MAX                 (BLE_CONHDL_MIN + BLE_ACTIVITY_MAX - 1)
#define BLE_LINKID_TO_CONHDL(link_id)  ((uint16_t) (BLE_CONHDL_MIN + (link_id)))
#define BLE_CONHDL_TO_LINKID(conhdl)   ((uint8_t) ((conhdl) - BLE_CONHDL_MIN))

/// Bit set for BLE sync handles (used in periodic advertising receiver mode)
#define BLE_SYNCHDL_MIN                (0)
#define BLE_SYNCHDL_MAX                (BLE_SYNCHDL_MIN + BLE_ACTIVITY_MAX - 1)
#define BLE_ACTID_TO_SYNCHDL(act_id)   ((uint16_t) (BLE_SYNCHDL_MIN + (act_id)))
#define BLE_SYNCHDL_TO_ACTID(synchdl)  ((uint8_t) ((synchdl) - BLE_SYNCHDL_MIN))

/// AM0 Channel handle mapping
#define BLE_AM0HDL_MIN                  (0x300)
#define BLE_CHANHDL_TO_AM0HDL(chanhdl)  ((uint16_t) (BLE_AM0HDL_MIN + (chanhdl)))
#define BLE_AM0HDL_TO_CHANHDL(am0)      ((uint8_t) ((am0) - BLE_AM0HDL_MIN))

/// @} BLE stack configuration
/// @} ROOT

#endif // RWBLE_CONFIG_H_
