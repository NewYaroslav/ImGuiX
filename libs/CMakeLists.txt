# libs/CMakeLists.txt

# Main CMake file for the libs directory
# Dependencies are included through wrapper scripts

# Ensure module paths are relative to this folder
cmake_minimum_required(VERSION 3.18)

# Choose backend preparation script
if(IMGUIX_USE_SFML_BACKEND)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/prepare-imgui-sfml-deps.cmake)
elseif(IMGUIX_USE_GLFW_BACKEND)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/prepare-imgui-glfw-deps.cmake)
elseif(IMGUIX_USE_SDL2_BACKEND)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/prepare-imgui-sdl2-deps.cmake)
else()
    message(FATAL_ERROR "No backend selected for ImGuiX")
endif()
#
include(${CMAKE_CURRENT_LIST_DIR}/cmake/link-local-libs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/imgui-fonts.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/imguix-fonts.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/imguix-headers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/copy-and-embed-app-icon.cmake)
