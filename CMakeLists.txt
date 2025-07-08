cmake_minimum_required(VERSION 3.18)
project(ImGuiX LANGUAGES CXX)

option(IMGUIX_BUILD_STATIC_LIB    "Build ImGuiX as a static library" ON)
option(IMGUIX_USE_SFML_BACKEND   "Use SFML backend for ImGui" ON)
option(IMGUIX_BUILD_EXAMPLES     "Build examples in examples/ directory" ON)
option(IMGUIX_BUILD_TESTS        "Build tests in tests/ directory" ON)

# --- Output directories ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)

# --- Use only build/include ---
# include_directories(${CMAKE_BINARY_DIR}/include)

# --- Add external libraries ---
add_subdirectory(libs)

# --- ImGuiX source files ---
file(GLOB_RECURSE IMGUIX_HEADERS "include/imguix/*.hpp" "include/imguix/*.ipp")
file(GLOB_RECURSE IMGUIX_SOURCES "src/*.cpp")

# --- Build static lib if requested ---
if(IMGUIX_BUILD_STATIC_LIB)
    add_library(imguix STATIC ${IMGUIX_SOURCES} ${IMGUIX_HEADERS})
    target_include_directories(imguix PUBLIC ${CMAKE_BINARY_DIR}/include)

    if(IMGUIX_USE_SFML_BACKEND)
        target_link_libraries(imguix PUBLIC imgui sfml-graphics sfml-window sfml-system)
        target_compile_definitions(imguix PUBLIC IMGUIX_USE_SFML)
    endif()

    set_target_properties(imguix PROPERTIES OUTPUT_NAME "imguix")
endif()

# --- Build examples ---
if(IMGUIX_BUILD_EXAMPLES)
    file(GLOB EXAMPLES "examples/*.cpp")
    foreach(EXAMPLE_FILE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)

        add_executable(${EXAMPLE_NAME} ${EXAMPLE_FILE})
        target_include_directories(${EXAMPLE_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)

        if(IMGUIX_BUILD_STATIC_LIB)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE imguix)
        else()
            target_sources(${EXAMPLE_NAME} PRIVATE ${IMGUIX_HEADERS} ${IMGUIX_SOURCES})
            if(IMGUIX_USE_SFML_BACKEND)
                target_link_libraries(${TEST_NAME}
                    PRIVATE
                        freetype
                        sfml-graphics-s
                        sfml-window-s
                        sfml-system-s
                        imgui
                )
                target_compile_definitions(${EXAMPLE_NAME} PRIVATE IMGUIX_USE_SFML)
            endif()
        endif()

        copy_imgui_fonts(${EXAMPLE_NAME})
    endforeach()
endif()

# --- Build tests ---
if(IMGUIX_BUILD_TESTS)
    file(GLOB TESTS "tests/*.cpp")
    foreach(TEST_FILE ${TESTS})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

        add_executable(${TEST_NAME} ${TEST_FILE})
        target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)
        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
        target_compile_definitions(${TEST_NAME} PRIVATE SFML_STATIC)

        target_link_directories(${TEST_NAME} PRIVATE ${CMAKE_BINARY_DIR}/libs)
        target_link_libraries(${TEST_NAME}
            PRIVATE
                freetype
                sfml-graphics-s
                sfml-window-s
                sfml-system-s
                imgui
        )
        if (WIN32)
            target_link_libraries(${TEST_NAME} PRIVATE opengl32 gdi32 user32 kernel32 winmm)
        elseif(UNIX)
            target_link_libraries(${TEST_NAME} PRIVATE GL)
        endif()
        
        copy_imgui_fonts(${TEST_NAME})
    endforeach()

    # Also define SFML_STATIC in imgui if it was not set earlier
    target_compile_definitions(imgui PUBLIC SFML_STATIC)
endif()