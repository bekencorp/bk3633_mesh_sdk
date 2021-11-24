#ifndef __APPLICATION_H
#define __APPLICATION_H

#define CONFIG_CID  0x01A8  //TAOBAO
#define CONFIG_CID  0x027d  //HILINK

#define LINK_CLOSE_SUCCESS   0x00
#define LINK_CLOSE_TIMEOUT   0x01
#define LINK_CLOSE_FAILED    0x02
#define PROVISIONER_SIZE_MAC     6
#define CONFIG_BT_MESH_PROVISIONER_CONFIG_PRIO  16
#define PROVISIONER_CONFIG_THREAD_STACK_SIZE    384     //512
#define PROVISIONER_DEFAULT_MAC "\x72\x62\x12\x4D\x23\xAB"  // 3AB7104D23AB

typedef enum {
    PROVISIONER_COMP_DATA_GET,
    PROVISIONER_APP_KEY_ADD,
} prvo_msg_type_t;

typedef struct provisioner_msg {
    prvo_msg_type_t msg_type;
    union {
        struct comp_get {
            u16_t unicast_addr;
            u16_t netkey_idx;            
        } comp_get_t;
    };
} provisioner_msg_t;
static provisioner_msg_t prov_msg;

static struct bt_mesh_comp comp;

static struct k_sem provisioner_config_sem;


void provisioner_init(void);

#endif  //__APPLICATION_H