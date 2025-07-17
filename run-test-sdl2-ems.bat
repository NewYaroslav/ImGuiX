@echo off
setlocal EnableDelayedExpansion

:: Read emsdk and build path from configuration file
set EMSDK_CONF_FILE=%~dp0\emsdk-path.txt
if not exist "%EMSDK_CONF_FILE%" (
    echo emsdk-path.txt not found at %EMSDK_CONF_FILE%
    PAUSE
    exit /b
)

set /p EMSDK_PATH=<"%EMSDK_CONF_FILE%"
set /a __skip=0
for /f "skip=1 delims=" %%i in (%EMSDK_CONF_FILE%) do (
    if !__skip! EQU 0 (
        set BUILD_PATH=%%i
        set /a __skip=1
    )
)

if "%EMSDK_PATH%"=="" (
    echo EMSDK_PATH is empty.
    PAUSE
    exit /b
)

if "%BUILD_PATH%"=="" (
    echo BUILD_PATH is empty.
    PAUSE
    exit /b
)

:: Check if the path begins with a drive letter (e.g., C:) or with "\\"
echo Checking BUILD_PATH...

:: If the path does NOT start with "X:\" it's considered relative
echo %BUILD_PATH% | findstr /B /R "^[A-Za-z]:\\\|^\\\\">nul
if errorlevel 1 (
    echo BUILD_PATH is relative, prepending script path...
    set "BUILD_PATH=%~dp0%BUILD_PATH%"
)

:: Strip possible trailing slash
if "%BUILD_PATH:~-1%"=="\" set "BUILD_PATH=%BUILD_PATH:~0,-1%"

echo Using EMSDK_PATH: %EMSDK_PATH%
echo Using BUILD_PATH: %BUILD_PATH%

:: Activate emsdk
cd /d "%EMSDK_PATH%"
call emsdk_env.bat
cd /d "%BUILD_PATH%"

:: Run emrun
emrun --verbose --no_browser --port 8081 index.html > emrun_log.txt 2>&1
PAUSE
