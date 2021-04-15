/**
****************************************************************************************
*
* @file driver.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken Leonardo 2021
*
* $Rev: $
*
****************************************************************************************
*/

#include "drv_model_pub.h"



UINT32 soc_driver_init(void)
{    
    drv_model_init();

    intc_init();

    g_dd_init();

    return 0;
}

// eof
