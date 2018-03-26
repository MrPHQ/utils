@echo off
SET VSCMD=%VS120COMNTOOLS%../IDE/devenv
if exist log.txt (
	Del log.txt /q
)
"%VSCMD%" "utils_vs2013.sln" /Rebuild release /Project utils "utils\utils.vcxproj" /Out log.txt
echo "copy file.........."
set file_name=""
set file_path=""
set des_path=E:/install_file/common
set bin_path=%~dp0../bin
rem echo %bin_path%
for /R %bin_path% %%i in (
	"*utils.dll"
	) do (
		set file_name=%%~nxi
		set file_path=%%i
		call:find_file
		)

pause
goto end

:find_file
rem echo %file_name%
rem echo %file_path%

if exist "%des_path%\%file_name%" (
	Del "%des_path%\%file_name%" /q
)
Xcopy "%file_path%" "%des_path%" /y /q
if '%errorlevel%' NEQ '0' ( 
	echo %des_path%\%file_name%
	echo "copy error.....1"
	pause 
) else ( 
	echo."%des_path%\%file_name% sucess"
	)
:end