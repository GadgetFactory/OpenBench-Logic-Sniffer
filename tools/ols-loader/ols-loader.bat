ols-loader.exe -erase -p:COM4 
pause
ols-loader.exe -write -p:COM4 -wB:logic_sniffer-2.12-newSPI.bit
rem ols-loader.exe -read -p:COM4 -rB:logic_sniffer_dump.bit
pause