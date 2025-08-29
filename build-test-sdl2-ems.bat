@echo off
setlocal EnableDelayedExpansion

:: Get the directory of this script (assumed to be the ImGuiX folder)
set IMGUIX_PATH=%~dp0
:: Remove trailing slash if present
if "%IMGUIX_PATH:~-1%"=="\" set IMGUIX_PATH=%IMGUIX_PATH:~0,-1%

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

cd /d %EMSDK_PATH%
if errorlevel 1 exit /b %errorlevel%
call emsdk_env.bat
if errorlevel 1 exit /b %errorlevel%
cd /d %IMGUIX_PATH%

:: Create build directory
if not exist "%BUILD_PATH%" mkdir "%BUILD_PATH%"

echo Starting compilation...
emcc ^
    tests/demo_backend.cpp ^
    libs/imgui/imgui.cpp ^
    libs/imgui/imgui_draw.cpp ^
    libs/imgui/imgui_tables.cpp ^
    libs/imgui/imgui_widgets.cpp ^
    libs/imgui/imgui_demo.cpp ^
    libs/imgui/backends/imgui_impl_sdl2.cpp ^
    libs/imgui/backends/imgui_impl_opengl3.cpp ^
    libs/imgui/misc/freetype/imgui_freetype.cpp ^
    -DIMGUIX_USE_SDL2_BACKEND ^
    -DIMGUI_ENABLE_FREETYPE ^
    -Iinclude ^
    -Ilibs/imgui ^
    -Ilibs/imgui/backends ^
    -Ilibs/imgui/misc/freetype ^
    -s USE_SDL=2 ^
    -s USE_WEBGL2=1 ^
    -s USE_FREETYPE=1 ^
    -s WASM=1 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    --preload-file assets/data/resources/fonts/Roboto-Light.ttf@data/resources/fonts/Roboto-Light.ttf ^
    --shell-file assets/data/web/shell-fullscreen.html ^
    -o %BUILD_PATH%\index.html > %BUILD_PATH%\build_log.txt 2>&1
if errorlevel 1 (
    echo Compilation failed with error code %errorlevel%.
    PAUSE
    exit /b %errorlevel%
)

:: Display success message
echo Compilation successful! The build is located in %BUILD_PATH%.
echo You can now run the application in your browser.
PAUSE
