D:\Software\Keil_v5\ARM\ARMCC\bin\fromelf .\obj\bk3633_boot.axf --output .\output\bk3633_boot.bin --bin
.\output\encrypt.exe .\output\bk3633_boot.bin 00000000
copy .\output\bk3633_boot.bin ..\..\..\bin\
start python boot_rewrite.py