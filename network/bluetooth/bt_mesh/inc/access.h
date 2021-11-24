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

#endif
