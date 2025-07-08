# libs/CMakeLists.txt

# Main CMake file for the libs directory
# Dependencies are included through wrapper scripts

# Ensure module paths are relative to this folder
cmake_minimum_required(VERSION 3.18)

# Include the script that prepares imgui and SFML
include(${CMAKE_CURRENT_LIST_DIR}/cmake/prepare-imgui-sfml-deps.cmake)
#
include(${CMAKE_CURRENT_LIST_DIR}/cmake/link_local_libs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/imgui-fonts.cmake)
