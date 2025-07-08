@echo off
setlocal enabledelayedexpansion

set BUILD_DIR=build-msvc-glfw

echo [INFO] Searching for installed Visual Studio using vswhere...

:: Get the installation path of the latest Visual Studio instance
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set VS_PATH=%%i
)

if not defined VS_PATH (
    echo [ERROR] Visual Studio not found!
    exit /b 1
)

echo [INFO] Visual Studio found at: !VS_PATH!

:: Determine appropriate CMake generator based on VS path
set GEN=

if exist "!VS_PATH!\Common7\IDE\devenv.exe" (
    if exist "!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat" (
        rem Detect Visual Studio version from path
        echo !VS_PATH! | findstr /C:"2019" >nul && set GEN=Visual Studio 16 2019
        echo !VS_PATH! | findstr /C:"2022" >nul && set GEN=Visual Studio 17 2022
        echo !VS_PATH! | findstr /C:"2017" >nul && set GEN=Visual Studio 15 2017
    )
)

:: Fallback if version could not be determined
if not defined GEN (
    echo [WARN] Could not detect Visual Studio version, falling back to VS 2022
    set GEN=Visual Studio 17 2022
)

echo [INFO] Using CMake generator: !GEN!

:: CMake configuration
echo [INFO] Configuring project...
cmake -S . -B %BUILD_DIR% -G "!GEN!" -A x64 ^
  -DIMGUIX_BUILD_STATIC_LIB=OFF ^
  -DIMGUIX_USE_SFML_BACKEND=OFF ^
  -DIMGUIX_USE_GLFW_BACKEND=ON ^
  -DIMGUIX_USE_SDL2_BACKEND=OFF ^
  -DIMGUIX_BUILD_EXAMPLES=OFF ^
  -DIMGUIX_BUILD_TESTS=ON

:: Build step
echo [INFO] Building project...
cmake --build %BUILD_DIR% --config Release

pause
