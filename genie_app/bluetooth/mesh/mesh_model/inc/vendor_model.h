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



#define VD_HW_ATTR_GET                  0xD0//网关→设备 网关查询设备状态
#define VD_HW_ATTR_SET                  0xD1//网关→设备 网关设置设备状态
#define VD_HW_ATTR_SET_NOACK            0xD2//网关→设备 网关设置设备状态 不需回复
#define VD_HW_ATTR_SET_STATUS           0xD3//设备→网关 设备回复网关的设置状态D1
#define VD_HW_ATTR_NOTIFY               0xD4//设备→网关 设备状态改变后的主动上报 需要回复
#define VD_HW_ATTR_NOTIFY_STATUS        0xD5//网关→设备 网关回复设备上报的状态改变消息
#define VD_HW_ATTR_NOTIFY_NOACK         0xD6//设备→网关 设备状态改变后的主动上报 不需回复
#define VD_HW_ATTR_GET_STATUS           0xD7//设备→网关 设备回复网关的get消息D7

#define ONOFF_T						0x0100
#define LIGHTNESS_T					0x0121
#define TEMPERATURE_T				0x0122

#define SECOND_ONOFF				0x0124//第二开关属性，用作灵动开关
#define THIRD_ONOFF					0x0125//第三开关属性，用作背光开关
#define DYMIC_SWITCH_ONOFF			SECOND_ONOFF//第二开关属性，用作灵动开关
#define BACKLIGHT_ONOFF				THIRD_ONOFF//第三开关属性，用作背光开关

#define DEVICE_EVENT_T				0xF009

#define ATTR_GET_ALL                (0xE001)//获取设备所有属性
#define ATTR_COLOR_MODE             (0xE002)//设备颜色模式
#define ATTR_VERSION                (0xE005)//软硬件版本信息

#define ATTR_HSL					(0x0123)

#define ATTR_RGB					(0XE004)

#define ATTR_SCENE					(0xF004)
#define ATTR_SCENE_LEN			    (4)

#define ATTR_EVENT                  (0xF009)
#define EVENT_ID_RESET              (0x23)//硬件复位事件

#define ATTR_EVENT_DOOR              (0xE101)
#define ATTR_EVENT_TAMPER            (0xE102)
#define ATTR_EVENT_SENSOR            (0xE103)
#define ATTR_EVENT_RESET             (0xE104)

/* Event List - Refer to Device Event List defined in https://yuque.antfin-inc.com/iotconnect/wwfb/dbyytw#1c38cf1b */
#define EL_FAULT_T        0x00 /* malfunction event */
#define EL_LOW_BAT_T      0x01 /* low battery event */
#define EL_DEV_UP_T       0x03 /* device power up */
#define EL_HW_RESET_T     0x23 /* hardware reset event */


#define VENDOR_MODEL_MSG_DFT_RETRY_TIMES	6
#define VENDOR_MODEL_MSG_MAX_RETRY_TIMES	10
#define VENDOR_MODEL_MSG_RETRY_PERIOD		400

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
