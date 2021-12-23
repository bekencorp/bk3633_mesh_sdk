#include "bim_updataImage.h"
#include "bim_app.h"
#include "bim_uart2.h"
#include "bim_flash.h"
#include "bim_icu.h"
#include "bim_wdt.h"
const  unsigned int BK36 __attribute__((at(0x100)))= {0x36334B42};
const  unsigned int BK33 __attribute__((at(0x104)))= {0x00103333};
typedef void (*FUNCPTR)(void);

void  updata_memset32(void * dest, uint32 value, uint8 size)
{
    uint32 *_u8_dest = (uint32 *)dest;
    uint32 *_u8_end  = (uint32 *)dest+size*4;

    while (_u8_dest < _u8_end)
    {
        *_u8_dest++ = value;
    }

}

struct reset_register
{
    uint32_t cpsr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10; // sl stack limit
    uint32_t r11; // fp frame pointer
    uint32_t r12; // ip Intra-Procedure-call scratch register
    uint32_t r13; // sp Stack Pointer.
    uint32_t r14; // lr Link Register.
};

void reset_register_dump(void)
{
    struct reset_register *reg = (struct reset_register *)0x400020;
    bim_printf("\r\nr13: %x\n", reg->r13);
    bim_printf("\r\nR14(LR): %x\n", (reg->r14));
}

void bim_main(void)	
{
    //icu_init();
    wdt_disable();
    updata_memset32((uint8 *)0x00400000, 1, 1);
    //uart2_init(1000000);
    uart0_init(1000000);
	//reset_register_dump();
    bim_flash_init();

    if(1 == bim_select_sec())
    {
        // bim_printf("run stack, %x\r\n", PARTITION_STACK_CPU_ADDR);
        (*(FUNCPTR)PARTITION_STACK_CPU_ADDR)();
    }

    updata_memset32((uint8 *)0x00400000, 0, 1);
    
}





