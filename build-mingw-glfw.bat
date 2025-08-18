@echo off
setlocal
pushd "%~dp0" || (echo FAIL & pause & exit /b 1)

set "ROOT=%CD%"
set "BUILD=%ROOT%\build-mingw-glfw"
set "INSTALL=%ROOT%\build-mingw-glfw\dist\sdk-glfw"
set "LOG=%BUILD%\build.log"

if not exist "%BUILD%"   mkdir "%BUILD%"
if not exist "%INSTALL%" mkdir "%INSTALL%"
if exist "%LOG%" del "%LOG%"

REM --- CONFIGURE ---
echo [CONFIGURE]
cmake -S "%ROOT%" -B "%BUILD%" -G "MinGW Makefiles" ^
 -DCMAKE_BUILD_TYPE=Release ^
 -DCMAKE_INSTALL_PREFIX="%INSTALL%" ^
 -DBUILD_SHARED_LIBS=OFF ^
 -DIMGUIX_DEPS_MODE=BUNDLED ^
 -DIMGUIX_BUILD_SHARED=OFF ^
 -DIMGUIX_BUILD_TESTS=ON ^
 -DIMGUIX_BUILD_EXAMPLES=OFF ^
 -DIMGUIX_USE_SFML_BACKEND=OFF ^
 -DIMGUIX_USE_GLFW_BACKEND=ON ^
 -DIMGUIX_USE_SDL2_BACKEND=OFF ^
 -DIMGUIX_IMGUI_FREETYPE=ON ^
 -DIMGUIX_VENDOR_JSON=ON ^
 -DIMGUIX_SFML_MAJOR=3 >>"%LOG%" 2>&1 || goto :fail

REM --- BUILD ---
echo [BUILD]
cmake --build "%BUILD%" --parallel %NUMBER_OF_PROCESSORS% >>"%LOG%" 2>&1 || goto :fail

REM --- INSTALL ---
cmake --build "%BUILD%" --target install >>"%LOG%" 2>&1 || goto :fail

REM --- TESTS ---
REM ctest --test-dir "%BUILD%" --output-on-failure >>"%LOG%" 2>&1 || goto :fail

echo [OK] Stage: "%INSTALL%"
pause
exit /b 0

:fail
echo [ERROR] See log: "%LOG%"
type "%LOG%"
pause
exit /b 1
