# Provides: imguix_use_or_fetch_imguifiledialog(<OUT_VAR>)
# Exports target: imguifiledialog::imguifiledialog
function(imguix_use_or_fetch_imguifiledialog OUT_VAR)
    set(_DIR "${IMGUIFILEDIALOG_DIR}")
    if(NOT _DIR)
        set(_DIR "${PROJECT_SOURCE_DIR}/libs/ImGuiFileDialog")
    endif()
    if(NOT EXISTS "${_DIR}/ImGuiFileDialog.h")
        message(FATAL_ERROR "ImGuiFileDialog not found at ${_DIR}. "
                            "Set IMGUIFILEDIALOG_DIR or add submodule libs/ImGuiFileDialog.")
    endif()

    add_library(imguifiledialog STATIC
        "${_DIR}/ImGuiFileDialog.cpp"
    )
    add_library(imguifiledialog::imguifiledialog ALIAS imguifiledialog)

    target_include_directories(imguifiledialog PUBLIC "${_DIR}")
    target_link_libraries(imguifiledialog PUBLIC imgui::imgui)

    # Опционально: переключить на std::filesystem (иначе используется dirent)
    # См. #ifdef USE_STD_FILESYSTEM в исходниках. :contentReference[oaicite:1]{index=1}
    if(IMGUIX_IMGUIFILEDIALOG_USE_STD_FS)
        target_compile_definitions(imguifiledialog PUBLIC USE_STD_FILESYSTEM)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
           CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
            target_link_libraries(imguifiledialog PUBLIC stdc++fs) # для старых GCC
        endif()
    endif()

    set_target_properties(imguifiledialog PROPERTIES
        CXX_STANDARD 17
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_DIR}/ImGuiFileDialog.h;${_DIR}/ImGuiFileDialogConfig.h"
    )

    set(${OUT_VAR} imguifiledialog::imguifiledialog PARENT_SCOPE)
endfunction()
