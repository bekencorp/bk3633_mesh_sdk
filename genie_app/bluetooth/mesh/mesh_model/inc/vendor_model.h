//
// Created by ethan on 2019/7/18.
//

#ifndef _VENDOR_MODEL_H
#define _VENDOR_MODEL_H

#define VENDOR_OP_ATTR_GET_STATUS        0xCC
#define VENDOR_OP_ATTR_SET_ACK           0xC9
#define VENDOR_OP_ATTR_SET_UNACK         0xCA
#define VENDOR_OP_ATTR_STATUS            0xCB
#define VENDOR_OP_ATTR_INDICATE          0xD4
#define VENDOR_OP_ATTR_CONFIME           0xD5
#define VENDOR_OP_ATTR_INDICATE_TG       0xDE
#define VENDOR_OP_ATTR_CONFIME_TG        0xDF
#define VENDOR_OP_ATTR_TRANS_MSG         0xCF
#define VENDOR_OP_ATTR_TRANS_INDICATE    0xCE
#define VENDOR_OP_ATTR_TRANS_ACK         0xCD

#define VENDOR_OP_C7_INDICATE            0xC7

#define ONOFF_T           0x0100
#define LIGHTNESS_T       0x0121
#define TEMPERATURE_T     0x0122
#define DEVICE_EVENT_T    0xF009

/* Event List - Refer to Device Event List defined in https://yuque.antfin-inc.com/iotconnect/wwfb/dbyytw#1c38cf1b */
#define EL_FAULT_T        0x00 /* malfunction event */
#define EL_LOW_BAT_T      0x01 /* low battery event */
#define EL_DEV_UP_T       0x03 /* device power up */
#define EL_HW_RESET_T     0x23 /* hardware reset event */


#define VENDOR_MODEL_MSG_DFT_RETRY_TIMES 6
#define VENDOR_MODEL_MSG_MAX_RETRY_TIMES 10
#define VENDOR_MODEL_MSG_RETRY_PERIOD 400

/**
 * p_elem: pointer to the element which the messsage want to be sent to
 * retry: retry counts before desired confirm message received
 * * fill negative value if retransmission is not necessary
 * * fill positive value if retransmission is needed
 * * * will be round in this scope - [VENDOR_MODEL_MSG_DFT_RETRY_TIMES, VENDOR_MODEL_MSG_MAX_RETRY_TIMES]
 * retry_period: wait for retry_period before retransmit the message, in unit of ms
 * opid: hightest byte in Opcode defined in the vendor model spec designed by Alibaba IoT Group
 * * e.g. for Vendor Message Attr Get message, Opcode is 0xD001A8, corresponding opid is 0xD0
 * * refer to the marco named VENDOR_OP_ATTR_xxxx
 * tid:
 * * if the message is with type of VENDOR_OP_ATTR_CONFIME or VENDOR_OP_ATTR_CONFIME_TG,
 * * tid should be filled with the replied message's tid
 * len: payload length
 * data: pointer to the vendor message's payload
 * */
typedef struct _vnd_model_msg {
    struct bt_mesh_elem *p_elem;
    s8_t  retry;
    u16_t  retry_period;
    u8_t  opid;
    u8_t  tid;
    u16_t len;
    u8_t  *data;
} vnd_model_msg;

/** @def vendor_model_msg_send
 *
 *  @brief send vendor model message
 *
 *  @param pointer to the vendor_model_msg struct
 *
 *  @return if success, return 0; if fail, return fail reason
 */
s16_t vendor_model_msg_send(vnd_model_msg *p_model_msg);

#endif //_VENDOR_MODEL_H
