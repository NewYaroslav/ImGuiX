cmake_minimum_required(VERSION 3.18)
project(imguix_framed_showcase LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== examples/framed_showcase/CMakeLists.txt =====
# Purpose: Build the framed showcase sample using the ImGuiX SDK.
# Inputs:  IMGUIX_SDK_DIR, CMAKE_PREFIX_PATH
# Outputs: executable imguix_framed_showcase
# Notes:   Serves as a template for SDK consumers.

# Specify SDK prefix via CMAKE_PREFIX_PATH:
#   cmake -S . -B build -DCMAKE_PREFIX_PATH=".../sdk"
find_package(ImGuiX REQUIRED)

add_executable(imguix_framed_showcase main.cpp)
target_link_libraries(imguix_framed_showcase PRIVATE ImGuiX::imguix)

if(IMGUIX_USE_SFML_BACKEND)
    target_compile_definitions(imguix_framed_showcase PRIVATE IMGUIX_USE_SFML_BACKEND)
elseif(IMGUIX_USE_GLFW_BACKEND)
    target_compile_definitions(imguix_framed_showcase PRIVATE IMGUIX_USE_GLFW_BACKEND)
elseif(IMGUIX_USE_SDL2_BACKEND)
    target_compile_definitions(imguix_framed_showcase PRIVATE IMGUIX_USE_SDL2_BACKEND)
endif()

# ImGuiX links SFML/ImGui-SFML privately; add SDK includes/libs here.
# Option 1: pass path explicitly at configure time:
#   cmake -S . -B build -DIMGUIX_SDK_DIR=".../sdk"
if(DEFINED IMGUIX_SDK_DIR)
    target_include_directories(imguix_framed_showcase PRIVATE "${IMGUIX_SDK_DIR}/include")
    target_link_directories(imguix_framed_showcase PRIVATE "${IMGUIX_SDK_DIR}/lib")
endif()

# Option 2: derive prefix from found package (works with standard layout)
if(NOT DEFINED IMGUIX_SDK_DIR AND DEFINED ImGuiX_DIR)
    get_filename_component(_imx_prefix "${ImGuiX_DIR}/../.." ABSOLUTE)
    target_include_directories(imguix_framed_showcase PRIVATE "${_imx_prefix}/include")
    target_link_directories(imguix_framed_showcase PRIVATE "${_imx_prefix}/lib")
endif()

# MinGW/Windows: link static SFML and system dependencies
if(MINGW)
    target_link_libraries(imguix_framed_showcase PRIVATE
        sfml-graphics-s sfml-window-s sfml-system-s ImGui-SFML freetype
        gdi32 user32 comctl32 dwmapi opengl32)
endif()
