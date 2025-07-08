# cmake/imgui-fonts.cmake

# Function: copy_imgui_fonts
# Copies fonts from the ImGui misc/fonts folder into the output dir of the given target.
function(copy_imgui_fonts TARGET)
    if(NOT TARGET)
        message(FATAL_ERROR "copy_imgui_fonts: TARGET not specified")
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/libs/imgui/misc/fonts")
        message(WARNING "ImGui fonts directory not found: ${CMAKE_SOURCE_DIR}/libs/imgui/misc/fonts")
        return()
    endif()

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${TARGET}>/data/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/libs/imgui/misc/fonts
            $<TARGET_FILE_DIR:${TARGET}>/data/fonts
        COMMENT "Copying ImGui fonts to output directory for ${TARGET}"
    )
endfunction()