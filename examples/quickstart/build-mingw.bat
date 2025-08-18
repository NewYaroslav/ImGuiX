@echo off
setlocal
set SDK=%~1
if "%SDK%"=="" echo Usage: build-mingw.bat ^<path-to-sdk^> & exit /b 1

cmake -S . -B build -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH="%SDK%" ^
  -DIMGUIX_SDK_DIR="%SDK%"
cmake --build build -j
