/* buf.c - Buffer management */

/*
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_MODULE_NAME mesh_net_buf
#define LOG_LEVEL CONFIG_NET_BUF_LOG_LEVEL

#include "common/log.h"
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <misc/byteorder.h>

#include "buf.h"

#if defined(CONFIG_NET_BUF_LOG)
#define NET_BUF_DBG(fmt, ...) LOG_DBG("(%p) " fmt, k_current_get(), \
				      ##__VA_ARGS__)
#define NET_BUF_ERR(fmt, ...) LOG_ERR(fmt, ##__VA_ARGS__)
#define NET_BUF_WARN(fmt, ...) LOG_WRN(fmt, ##__VA_ARGS__)
#define NET_BUF_INFO(fmt, ...) LOG_INF(fmt, ##__VA_ARGS__)
#else

#define NET_BUF_DBG(fmt, ...)
#define NET_BUF_ERR(fmt, ...)
#define NET_BUF_WARN(fmt, ...)
#define NET_BUF_INFO(fmt, ...)
#endif /* CONFIG_NET_BUF_LOG */

#define NET_BUF_ASSERT(cond, ...) __ASSERT(cond, "" __VA_ARGS__)

#if CONFIG_NET_BUF_WARN_ALLOC_INTERVAL > 0
#define WARN_ALLOC_INTERVAL K_SECONDS(CONFIG_NET_BUF_WARN_ALLOC_INTERVAL)
#else
#define WARN_ALLOC_INTERVAL K_FOREVER
#endif

void mesh_net_buf_simple_init_with_data(struct mesh_net_buf_simple *buf,
				   void *data, size_t size)
{
	buf->__buf = data;
	buf->data  = data;
	buf->size  = size;
	buf->len   = size;
}

#define NET_BUF_SIMPLE_DBG(fmt, ...)

void *mesh_net_buf_simple_add(struct mesh_net_buf_simple *buf, size_t len)
{
	uint8_t *tail = mesh_net_buf_simple_tail(buf);

	NET_BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(mesh_net_buf_simple_tailroom(buf) >= len);

	buf->len += len;
	return tail;
}

void *mesh_net_buf_simple_add_mem(struct mesh_net_buf_simple *buf, const void *mem,
			     size_t len)
{
	NET_BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	return memcpy(mesh_net_buf_simple_add(buf, len), mem, len);
}

uint8_t *mesh_net_buf_simple_add_u8(struct mesh_net_buf_simple *buf, uint8_t val)
{
	uint8_t *u8;

	NET_BUF_SIMPLE_DBG("buf %p val 0x%02x", buf, val);

	u8 = mesh_net_buf_simple_add(buf, 1);
	*u8 = val;

	return u8;
}

void mesh_net_buf_simple_add_le16(struct mesh_net_buf_simple *buf, uint16_t val)
{
	NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le16(val, mesh_net_buf_simple_add(buf, sizeof(val)));
}

void mesh_net_buf_simple_add_le32(struct mesh_net_buf_simple *buf, uint32_t val)
{
	NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le32(val, mesh_net_buf_simple_add(buf, sizeof(val)));
}

void *mesh_net_buf_simple_pull(struct mesh_net_buf_simple *buf, size_t len)
{
	NET_BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(buf->len >= len);

	buf->len -= len;
	return buf->data += len;
}

uint8_t mesh_net_buf_simple_pull_u8(struct mesh_net_buf_simple *buf)
{
	uint8_t val;

	val = buf->data[0];
	mesh_net_buf_simple_pull(buf, 1);

	return val;
}

uint16_t mesh_net_buf_simple_pull_le16(struct mesh_net_buf_simple *buf)
{
	uint16_t val;

	val = UNALIGNED_GET((uint16_t *)buf->data);
	mesh_net_buf_simple_pull(buf, sizeof(val));

	return sys_le16_to_cpu(val);
}