# ===== ImGuiXDemos.cmake =====
# Purpose: Add Dear ImGui and ImPlot demo sources to a target.
# Inputs:  IMGUI_DIR, IMPLOT_DIR
# Outputs: none; modifies provided target
# Notes:   Include after targets are declared.

# Add Dear ImGui demo source to a target
# Params:
# - tgt: target to extend
# Behavior:
# - Adds imgui_demo.cpp if present, otherwise warns
# Usage:
#   imguix_add_imgui_demo(my_app)
function(imguix_add_imgui_demo tgt)
    if(NOT TARGET ${tgt})
        message(FATAL_ERROR "Target ${tgt} not found")
    endif()
    if(NOT DEFINED IMGUI_DIR OR IMGUI_DIR STREQUAL "")
        set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui")
    endif()
    if(EXISTS "${IMGUI_DIR}/imgui_demo.cpp")
        target_sources(${tgt} PRIVATE "${IMGUI_DIR}/imgui_demo.cpp")
    else()
        message(WARNING "imgui_demo.cpp not found at ${IMGUI_DIR}")
    endif()
endfunction()

# Add ImPlot demo source to a target
# Params:
# - tgt: target to extend
# Behavior:
# - Adds implot_demo.cpp if present, otherwise warns
# Usage:
#   imguix_add_implot_demo(my_app)
function(imguix_add_implot_demo tgt)
    if(NOT TARGET ${tgt})
        message(FATAL_ERROR "Target ${tgt} not found")
    endif()
    if(NOT DEFINED IMPLOT_DIR OR IMPLOT_DIR STREQUAL "")
        set(IMPLOT_DIR "${PROJECT_SOURCE_DIR}/libs/implot")
    endif()
    if(EXISTS "${IMPLOT_DIR}/implot_demo.cpp")
        target_sources(${tgt} PRIVATE "${IMPLOT_DIR}/implot_demo.cpp")
    else()
        message(WARNING "implot_demo.cpp not found at ${IMPLOT_DIR}")
    endif()
endfunction()

