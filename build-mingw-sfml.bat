@echo off
setlocal

set BUILD_DIR=build-mingw-sfml

echo [INFO] Configuring with MinGW Makefiles...
cmake -S . -B %BUILD_DIR% -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DIMGUIX_BUILD_STATIC_LIB=OFF ^
  -DIMGUIX_USE_SFML_BACKEND=ON ^
  -DIMGUIX_USE_GLFW_BACKEND=OFF ^
  -DIMGUIX_USE_SDL2_BACKEND=OFF ^
  -DIMGUIX_BUILD_EXAMPLES=OFF ^
  -DIMGUIX_BUILD_TESTS=ON

echo [INFO] Building...
cmake --build %BUILD_DIR% -j > build.log 2>&1

pause