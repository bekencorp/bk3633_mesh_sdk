#include "RomCallFlash.h"
#include "co_bt.h"
#include "co_utils.h"
#include "rwip.h"


struct rwip_rf_api rwip_rf;
__attribute__((section("STACK_RAM"))) struct rom_env_tag rom_env;

/// Default BD address
struct bd_addr co_default_bdaddr={0x88, 0x77, 0x22, 0x24, 0x34, 0xbb};
//struct bd_addr co_default_bdaddr={0xaa, 0xbb, 0xcc, 0x00, 0x00, 0x01 };
//struct bd_addr co_default_bdaddr={0x78, 0xda, 0x07, 0x11, 0x27, 0xff };



