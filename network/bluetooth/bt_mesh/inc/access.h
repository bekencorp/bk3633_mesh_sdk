/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __MESH_ACCESS_H
#define __MESH_ACCESS_H


#define CONFIG_PROCESS_TIMER    (3000)

void bt_mesh_elem_register(struct bt_mesh_elem *elem, u8_t count);

u8_t bt_mesh_elem_count(void);

/* Find local element based on unicast or group address */
struct bt_mesh_elem *bt_mesh_elem_find(u16_t addr);

struct bt_mesh_model *bt_mesh_model_find_vnd(struct bt_mesh_elem *elem,
					     u16_t company, u16_t id);
struct bt_mesh_model *bt_mesh_model_find(struct bt_mesh_elem *elem,
					 u16_t id);

u16_t *bt_mesh_model_find_group(struct bt_mesh_model *mod, u16_t addr);

/** @brief Get whether the model is in the primary element of the device.
 *
 *  @param mod Mesh model.
 *
 *  @return true if the model is on the primary element, false otherwise.
 */
bool bt_mesh_model_in_primary(struct bt_mesh_model *mod);

bool bt_mesh_fixed_group_match(u16_t addr);

void bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod,
					struct bt_mesh_elem *elem,
					bool vnd, bool primary,
					void *user_data),
			   void *user_data);

s32_t bt_mesh_model_pub_period_get(struct bt_mesh_model *mod);

void bt_mesh_comp_provision(u16_t addr);
void bt_mesh_comp_unprovision(void);

u16_t bt_mesh_primary_addr(void);

const struct bt_mesh_comp *bt_mesh_comp_get(void);

void bt_mesh_model_recv(struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);

int bt_mesh_comp_register(const struct bt_mesh_comp *comp);

#define MESH_UTIL_LISTIFY(LEN, F, ...) UTIL_EVAL(UTIL_REPEAT(LEN, F, __VA_ARGS__))

/* Internal macros used to initialize array members */
#define BT_MESH_KEY_UNUSED_ELT_(IDX, _) BT_MESH_KEY_UNUSED,
#define BT_MESH_ADDR_UNASSIGNED_ELT_(IDX, _) BT_MESH_ADDR_UNASSIGNED,
#define BT_MESH_MODEL_KEYS_UNUSED			\
	{ MESH_UTIL_LISTIFY(CONFIG_BT_MESH_MODEL_KEY_COUNT,	\
		       BT_MESH_KEY_UNUSED_ELT_) }
#define BT_MESH_MODEL_GROUPS_UNASSIGNED				\
	{ MESH_UTIL_LISTIFY(CONFIG_BT_MESH_MODEL_GROUP_COUNT,	\
		       BT_MESH_ADDR_UNASSIGNED_ELT_) }


/** @def BT_MESH_MODEL_CB
 *
 *  @brief Composition data SIG model entry with callback functions.
 *
 *  @param _id        Model ID.
 *  @param _op        Array of model opcode handlers.
 *  @param _pub       Model publish parameters.
 *  @param _user_data User data for the model.
 *  @param _cb        Callback structure, or NULL to keep no callbacks.
 */
#define BT_MESH_MODEL_CB(_id, _op, _pub, _user_data, _cb)                    \
{                                                                            \
	.id = (_id),                                                         \
	.pub = _pub,                                                         \
	.keys = BT_MESH_MODEL_KEYS_UNUSED,                                   \
	.groups = BT_MESH_MODEL_GROUPS_UNASSIGNED,                           \
	.op = _op,                                                           \
	.cb = _cb,                                                           \
	.user_data = _user_data,                                             \
}
#endif
