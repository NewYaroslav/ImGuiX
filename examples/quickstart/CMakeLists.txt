cmake_minimum_required(VERSION 3.18)
project(imguix_quickstart LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажи SDK префикс через CMAKE_PREFIX_PATH:
#   cmake -S . -B build -DCMAKE_PREFIX_PATH=".../sdk"
find_package(ImGuiX REQUIRED)

add_executable(imguix_quickstart main.cpp)
target_link_libraries(imguix_quickstart PRIVATE ImGuiX::imguix)

# Так как ImGuiX линкует SFML/ImGui-SFML приватно, подтянем инклюды/библиотеки SDK в потребителе:
# Вариант 1: передать путь явно переменной при конфиге:
#   cmake -S . -B build -DIMGUIX_SDK_DIR=".../sdk"
if(DEFINED IMGUIX_SDK_DIR)
    target_include_directories(imguix_quickstart PRIVATE "${IMGUIX_SDK_DIR}/include")
    target_link_directories(imguix_quickstart PRIVATE "${IMGUIX_SDK_DIR}/lib")
endif()

# Вариант 2: вывести префикс из найденного пакета (работает, если структура стандартная)
if(NOT DEFINED IMGUIX_SDK_DIR AND DEFINED ImGuiX_DIR)
    get_filename_component(_imx_prefix "${ImGuiX_DIR}/../.." ABSOLUTE)
    target_include_directories(imguix_quickstart PRIVATE "${_imx_prefix}/include")
    target_link_directories(imguix_quickstart PRIVATE "${_imx_prefix}/lib")
endif()

# MinGW/Windows: статический SFML + зависимости линковки
if(MINGW)
    target_link_libraries(imguix_quickstart PRIVATE
        sfml-graphics-s sfml-window-s sfml-system-s ImGui-SFML freetype
        gdi32 user32 comctl32 dwmapi opengl32)
endif()
