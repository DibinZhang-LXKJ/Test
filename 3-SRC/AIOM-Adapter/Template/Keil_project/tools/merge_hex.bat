@echo off
setlocal enabledelayedexpansion

:: === 调试信息 ===
echo [DEBUG] 开始执行时间: %date% %time% > "%~dp0merge_log.txt"
echo [DEBUG] 参数: %* >> "%~dp0merge_log.txt"
echo [DEBUG] 当前目录: %cd% >> "%~dp0merge_log.txt"
echo [DEBUG] 脚本路径: %~dp0 >> "%~dp0merge_log.txt"

cd /d "%~dp0" 2>> "%~dp0merge_log.txt"  :: 切换到 .\Keil_project\tools\ 目录下
echo [DEBUG] 切换后目录: %cd% >> "%~dp0merge_log.txt"
::echo [%date% %time%] 开始执行
::echo 脚本路径: %~dp0
::echo 当前目录: %cd%

:: 使用WMIC获取标准时间
for /f "tokens=2 delims==" %%a in ('wmic os get localdatetime /value 2^>nul') do set "datetime=%%a"
set "year=!datetime:~0,4!"
set "month=!datetime:~4,2!"
set "day=!datetime:~6,2!"
set "hour=!datetime:~8,2!"
set "minute=!datetime:~10,2!"

:: 配置工具路径（相对于脚本目录
set "SREC_PATH=%~dp0srec_cat.exe"

:: 设置输入输出路径（相对于脚本目录）

set "OUTPUT_DIR=%~dp0..\output\hex"   :: 上级目录的output文件夹

set "BOOTLOADER_HEX=%~dp0..\output\bootloader\bootloader.hex"

:: 获取Keil传递的HEX文件路径（参数1）
set "APP_HEX=%~1"
echo [DEBUG] 接收到的APP_HEX: %APP_HEX% >> "%~dp0merge_log.txt"

:: 验证文件
if not exist "%SREC_PATH%" (
    echo [错误] srec_cat工具不存在: %SREC_PATH% >> "%~dp0merge_log.txt"
    echo [错误] srec_cat工具不存在: %SREC_PATH%
    exit /b 1
)

if not exist "%BOOTLOADER_HEX%" (
    echo [错误] Bootloader文件不存在: %BOOTLOADER_HEX% >> "%~dp0merge_log.txt"
    echo [错误] Bootloader文件不存在: %BOOTLOADER_HEX%
    exit /b 2
)

if not exist "%APP_HEX%" (
    echo [错误] 应用固件文件不存在: %APP_HEX% >> "%~dp0merge_log.txt"
    echo [错误] 应用固件文件不存在: %APP_HEX%
    exit /b 3
)

:: === 确保输出目录存在 ===
if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%" 2>> "%~dp0merge_log.txt"
    if errorlevel 1 (
        echo [错误] 无法创建输出目录: %OUTPUT_DIR% >> "%~dp0merge_log.txt"
        echo [错误] 无法创建输出目录: %OUTPUT_DIR%
        exit /b 4
    )
)

:: 创建时间戳文件名 
set "TIMESTAMP=%year%_%month%_%day%_%hour%_%minute%"
set "OUTPUT_FILE=%OUTPUT_DIR%\hex_%TIMESTAMP%.hex"

echo [INFO] 正在合并固件... >> "%~dp0merge_log.txt"
echo [DEBUG] 输出文件: %OUTPUT_FILE% >> "%~dp0merge_log.txt"

:: 执行合并
%SREC_PATH% %BOOTLOADER_HEX% -Intel %APP_HEX% -Intel -o %OUTPUT_FILE% -Intel

if %errorlevel% equ 0 (
    echo [成功] 固件合并完成: %MERGED_HEX% >> "%~dp0merge_log.txt"
    echo [成功] 固件合并完成: %MERGED_HEX%
) else (
    echo [错误] 合并失败! 错误代码: %errorlevel% >> "%~dp0merge_log.txt"
    echo [错误] 合并失败! 错误代码: %errorlevel%
    exit /b 5
)

:: === 创建成功标记 ===
echo SUCCESS > "%~dp0merge_success.txt"

endlocal

