# ===== deps/imgui.cmake =====
# Purpose: Build or reuse Dear ImGui with optional FreeType and std::string helpers.
# Inputs:  IMGUI_DIR, IMGUIX_IMGUI_STDLIB, IMGUIX_IMGUI_FREETYPE
# Outputs: out_imgui receives imgui::imgui
# Notes:   Prefer existing imgui::imgui if available.

# Build or reuse Dear ImGui and expose it as imgui::imgui
# Params:
# - out_imgui: variable to receive target name
# Behavior:
# - Uses existing imgui::imgui or compiles from ${IMGUI_DIR}
# Usage:
#   imguix_use_or_fetch_imgui(IMGUI_TARGET)
# Idempotent: reuse target if already created
function(imguix_use_or_fetch_imgui out_imgui)
    # Locate Dear ImGui directory (default to submodule)
    if(NOT DEFINED IMGUI_DIR OR IMGUI_DIR STREQUAL "")
        set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui")
    endif()

    # If target already exists, return it
    if(TARGET imgui::imgui)
        set(${out_imgui} imgui::imgui PARENT_SCOPE)
        return()
    elseif(TARGET imgui)
        add_library(imgui::imgui ALIAS imgui)
        set(${out_imgui} imgui::imgui PARENT_SCOPE)
        return()
    endif()

    # Core ImGui sources
    set(_src
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
    )

    # std::string helpers (imgui_stdlib) controlled by IMGUIX_IMGUI_STDLIB
    # SFML backend typically does not need this because ImGui-SFML adds it
    if(IMGUIX_IMGUI_STDLIB)
        list(APPEND _src "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp")
    endif()

    # Optional FreeType integration
    if(IMGUIX_IMGUI_FREETYPE)
        list(APPEND _src "${IMGUI_DIR}/misc/freetype/imgui_freetype.cpp")
    endif()

    add_library(imgui STATIC ${_src})
    add_library(imgui::imgui ALIAS imgui)

    # BUILD_INTERFACE: headers from source tree; INSTALL_INTERFACE: installed headers
    target_include_directories(imgui PUBLIC
        $<BUILD_INTERFACE:${IMGUI_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    if(DEFINED IMGUIX_USER_CONFIG_DIR AND DEFINED IMGUIX_USER_CONFIG_NAME)
        target_include_directories(imgui PUBLIC
            $<BUILD_INTERFACE:${IMGUIX_USER_CONFIG_DIR}>
        )
        target_compile_definitions(imgui PUBLIC
            IMGUI_USER_CONFIG="${IMGUIX_USER_CONFIG_NAME}"
        )
    endif()

    if(IMGUIX_IMGUI_FREETYPE)
        target_include_directories(imgui PRIVATE "${IMGUI_DIR}/misc/freetype")
        target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_FREETYPE)

        if(COMMAND imguix_use_or_find_freetype)
            imguix_use_or_find_freetype(_FREETYPE_TGT)
        else()
            find_package(Freetype REQUIRED)
            set(_FREETYPE_TGT Freetype::Freetype)
        endif()
        target_link_libraries(imgui PRIVATE ${_FREETYPE_TGT})
    endif()

    set(${out_imgui} imgui::imgui PARENT_SCOPE)
endfunction()
