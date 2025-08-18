# cmake/deps/assets.cmake

# Copy arbitrary asset dirs to a target's runtime dir and/or install tree.
# Usage:
# imguix_add_assets(<TARGET>
#     DEST_RUNTIME <subdir-under-runtime>
#     DIRS <dir1> [<dir2> ...]
#     [EXCLUDE_DIRS <d1> <d2> ...]
#     [EXCLUDE_PATTERNS <p1> <p2> ...]  # на будущее, ниже удаляем только EXCLUDE_DIRS
# )
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

        # Удаляем исключённые подпапки уже в runtime-папке
        foreach(_ex IN LISTS IMGA_EXCLUDE_DIRS)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -rf "${_dst}/${_ex}"
                COMMENT "Prune assets: remove '${_ex}' from ${_dst}"
                VERBATIM
            )
        endforeach()
    endforeach()
endfunction()


# One global copy for tests (avoids N copies per test)
# Usage:
#   imguix_stage_test_assets(
#       SRC  <source-dir>   # default: ${PROJECT_SOURCE_DIR}/assets/data/resources
#       DEST <dest-dir>     # default: ${CMAKE_BINARY_DIR}/tests/data/resources
#   )
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


