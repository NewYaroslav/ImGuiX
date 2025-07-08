# cmake/imguix-headers.cmake

set(IMGUIX_SOURCE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(IMGUIX_TARGET_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")

file(GLOB_RECURSE IMGUIX_HEADER_FILES
    RELATIVE "${IMGUIX_SOURCE_INCLUDE_DIR}"
    "${IMGUIX_SOURCE_INCLUDE_DIR}/imguix/*.hpp"
    "${IMGUIX_SOURCE_INCLUDE_DIR}/imguix/*.ipp"
)

set(COUNT 0)

foreach(REL_PATH ${IMGUIX_HEADER_FILES})
    get_filename_component(DIR "${REL_PATH}" DIRECTORY)

    set(SRC_PATH "${IMGUIX_SOURCE_INCLUDE_DIR}/${REL_PATH}")
    set(DST_PATH "${IMGUIX_TARGET_INCLUDE_DIR}/${REL_PATH}")

    file(MAKE_DIRECTORY "${IMGUIX_TARGET_INCLUDE_DIR}/${DIR}")
    configure_file("${SRC_PATH}" "${DST_PATH}" COPYONLY)
    message(STATUS "[ImGuiX] Copied: ${REL_PATH}")
    math(EXPR COUNT "${COUNT}+1")
endforeach()

message(STATUS "[ImGuiX] Total copied headers: ${COUNT}")
message(STATUS "[ImGuiX] Headers copied to: ${IMGUIX_TARGET_INCLUDE_DIR}")