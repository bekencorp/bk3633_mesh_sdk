#include "include.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <k_api.h>

#include "co_utils.h"
#include "ll.h"

INT32 os_memcmp(const void *s1, const void *s2, UINT32 n)
{
    return memcmp(s1, s2, (unsigned int)n);
}

void *os_memmove(void *out, const void *in, UINT32 n)
{
    return memmove(out, in, n);
}

void *os_memcpy(void *out, const void *in, UINT32 n)
{
    return memcpy(out, in, n);
}

void *os_memset(void *b, int c, UINT32 len)
{
    return (void *)memset(b, c, (unsigned int)len);
}

void *os_realloc(void *ptr, size_t size)
{
	return (void *)aos_realloc(ptr, size);
}

int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const u8 *aa = a;
	const u8 *bb = b;
	size_t i;
	u8 res;

	for (res = 0, i = 0; i < len; i++)
        res |= aa[i] ^ bb[i];

    return res;
}

void *os_malloc(size_t size)
{
	return (void *)aos_malloc(size);
}

void * os_zalloc(size_t size)
{
	return (void *)aos_zalloc(size);
}

void os_free(void *ptr)
{
	if(ptr)
	{
		aos_free(ptr);
	}
}

/**  **/
uint16_t dbg_flow_tag = 0;

void ke_mem_show_info(uint16_t at, uint16_t cnt)
{

	///show mem info
	static uint32_t show_timestamp = 0;
	int32_t  time = CLK_DIFF(show_timestamp, ea_time_get_slot_rounded());
	if(time > 0) {
		printf("--- flow @%d, tag:%d, cnt:%d\r\n", at, dbg_flow_tag, cnt);
		show_timestamp = CLK_ADD_2(show_timestamp,((time/1600)+1)*1600);
	}

}

#ifdef ALIOS_MEM
void ke_mem_init(uint8_t type, uint8_t* heap, uint16_t heap_size)
{
}

void *ke_malloc(uint32_t size, uint8_t type)
{
	return krhino_mm_alloc(size);
}

ke_check_malloc(uint32_t size, uint8_t type)
{
	return 0;
}

void ke_free(void *mem_ptr)
{
	krhino_mm_free(mem_ptr);
}

bool ke_mem_is_empty(uint8_t type)
{
	return 0;
}

bool ke_is_free(void* mem_ptr)
{
	return (MM_GET_THIS_BLK(mem_ptr)->buf_size & RHINO_MM_FREE);
}
#endif

// EOF
