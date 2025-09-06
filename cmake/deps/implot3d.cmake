# cmake/deps/implot3d.cmake
# Provides: imguix_use_or_fetch_implot3d(<OUT_VAR>)
# Exports target: implot3d::implot3d

function(imguix_use_or_fetch_implot3d OUT_VAR)
    # Mode: INHERIT -> falls back to IMGUIX_DEPS_MODE (AUTO|SYSTEM|BUNDLED)
    if(NOT DEFINED IMGUIX_DEPS_IMPLOT3D_MODE)
        set(IMGUIX_DEPS_IMPLOT3D_MODE "INHERIT")
    endif()
    set(_mode "${IMGUIX_DEPS_IMPLOT3D_MODE}")
    if(_mode STREQUAL "INHERIT")
        set(_mode "${IMGUIX_DEPS_MODE}")
    endif()

    # Try system first when AUTO or SYSTEM (e.g. vcpkg port)
    # We try both names just in case packages differ.
    set(_found FALSE)
    if(_mode STREQUAL "AUTO" OR _mode STREQUAL "SYSTEM")
        find_package(implot3d CONFIG QUIET)
        if(TARGET implot3d::implot3d)
            set(${OUT_VAR} implot3d::implot3d PARENT_SCOPE)
            set(_found TRUE)
        endif()
        if(NOT _found)
            find_package(ImPlot3D CONFIG QUIET)
            if(TARGET ImPlot3D::ImPlot3D)
                # Normalize to implot3d::implot3d for consistency
                add_library(implot3d::implot3d ALIAS ImPlot3D::ImPlot3D)
                set(${OUT_VAR} implot3d::implot3d PARENT_SCOPE)
                set(_found TRUE)
            endif()
        endif()
    endif()

    if(_found)
        return()
    endif()

    # BUNDLED (or AUTO fallback): build from submodule sources
    # Allow override via IMPLOT3D_DIR; default to libs/implot3d
    set(_IMPLOT3D_DIR "${IMPLOT3D_DIR}")
    if(NOT _IMPLOT3D_DIR)
        set(_IMPLOT3D_DIR "${PROJECT_SOURCE_DIR}/libs/implot3d")
    endif()

    if(NOT EXISTS "${_IMPLOT3D_DIR}/implot3d.h")
        message(FATAL_ERROR "ImPlot3D not found at ${_IMPLOT3D_DIR}. "
                            "Set IMPLOT3D_DIR or add submodule libs/implot3d.")
    endif()

    add_library(implot3d STATIC
        "${_IMPLOT3D_DIR}/implot3d.cpp"
        "${_IMPLOT3D_DIR}/implot3d_items.cpp"
        "${_IMPLOT3D_DIR}/implot3d_meshes.cpp"  # optional but useful
    )
    add_library(implot3d::implot3d ALIAS implot3d)

    # ImPlot3D depends only on Dear ImGui
    target_link_libraries(implot3d PUBLIC imgui::imgui)
    target_include_directories(implot3d PUBLIC
        $<BUILD_INTERFACE:${_IMPLOT3D_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    # Optionally expose public header for SDK installers
    set_target_properties(implot3d PROPERTIES
        PUBLIC_HEADER "${_IMPLOT3D_DIR}/implot3d.h"
        POSITION_INDEPENDENT_CODE ON
    )

    set(${OUT_VAR} implot3d::implot3d PARENT_SCOPE)
endfunction()

# Helper: optionally add the demo file to an executable target
function(imguix_add_implot3d_demo tgt)
    if(TARGET ${tgt})
        set(_IMPLOT3D_DIR "${IMPLOT3D_DIR}")
        if(NOT _IMPLOT3D_DIR)
            set(_IMPLOT3D_DIR "${PROJECT_SOURCE_DIR}/libs/implot3d")
        endif()
        if(EXISTS "${_IMPLOT3D_DIR}/implot3d_demo.cpp")
            target_sources(${tgt} PRIVATE "${_IMPLOT3D_DIR}/implot3d_demo.cpp")
        endif()
    endif()
endfunction()
