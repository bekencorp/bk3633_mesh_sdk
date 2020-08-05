.\BinConvert_3633_OAD_win -oad .\bk3633_boot.bin .\bk3633_controller.bin .\bk3633_ble_app.bin -m 0x2E00 -l 0x15B00 -v 0x1111 -rom_v 0x0005 -e 00000000 00000000 00000000 00000000
copy .\bk3633_ble_app_merge_crc.bin r:\
@echo off
SET /P ST=Press 'Enter' key to exit.
