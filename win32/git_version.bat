@echo off
setlocal enabledelayedexpansion
set version=""
set tmp=""
set data=""
:: setlocal
set GIT_PATH=D:\Program Files\Git\bin
:: git.exe rev-list HEAD --timestamp --max-count=1
for /f "delims= " %%i in ('git.exe rev-list HEAD --timestamp --max-count=1') do (set version=%%i) 
echo %version%

if exist win32\revision.h ( Del win32\revision.h /q)
if exist revision.h ( Del revision.h /q)

set /a str_len=0
set /a ver_err=0
call:strlen %version% str_len
if %str_len% GTR 0 ( set /a ver_err=0)else ( set /a ver_err=1)

for /f "delims=" %%i in (revision-template.h) do (
	set data=%%i
	:: echo %%i | findstr /c:$WCREV$ && (set tmp=!data:$WCREV$=%version%!&echo !tmp! >> revision.h) || echo !data! >> revision.h
	echo %%i | findstr /c:$WCREV$ && (set tmp=!data:$WCREV$=%version%!&echo !tmp! >> revision.h) || (echo %%i | findstr /c:$VER_ERROR$ && (set tmp=!data:$VER_ERROR$=%ver_err%!&echo !tmp! >> revision.h) || echo !data! >> revision.h)
)

:: endlocal
pause
goto:eof

:strlen <stringVarName> [retvar] 
:: 思路： 二分回溯联合查表法
:: 说明： 所求字符串大小范围 0K ~ 8K；
::    stringVarName ---- 存放字符串的变量名
::    retvar      ---- 接收字符长度的变量名
setlocal enabledelayedexpansion
:: set "$=!%1!#"
set "$=%1#"
set N=&for %%a in (8 4 2 1)do if !$:~%%a!. NEQ . set/aN+=%%a&set $=!$:~%%a!
set $=!$!fedcba9876543210&set/aN+=0x!$:~16,1!
endlocal&If %2. neq . (set/a%2=%N%)else echo %N%
goto:eof