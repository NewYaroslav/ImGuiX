# libs/cmake/imgui-wrapper.cmake

set(IMGUI_SRC      ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SFML_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml)
set(FREETYPE_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/freetype)

# Целевая директория заголовков
set(IMGUI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)

# --- Создаём include-структуру ---
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR})
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/misc/freetype)
file(MAKE_DIRECTORY ${IMGUI_INCLUDE_DIR}/misc/cpp)

# --- Копируем заголовки ImGui и ImGui-SFML ---
file(GLOB IMGUI_HEADERS "${IMGUI_SRC}/*.h")
file(GLOB IMGUI_SFML_HEADERS "${IMGUI_SFML_SRC}/*.h")

foreach(HDR ${IMGUI_HEADERS} ${IMGUI_SFML_HEADERS})
    configure_file(${HDR} ${IMGUI_INCLUDE_DIR}/ COPYONLY)
endforeach()

# --- Копируем дополнительные заголовки ---
configure_file(${IMGUI_SRC}/misc/freetype/imgui_freetype.h ${IMGUI_INCLUDE_DIR}/misc/freetype/ COPYONLY)
configure_file(${IMGUI_SRC}/misc/cpp/imgui_stdlib.h        ${IMGUI_INCLUDE_DIR}/misc/cpp/ COPYONLY)

# --- Копируем выбранный конфиг вместо генерации ---
configure_file(${IMGUI_SFML_SRC}/imconfig-SFML.h ${IMGUI_INCLUDE_DIR}/imconfig-SFML.h COPYONLY)

# --- Список исходников ImGui (используем напрямую из оригинала) ---
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

# --- Подключение FreeType как сабдиректории ---
if (NOT TARGET freetype)
    add_subdirectory(${FREETYPE_SRC} EXCLUDE_FROM_ALL)
endif()

# --- Создание библиотеки ---
add_library(imgui STATIC ${IMGUI_SOURCES})

# --- Пути к заголовкам (публичные — для зависимых проектов) ---
target_include_directories(imgui
    PRIVATE ${FREETYPE_SRC}/include
            ${IMGUI_SRC}
            ${IMGUI_SFML_SRC}
)

# --- Определение пользовательского конфигурационного файла ---
target_compile_definitions(imgui
    PUBLIC IMGUI_USER_CONFIG=\"imconfig-SFML.h\"
	PRIVATE IMGUI_ENABLE_FREETYPE
)

# --- Линковка с зависимостями ---
target_link_libraries(imgui PRIVATE sfml-graphics sfml-window sfml-system freetype)

message(STATUS "[ImGuiX] FreeType поддержка активирована.")
message(STATUS "[ImGuiX] Заголовки скопированы в: ${IMGUI_INCLUDE_DIR}")
message(STATUS "[ImGuiX] Конфигурация использует: imconfig-SFML.h")
message(STATUS "[ImGuiX] ImGui и ImGui-SFML собраны как единая библиотека.")