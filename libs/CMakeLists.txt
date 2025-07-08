# libs/CMakeLists.txt

# Основной CMake-файл для папки libs/
# Здесь подключаются все зависимости через соответствующие обёртки в cmake/

# Гарантируем, что пути к модулям заданы относительно этой папки
cmake_minimum_required(VERSION 3.18)

# Подключаем единый скрипт подготовки imgui + sfml
include(${CMAKE_CURRENT_LIST_DIR}/cmake/prepare-imgui-sfml-deps.cmake)
#
include(${CMAKE_CURRENT_LIST_DIR}/cmake/link_local_libs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/imgui-fonts.cmake)
