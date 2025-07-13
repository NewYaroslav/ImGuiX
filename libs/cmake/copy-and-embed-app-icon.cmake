# Function: copy_and_embed_app_icon
# Copies app_icon.rc and icon.ico from assets/resources/icons to build output,
# then adds the .rc file as a source to the target.

function(copy_and_embed_app_icon TARGET)
    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET)
        message(FATAL_ERROR "copy_and_embed_app_icon: TARGET not specified")
    endif()

    set(SRC_ICON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/data/resources/icons")
    set(DST_ICON_DIR "${CMAKE_BINARY_DIR}/assets/data/resources/icons")
	set(RUNTIME_ICON_DIR "$<TARGET_FILE_DIR:${TARGET}>/data/resources/icons")

    set(RC_SOURCE "${SRC_ICON_DIR}/app_icon.rc")
    set(ICO_SOURCE "${SRC_ICON_DIR}/icon.ico")
	set(PNG_SOURCE "${SRC_ICON_DIR}/icon.png")

    set(RC_TARGET "${DST_ICON_DIR}/app_icon.rc")
    set(ICO_TARGET "${DST_ICON_DIR}/icon.ico")

    if(NOT EXISTS "${RC_SOURCE}")
        message(FATAL_ERROR "app_icon.rc not found at ${RC_SOURCE}")
    endif()

    if(NOT EXISTS "${ICO_SOURCE}")
        message(FATAL_ERROR "icon.ico not found at ${ICO_SOURCE}")
    endif()
	
    if(NOT EXISTS "${PNG_SOURCE}")
        message(FATAL_ERROR "icon.png not found at ${PNG_SOURCE}")
    endif()

    # Copy .rc and .ico after build
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DST_ICON_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RC_SOURCE}" "${RC_TARGET}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ICO_SOURCE}" "${ICO_TARGET}"
        COMMENT "Copying app_icon.rc and icon.ico for ${TARGET}"
    )
	
	# Copy icon.png in runtime
	add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNTIME_ICON_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PNG_SOURCE}" "${RUNTIME_ICON_DIR}/icon.png"
        COMMENT "Copying icon.png to runtime directory for ${TARGET}"
    )

    # Embed resource file from source (icon must be available at configure time)
    target_sources(${TARGET} PRIVATE "${RC_SOURCE}")
endfunction()