# libs/cmake/imgui-sdl2-gles-wrapper.cmake

set(IMGUI_SRC      ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(FREETYPE_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/freetype)

set(IMGUI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR})
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/backends)
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/misc/freetype)
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/misc/cpp)

file(GLOB IMGUI_HEADERS "${IMGUI_SRC}/*.h")
foreach(HDR ${IMGUI_HEADERS})
    configure_file(${HDR} ${IMGUI_INCLUDE_DIR}/ COPYONLY)
endforeach()
configure_file(${IMGUI_SRC}/backends/imgui_impl_sdl2.h     ${IMGUI_INCLUDE_DIR} COPYONLY)
configure_file(${IMGUI_SRC}/backends/imgui_impl_opengl3.h  ${IMGUI_INCLUDE_DIR} COPYONLY)
configure_file(${IMGUI_SRC}/misc/freetype/imgui_freetype.h ${IMGUI_INCLUDE_DIR} COPYONLY)
configure_file(${IMGUI_SRC}/misc/cpp/imgui_stdlib.h        ${IMGUI_INCLUDE_DIR} COPYONLY)

set(IMGUI_SOURCES
    ${IMGUI_SRC}/imgui.cpp
    ${IMGUI_SRC}/imgui_draw.cpp
    ${IMGUI_SRC}/imgui_widgets.cpp
    ${IMGUI_SRC}/imgui_tables.cpp
    ${IMGUI_SRC}/imgui_demo.cpp
    ${IMGUI_SRC}/misc/freetype/imgui_freetype.cpp
    ${IMGUI_SRC}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_SRC}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_SRC}/backends/imgui_impl_opengl3.cpp
)

if (NOT TARGET freetype)
    add_subdirectory(${FREETYPE_SRC} EXCLUDE_FROM_ALL)
endif()

find_package(SDL2 REQUIRED)
find_package(OpenGLES2 REQUIRED)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui
    PRIVATE ${FREETYPE_SRC}/include
            ${IMGUI_SRC}
            ${IMGUI_SRC}/backends
)

target_compile_definitions(imgui PRIVATE IMGUI_ENABLE_FREETYPE IMGUI_IMPL_OPENGL_ES2)

target_link_libraries(imgui PRIVATE SDL2::SDL2 OpenGLES2::GLESv2 freetype)

message(STATUS "[ImGuiX] ImGui with SDL2 + GLES2 backend built.")
