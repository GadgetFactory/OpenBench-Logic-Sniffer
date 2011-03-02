cd %0\..
@if /I "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  echo Installing driver from 64-bit installer on 64-bit OS
  dpinstx64.exe
) else (
  if /I "%PROCESSOR_ARCHITECTURE%" == "X86" (
	  @if /I "%PROCESSOR_ARCHITEW6432%" == "AMD64" (
		echo Installing driver from 32-bit installer on 64-bit OS WOW64
		dpinstx64.exe 
	   ) else (
		echo Installing driver from 32-bit installer on 32-bit OS
		dpinstx86.exe /LM
	   )
  )
)