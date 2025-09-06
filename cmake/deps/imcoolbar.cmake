# Provides: imguix_use_or_fetch_imcoolbar(<OUT_VAR>)
# Exports target: imcoolbar::imcoolbar
function(imguix_use_or_fetch_imcoolbar OUT_VAR)
    set(_DIR "${IMCOOLBAR_DIR}")
    if(NOT _DIR)
        set(_DIR "${PROJECT_SOURCE_DIR}/libs/ImCoolBar")
    endif()

    if(NOT EXISTS "${_DIR}/ImCoolBar.cpp")
        message(FATAL_ERROR "ImCoolBar not found at ${_DIR}. "
                            "Set IMCOOLBAR_DIR or add submodule libs/ImCoolBar.")
    endif()

    add_library(imcoolbar STATIC "${_DIR}/ImCoolBar.cpp")
    add_library(imcoolbar::imcoolbar ALIAS imcoolbar)

    target_include_directories(imcoolbar PUBLIC
        $<BUILD_INTERFACE:${_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(imcoolbar PUBLIC imgui::imgui)

    set_target_properties(imcoolbar PROPERTIES
        CXX_STANDARD 17
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_DIR}/ImCoolBar.h"
    )

    set(${OUT_VAR} imcoolbar::imcoolbar PARENT_SCOPE)
endfunction()
