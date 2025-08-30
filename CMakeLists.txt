cmake_minimum_required(VERSION 3.18)
project(ImGuiX LANGUAGES CXX VERSION 0.1.0)

# ===== Root CMakeLists.txt =====
# Purpose: Configure and build the ImGuiX library and optional SDK.
# Inputs:  IMGUIX_* options and IMGUIX_DEPS_* modes, IMGUI_DIR
# Outputs: target imguix and export ImGuiXTargets
# Notes:   Select backend via IMGUIX_USE_*_BACKEND options.

# Normalize REAL_PATH behavior so MSYS/Windows paths resolve to native form first
if(POLICY CMP0152)
    cmake_policy(SET CMP0152 NEW)
endif()

# ===== Options =====
option(IMGUIX_HEADER_ONLY      "Header-only mode (no .cpp compiled)" OFF)
option(IMGUIX_BUILD_SHARED     "Build ImGuiX as shared library" OFF)
option(IMGUIX_BUILD_TESTS      "Build tests in tests/ folder"   ON)

# Backend selection
option(IMGUIX_USE_SFML_BACKEND "Use SFML backend via ImGui-SFML" ON)
option(IMGUIX_USE_GLFW_BACKEND "Use GLFW backend" OFF)
option(IMGUIX_USE_SDL2_BACKEND "Use SDL2 backend" OFF)

# ImGui / FreeType
option(IMGUIX_IMGUI_FREETYPE   "Enable ImGui FreeType rasterizer" ON)

# ImGui / ImPlot, ImPlot3D
option(IMGUIX_USE_IMPLOT       "Build ImPlot and link it in" ON)
option(IMGUIX_USE_IMPLOT3D     "Build ImPlot3D and link it in" ON)

# JSON vendoring (default OFF to prefer system package)
option(IMGUIX_VENDOR_JSON      "Install vendored nlohmann_json headers with ImGuiX" OFF)

option(IMGUIX_SDK_FLATTEN_MISC_HEADERS
    "Also copy imgui_stdlib.h and imgui_freetype.h into include/ root" ON)

# SDK
# Install SDK extras (quickstart, templates, assets)
option(IMGUIX_SDK_INSTALL      "Install SDK extras (quickstart, templates, assets)" ON)
option(IMGUIX_SDK_BUNDLE_DEPS  "Install 3rd-party deps into SDK stage" ON)
# Place quickstart template into SDK
option(IMGUIX_SDK_INSTALL_QUICKSTART "Install quickstart template into SDK" ON)

# ===== Deps mode =====
set(IMGUIX_DEPS_MODE "AUTO" CACHE STRING "AUTO|SYSTEM|BUNDLED")
set_property(CACHE IMGUIX_DEPS_MODE PROPERTY STRINGS AUTO SYSTEM BUNDLED)
set(IMGUIX_DEPS_FMT_MODE        "INHERIT" CACHE STRING "INHERIT|AUTO|SYSTEM|BUNDLED")
set(IMGUIX_DEPS_SFML_MODE       "INHERIT" CACHE STRING "INHERIT|AUTO|SYSTEM|BUNDLED")
set(IMGUIX_DEPS_IMGUI_MODE      "INHERIT" CACHE STRING "INHERIT|AUTO|SYSTEM|BUNDLED")
set(IMGUIX_DEPS_IMGUI_SFML_MODE "INHERIT" CACHE STRING "INHERIT|AUTO|SYSTEM|BUNDLED")
set(IMGUIX_DEPS_FREETYPE_MODE   "INHERIT" CACHE STRING "INHERIT|AUTO|SYSTEM|BUNDLED")
set(IMGUIX_DEPS_JSON_MODE       "INHERIT" CACHE STRING "INHERIT|AUTO|SYSTEM|BUNDLED")

# ===== C++ =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(EMSCRIPTEN)
    add_link_options(-sASYNCIFY)
    add_compile_definitions(IMGUIX_ASYNCIFY)
endif()

set(IMGUIX_USER_CONFIG_DIR  "${PROJECT_SOURCE_DIR}/include")
set(IMGUIX_USER_CONFIG_NAME "imguix/config/imconfig-imguix.hpp")

# ===== Module path =====
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# --- std::string helpers for ImGui (imgui_stdlib) ---
# If user does not set the option, choose default:
# - with SFML backend: OFF (ImGui-SFML already provides imgui_stdlib.cpp)
# - otherwise: ON
if(NOT DEFINED IMGUIX_IMGUI_STDLIB)
    set(_imgui_stdlib_default ON)
    if(IMGUIX_USE_SFML_BACKEND)
        set(_imgui_stdlib_default OFF)
    endif()
    set(IMGUIX_IMGUI_STDLIB ${_imgui_stdlib_default}
        CACHE BOOL "Build ImGui std::string helpers (imgui_stdlib.cpp) for non-SFML backends")
endif()

if(IMGUIX_USE_SFML_BACKEND AND IMGUIX_IMGUI_STDLIB)
    message(STATUS "IMGUIX_USE_SFML_BACKEND=ON: forcing IMGUIX_IMGUI_STDLIB=OFF to avoid duplicate imgui_stdlib.cpp")
    set(IMGUIX_IMGUI_STDLIB OFF CACHE BOOL "" FORCE)
endif()

# ===== Deps (modular) =====
include(cmake/deps/fmt.cmake)
include(cmake/deps/freetype.cmake)
include(cmake/deps/sfml.cmake)
include(cmake/deps/imgui.cmake)
include(cmake/deps/imgui_sfml.cmake)
include(cmake/deps/implot.cmake)
include(cmake/deps/implot3d.cmake)
include(cmake/deps/nlohmann_json.cmake)
include(cmake/deps/assets.cmake)
include(cmake/deps/app_icon.cmake)
include(cmake/ImGuiXDemos.cmake)

# ===== Resolve core deps =====
imguix_use_or_fetch_fmt(FMT_TARGET)                 # Expect fmt::fmt
imguix_use_or_fetch_nlohmann_json(JSON_TARGET)      # Always nlohmann_json::nlohmann_json or alias

# ===== Resolve GUI/backend deps =====
set(BACKEND_LIBS "")
set(IMGUI_LIB imgui::imgui)

if(IMGUIX_USE_SFML_BACKEND)
    imguix_use_or_fetch_sfml(SFML_TARGETS)          # SFML::Graphics;SFML::Window;SFML::System
    imguix_use_or_fetch_imgui_sfml(IMGUI_SFML_TARGET IMGUI_TARGET_OUT)
    set(IMGUI_LIB "${IMGUI_TARGET_OUT}")            # ImGui-SFML
    list(APPEND BACKEND_LIBS ${SFML_TARGETS})
elseif(IMGUIX_USE_GLFW_BACKEND)
    message(STATUS "GLFW backend selected (add deps/glfw.cmake if needed)")
elseif(IMGUIX_USE_SDL2_BACKEND)
    message(STATUS "SDL2 backend selected (add deps/sdl2.cmake if needed)")
else()
    message(STATUS "No GUI backend selected; using pure Dear ImGui")
    imguix_use_or_fetch_imgui(IMGUI_TARGET)
    set(IMGUI_LIB "${IMGUI_TARGET}")
endif()

# --- Select backend define ---
set(_IMGUIX_BACKEND_DEFINE "")
if(IMGUIX_USE_SFML_BACKEND)
    set(_IMGUIX_BACKEND_DEFINE IMGUIX_USE_SFML_BACKEND)
elseif(IMGUIX_USE_GLFW_BACKEND)
    set(_IMGUIX_BACKEND_DEFINE IMGUIX_USE_GLFW_BACKEND)
elseif(IMGUIX_USE_SDL2_BACKEND)
    set(_IMGUIX_BACKEND_DEFINE IMGUIX_USE_SDL2_BACKEND)
endif()

if(IMGUIX_USE_IMPLOT)
    imguix_use_or_fetch_implot(IMPLOT_TARGET)
endif()

if(IMGUIX_USE_IMPLOT3D)
    imguix_use_or_fetch_implot3d(IMPLOT3D_TARGET)
endif()

# ===== Library / Header-only switch =====
if(IMGUIX_HEADER_ONLY)
    message(STATUS "ImGuiX: header-only mode ON")
    add_library(imguix INTERFACE)
    add_library(ImGuiX::imguix ALIAS imguix)

    # BUILD_INTERFACE: headers from source tree during build
    # INSTALL_INTERFACE: headers exposed after installation
    target_include_directories(imguix INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # Allow user configuration overrides via IMGUI_USER_CONFIG
    target_include_directories(imguix INTERFACE "${IMGUIX_USER_CONFIG_DIR}")
    target_compile_definitions(imguix INTERFACE IMGUI_USER_CONFIG="${IMGUIX_USER_CONFIG_NAME}")

    # Variant B: when vendoring, add json headers to build include path
    if(IMGUIX_VENDOR_JSON)
        target_include_directories(imguix INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/libs/json/include>
        )
    endif()

    target_compile_definitions(imguix INTERFACE IMGUIX_HEADER_ONLY)

    if(_IMGUIX_BACKEND_DEFINE)
        target_compile_definitions(imguix INTERFACE ${_IMGUIX_BACKEND_DEFINE})
    endif()

    if(IMGUIX_IMGUI_FREETYPE)
        target_compile_definitions(imguix INTERFACE IMGUI_ENABLE_FREETYPE)
        # Do not link freetype/SFML/ImGui-SFML in INTERFACE; consumers link them
    endif()

    if(IMGUIX_USE_IMPLOT)
        target_compile_definitions(imguix INTERFACE IMGUI_ENABLE_IMPLOT)
    endif()

    if(IMGUIX_USE_IMPLOT3D)
        target_compile_definitions(imguix INTERFACE IMGUI_ENABLE_IMPLOT3D)
    endif()

    # Clean interface to avoid exporting foreign targets
    set_property(TARGET imguix PROPERTY INTERFACE_LINK_LIBRARIES "")

else()
    set(_imguix_type STATIC)
    if(IMGUIX_BUILD_SHARED)
        set(_imguix_type SHARED)
    endif()

    add_library(imguix ${_imguix_type}
        src/imguix.cpp
    )
    add_library(ImGuiX::imguix ALIAS imguix)

    target_include_directories(imguix
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

    if(WIN32 AND IMGUIX_BUILD_SHARED)
        set_target_properties(imguix PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()

    # Link external dependencies as PRIVATE to keep export minimal
    target_link_libraries(imguix
        PRIVATE fmt::fmt nlohmann_json::nlohmann_json
        PRIVATE ${IMGUI_LIB} ${BACKEND_LIBS}
    )

    # Ensure backend define participates in this target's compilation
    if(_IMGUIX_BACKEND_DEFINE)
        target_compile_definitions(imguix PUBLIC ${_IMGUIX_BACKEND_DEFINE})
    endif()

    if(IMGUIX_IMGUI_FREETYPE)
        target_compile_definitions(imguix PUBLIC IMGUI_ENABLE_FREETYPE)

        # Require include path for imgui_freetype.h
        set(_IMGUI_DIR_LOCAL "${IMGUI_DIR}")
        if(NOT _IMGUI_DIR_LOCAL)
            set(_IMGUI_DIR_LOCAL "${PROJECT_SOURCE_DIR}/libs/imgui")
        endif()
        target_include_directories(imguix PRIVATE "${_IMGUI_DIR_LOCAL}/misc/freetype")

        # Resolve Dear ImGui root (same approach as for FreeType)
        set(_IMGUI_DIR_LOCAL "${IMGUI_DIR}")
        if(NOT _IMGUI_DIR_LOCAL)
            set(_IMGUI_DIR_LOCAL "${PROJECT_SOURCE_DIR}/libs/imgui")
        endif()

        # Require include path for imgui_stdlib.h (misc/cpp)
        target_include_directories(imguix PRIVATE "${_IMGUI_DIR_LOCAL}/misc/cpp")

        imguix_use_or_find_freetype(FREETYPE_TARGET)
        target_link_libraries(imguix PRIVATE ${FREETYPE_TARGET})
    endif()

    # Variant B: when vendoring, add build include for json submodule
    if(IMGUIX_VENDOR_JSON)
        target_include_directories(imguix PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/libs/json/include>
        )
    endif()

    if(IMGUIX_USE_IMPLOT)
        target_compile_definitions(imguix PUBLIC IMGUI_ENABLE_IMPLOT)
        target_link_libraries(imguix PRIVATE ${IMPLOT_TARGET})
    endif()
    
    if(IMGUIX_USE_IMPLOT3D)
        target_compile_definitions(imguix PUBLIC IMGUI_ENABLE_IMPLOT3D)
        target_link_libraries(imguix PRIVATE ${IMPLOT3D_TARGET})
    endif()

    # Clean interface to avoid exporting foreign targets
    set_property(TARGET imguix PROPERTY INTERFACE_LINK_LIBRARIES "")
endif()

# ===== Tests =====
if(IMGUIX_BUILD_TESTS)
    enable_testing()
    file(GLOB TESTS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cpp")

    set(_IMGUI_DIR "${IMGUI_DIR}")
    if(NOT _IMGUI_DIR)
        set(_IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui")
    endif()

    foreach(TEST_FILE ${TESTS})
        get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
        add_executable(${TEST_NAME} "${TEST_FILE}")

        target_compile_definitions(${TEST_NAME} PRIVATE IMGUIX_HEADER_ONLY)

        if(IMGUIX_USE_SFML_BACKEND)
            target_compile_definitions(${TEST_NAME} PRIVATE IMGUIX_USE_SFML_BACKEND)
        elseif(IMGUIX_USE_GLFW_BACKEND)
            target_compile_definitions(${TEST_NAME} PRIVATE IMGUIX_USE_GLFW_BACKEND)
        elseif(IMGUIX_USE_SDL2_BACKEND)
            target_compile_definitions(${TEST_NAME} PRIVATE IMGUIX_USE_SDL2_BACKEND)
        endif()

        target_include_directories(${TEST_NAME} PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/include"
        )
        # Provide access to imgui_stdlib.h
        target_include_directories(${TEST_NAME} PRIVATE 
            "${_IMGUI_DIR}/misc/cpp"
        )

        if(IMGUIX_IMGUI_FREETYPE)
            target_compile_definitions(${TEST_NAME} PRIVATE IMGUI_ENABLE_FREETYPE)
            target_include_directories(${TEST_NAME} PRIVATE
                "${_IMGUI_DIR}/misc/freetype"
            )
            imguix_use_or_find_freetype(FREETYPE_TARGET)
            target_link_libraries(${TEST_NAME} PRIVATE ${FREETYPE_TARGET})
        endif()

        target_link_libraries(${TEST_NAME} PRIVATE ${IMGUI_LIB} ${BACKEND_LIBS})
        target_link_libraries(${TEST_NAME} PRIVATE fmt::fmt nlohmann_json::nlohmann_json)

        if(IMGUIX_USE_IMPLOT)
            target_compile_definitions(${TEST_NAME} PRIVATE IMGUI_ENABLE_IMPLOT)
            target_link_libraries(${TEST_NAME} PRIVATE implot::implot)
            imguix_add_implot_demo(${TEST_NAME})
        endif()
        
        if(IMGUIX_USE_IMPLOT3D)
            target_compile_definitions(${TEST_NAME} PRIVATE IMGUI_ENABLE_IMPLOT3D)
            target_link_libraries(${TEST_NAME} PRIVATE implot3d::implot3d)
            imguix_add_implot3d_demo(${TEST_NAME})
        endif()

        imguix_add_imgui_demo(${TEST_NAME})

        if(WIN32)
            target_link_libraries(${TEST_NAME} PRIVATE gdi32 user32 comctl32 dwmapi)
        endif()

        # Copy assets after build
        imguix_add_assets(${TEST_NAME}
            DEST_RUNTIME data
            DIRS ${PROJECT_SOURCE_DIR}/assets/data
            EXCLUDE_DIRS web  # exclude assets/data/web
        )

        imguix_copy_and_embed_app_icon(${TEST_NAME})

        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
        )

        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()


# ===== Install / Export =====
include(CMakePackageConfigHelpers)

install(TARGETS imguix
    EXPORT ImGuiXTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Public headers of ImGuiX
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/imguix
        DESTINATION include)

# Variant B: vendor json headers when enabled
if(IMGUIX_VENDOR_JSON)
    # Place headers so <nlohmann/json.hpp> works out of the box
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/libs/json/include/nlohmann
            DESTINATION include/nlohmann)
endif()

install(EXPORT ImGuiXTargets
    NAMESPACE ImGuiX::
    DESTINATION lib/cmake/ImGuiX
)

configure_package_config_file(
    cmake/ImGuiXConfig.cmake.in
    ${CMAKE_BINARY_DIR}/ImGuiXConfig.cmake
    INSTALL_DESTINATION lib/cmake/ImGuiX
)
install(FILES ${CMAKE_BINARY_DIR}/ImGuiXConfig.cmake
        DESTINATION lib/cmake/ImGuiX)

# Export from build tree for local find_package()
export(EXPORT ImGuiXTargets
    NAMESPACE ImGuiX::
    FILE "${CMAKE_BINARY_DIR}/ImGuiXTargets.cmake")

# ===== SDK bundle of 3rd-party deps =====

# Install target and optionally its PUBLIC_HEADER files
# Params:
# - tgt: target name to install
# Behavior:
# - Installs library and headers if PUBLIC_HEADER is defined
# Usage:
#   imguix_install_target_with_headers(imgui)
# Idempotent: no-op if target is missing
function(imguix_install_target_with_headers tgt)
    if(NOT TARGET ${tgt})
        return()
    endif()
    get_target_property(_pub ${tgt} PUBLIC_HEADER)
    if(_pub)
        install(TARGETS ${tgt}
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
            PUBLIC_HEADER DESTINATION include
        )
    else()
        install(TARGETS ${tgt}
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
        )
    endif()
endfunction()


if(IMGUIX_SDK_BUNDLE_DEPS)
    # --- Dear ImGui: install only required headers, skip aux folders ---
    set(_IMGUI_DIR "${IMGUI_DIR}")
    if(NOT _IMGUI_DIR)
        set(_IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui")
    endif()

    if(EXISTS "${_IMGUI_DIR}/imgui.h")
        install(
            DIRECTORY "${_IMGUI_DIR}/"
            DESTINATION "include"
            FILES_MATCHING
                # Include headers only
                PATTERN "*.h"
                # Also take headers from misc/cpp and misc/freetype
                PATTERN "misc/cpp/*.h"
                PATTERN "misc/freetype/*.h"
            # Exclude irrelevant directories
            PATTERN ".git*"     EXCLUDE
            PATTERN ".github*"  EXCLUDE
            PATTERN "backends"  EXCLUDE
            PATTERN "docs"      EXCLUDE
            PATTERN "examples*" EXCLUDE
            PATTERN "scripts"   EXCLUDE
            PATTERN "bindings"  EXCLUDE
        )

        if(IMGUIX_SDK_FLATTEN_MISC_HEADERS)
            # Duplicate misc headers into root include for simple includes
            if(EXISTS "${_IMGUI_DIR}/misc/cpp/imgui_stdlib.h")
                install(FILES "${_IMGUI_DIR}/misc/cpp/imgui_stdlib.h"
                        DESTINATION "include")
            endif()
            if(EXISTS "${_IMGUI_DIR}/misc/freetype/imgui_freetype.h")
                install(FILES "${_IMGUI_DIR}/misc/freetype/imgui_freetype.h"
                        DESTINATION "include")
            endif()
        endif()
    endif()


    # --- ImGui-SFML: static library and headers ---
    if(TARGET ImGui-SFML)
        imguix_install_target_with_headers(ImGui-SFML)
        if(NOT DEFINED IMGUI_SFML_DIR OR IMGUI_SFML_DIR STREQUAL "")
            set(IMGUI_SFML_DIR "${PROJECT_SOURCE_DIR}/libs/imgui-sfml")
        endif()
        if(EXISTS "${IMGUI_SFML_DIR}/imgui-SFML.h")
            install(FILES
                    "${IMGUI_SFML_DIR}/imgui-SFML.h"
                    "${IMGUI_SFML_DIR}/imconfig-SFML.h"
                    DESTINATION include)
        endif()
    endif()

    # --- fmt: library and headers if submodule ---
    if(TARGET fmt)
        if(DEFINED FMT_INSTALL AND FMT_INSTALL)
            message(STATUS "SDK: fmt installs itself (FMT_INSTALL=ON) — skipping manual header install")
        else()
            imguix_install_target_with_headers(fmt)
            if(EXISTS "${PROJECT_SOURCE_DIR}/libs/fmt/include/fmt")
                install(DIRECTORY "${PROJECT_SOURCE_DIR}/libs/fmt/include/fmt"
                        DESTINATION include)
            endif()
        endif()
    endif()

    # --- nlohmann-json (header-only) when vendored ---
    if(IMGUIX_VENDOR_JSON)
        if(EXISTS "${PROJECT_SOURCE_DIR}/libs/json/include/nlohmann")
            install(DIRECTORY "${PROJECT_SOURCE_DIR}/libs/json/include/nlohmann"
                    DESTINATION include)
        endif()
    endif()

    # --- ImPlot: headers ---
    if(EXISTS "${PROJECT_SOURCE_DIR}/libs/implot/implot.h")
        install(FILES
            "${PROJECT_SOURCE_DIR}/libs/implot/implot.h"
            "${PROJECT_SOURCE_DIR}/libs/implot/implot_internal.h"
            DESTINATION include
        )
    endif()
    
    # --- ImPlot3D: headers ---
    if(EXISTS "${PROJECT_SOURCE_DIR}/libs/implot3d/implot3d.h")
        install(FILES
            "${PROJECT_SOURCE_DIR}/libs/implot3d/implot3d.h"
            "${PROJECT_SOURCE_DIR}/libs/implot3d/implot3d_internal.h"
            DESTINATION include
        )
    endif()

endif()

# --- Quickstart and resources (guarded by IMGUIX_SDK_INSTALL) ---
if(IMGUIX_SDK_INSTALL AND IMGUIX_SDK_INSTALL_QUICKSTART)
    # Copy resources required by quickstart
    if(EXISTS "${PROJECT_SOURCE_DIR}/assets/data/resources")
        install(DIRECTORY "${PROJECT_SOURCE_DIR}/assets/data/resources/"
                DESTINATION quickstart/data/resources)
    endif()

    # Install quickstart code if present in repository
    if(EXISTS "${PROJECT_SOURCE_DIR}/quickstart/CMakeLists.txt")
        install(DIRECTORY "${PROJECT_SOURCE_DIR}/quickstart/"
                DESTINATION quickstart
                PATTERN "build*" EXCLUDE
                PATTERN ".vs" EXCLUDE
                PATTERN ".idea" EXCLUDE)
    endif()
endif()
