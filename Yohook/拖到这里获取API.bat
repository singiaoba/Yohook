@echo off
chcp 65001 > nul

set LANG=zh_CN.UTF-8
set LC_ALL=zh_CN.UTF-8

set "SCRIPT_DIR=%~dp0"

"%SCRIPT_DIR%exe\apiconfig.exe" -ini "%SCRIPT_DIR%config\yohook.ini" -exe "%~1" -output "%SCRIPT_DIR%yohook.ini"

pause
