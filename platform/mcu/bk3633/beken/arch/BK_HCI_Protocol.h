/*************************************************************
 * File Name:     BK_HCI_Protocol.h
 * Author:        GuWenFu
 * Created Date:  @2015-11-19
 * Description:   Header file of BK_HCI_Protocol.c
 *
 * History:       2015-11-19 gwf    create this file
 */

#ifndef __BK_HCI_PROTOCOL_H__

#define __BK_HCI_PROTOCOL_H__

#include "uart2.h"



#define HCI_EVENT_HEAD_LENGTH       0x03
#define HCI_COMMAND_HEAD_LENGTH     0x04

/*
 * HCI transport type bytes
 */
enum {
    TRA_HCIT_COMMAND = 1,
    TRA_HCIT_ACLDATA = 2,
    TRA_HCIT_SCODATA = 3,
    TRA_HCIT_EVENT   = 4
};


#define VENDOR_SPECIFIC_DEBUG_OGF   0x3F
#define BEKEN_OCF                   0xE0
#define HCI_COMMAND_COMPLETE_EVENT  0x0E

enum {
    LINK_CHECK_CMD              = 0x00,
    REGISTER_WRITE_CMD          = 0x01,
    REGISTER_READ_CMD           = 0x03,

    TX_EVM_MACBYPASS            = 0xA0,
    TX_EVM_MAC                  = 0xA1,
    RX_SENSITIVITY              = 0xA2,
    
    MODULE_TEST_CMD             = 0xCC,
	  TEST_FIRMWARE_COPY          = 0x05,
	CHIP_RST_CMD                 = 0xFE,

		
} ;//BEKEN_CMD;

#define __PACKED_POST__  __attribute__((packed))

typedef struct {
    uint8_t code;             /**< 0x01: HCI Command Packet
                              0x02: HCI ACL Data Packet
                              0x03: HCI Synchronous Data Packet
                              0x04: HCI Event Packet */
    struct {
        uint16_t ocf : 10;    /**< OpCode Command Field */
        uint16_t ogf : 6;     /**< OpCode Group Field */
    } __PACKED_POST__ opcode;
    uint8_t total;
    uint8_t cmd;              /**< private command */
    uint8_t param[];
} __PACKED_POST__ HCI_COMMAND_PACKET;

typedef struct {
    uint8_t code;             /**< 0x01: HCI Command Packet
                              0x02: HCI ACL Data Packet
                              0x03: HCI Synchronous Data Packet
                              0x04: HCI Event Packet */
    uint8_t event;            /**< 0x00-0xFF: Each event is assigned a 1-Octet event code used to uniquely identify different types of events*/
    uint8_t total;            /**< Parameter Total Length */
    uint8_t param[];
} __PACKED_POST__ HCI_EVENT_PACKET;

typedef struct {
    uint32_t addr;
    uint32_t value;
} __PACKED_POST__ REGISTER_PARAM;

#endif      /* #ifndef __BK_HCI_PROTOCOL_H__ */
