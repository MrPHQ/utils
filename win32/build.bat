@echo off
SET VSCMD=%VS120COMNTOOLS%../IDE/devenv
if exist log.txt (
	Del log.txt /q
)
"%VSCMD%" "utils_vs2013.sln" /Rebuild release /Project utils "utils\utils.vcxproj" /Out log.txt
pause