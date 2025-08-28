# ===== deps/implot.cmake =====
# Purpose: Build ImPlot from sources and expose implot::implot target.
# Inputs:  IMPLOT_DIR
# Outputs: out_implot receives implot::implot
# Notes:   Depends on imgui::imgui.

# Build or reuse ImPlot and expose it as implot::implot
# Params:
# - out_implot: variable to receive target name
# Behavior:
# - Uses existing target or compiles from ${IMPLOT_DIR} without demo
# Usage:
#   imguix_use_or_fetch_implot(IMPLOT_TARGET)
# Idempotent: reuse target if already created
function(imguix_use_or_fetch_implot out_implot)
    # Locate ImPlot directory (default to submodule)
    if(NOT DEFINED IMPLOT_DIR OR IMPLOT_DIR STREQUAL "")
        set(IMPLOT_DIR "${PROJECT_SOURCE_DIR}/libs/implot")
    endif()

    # If target already exists, return it
    if(TARGET implot::implot)
        set(${out_implot} implot::implot PARENT_SCOPE)
        return()
    elseif(TARGET implot)
        add_library(implot::implot ALIAS implot)
        set(${out_implot} implot::implot PARENT_SCOPE)
        return()
    endif()

    # Ensure imgui::imgui exists (create if missing)
    if(NOT TARGET imgui::imgui)
        if(COMMAND imguix_use_or_fetch_imgui)
            imguix_use_or_fetch_imgui(_IMGUI_TGT)
        else()
            message(FATAL_ERROR "imgui::imgui target not found and no helper to create it")
        endif()
    else()
        set(_IMGUI_TGT imgui::imgui)
    endif()

    # ImPlot sources (exclude demo file)
    set(_src
        "${IMPLOT_DIR}/implot.cpp"
        "${IMPLOT_DIR}/implot_items.cpp"
    )

    add_library(implot STATIC ${_src})
    add_library(implot::implot ALIAS implot)

    # BUILD_INTERFACE: headers during build; INSTALL_INTERFACE: after install
    target_include_directories(implot
        PUBLIC
            $<BUILD_INTERFACE:${IMPLOT_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    # ImPlot depends on Dear ImGui
    target_link_libraries(implot PUBLIC ${_IMGUI_TGT})

    # Optional warnings
    if(MSVC)
        target_compile_options(implot PRIVATE /W3)
    else()
        target_compile_options(implot PRIVATE -Wall -Wextra -Wno-unused-parameter)
    endif()

    set(${out_implot} implot::implot PARENT_SCOPE)
endfunction()
