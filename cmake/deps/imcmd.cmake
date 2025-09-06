# Provides: imguix_use_or_fetch_imcmd(<OUT_VAR>)
# Exports target: imcmd::imcmd
function(imguix_use_or_fetch_imcmd OUT_VAR)
    set(_DIR "${IMCMD_DIR}")
    if(NOT _DIR)
        set(_DIR "${PROJECT_SOURCE_DIR}/libs/imgui-command-palette")
    endif()
    if(NOT EXISTS "${_DIR}/imcmd_command_palette.cpp")
        message(FATAL_ERROR "imgui-command-palette not found at ${_DIR}. "
                            "Set IMCMD_DIR or add submodule libs/imgui-command-palette.")
    endif()

    add_library(imcmd STATIC
        "${_DIR}/imcmd_command_palette.cpp"
        "${_DIR}/imcmd_fuzzy_search.cpp"
    )
    add_library(imcmd::imcmd ALIAS imcmd)

    target_include_directories(imcmd PUBLIC
        $<BUILD_INTERFACE:${_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(imcmd PUBLIC imgui::imgui)

    set_target_properties(imcmd PROPERTIES
        CXX_STANDARD 17
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_DIR}/imcmd_command_palette.h;${_DIR}/imcmd_fuzzy_search.h"
    )
    set(${OUT_VAR} imcmd::imcmd PARENT_SCOPE)
endfunction()
