@echo off
cd /d E:\_repoz\emsdk
call emsdk_env.bat
cd /d E:\_repoz\ImGuiX\build-emscripten-sdl2
emrun index.html
PAUSE
