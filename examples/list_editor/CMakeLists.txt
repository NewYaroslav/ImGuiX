cmake_minimum_required(VERSION 3.18)
project(imguix_list_editor LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ImGuiX REQUIRED)

add_executable(imguix_list_editor main.cpp)
target_link_libraries(imguix_list_editor PRIVATE ImGuiX::imguix)

if(DEFINED IMGUIX_SDK_DIR)
    target_include_directories(imguix_list_editor PRIVATE "${IMGUIX_SDK_DIR}/include")
    target_link_directories(imguix_list_editor PRIVATE "${IMGUIX_SDK_DIR}/lib")
endif()

if(NOT DEFINED IMGUIX_SDK_DIR AND DEFINED ImGuiX_DIR)
    get_filename_component(_imx_prefix "${ImGuiX_DIR}/../.." ABSOLUTE)
    target_include_directories(imguix_list_editor PRIVATE "${_imx_prefix}/include")
    target_link_directories(imguix_list_editor PRIVATE "${_imx_prefix}/lib")
endif()

if(MINGW)
    target_link_libraries(imguix_list_editor PRIVATE
        sfml-graphics-s sfml-window-s sfml-system-s ImGui-SFML freetype
        gdi32 user32 comctl32 dwmapi opengl32)
endif()
