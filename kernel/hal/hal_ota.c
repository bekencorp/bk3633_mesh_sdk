/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <hal/hal.h>

static hal_ota_module_t *ota_module = NULL;

hal_ota_module_t *hal_ota_get_default_module(void)
{
    return ota_module;
}

static void hal_ota_set_module(hal_ota_module_t* module)
{
    ota_module = module;
}

void hal_ota_register_module(hal_ota_module_t *module)
{
    hal_ota_module_t* m = hal_ota_get_default_module();

    if(m != NULL)
    {
        ;
    }

    hal_ota_set_module(module);
}

void hal_ota_unregister_module(void)
{
    hal_ota_module_t* m = hal_ota_get_default_module();

    if(m == NULL)
    {
        ;
    }

    hal_ota_set_module(NULL);
}

hal_stat_t hal_ota_init(void *something)
{
    hal_ota_module_t* m = hal_ota_get_default_module();

    if (m != NULL && m->init != NULL)
    {
        return m->init(something);
    }
	
	return 0;
}

hal_stat_t hal_ota_deinit(void *something)
{
    hal_ota_module_t* m = hal_ota_get_default_module();

    if (m != NULL && m->deinit != NULL)
    {
        m->deinit(something);
        hal_ota_unregister_module();
    }

    return 0;
}

hal_stat_t hal_ota_app_checksum(hal_ota_module_t *m, uint32_t app_checksum)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_tag_get != NULL) {
        return m->ota_app_checksum(app_checksum);
    }

    return 0;
}


hal_stat_t hal_ota_save(hal_ota_module_t *m, uint8_t *in_buf , uint32_t in_buf_len)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_save != NULL) {
        return m->ota_save(in_buf, in_buf_len);
    }

    return 0;
}

hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *out_buf, uint32_t out_buf_len)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_read != NULL) {
        return m->ota_read(off_set, out_buf, out_buf_len);
    }

    return 0;
}

hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_set_boot != NULL) {
        return m->ota_set_boot(something);
    }

    return 0;
}

hal_stat_t hal_ota_tag_check(hal_ota_module_t *m, uint32_t ota_type, uint16_t ver, uint16_t rom_ver, uint32_t size)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_tag_check != NULL) {
        return m->ota_tag_check(ota_type, ver, rom_ver, size);
    }

    return 0;
}

hal_stat_t hal_ota_tag_get(hal_ota_module_t *m, void *something)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_tag_get != NULL) {
        return m->ota_tag_get(something);
    }

    return 0;
}

hal_stat_t hal_ota_tag_init(hal_ota_module_t *m, uint16_t ver, uint16_t rom_ver)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_tag_init != NULL) {
        return m->ota_tag_init(ver, rom_ver);
    }

    return 0;
}

hal_stat_t hal_ota_mutex_init(hal_ota_module_t *m)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_app_mutex_init != NULL) {
        m->ota_app_mutex_init();
    }

    return 0;
}

