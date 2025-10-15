@echo off
setlocal enabledelayedexpansion

:: === ������Ϣ ===
echo [DEBUG] ��ʼִ��ʱ��: %date% %time% > "%~dp0merge_log.txt"
echo [DEBUG] ����: %* >> "%~dp0merge_log.txt"
echo [DEBUG] ��ǰĿ¼: %cd% >> "%~dp0merge_log.txt"
echo [DEBUG] �ű�·��: %~dp0 >> "%~dp0merge_log.txt"

cd /d "%~dp0" 2>> "%~dp0merge_log.txt"  :: �л��� .\Keil_project\tools\ Ŀ¼��
echo [DEBUG] �л���Ŀ¼: %cd% >> "%~dp0merge_log.txt"
::echo [%date% %time%] ��ʼִ��
::echo �ű�·��: %~dp0
::echo ��ǰĿ¼: %cd%

:: ʹ��WMIC��ȡ��׼ʱ��
for /f "tokens=2 delims==" %%a in ('wmic os get localdatetime /value 2^>nul') do set "datetime=%%a"
set "year=!datetime:~0,4!"
set "month=!datetime:~4,2!"
set "day=!datetime:~6,2!"
set "hour=!datetime:~8,2!"
set "minute=!datetime:~10,2!"

:: ���ù���·��������ڽű�Ŀ¼
set "SREC_PATH=%~dp0srec_cat.exe"

:: �����������·��������ڽű�Ŀ¼��

set "OUTPUT_DIR=%~dp0..\output\hex"   :: �ϼ�Ŀ¼��output�ļ���

set "BOOTLOADER_HEX=%~dp0..\output\bootloader\bootloader.hex"

:: ��ȡKeil���ݵ�HEX�ļ�·��������1��
set "APP_HEX=%~1"
echo [DEBUG] ���յ���APP_HEX: %APP_HEX% >> "%~dp0merge_log.txt"

:: ��֤�ļ�
if not exist "%SREC_PATH%" (
    echo [����] srec_cat���߲�����: %SREC_PATH% >> "%~dp0merge_log.txt"
    echo [����] srec_cat���߲�����: %SREC_PATH%
    exit /b 1
)

if not exist "%BOOTLOADER_HEX%" (
    echo [����] Bootloader�ļ�������: %BOOTLOADER_HEX% >> "%~dp0merge_log.txt"
    echo [����] Bootloader�ļ�������: %BOOTLOADER_HEX%
    exit /b 2
)

if not exist "%APP_HEX%" (
    echo [����] Ӧ�ù̼��ļ�������: %APP_HEX% >> "%~dp0merge_log.txt"
    echo [����] Ӧ�ù̼��ļ�������: %APP_HEX%
    exit /b 3
)

:: === ȷ�����Ŀ¼���� ===
if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%" 2>> "%~dp0merge_log.txt"
    if errorlevel 1 (
        echo [����] �޷��������Ŀ¼: %OUTPUT_DIR% >> "%~dp0merge_log.txt"
        echo [����] �޷��������Ŀ¼: %OUTPUT_DIR%
        exit /b 4
    )
)

:: ����ʱ����ļ��� 
set "TIMESTAMP=%year%_%month%_%day%_%hour%_%minute%"
set "OUTPUT_FILE=%OUTPUT_DIR%\hex_%TIMESTAMP%.hex"

echo [INFO] ���ںϲ��̼�... >> "%~dp0merge_log.txt"
echo [DEBUG] ����ļ�: %OUTPUT_FILE% >> "%~dp0merge_log.txt"

:: ִ�кϲ�
%SREC_PATH% %BOOTLOADER_HEX% -Intel %APP_HEX% -Intel -o %OUTPUT_FILE% -Intel

if %errorlevel% equ 0 (
    echo [�ɹ�] �̼��ϲ����: %MERGED_HEX% >> "%~dp0merge_log.txt"
    echo [�ɹ�] �̼��ϲ����: %MERGED_HEX%
) else (
    echo [����] �ϲ�ʧ��! �������: %errorlevel% >> "%~dp0merge_log.txt"
    echo [����] �ϲ�ʧ��! �������: %errorlevel%
    exit /b 5
)

:: === �����ɹ���� ===
echo SUCCESS > "%~dp0merge_success.txt"

endlocal

