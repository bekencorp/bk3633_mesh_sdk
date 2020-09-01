#GDB_PATH=cmd /C  arm-none-eabi-gdb.exe 
set remotetimeout 20 
shell D:\YL_Work\BK3633\bk3633_aliOS\bk3633_mesh_sdk\build/cmd/win32/taskkill /F /IM openocd.exe 
shell start /B D:\YL_Work\BK3633\bk3633_aliOS\bk3633_mesh_sdk\build/OpenOCD/Win32/bin/openocd.exe -f .//build/OpenOCD/Win32/interface/jlink.cfg -f .//build/OpenOCD/Win32/bk3633/bk3633.cfg -f .//build/OpenOCD/Win32/bk3633/bk3633_gdb_jtag.cfg -l out/openocd.log 
