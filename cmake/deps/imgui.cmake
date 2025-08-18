# cmake/deps/imgui.cmake
# Чистый Dear ImGui (бандл/система) + опционально FreeType и std::string helpers
# Использование:
#   imguix_use_or_fetch_imgui(IMGUI_TARGET_OUT)
#
# Выход:
#   IMGUI_TARGET_OUT = imgui::imgui

function(imguix_use_or_fetch_imgui out_imgui)
    # Где лежит Dear ImGui (по умолчанию сабмодуль)
    if(NOT DEFINED IMGUI_DIR OR IMGUI_DIR STREQUAL "")
        set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui")
    endif()

    # Если цель уже есть — вернуть её
    if(TARGET imgui::imgui)
        set(${out_imgui} imgui::imgui PARENT_SCOPE)
        return()
    elseif(TARGET imgui)
        add_library(imgui::imgui ALIAS imgui)
        set(${out_imgui} imgui::imgui PARENT_SCOPE)
        return()
    endif()

    # Базовые исходники ImGui
    set(_src
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
    )

    # std::string helpers (imgui_stdlib) — только по флагу
    # (для SFML-бэкенда обычно не нужно, т.к. ImGui-SFML уже добавляет это сам)
    if(IMGUIX_IMGUI_STDLIB)
        list(APPEND _src "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp")
    endif()

    # Freetype интеграция (опционально)
    if(IMGUIX_IMGUI_FREETYPE)
        list(APPEND _src "${IMGUI_DIR}/misc/freetype/imgui_freetype.cpp")
    endif()

    add_library(imgui STATIC ${_src})
    add_library(imgui::imgui ALIAS imgui)

    target_include_directories(imgui PUBLIC
        $<BUILD_INTERFACE:${IMGUI_DIR}>
        $<INSTALL_INTERFACE:include>
    )

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
