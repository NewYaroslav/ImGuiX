@echo off
setlocal

cd /d E:\_repoz\emsdk
if errorlevel 1 exit /b %errorlevel%
call emsdk_env.bat
if errorlevel 1 exit /b %errorlevel%
cd /d E:\_repoz\ImGuiX

set BUILD_DIR=build-emscripten-sdl2
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

emcc tests/imgui-backend.cpp -DIMGUIX_USE_SDL2_BACKEND ^
    -Iinclude -Ilibs/imgui -Ilibs/imgui/backends ^
    -s USE_SDL=2 -s USE_WEBGL2=1 -s WASM=1 ^
    -o %BUILD_DIR%/imgui-backend.js > %BUILD_DIR%/build_log.txt 2>&1
if errorlevel 1 (
    echo Compilation failed with error code %errorlevel%.
    pause
    exit /b %errorlevel%
)

copy /Y emscripten-index.html %BUILD_DIR%\index.html >nul

pause
