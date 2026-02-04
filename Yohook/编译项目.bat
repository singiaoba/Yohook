@echo off
chcp 65001 > nul
set LANG=zh_CN.UTF-8
set LC_ALL=zh_CN.UTF-8

pushd "%~1"
for %%i in (*.sln) do (
    call :BuildSolution "%%i"
)
echo.

popd
echo 构建完成
set /p dummy=运行结束，回车关闭窗口...
exit /b

:BuildSolution
set "SLN_FILE=%~1"

REM 检测sln文件中SolutionConfigurationPlatforms定义的32位平台
REM 检测 "Release|x86 =" 格式（解决方案级别的x86配置）
findstr "Release|x86" "%SLN_FILE%" > nul 2>&1
if %errorlevel%==0 (
    set "PLATFORM=x86"
) else (
    set "PLATFORM=Win32"
)

echo 正在构建: %SLN_FILE% [Platform=%PLATFORM%]
msbuild "%SLN_FILE%" /p:Configuration=Release /p:Platform=%PLATFORM% /t:Rebuild /v:m
exit /b