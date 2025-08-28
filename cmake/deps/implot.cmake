# cmake/deps/implot.cmake
# Собирает ImPlot из исходников (без демо), создаёт цель implot::implot
# Зависит от imgui::imgui

function(imguix_use_or_fetch_implot out_implot)
    # Где лежит ImPlot (по умолчанию сабмодуль)
    if(NOT DEFINED IMPLOT_DIR OR IMPLOT_DIR STREQUAL "")
        set(IMPLOT_DIR "${PROJECT_SOURCE_DIR}/libs/implot")
    endif()

    # Если цель уже есть — вернуть её
    if(TARGET implot::implot)
        set(${out_implot} implot::implot PARENT_SCOPE)
        return()
    elseif(TARGET implot)
        add_library(implot::implot ALIAS implot)
        set(${out_implot} implot::implot PARENT_SCOPE)
        return()
    endif()

    # Убедимся, что есть imgui::imgui (создаём, если нет)
    if(NOT TARGET imgui::imgui)
        if(COMMAND imguix_use_or_fetch_imgui)
            imguix_use_or_fetch_imgui(_IMGUI_TGT)
        else()
            message(FATAL_ERROR "imgui::imgui target not found and no helper to create it")
        endif()
    else()
        set(_IMGUI_TGT imgui::imgui)
    endif()

    # Исходники ImPlot (демо-файл не включаем)
    set(_src
        "${IMPLOT_DIR}/implot.cpp"
        "${IMPLOT_DIR}/implot_items.cpp"
    )

    add_library(implot STATIC ${_src})
    add_library(implot::implot ALIAS implot)

    target_include_directories(implot
        PUBLIC
            $<BUILD_INTERFACE:${IMPLOT_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    # ImPlot зависит от Dear ImGui
    target_link_libraries(implot PUBLIC ${_IMGUI_TGT})

    # Опциональные предупреждения
    if(MSVC)
        target_compile_options(implot PRIVATE /W3)
    else()
        target_compile_options(implot PRIVATE -Wall -Wextra -Wno-unused-parameter)
    endif()

    set(${out_implot} implot::implot PARENT_SCOPE)
endfunction()
