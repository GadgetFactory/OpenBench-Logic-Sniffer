@echo off
set CYGWIN=nodosfilewarning

rem set PATH=%0\..winbin\;%PATH%

rem cd %0\..\bin
rem bash.exe %0\..\OLS_Upgrader.sh
%0\..\winbin\bash.exe ols-upgrader.sh

pause