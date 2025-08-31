# Provides: imguix_use_or_fetch_imtexteditor(<OUT_VAR>)
# Exports target: imtexteditor::imtexteditor
function(imguix_use_or_fetch_imtexteditor OUT_VAR)
    set(_DIR "${IMTEXTEDITOR_DIR}")
    if(NOT _DIR)
        set(_DIR "${PROJECT_SOURCE_DIR}/libs/ImGuiColorTextEdit")
    endif()

    if(NOT EXISTS "${_DIR}/TextEditor.cpp")
        message(FATAL_ERROR "ImGuiColorTextEdit not found at ${_DIR}. "
                            "Set IMTEXTEDITOR_DIR or add submodule libs/ImGuiColorTextEdit.")
    endif()

    add_library(imtexteditor STATIC
        "${_DIR}/TextEditor.cpp"
    )
    add_library(imtexteditor::imtexteditor ALIAS imtexteditor)

    target_include_directories(imtexteditor PUBLIC "${_DIR}")
    target_link_libraries(imtexteditor PUBLIC imgui::imgui)

    set_target_properties(imtexteditor PROPERTIES
        CXX_STANDARD 17
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_DIR}/TextEditor.h"
    )

    set(${OUT_VAR} imtexteditor::imtexteditor PARENT_SCOPE)
endfunction()
