# libs/cmake/prepare-imgui-sfml-deps.cmake
# Sets up dependencies for ImGuiX using SFML and ImGui-SFML

# --- Include CMake wrappers for all dependencies ---

include(${CMAKE_CURRENT_LIST_DIR}/sfml-wrapper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/imgui-wrapper.cmake)

message(STATUS "[ImGuiX] All dependencies prepared (SFML + ImGui + ImGui-SFML).")
