#ifndef _BK3633_MESH_CONFIG_H_
#define _BK3633_MESH_CONFIG_H_

/* Generic options */
#define CONFIG_BT_MESH_MODEL_KEY_COUNT 2
#define CONFIG_BT_MESH_MODEL_GROUP_COUNT 4
#define CONFIG_BT_MESH_APP_KEY_COUNT 1
#define CONFIG_BT_MESH_SUBNET_COUNT 2
#define CONFIG_BT_MESH_CRPL 5
#define CONFIG_BT_MESH_ADV_BUF_COUNT 6
#define CONFIG_BT_MESH_LABEL_COUNT 1
#define CONFIG_BT_MESH_MSG_CACHE_SIZE 2
#define CONFIG_BT_MESH_TX_SEG_MSG_COUNT 2
#define CONFIG_BT_MESH_RX_SDU_MAX 36
#define CONFIG_BT_MESH_RX_SEG_MSG_COUNT 2
#define CONFIG_BT_MESH_ADV_PRIO 41
#define CONFIG_BT_MESH_PROXY_FILTER_SIZE 50
#define CONFIG_BT_MESH_NODE_ID_TIMEOUT 60

/* Friend related options */
#ifdef CONFIG_BT_MESH_FRIEND
#define CONFIG_BT_MESH_FRIEND_RECV_WIN 255
#define CONFIG_BT_MESH_FRIEND_QUEUE_SIZE 16
#define CONFIG_BT_MESH_FRIEND_SUB_LIST_SIZE 3
#define CONFIG_BT_MESH_FRIEND_LPN_COUNT 2
#define CONFIG_BT_MESH_FRIEND_SEG_RX 1
#endif // CONFIG_BT_MESH_FRIEND

/* Low power related options */
#ifdef CONFIG_BT_MESH_LOW_POWER
#define CONFIG_BT_MESH_LPN_ESTABLISHMENT
#define CONFIG_BT_MESH_LPN_AUTO
#define CONFIG_BT_MESH_LPN_AUTO_TIMEOUT 15
#define CONFIG_BT_MESH_LPN_RETRY_TIMEOUT 8
#define CONFIG_BT_MESH_LPN_RSSI_FACTOR 0
#define CONFIG_BT_MESH_LPN_RECV_WIN_FACTOR 0
#define CONFIG_BT_MESH_LPN_MIN_QUEUE_SIZE 1
#define CONFIG_BT_MESH_LPN_RECV_DELAY 100
#define CONFIG_BT_MESH_LPN_POLL_TIMEOUT 300
#define CONFIG_BT_MESH_LPN_INIT_POLL_TIMEOUT CONFIG_BT_MESH_LPN_POLL_TIMEOUT
#define CONFIG_BT_MESH_LPN_SCAN_LATENCY 10
#define CONFIG_BT_MESH_LPN_GROUPS 8
#endif // CONFIG_BT_MESH_LOW_POWER

/* Proxy related options */
#ifdef CONFIG_BT_MESH_PROXY
#define CONFIG_BT_MESH_PROXY_FILTER_SIZE 50
#endif // BT_MESH_PROXY

#endif
