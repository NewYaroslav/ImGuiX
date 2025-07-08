# libs/cmake/imgui-wrapper.cmake

set(IMGUI_SRC      ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SFML_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml)
set(FREETYPE_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/freetype)

# Target directory for headers
set(IMGUI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)

# --- Create include structure ---
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR})
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/misc/freetype)
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/misc/cpp)

# --- Copy ImGui and ImGui-SFML headers ---
file(GLOB IMGUI_HEADERS "${IMGUI_SRC}/*.h")
file(GLOB IMGUI_SFML_HEADERS "${IMGUI_SFML_SRC}/*.h")

foreach(HDR ${IMGUI_HEADERS} ${IMGUI_SFML_HEADERS})
    configure_file(${HDR} ${IMGUI_INCLUDE_DIR}/ COPYONLY)
endforeach()

# --- Copy additional headers ---
configure_file(${IMGUI_SRC}/misc/freetype/imgui_freetype.h ${IMGUI_INCLUDE_DIR} COPYONLY)
configure_file(${IMGUI_SRC}/misc/cpp/imgui_stdlib.h        ${IMGUI_INCLUDE_DIR} COPYONLY)

# --- Copy selected config instead of generating ---
configure_file(${IMGUI_SFML_SRC}/imconfig-SFML.h ${IMGUI_INCLUDE_DIR}/imconfig-SFML.h COPYONLY)

# --- List of ImGui sources (used directly from upstream) ---
set(IMGUI_SOURCES
    ${IMGUI_SRC}/imgui.cpp
    ${IMGUI_SRC}/imgui_draw.cpp
    ${IMGUI_SRC}/imgui_widgets.cpp
    ${IMGUI_SRC}/imgui_tables.cpp
    ${IMGUI_SRC}/imgui_demo.cpp
    ${IMGUI_SRC}/misc/freetype/imgui_freetype.cpp
    ${IMGUI_SRC}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_SFML_SRC}/imgui-SFML.cpp
)

# --- Add FreeType as a subdirectory ---
if (NOT TARGET freetype)
    add_subdirectory(${FREETYPE_SRC} EXCLUDE_FROM_ALL)
endif()

# --- Create library ---
add_library(imgui STATIC ${IMGUI_SOURCES})

# --- Include paths (public for dependent projects) ---
target_include_directories(imgui
    PRIVATE ${FREETYPE_SRC}/include
            ${IMGUI_SRC}
            ${IMGUI_SFML_SRC}
)

# --- Define user configuration file ---
target_compile_definitions(imgui
    PUBLIC IMGUI_USER_CONFIG=\"imconfig-SFML.h\"
	PRIVATE IMGUI_ENABLE_FREETYPE
)

# --- Link with dependencies ---
target_link_libraries(imgui PRIVATE sfml-graphics sfml-window sfml-system freetype)

message(STATUS "[ImGuiX] FreeType support enabled.")
message(STATUS "[ImGuiX] Headers copied to: ${IMGUI_INCLUDE_DIR}")
message(STATUS "[ImGuiX] Using configuration: imconfig-SFML.h")
message(STATUS "[ImGuiX] ImGui and ImGui-SFML built as a single library.")
