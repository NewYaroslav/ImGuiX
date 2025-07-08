# libs/cmake/sfml-wrapper.cmake

# Build SFML from submodule sources
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
set(SFML_ENABLE_PCH OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SFML)

message(STATUS "[ImGuiX] SFML built as a submodule.")

# --- Copy SFML headers to build/include/SFML ---
file(GLOB_RECURSE SFML_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include/SFML/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include/SFML/*.inl"
)

foreach(HDR ${SFML_HEADERS})
    file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include" "${HDR}")
    configure_file(${HDR} "${CMAKE_BINARY_DIR}/include/${REL_PATH}" COPYONLY)
endforeach()

message(STATUS "[ImGuiX] SFML headers copied to: ${CMAKE_BINARY_DIR}/include/SFML")
