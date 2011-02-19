@echo off
echo Please ensure the OLS is in update mode by holding down the update button and pressing reset. Both ACT and PWR LED's should be lit.
choice /C 123456789 /M "Choose the COM port that the OLS is connected to."
..\..\bin\pump-loader.bat COM%ERRORLEVEL% %0\..\logic_sniffer.mcs
pause