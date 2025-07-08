# libs/cmake/prepare-imgui-sfml-deps.cmake
# Подключает и настраивает зависимости для ImGuiX с использованием SFML и ImGui-SFML

# --- Подключаем CMake-обёртки всех зависимостей ---

include(${CMAKE_CURRENT_LIST_DIR}/sfml-wrapper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/imgui-wrapper.cmake)

message(STATUS "[ImGuiX] Все зависимости подготовлены (SFML + ImGui + ImGui-SFML).")
