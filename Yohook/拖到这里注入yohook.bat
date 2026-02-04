@echo off
chcp 65001 > nul

set LANG=zh_CN.UTF-8
set LC_ALL=zh_CN.UTF-8

REM 获取脚本所在目录的绝对路径
set "SCRIPT_DIR=%~dp0"

REM 确保路径以反斜杠结尾
if not "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR%\"

REM 检查是否拖拽了文件
if "%~1"=="" (
    echo 请将exe文件拖拽到本脚本上
    pause
    exit /b 1
)

REM 获取拖拽文件的完整路径和基本信息
set "SOURCE_FILE=%~1"
set "SOURCE_NAME=%~n1"
set "SOURCE_EXT=%~x1"
set "SOURCE_DIR=%~dp1"

REM 在当前目录创建副本（用原文件名加_new后缀）
copy "%SOURCE_FILE%" "%SCRIPT_DIR%%SOURCE_NAME%%SOURCE_EXT%_new" >nul

REM 使用脚本目录的setdll32和dll进行处理
"%SCRIPT_DIR%EXE\setdll32" /d:".\yohook.dll" "%SCRIPT_DIR%%SOURCE_NAME%%SOURCE_EXT%_new"

REM 重命名处理后的文件（在脚本目录）
ren "%SCRIPT_DIR%%SOURCE_NAME%%SOURCE_EXT%_new" "%SOURCE_NAME%_chs%SOURCE_EXT%"


pause
