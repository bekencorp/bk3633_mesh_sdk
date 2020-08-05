AOS_SDK_MAKEFILES           		+= ./app/example/bluetooth/light/light.mk ./board/bk3633devkit/bk3633devkit.mk ././platform/mcu/bk3633/bk3633.mk ././osal/osal.mk ./kernel/init/init.mk ./out/bluetooth.light@bk3633devkit/auto_component/auto_component.mk ././genie_app/genie_app.mk ./network/bluetooth/bt_host/bt_host.mk ./network/bluetooth/bt_mesh/bt_mesh.mk ./kernel/yloop/yloop.mk ./tools/cli/cli.mk ./utility/libc/libc.mk ./kernel/rhino/rhino.mk ./kernel/hal/hal.mk ./kernel/rhino/pwrmgmt/pwrmgmt.mk ./kernel/rhino/fs/kv/kv.mk ././platform/arch/arm/armv5/armv5.mk ././platform/mcu/bk3633/hal_init/hal_init.mk ././platform/mcu/bk3633/aos/framework_runtime/framework_runtime.mk ././platform/mcu/bk3633/aos/app_runtime/app_runtime.mk ././platform/mcu/bk3633/beken/beken.mk ./utility/crc/crc.mk ./network/bluetooth/bt_dfu/bt_dfu.mk ./network/bluetooth/bt_host/bt_common/bt_common.mk ./network/bluetooth/bt_mesh/ref_impl/ref_impl.mk ./utility/log/log.mk ./kernel/rhino/vfs/vfs.mk ././kernel/rhino/vfs/device/device.mk
TOOLCHAIN_NAME            		:= GCC
AOS_SDK_LDFLAGS             		+= -Wl,--gc-sections -Wl,--cref -L .//board/bk3633devkit -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian --specs=nosys.specs -nostartfiles --specs=nano.specs -u _printf_float -T ./platform/mcu/bk3633/bk3633.ld -uframework_info -uapp_info
AOS_SDK_LDS_FILES                     += 
AOS_SDK_LDS_INCLUDES                  += 
RESOURCE_CFLAGS					+= -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -DSYSINFO_BUILD_TIME=\"20200805.1424\" -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"
AOS_SDK_LINK_SCRIPT         		+= 
AOS_SDK_LINK_SCRIPT_CMD    	 	+= 
AOS_SDK_PREBUILT_LIBRARIES 	 	+= 
AOS_SDK_CERTIFICATES       	 	+= 
AOS_SDK_PRE_APP_BUILDS      		+= 
AOS_SDK_LINK_FILES          		+=                                                        
AOS_SDK_INCLUDES           	 	+=                                                                        -I./app/example/bluetooth/light/../ -I./board/bk3633devkit/. -I././platform/mcu/bk3633/beken/arch -I././platform/mcu/bk3633/beken/arch/ll -I././platform/mcu/bk3633/beken/arch/compiler -I././platform/mcu/bk3633/beken/arch/include -I././platform/mcu/bk3633/beken/arch/config -I././platform/mcu/bk3633/beken/arch/app -I././platform/mcu/bk3633/beken/func -I././platform/mcu/bk3633/beken/func/include -I././platform/mcu/bk3633/beken/driver/include -I././platform/mcu/bk3633/modules/common/api -I././platform/mcu/bk3633/modules/common -I././osal/./mico/include -I././osal/./include -I./kernel/init/include -I./network/bluetooth/bt_host/include -I./network/bluetooth/bt_host/include/drivers -I./network/bluetooth/bt_mesh/./inc/ -I./network/bluetooth/bt_mesh/./inc/api -I./kernel/yloop/include -I./tools/cli/include -I./kernel/rhino/core/include -I./kernel/rhino/uspace/include -I./kernel/rhino/debug/include -I./kernel/rhino/hal/soc -I./kernel/rhino/./ -I./kernel/hal/include -I./kernel/hal/../rhino -I./kernel/hal/../rhino/hal/soc -I./kernel/hal/./include/hal/ -I./kernel/rhino/pwrmgmt/include -I./kernel/rhino/fs/kv/include -I././platform/arch/arm/armv5/./gcc -I././platform/mcu/bk3633/beken/plf/refip/src/arch -I././platform/mcu/bk3633/beken/plf/refip/src/arch/ll -I././platform/mcu/bk3633/beken/plf/refip/import/reg -I././platform/mcu/bk3633/beken/plf/rwip/api -I././platform/mcu/bk3633/beken/plf/rwip/import/reg -I././platform/mcu/bk3633/beken/plf/plf -I././platform/mcu/bk3633/beken/src/system -I././platform/mcu/bk3633/beken/plf/refip/src/arch/compiler -I././platform/mcu/bk3633/beken/modules/common/api -I././platform/mcu/bk3633/beken/modules/common -I././platform/mcu/bk3633/beken/ip/ble/ll/api -I./utility/crc/include -I./network/bluetooth/bt_dfu/./ -I./network/bluetooth/bt_host/bt_common/include -I./network/bluetooth/bt_host/bt_common/tinycrypt/include -I./network/bluetooth/bt_host/bt_common/port/include -I./network/bluetooth/bt_host/bt_common/../../bt_mesh/inc -I./network/bluetooth/bt_mesh/ref_impl/../bt_host/host/ -I./network/bluetooth/bt_mesh/ref_impl/../bt_host/include/ -I./utility/log/include -I./kernel/rhino/vfs/include -I.//network/include -I./app/example/bluetooth/light -I./kernel/hal/include/hal -I./kernel/hal/include/hal/soc
AOS_SDK_DEFINES             		+=                                                                             -DBUILD_BIN -DCONFIG_BLUETOOTH -DCONFIG_BT_TINYCRYPT_ECC -DCONFIG_BT_CONN -DCONFIG_BT_PERIPHERAL -DCONFIG_BT_HCI_VS_EXT -DCONFIG_BT_MESH -DCONFIG_BT_MESH_PROV -DCONFIG_BT_MESH_PB_ADV -DCONFIG_BT_MESH_PB_GATT -DCONFIG_BT_MESH_PROXY -DCONFIG_BT_MESH_GATT_PROXY -DCONFIG_BT_MESH_RELAY -DCONFIG_BT_MESH_CFG_SRV -DCONFIG_BT_MESH_HEALTH_SRV -DMESH_MODEL_HSL_SRV -DGENIE_DEBUG_COLOR -DMESH_DEBUG_RX -DMESH_DEBUG_TX -DMESH_DEBUG_PROV -DSTDIO_UART=1 -DBOARD_BK3633DEVKIT -DCONFIG_BT_CONTROLLER_NAME="BK3633" -DCONFIG_BT_WORK_QUEUE_STACK_SIZE=512-256 -DCONFIG_BT_MESH_ADV_BUF_COUNT=16 -DCONFIG_ARM -DCONFIG_NO_TCPIP -DCFG_SUPPORT_ALIOS=1 -DCONFIG_AOS_KV_MULTIPTN_MODE -DCONFIG_AOS_KV_PTN=6 -DCONFIG_AOS_KV_SECOND_PTN=7 -DCONFIG_AOS_KV_PTN_SIZE=4096 -DCONFIG_AOS_KV_BUFFER_SIZE=8192 -DCONFIG_AOS_UOTA_BREAKPOINT -DCONFIG_AOS_CLI_STACK_SIZE=512 -DCONFIG_AOS_CLI_BOARD -DBLE_4_2 -DOSAL_RHINO -DCONFIG_GENIE_CLI -DAPP_SDK_VERSION=107 -DAOS_BT -DCONFIG_BT_SMP -DCONFIG_BT_DEBUG_LOG -DCONFIG_BT_DEBUG -DCONFIG_MESH_MODEL_TRANS -DCONFIG_MESH_MODEL_GEN_ONOFF_SRV -DCONFIG_MESH_MODEL_LIGHTNESS_SRV -DCONFIG_MESH_MODEL_LIGHT_CMD -DCONFIG_MESH_MODEL_CTL_SRV -DCONFIG_MESH_MODEL_CTL_SETUP_SRV -DCONFIG_MESH_MODEL_HSL_SRV=1 -DCONFIG_MESH_MODEL_HSL_SETUP_SRV=1 -DCONFIG_MESH_MODEL_HSL_HUE_SRV=1 -DCONFIG_MESH_MODEL_HSL_SAT_SRV=1 -DCONFIG_MESH_MODEL_VENDOR_SRV -DCONFIG_ALI_SIMPLE_MODLE -DCRC16_ENABLED -DCONFIG_BT_MESH_DEBUG -DCONFIG_BT_MESH_DEBUG_ADV -DCONFIG_BT_MESH_DEBUG_BEACON -DCONFIG_BT_MESH_DEBUG_PROV -DCONFIG_BT_MESH_DEBUG_NET -DCONFIG_BT_MESH_DEBUG_MODEL -DCONFIG_BT_MESH_DEBUG_VENDOR_MODEL -DCONFIG_GENIE_DEBUG_CMD_FLASH -DAOS_LOOP -DAOS_HAL -DAOS_PWRMGMT -DAOS_KV -DENABLE_BLE_CONTROLLER -DCONFIG_BT_DFU -DCONFIG_BT_MESH_GATT_OTA -DAOS_VFS -DMCU_FAMILY=\"bk3633\"
COMPONENTS                		:= light board_bk3633 bk3633 osal kernel_init auto_component genie_app bt_host bt_mesh yloop cli newlib_stub rhino hal pwrmgmt kv armv5 hal_init framework_runtime app_runtime beken crc bt_dfu bt_common ref_impl log vfs vfs_device
PLATFORM_DIRECTORY        		:= bk3633devkit
APP_FULL                  		:= bluetooth/light
PLATFORM                  		:= bk3633devkit
HOST_MCU_FAMILY                  	:= bk3633
SUPPORT_BINS                          := 
APP                       		:= light
HOST_OPENOCD              		:= bk3633
JTAG              		        := jlink
HOST_ARCH                 		:= ARM968E-S
NO_BUILD_BOOTLOADER           	:= 
NO_BOOTLOADER_REQUIRED         	:= 
light_LOCATION         := ./app/example/bluetooth/light/
board_bk3633_LOCATION         := ./board/bk3633devkit/
bk3633_LOCATION         := ././platform/mcu/bk3633/
osal_LOCATION         := ././osal/
kernel_init_LOCATION         := ./kernel/init/
auto_component_LOCATION         := ./out/bluetooth.light@bk3633devkit/auto_component/
genie_app_LOCATION         := ././genie_app/
bt_host_LOCATION         := ./network/bluetooth/bt_host/
bt_mesh_LOCATION         := ./network/bluetooth/bt_mesh/
yloop_LOCATION         := ./kernel/yloop/
cli_LOCATION         := ./tools/cli/
newlib_stub_LOCATION         := ./utility/libc/
rhino_LOCATION         := ./kernel/rhino/
hal_LOCATION         := ./kernel/hal/
pwrmgmt_LOCATION         := ./kernel/rhino/pwrmgmt/
kv_LOCATION         := ./kernel/rhino/fs/kv/
armv5_LOCATION         := ././platform/arch/arm/armv5/
hal_init_LOCATION         := ././platform/mcu/bk3633/hal_init/
framework_runtime_LOCATION         := ././platform/mcu/bk3633/aos/framework_runtime/
app_runtime_LOCATION         := ././platform/mcu/bk3633/aos/app_runtime/
beken_LOCATION         := ././platform/mcu/bk3633/beken/
crc_LOCATION         := ./utility/crc/
bt_dfu_LOCATION         := ./network/bluetooth/bt_dfu/
bt_common_LOCATION         := ./network/bluetooth/bt_host/bt_common/
ref_impl_LOCATION         := ./network/bluetooth/bt_mesh/ref_impl/
log_LOCATION         := ./utility/log/
vfs_LOCATION         := ./kernel/rhino/vfs/
vfs_device_LOCATION         := ././kernel/rhino/vfs/device/
light_SOURCES          += light.c 
board_bk3633_SOURCES          += board.c 
bk3633_SOURCES          += aos/aos_main.c aos/soc_impl.c aos/trace_impl.c hal/beken_rhino.c hal/flash.c hal/gpio.c hal/hci_driver.c hal/hw.c hal/pwm.c hal/pwrmgmt_hal/board_cpu_pwr.c hal/pwrmgmt_hal/board_cpu_pwr_systick.c hal/pwrmgmt_hal/board_cpu_pwr_timer.c hal/uart.c hal/wdg.c port/ais_ota_port.c port/ali_dfu_port.c 
osal_SOURCES          += aos/aos_rhino.c 
kernel_init_SOURCES          += aos_init.c 
auto_component_SOURCES          +=  component_init.c testcase_register.c
genie_app_SOURCES          += base/genie_cmds.c base/genie_flash.c base/genie_reset.c base/tri_tuple.c genie_app.c genie_mesh.c 
bt_host_SOURCES          += host/att.c host/conn.c host/crypto.c host/gatt.c host/hci_core.c host/hci_ecc.c host/keys.c host/l2cap.c host/multi_adv.c host/rpa.c host/smp.c host/uuid.c 
bt_mesh_SOURCES          += ./src/access.c ./src/adv.c ./src/beacon.c ./src/cfg_cli.c ./src/cfg_srv.c ./src/crypto.c ./src/friend.c ./src/health_cli.c ./src/health_srv.c ./src/lpn.c ./src/main.c ./src/net.c ./src/prov.c ./src/provisioner_beacon.c ./src/provisioner_main.c ./src/provisioner_prov.c ./src/provisioner_proxy.c ./src/proxy.c ./src/shell.c ./src/transport.c mesh_model/src/gen_onoff_srv.c mesh_model/src/light_ctl_srv.c mesh_model/src/light_hsl_srv.c mesh_model/src/lightness_srv.c mesh_model/src/model_bind_ops.c mesh_model/src/vendor_model_srv.c 
yloop_SOURCES          += device.c local_event.c select.c yloop.c 
cli_SOURCES          += cli.c dumpsys.c 
newlib_stub_SOURCES          += newlib_stub.c 
rhino_SOURCES          += common/k_fifo.c common/k_trace.c core/k_buf_queue.c core/k_dyn_mem_proc.c core/k_err.c core/k_event.c core/k_idle.c core/k_mm.c core/k_mm_blk.c core/k_mm_debug.c core/k_mutex.c core/k_obj.c core/k_pend.c core/k_queue.c core/k_ringbuf.c core/k_sched.c core/k_sem.c core/k_stats.c core/k_sys.c core/k_task.c core/k_task_sem.c core/k_tick.c core/k_time.c core/k_timer.c core/k_workqueue.c debug/k_backtrace.c debug/k_infoget.c debug/k_overview.c debug/k_panic.c uspace/u_task.c 
hal_SOURCES          += ota.c wifi.c 
pwrmgmt_SOURCES          += cpu_pwr_hal_lib.c cpu_pwr_lib.c cpu_pwr_show.c cpu_tickless.c 
kv_SOURCES          += kv_osal.c kvmgr.c 
armv5_SOURCES          += gcc/port_c.c gcc/port_s.S panic/panic_c.c panic/panic_gcc.S 
hal_init_SOURCES          += hal_init.c 
framework_runtime_SOURCES          += framework_runtime.c 
app_runtime_SOURCES          += app_runtime.c 
beken_SOURCES          += arch/app/app.c arch/app/app_task.c arch/arch_main.c arch/mem_arch.c arch/str_arch.c driver/common/dd.c driver/common/drv_model.c driver/driver.c driver/flash/flash.c driver/gpio/gpio.c driver/icu/icu.c driver/intc/intc.c driver/pwm/pwm.c driver/spi/spi.c driver/uart/uart.c driver/wdt/wdt.c func/func.c func/misc/fake_clock.c func/power_save/mcu_ps.c modules/common/src/co_list.c modules/common/src/co_utils.c plf/refip/src/arch/ll/ll.S system/boot_handlers.S system/boot_vectors.S 
crc_SOURCES          += crc16.c crc32.c 
bt_dfu_SOURCES          += ./core/bt_dfu_plat.c ./core/bt_dfu_service.c ./core/bt_dfu_transport.c ./core/gatt_ota/src/ais_btmesh.c ./core/gatt_ota/src/ais_btmesh_service.c ./hal/ali_dfu.c ./hal/ali_dfu_port.c 
bt_common_SOURCES          += ./atomic_c.c ./buf.c ./event_scheduler.c ./log.c ./poll.c ./queue.c ./tinycrypt/source/aes_encrypt.c ./tinycrypt/source/cmac_mode.c ./tinycrypt/source/ecc.c ./tinycrypt/source/ecc_dh.c ./tinycrypt/source/hmac.c ./tinycrypt/source/hmac_prng.c ./tinycrypt/source/sha256.c ./tinycrypt/source/utils.c ./work.c port/aos_port.c 
ref_impl_SOURCES          += ./mesh_hal_ble.c ./mesh_hal_sec.c 
log_SOURCES          += log.c 
vfs_SOURCES          += vfs.c vfs_file.c vfs_inode.c vfs_register.c 
vfs_device_SOURCES          += vfs_adc.c vfs_gpio.c vfs_i2c.c vfs_pwm.c vfs_rtc.c vfs_spi.c vfs_uart.c vfs_wdg.c 
light_CHECK_HEADERS    += 
board_bk3633_CHECK_HEADERS    += 
bk3633_CHECK_HEADERS    += 
osal_CHECK_HEADERS    += 
kernel_init_CHECK_HEADERS    += 
auto_component_CHECK_HEADERS    += 
genie_app_CHECK_HEADERS    += 
bt_host_CHECK_HEADERS    += 
bt_mesh_CHECK_HEADERS    += 
yloop_CHECK_HEADERS    += 
cli_CHECK_HEADERS    += 
newlib_stub_CHECK_HEADERS    += 
rhino_CHECK_HEADERS    += 
hal_CHECK_HEADERS    += 
pwrmgmt_CHECK_HEADERS    += 
kv_CHECK_HEADERS    += 
armv5_CHECK_HEADERS    += 
hal_init_CHECK_HEADERS    += 
framework_runtime_CHECK_HEADERS    += 
app_runtime_CHECK_HEADERS    += 
beken_CHECK_HEADERS    += 
crc_CHECK_HEADERS    += 
bt_dfu_CHECK_HEADERS    += 
bt_common_CHECK_HEADERS    += 
ref_impl_CHECK_HEADERS    += 
log_CHECK_HEADERS    += 
vfs_CHECK_HEADERS    += 
vfs_device_CHECK_HEADERS    += 
light_INCLUDES         := -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/inc/ -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/inc/api -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/mesh_model -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/mesh_model/inc -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/vendor_model -I./app/example/bluetooth/light/../../../../genie_app -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/inc/ -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/inc/api -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/mesh_model -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/mesh_model/inc -I./app/example/bluetooth/light/../../../../network/bluetooth/bt_mesh/vendor_model -I./app/example/bluetooth/light/../../../../genie_app
board_bk3633_INCLUDES         := 
bk3633_INCLUDES         := -I././platform/mcu/bk3633/aos -I././platform/mcu/bk3633/aos -I././platform/mcu/bk3633/aos
osal_INCLUDES         := 
kernel_init_INCLUDES         := 
auto_component_INCLUDES         := 
genie_app_INCLUDES         := -I././genie_app/../network/bluetooth/bt_mesh/inc -I././genie_app/../network/bluetooth/bt_mesh/inc/api -I././genie_app/../network/bluetooth/bt_mesh/mesh_model -I././genie_app/../network/bluetooth/bt_mesh/vendor_model -I././genie_app/../genie_app -I././genie_app/../utility/crc -I././genie_app/../network/bluetooth/bt_dfu/inc -I././genie_app/../network/bluetooth/bt_mesh/inc -I././genie_app/../network/bluetooth/bt_mesh/inc/api -I././genie_app/../network/bluetooth/bt_mesh/mesh_model -I././genie_app/../network/bluetooth/bt_mesh/vendor_model -I././genie_app/../genie_app -I././genie_app/../utility/crc -I././genie_app/../network/bluetooth/bt_dfu/inc
bt_host_INCLUDES         := 
bt_mesh_INCLUDES         := -I./network/bluetooth/bt_mesh/./inc/ -I./network/bluetooth/bt_mesh/./inc/api/ -I./network/bluetooth/bt_mesh/./inc/api/mesh/ -I./network/bluetooth/bt_mesh/../../../genie_app/ -I./network/bluetooth/bt_mesh/mesh_model/ -I./network/bluetooth/bt_mesh/mesh_model/inc/ -I./network/bluetooth/bt_mesh/vendor_model/ -I./network/bluetooth/bt_mesh/./inc/ -I./network/bluetooth/bt_mesh/./inc/api/ -I./network/bluetooth/bt_mesh/./inc/api/mesh/ -I./network/bluetooth/bt_mesh/../../../genie_app/ -I./network/bluetooth/bt_mesh/mesh_model/ -I./network/bluetooth/bt_mesh/mesh_model/inc/ -I./network/bluetooth/bt_mesh/vendor_model/
yloop_INCLUDES         := 
cli_INCLUDES         := 
newlib_stub_INCLUDES         := 
rhino_INCLUDES         := 
hal_INCLUDES         := 
pwrmgmt_INCLUDES         := 
kv_INCLUDES         := 
armv5_INCLUDES         := 
hal_init_INCLUDES         := 
framework_runtime_INCLUDES         := 
app_runtime_INCLUDES         := 
beken_INCLUDES         := -I././platform/mcu/bk3633/beken/driver/icu -I././platform/mcu/bk3633/beken/driver/uart -I././platform/mcu/bk3633/beken/driver/sys_ctrl -I././platform/mcu/bk3633/beken/driver/gpio -I././platform/mcu/bk3633/beken/driver/intc -I././platform/mcu/bk3633/beken/driver/pwm -I././platform/mcu/bk3633/beken/driver/common -I././platform/mcu/bk3633/beken/driver/rf -I././platform/mcu/bk3633/beken/driver/wdt -I././platform/mcu/bk3633/beken/driver/dma -I././platform/mcu/bk3633/beken/driver/reg -I././platform/mcu/bk3633/beken/driver/aon_rtc -I././platform/mcu/bk3633/beken/modules/ke/api -I././platform/mcu/bk3633/beken/ip/ble/ll/src/llc -I././platform/mcu/bk3633/beken/ip/ble/ll/src/lld -I././platform/mcu/bk3633/beken/ip/ble/ll/src/llm -I././platform/mcu/bk3633/beken/ip/ble/ll/src -I././platform/mcu/bk3633/beken/ip/hci/api -I././platform/mcu/bk3633/beken/ip/em/api -I././platform/mcu/bk3633/beken/ip/ble/ll/import/reg -I././platform/mcu/bk3633/beken/ip/sch/import -I././platform/mcu/bk3633/beken/modules/ke/src -I././platform/mcu/bk3633/beken/ip/platform/driver/rf -I././platform/mcu/bk3633/beken/ip/platform/driver/reg
crc_INCLUDES         := 
bt_dfu_INCLUDES         := -I./network/bluetooth/bt_dfu/./inc/ -I./network/bluetooth/bt_dfu/./core/gatt_ota/inc/ -I./network/bluetooth/bt_dfu/../../kernel/rhino/ -I./network/bluetooth/bt_dfu/./core/ -I./network/bluetooth/bt_dfu/./inc/api/ -I./network/bluetooth/bt_dfu/./inc/api/mesh/ -I./network/bluetooth/bt_dfu/../../../genie_app/ -I./network/bluetooth/bt_dfu/../bt_mesh/mesh_model/ -I./network/bluetooth/bt_dfu/../bt_mesh/mesh_model/inc/ -I./network/bluetooth/bt_dfu/../bt_mesh/vendor_model/ -I./network/bluetooth/bt_dfu/../bt_mesh/inc/ -I./network/bluetooth/bt_dfu/./inc/ -I./network/bluetooth/bt_dfu/./core/gatt_ota/inc/ -I./network/bluetooth/bt_dfu/../../kernel/rhino/ -I./network/bluetooth/bt_dfu/./core/ -I./network/bluetooth/bt_dfu/./inc/api/ -I./network/bluetooth/bt_dfu/./inc/api/mesh/ -I./network/bluetooth/bt_dfu/../../../genie_app/ -I./network/bluetooth/bt_dfu/../bt_mesh/mesh_model/ -I./network/bluetooth/bt_dfu/../bt_mesh/mesh_model/inc/ -I./network/bluetooth/bt_dfu/../bt_mesh/vendor_model/ -I./network/bluetooth/bt_dfu/../bt_mesh/inc/
bt_common_INCLUDES         := -I./network/bluetooth/bt_host/bt_common/include
ref_impl_INCLUDES         := 
log_INCLUDES         := 
vfs_INCLUDES         := 
vfs_device_INCLUDES         := -I././kernel/rhino/vfs/device/../include/device/ -I././kernel/rhino/vfs/device/../include/ -I././kernel/rhino/vfs/device/../../hal/soc/ -I././kernel/rhino/vfs/device/../include/device/ -I././kernel/rhino/vfs/device/../include/ -I././kernel/rhino/vfs/device/../../hal/soc/
light_DEFINES          := 
board_bk3633_DEFINES          := 
bk3633_DEFINES          := 
osal_DEFINES          := 
kernel_init_DEFINES          := 
auto_component_DEFINES          := 
genie_app_DEFINES          := 
bt_host_DEFINES          := 
bt_mesh_DEFINES          := 
yloop_DEFINES          := 
cli_DEFINES          := 
newlib_stub_DEFINES          := 
rhino_DEFINES          := 
hal_DEFINES          := 
pwrmgmt_DEFINES          := 
kv_DEFINES          := 
armv5_DEFINES          := 
hal_init_DEFINES          := 
framework_runtime_DEFINES          := 
app_runtime_DEFINES          := 
beken_DEFINES          := 
crc_DEFINES          := 
bt_dfu_DEFINES          := 
bt_common_DEFINES          := 
ref_impl_DEFINES          := 
log_DEFINES          := 
vfs_DEFINES          := 
vfs_device_DEFINES          := 
light_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
board_bk3633_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bk3633_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-strict-aliasing -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-strict-aliasing -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-strict-aliasing
osal_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -marm -Wall -Werror -marm
kernel_init_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
auto_component_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
genie_app_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_host_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Wall -Wall
bt_mesh_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
yloop_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
cli_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
newlib_stub_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
rhino_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
hal_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
pwrmgmt_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
kv_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
armv5_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -marm -mthumb-interwork
hal_init_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
framework_runtime_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
app_runtime_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
beken_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
crc_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_dfu_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_common_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
ref_impl_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
log_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
vfs_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" -Wall -Werror -Wall -Werror
vfs_device_CFLAGS           :=  -DSYSINFO_OS_VERSION=\"\" -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_BK3633\" -DSYSINFO_DEVICE_NAME=\"BK3633\" -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w    -DSYSINFO_BUILD_TIME=\"20200805.1424\"      -DSYSINFO_KERNEL_VERSION=\"AOS-R-2.0.0\"                -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
light_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
board_bk3633_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bk3633_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
osal_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
kernel_init_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
auto_component_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
genie_app_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_host_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_mesh_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
yloop_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
cli_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
newlib_stub_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
rhino_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
hal_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
pwrmgmt_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
kv_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
armv5_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
hal_init_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
framework_runtime_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
app_runtime_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
beken_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
crc_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_dfu_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
bt_common_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
ref_impl_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
log_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
vfs_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
vfs_device_CXXFLAGS         :=                             -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions   -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/bluetooth.light@bk3633devkit/resources/ -DPLATFORM=\"bk3633devkit\" 
light_ASMFLAGS         :=                             -ggdb
board_bk3633_ASMFLAGS         :=                             -ggdb
bk3633_ASMFLAGS         :=                             -ggdb
osal_ASMFLAGS         :=                             -ggdb
kernel_init_ASMFLAGS         :=                             -ggdb
auto_component_ASMFLAGS         :=                             -ggdb
genie_app_ASMFLAGS         :=                             -ggdb
bt_host_ASMFLAGS         :=                             -ggdb
bt_mesh_ASMFLAGS         :=                             -ggdb
yloop_ASMFLAGS         :=                             -ggdb
cli_ASMFLAGS         :=                             -ggdb
newlib_stub_ASMFLAGS         :=                             -ggdb
rhino_ASMFLAGS         :=                             -ggdb
hal_ASMFLAGS         :=                             -ggdb
pwrmgmt_ASMFLAGS         :=                             -ggdb
kv_ASMFLAGS         :=                             -ggdb
armv5_ASMFLAGS         :=                             -ggdb
hal_init_ASMFLAGS         :=                             -ggdb
framework_runtime_ASMFLAGS         :=                             -ggdb
app_runtime_ASMFLAGS         :=                             -ggdb
beken_ASMFLAGS         :=                             -ggdb
crc_ASMFLAGS         :=                             -ggdb
bt_dfu_ASMFLAGS         :=                             -ggdb
bt_common_ASMFLAGS         :=                             -ggdb
ref_impl_ASMFLAGS         :=                             -ggdb
log_ASMFLAGS         :=                             -ggdb
vfs_ASMFLAGS         :=                             -ggdb
vfs_device_ASMFLAGS         :=                             -ggdb
light_RESOURCES        := 
board_bk3633_RESOURCES        := 
bk3633_RESOURCES        := 
osal_RESOURCES        := 
kernel_init_RESOURCES        := 
auto_component_RESOURCES        := 
genie_app_RESOURCES        := 
bt_host_RESOURCES        := 
bt_mesh_RESOURCES        := 
yloop_RESOURCES        := 
cli_RESOURCES        := 
newlib_stub_RESOURCES        := 
rhino_RESOURCES        := 
hal_RESOURCES        := 
pwrmgmt_RESOURCES        := 
kv_RESOURCES        := 
armv5_RESOURCES        := 
hal_init_RESOURCES        := 
framework_runtime_RESOURCES        := 
app_runtime_RESOURCES        := 
beken_RESOURCES        := 
crc_RESOURCES        := 
bt_dfu_RESOURCES        := 
bt_common_RESOURCES        := 
ref_impl_RESOURCES        := 
log_RESOURCES        := 
vfs_RESOURCES        := 
vfs_device_RESOURCES        := 
light_MAKEFILE         := ./app/example/bluetooth/light/light.mk
board_bk3633_MAKEFILE         := ./board/bk3633devkit/bk3633devkit.mk
bk3633_MAKEFILE         := ././platform/mcu/bk3633/bk3633.mk
osal_MAKEFILE         := ././osal/osal.mk
kernel_init_MAKEFILE         := ./kernel/init/init.mk
auto_component_MAKEFILE         := ./out/bluetooth.light@bk3633devkit/auto_component/auto_component.mk
genie_app_MAKEFILE         := ././genie_app/genie_app.mk
bt_host_MAKEFILE         := ./network/bluetooth/bt_host/bt_host.mk
bt_mesh_MAKEFILE         := ./network/bluetooth/bt_mesh/bt_mesh.mk
yloop_MAKEFILE         := ./kernel/yloop/yloop.mk
cli_MAKEFILE         := ./tools/cli/cli.mk
newlib_stub_MAKEFILE         := ./utility/libc/libc.mk
rhino_MAKEFILE         := ./kernel/rhino/rhino.mk
hal_MAKEFILE         := ./kernel/hal/hal.mk
pwrmgmt_MAKEFILE         := ./kernel/rhino/pwrmgmt/pwrmgmt.mk
kv_MAKEFILE         := ./kernel/rhino/fs/kv/kv.mk
armv5_MAKEFILE         := ././platform/arch/arm/armv5/armv5.mk
hal_init_MAKEFILE         := ././platform/mcu/bk3633/hal_init/hal_init.mk
framework_runtime_MAKEFILE         := ././platform/mcu/bk3633/aos/framework_runtime/framework_runtime.mk
app_runtime_MAKEFILE         := ././platform/mcu/bk3633/aos/app_runtime/app_runtime.mk
beken_MAKEFILE         := ././platform/mcu/bk3633/beken/beken.mk
crc_MAKEFILE         := ./utility/crc/crc.mk
bt_dfu_MAKEFILE         := ./network/bluetooth/bt_dfu/bt_dfu.mk
bt_common_MAKEFILE         := ./network/bluetooth/bt_host/bt_common/bt_common.mk
ref_impl_MAKEFILE         := ./network/bluetooth/bt_mesh/ref_impl/ref_impl.mk
log_MAKEFILE         := ./utility/log/log.mk
vfs_MAKEFILE         := ./kernel/rhino/vfs/vfs.mk
vfs_device_MAKEFILE         := ././kernel/rhino/vfs/device/device.mk
light_PRE_BUILD_TARGETS:= 
board_bk3633_PRE_BUILD_TARGETS:= 
bk3633_PRE_BUILD_TARGETS:= 
osal_PRE_BUILD_TARGETS:= 
kernel_init_PRE_BUILD_TARGETS:= 
auto_component_PRE_BUILD_TARGETS:= 
genie_app_PRE_BUILD_TARGETS:= 
bt_host_PRE_BUILD_TARGETS:= 
bt_mesh_PRE_BUILD_TARGETS:= 
yloop_PRE_BUILD_TARGETS:= 
cli_PRE_BUILD_TARGETS:= 
newlib_stub_PRE_BUILD_TARGETS:= 
rhino_PRE_BUILD_TARGETS:= 
hal_PRE_BUILD_TARGETS:= 
pwrmgmt_PRE_BUILD_TARGETS:= 
kv_PRE_BUILD_TARGETS:= 
armv5_PRE_BUILD_TARGETS:= 
hal_init_PRE_BUILD_TARGETS:= 
framework_runtime_PRE_BUILD_TARGETS:= 
app_runtime_PRE_BUILD_TARGETS:= 
beken_PRE_BUILD_TARGETS:= 
crc_PRE_BUILD_TARGETS:= 
bt_dfu_PRE_BUILD_TARGETS:= 
bt_common_PRE_BUILD_TARGETS:= 
ref_impl_PRE_BUILD_TARGETS:= 
log_PRE_BUILD_TARGETS:= 
vfs_PRE_BUILD_TARGETS:= 
vfs_device_PRE_BUILD_TARGETS:= 
light_PREBUILT_LIBRARY := 
board_bk3633_PREBUILT_LIBRARY := 
bk3633_PREBUILT_LIBRARY := 
osal_PREBUILT_LIBRARY := 
kernel_init_PREBUILT_LIBRARY := 
auto_component_PREBUILT_LIBRARY := 
genie_app_PREBUILT_LIBRARY := 
bt_host_PREBUILT_LIBRARY := 
bt_mesh_PREBUILT_LIBRARY := 
yloop_PREBUILT_LIBRARY := 
cli_PREBUILT_LIBRARY := 
newlib_stub_PREBUILT_LIBRARY := 
rhino_PREBUILT_LIBRARY := 
hal_PREBUILT_LIBRARY := 
pwrmgmt_PREBUILT_LIBRARY := 
kv_PREBUILT_LIBRARY := 
armv5_PREBUILT_LIBRARY := 
hal_init_PREBUILT_LIBRARY := 
framework_runtime_PREBUILT_LIBRARY := 
app_runtime_PREBUILT_LIBRARY := 
beken_PREBUILT_LIBRARY := 
crc_PREBUILT_LIBRARY := 
bt_dfu_PREBUILT_LIBRARY := 
bt_common_PREBUILT_LIBRARY := 
ref_impl_PREBUILT_LIBRARY := 
log_PREBUILT_LIBRARY := 
vfs_PREBUILT_LIBRARY := 
vfs_device_PREBUILT_LIBRARY := 
light_TYPE             := 
board_bk3633_TYPE             := 
bk3633_TYPE             := 
osal_TYPE             := 
kernel_init_TYPE             := kernel
auto_component_TYPE             := kernel
genie_app_TYPE             := 
bt_host_TYPE             := 
bt_mesh_TYPE             := 
yloop_TYPE             := kernel
cli_TYPE             := kernel
newlib_stub_TYPE             := share
rhino_TYPE             := kernel
hal_TYPE             := kernel
pwrmgmt_TYPE             := kernel
kv_TYPE             := 
armv5_TYPE             := kernel
hal_init_TYPE             := kernel
framework_runtime_TYPE             := framework
app_runtime_TYPE             := app
beken_TYPE             := kernel
crc_TYPE             := 
bt_dfu_TYPE             := 
bt_common_TYPE             := 
ref_impl_TYPE             := 
log_TYPE             := 
vfs_TYPE             := kernel
vfs_device_TYPE             := 
light_SELF_BUIlD_COMP_targets  := 
board_bk3633_SELF_BUIlD_COMP_targets  := 
bk3633_SELF_BUIlD_COMP_targets  := 
osal_SELF_BUIlD_COMP_targets  := 
kernel_init_SELF_BUIlD_COMP_targets  := 
auto_component_SELF_BUIlD_COMP_targets  := 
genie_app_SELF_BUIlD_COMP_targets  := 
bt_host_SELF_BUIlD_COMP_targets  := 
bt_mesh_SELF_BUIlD_COMP_targets  := 
yloop_SELF_BUIlD_COMP_targets  := 
cli_SELF_BUIlD_COMP_targets  := 
newlib_stub_SELF_BUIlD_COMP_targets  := 
rhino_SELF_BUIlD_COMP_targets  := 
hal_SELF_BUIlD_COMP_targets  := 
pwrmgmt_SELF_BUIlD_COMP_targets  := 
kv_SELF_BUIlD_COMP_targets  := 
armv5_SELF_BUIlD_COMP_targets  := 
hal_init_SELF_BUIlD_COMP_targets  := 
framework_runtime_SELF_BUIlD_COMP_targets  := 
app_runtime_SELF_BUIlD_COMP_targets  := 
beken_SELF_BUIlD_COMP_targets  := 
crc_SELF_BUIlD_COMP_targets  := 
bt_dfu_SELF_BUIlD_COMP_targets  := 
bt_common_SELF_BUIlD_COMP_targets  := 
ref_impl_SELF_BUIlD_COMP_targets  := 
log_SELF_BUIlD_COMP_targets  := 
vfs_SELF_BUIlD_COMP_targets  := 
vfs_device_SELF_BUIlD_COMP_targets  := 
light_SELF_BUIlD_COMP_scripts  := 
board_bk3633_SELF_BUIlD_COMP_scripts  := 
bk3633_SELF_BUIlD_COMP_scripts  := 
osal_SELF_BUIlD_COMP_scripts  := 
kernel_init_SELF_BUIlD_COMP_scripts  := 
auto_component_SELF_BUIlD_COMP_scripts  := 
genie_app_SELF_BUIlD_COMP_scripts  := 
bt_host_SELF_BUIlD_COMP_scripts  := 
bt_mesh_SELF_BUIlD_COMP_scripts  := 
yloop_SELF_BUIlD_COMP_scripts  := 
cli_SELF_BUIlD_COMP_scripts  := 
newlib_stub_SELF_BUIlD_COMP_scripts  := 
rhino_SELF_BUIlD_COMP_scripts  := 
hal_SELF_BUIlD_COMP_scripts  := 
pwrmgmt_SELF_BUIlD_COMP_scripts  := 
kv_SELF_BUIlD_COMP_scripts  := 
armv5_SELF_BUIlD_COMP_scripts  := 
hal_init_SELF_BUIlD_COMP_scripts  := 
framework_runtime_SELF_BUIlD_COMP_scripts  := 
app_runtime_SELF_BUIlD_COMP_scripts  := 
beken_SELF_BUIlD_COMP_scripts  := 
crc_SELF_BUIlD_COMP_scripts  := 
bt_dfu_SELF_BUIlD_COMP_scripts  := 
bt_common_SELF_BUIlD_COMP_scripts  := 
ref_impl_SELF_BUIlD_COMP_scripts  := 
log_SELF_BUIlD_COMP_scripts  := 
vfs_SELF_BUIlD_COMP_scripts  := 
vfs_device_SELF_BUIlD_COMP_scripts  := 
AOS_SDK_UNIT_TEST_SOURCES   		:=                                                        
ALL_RESOURCES             		:= 
INTERNAL_MEMORY_RESOURCES 		:= 
EXTRA_TARGET_MAKEFILES 			:=       .//build/aos_standard_targets.mk .//platform/mcu/bk3633/gen_ota_crc_bin.mk
APPS_START_SECTOR 				:=  
BOOTLOADER_FIRMWARE				:=  
ATE_FIRMWARE				        :=  
APPLICATION_FIRMWARE				:=  
PARAMETER_1_IMAGE					:=  
PARAMETER_2_IMAGE					:=  
FILESYSTEM_IMAGE					:=  
WIFI_FIRMWARE						:=  
BT_PATCH_FIRMWARE					:=  
AOS_ROM_SYMBOL_LIST_FILE 		:= 
AOS_SDK_CHIP_SPECIFIC_SCRIPT		:=                            
AOS_SDK_CONVERTER_OUTPUT_FILE	:=                            
AOS_SDK_FINAL_OUTPUT_FILE 		:=                            
AOS_RAM_STUB_LIST_FILE 			:= 
PING_PONG_OTA 					:= 0
