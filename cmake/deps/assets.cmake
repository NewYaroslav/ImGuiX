# ===== deps/assets.cmake =====
# Purpose: Copy asset directories to runtime or install locations.
# Inputs:  DIRS, DEST_RUNTIME, EXCLUDE_DIRS, EXCLUDE_PATTERNS
# Outputs: modifies given target or creates staging target
# Notes:   EXCLUDE_PATTERNS are reserved for future use.

# Copy asset directories to a target's runtime directory
# Params:
# - target: target to receive post-build asset copy
# - DEST_RUNTIME: subdirectory under target runtime dir
# - DIRS: list of source directories
# - EXCLUDE_DIRS: subdirectories to remove after copy
# Usage:
#   imguix_add_assets(my_app DIRS data EXCLUDE_DIRS web)
# Idempotent: no-op if target is missing
function(imguix_add_assets target)
    set(options)
    set(oneValueArgs DEST_RUNTIME)
    set(multiValueArgs DIRS EXCLUDE_DIRS EXCLUDE_PATTERNS)
    cmake_parse_arguments(IMGA "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TARGET ${target})
        message(FATAL_ERROR "imguix_add_assets: target '${target}' not found")
    endif()
    if(NOT IMGA_DIRS)
        message(FATAL_ERROR "imguix_add_assets: DIRS is required")
    endif()

    if(IMGA_DEST_RUNTIME)
        set(_dst "$<TARGET_FILE_DIR:${target}>/${IMGA_DEST_RUNTIME}")
    else()
        set(_dst "$<TARGET_FILE_DIR:${target}>")
    endif()

    foreach(_src IN LISTS IMGA_DIRS)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_dst}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${_src}" "${_dst}"
            COMMENT "Copy assets '${_src}' -> ${_dst}"
            VERBATIM
        )

        # Remove excluded subdirectories in runtime directory
        foreach(_ex IN LISTS IMGA_EXCLUDE_DIRS)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -rf "${_dst}/${_ex}"
                COMMENT "Prune assets: remove '${_ex}' from ${_dst}"
                VERBATIM
            )
        endforeach()
    endforeach()
endfunction()

function(imguix_stage_assets_once)
    set(options)
    set(oneValueArgs NAME DEST)
    set(multiValueArgs DIRS EXCLUDE_DIRS EXCLUDE_PATTERNS)
    cmake_parse_arguments(IMGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT IMGS_NAME)
        message(FATAL_ERROR "imguix_stage_assets_once: NAME is required")
    endif()
    if(NOT IMGS_DEST)
        message(FATAL_ERROR "imguix_stage_assets_once: DEST is required")
    endif()
    if(NOT IMGS_DIRS)
        message(FATAL_ERROR "imguix_stage_assets_once: DIRS is required")
    endif()

    # Shared runtime trees for tests/examples must be staged once to avoid
    # parallel POST_BUILD copy/prune races across multiple executable targets.
    add_custom_target(${IMGS_NAME} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${IMGS_DEST}"
        COMMENT "Stage shared assets into ${IMGS_DEST}"
        VERBATIM
    )

    foreach(_src IN LISTS IMGS_DIRS)
        add_custom_command(TARGET ${IMGS_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${_src}" "${IMGS_DEST}"
            COMMENT "Copy assets '${_src}' -> ${IMGS_DEST}"
            VERBATIM
        )
    endforeach()

    foreach(_ex IN LISTS IMGS_EXCLUDE_DIRS)
        add_custom_command(TARGET ${IMGS_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${IMGS_DEST}/${_ex}"
            COMMENT "Prune shared assets: remove '${_ex}' from ${IMGS_DEST}"
            VERBATIM
        )
    endforeach()
endfunction()


# Stage assets once for tests to avoid per-test copies
# Params:
# - SRC: source directory (default assets/data/resources)
# - DEST: destination directory (default build/tests/data/resources)
# Usage:
#   imguix_stage_test_assets(SRC my/assets)
# Idempotent: recreates staging target each run
function(imguix_stage_test_assets)
    set(oneValueArgs SRC DEST)
    cmake_parse_arguments(AA "" "${oneValueArgs}" "" ${ARGN})

    if(NOT AA_SRC)
        set(AA_SRC "${PROJECT_SOURCE_DIR}/assets/data/resources")
    endif()
    if(NOT AA_DEST)
        set(AA_DEST "${CMAKE_BINARY_DIR}/tests/data/resources")
    endif()

    add_custom_target(imguix_test_assets ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${AA_DEST}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${AA_SRC}" "${AA_DEST}"
        COMMENT "Stage test assets: ${AA_SRC} -> ${AA_DEST}"
        VERBATIM
    )
    set_property(GLOBAL PROPERTY IMGUIX_TEST_ASSETS_DEST "${AA_DEST}")
endfunction()

