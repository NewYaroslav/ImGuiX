# Function: copy_imguix_fonts
# Copies fonts from assets/data/resources/fonts into the output dir of the given target.
function(copy_imguix_fonts TARGET)
    if(NOT TARGET)
        message(FATAL_ERROR "copy_imguix_fonts: TARGET not specified")
    endif()

    set(IMGUIX_FONTS_DIR "${CMAKE_SOURCE_DIR}/assets/data/resources/fonts")

    if(NOT EXISTS "${IMGUIX_FONTS_DIR}")
        message(WARNING "ImGuiX fonts directory not found: ${IMGUIX_FONTS_DIR}")
        return()
    endif()

    file(GLOB IMGUIX_FONT_FILES "${IMGUIX_FONTS_DIR}/*.ttf" "${IMGUIX_FONTS_DIR}/*.otf")

    if(IMGUIX_FONT_FILES)
        foreach(FONT_FILE ${IMGUIX_FONT_FILES})
            get_filename_component(FONT_NAME ${FONT_FILE} NAME)
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                    $<TARGET_FILE_DIR:${TARGET}>/data/resources/fonts
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${FONT_FILE}"
                    $<TARGET_FILE_DIR:${TARGET}>/data/resources/fonts/${FONT_NAME}
                COMMENT "Copying ImGuiX font ${FONT_NAME} to output directory for target ${TARGET}"
            )
        endforeach()
    else()
        message(WARNING "No .ttf or .otf fonts found in ${IMGUIX_FONTS_DIR}")
    endif()
endfunction()
