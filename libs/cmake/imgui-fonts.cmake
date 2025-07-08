# cmake/imgui-fonts.cmake

# Function: copy_imgui_fonts
# Copies fonts from the ImGui misc/fonts folder into the output dir of the given target.
function(copy_imgui_fonts TARGET)
    if(NOT TARGET)
        message(FATAL_ERROR "copy_imgui_fonts: TARGET not specified")
    endif()

    set(FONTS_DIR "${CMAKE_SOURCE_DIR}/libs/imgui/misc/fonts")

    if(NOT EXISTS "${FONTS_DIR}")
        message(WARNING "ImGui fonts directory not found: ${FONTS_DIR}")
        return()
    endif()

    file(GLOB IMGUI_FONT_FILES "${FONTS_DIR}/*.ttf" "${FONTS_DIR}/*.otf")

    if(IMGUI_FONT_FILES)
        foreach(FONT_FILE ${IMGUI_FONT_FILES})
            get_filename_component(FONT_NAME ${FONT_FILE} NAME)
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                    $<TARGET_FILE_DIR:${TARGET}>/data/fonts
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${FONT_FILE}"
                    $<TARGET_FILE_DIR:${TARGET}>/data/fonts/${FONT_NAME}
                COMMENT "Copying ${FONT_NAME} to output fonts directory for ${TARGET}"
            )
        endforeach()
    else()
        message(WARNING "No .ttf or .otf fonts found in ${FONTS_DIR}")
    endif()
endfunction()