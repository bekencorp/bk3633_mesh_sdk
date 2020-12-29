#include "ota_port.h"


uint32_t crc32_table[256];

void CRC32_Init(CRC32_Context* contex)
{
    uint32_t c;
    int i = 0;
    int bit = 0;
    for (i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for (bit = 0; bit < 8; bit++)
        {
            if (c&1)
            {
                c = (c>>1)^(OTA_CRC32_INIT_PARA);
            }
            else
            {
                c = c >> 1;
            }
        }
        contex->crc_table[i] = c;

    }
    
    contex->crc = 0xFFFFFFFF;
}


uint32_t CRC32_Update(CRC32_Context* contex, uint8_t *f_data, uint32_t len)
{
    while (len--)
    {
        contex->crc = (contex->crc >> 8)^(contex->crc_table[(contex->crc^*f_data++)&0xff]);
    }
    return contex->crc;
}


