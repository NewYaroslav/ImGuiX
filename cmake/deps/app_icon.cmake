# ===== deps/app_icon.cmake =====
# Purpose: Copy application icon resources and embed them on Windows.
# Inputs:  SRC_DIR, RC, ICO, PNG
# Outputs: modifies given target by adding resource and copying PNG
# Notes:   No-op on non-Windows platforms.

# Copy and embed application icon for a target
# Params:
# - target: target to augment
# - SRC_DIR: directory with resources (default assets/data/resources/icons)
# - RC: resource script path
# - ICO: icon file path
# - PNG: runtime PNG path
# Usage:
#   imguix_copy_and_embed_app_icon(my_app SRC_DIR my/icons)
# Idempotent: warns if resources are missing
function(imguix_copy_and_embed_app_icon target)
    if(NOT WIN32)
        return()
    endif()
    if(NOT TARGET ${target})
        message(FATAL_ERROR "imguix_copy_and_embed_app_icon: target '${target}' does not exist")
    endif()

    set(oneValueArgs SRC_DIR RC ICO PNG)
    cmake_parse_arguments(AA "" "${oneValueArgs}" "" ${ARGN})

    if(NOT AA_SRC_DIR)
        set(AA_SRC_DIR "${PROJECT_SOURCE_DIR}/assets/data/resources/icons")
    endif()
    if(NOT AA_RC)
        set(AA_RC  "${AA_SRC_DIR}/app_icon.rc")
    endif()
    if(NOT AA_ICO)
        set(AA_ICO "${AA_SRC_DIR}/icon.ico")
    endif()
    if(NOT AA_PNG)
        set(AA_PNG "${AA_SRC_DIR}/icon.png")
    endif()

    if(NOT EXISTS "${AA_RC}")
        message(WARNING "app_icon.rc not found: ${AA_RC}")
        return()
    endif()
    if(NOT EXISTS "${AA_ICO}")
        message(WARNING "icon.ico not found: ${AA_ICO}")
    endif()

    # Embed resource (.rc contains a relative path to icon.ico)
    target_sources(${target} PRIVATE "${AA_RC}")

    # Runtime icon.png (if your app/tests load it at runtime)
    if(EXISTS "${AA_PNG}")
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/data/resources/icons"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AA_PNG}" "$<TARGET_FILE_DIR:${target}>/data/resources/icons/icon.png"
            COMMENT "Copy runtime icon.png for ${target}"
        )
    endif()
endfunction()
