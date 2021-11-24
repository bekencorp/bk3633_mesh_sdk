#GDB_PATH=cmd /C  D:\YL_Work\BK3633\bk3633_aliOS\alios_3633_full_git\bk3633_alios_jingxun_baotai_switch_sdk\bk3633_alios\build\compiler\gcc-arm-none-eabi\Win32\bin\arm-none-eabi-gdb.exe 
set remotetimeout 20 
shell D:\YL_Work\BK3633\bk3633_aliOS\alios_3633_full_git\bk3633_alios_jingxun_baotai_switch_sdk\bk3633_alios\build/cmd/win32/taskkill /F /IM openocd.exe 
shell start /B D:\YL_Work\BK3633\bk3633_aliOS\alios_3633_full_git\bk3633_alios_jingxun_baotai_switch_sdk\bk3633_alios\build/OpenOCD/Win32/bin/openocd.exe -f .//build/OpenOCD/Win32/interface/jlink.cfg -f .//build/OpenOCD/Win32/bk3633/bk3633.cfg -f .//build/OpenOCD/Win32/bk3633/bk3633_gdb_jtag.cfg -l out/openocd.log 
