# cmake/deps/fmt.cmake
# imguix_use_or_fetch_fmt(<out_target>)
function(imguix_use_or_fetch_fmt out_target)
    # Resolve effective mode
    set(mode "${IMGUIX_DEPS_FMT_MODE}")
    if(mode STREQUAL "INHERIT")
        set(mode "${IMGUIX_DEPS_MODE}")
    endif()

    # Idempotent: already have the target?
    if(TARGET fmt::fmt)
        set(${out_target} fmt::fmt PARENT_SCOPE)
        return()
    endif()

    # Try SYSTEM/AUTO first
    if(NOT mode STREQUAL "BUNDLED")
        find_package(fmt CONFIG QUIET)
        if(TARGET fmt::fmt)
            set(${out_target} fmt::fmt PARENT_SCOPE)
            return()
        endif()
    endif()

    if(mode STREQUAL "SYSTEM")
        message(FATAL_ERROR "fmt not found in SYSTEM mode")
    endif()

    # BUNDLED: submodule first (relative to this project, not top level)
    set(_FMT_SRC "${PROJECT_SOURCE_DIR}/libs/fmt")
    if(EXISTS "${_FMT_SRC}/CMakeLists.txt")
        if(IMGUIX_SDK_BUNDLE_DEPS)
            set(FMT_INSTALL ON CACHE BOOL "" FORCE)
        endif()
        # unique binary dir to avoid collisions
        add_subdirectory("${_FMT_SRC}" "${CMAKE_BINARY_DIR}/_deps/fmt-build")
        set(${out_target} fmt::fmt PARENT_SCOPE)
        return()
    endif()

    # Fallback: FetchContent
    include(FetchContent)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.2.0
    )
    if(IMGUIX_SDK_BUNDLE_DEPS)
        set(FMT_INSTALL ON CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(fmt)
    set(${out_target} fmt::fmt PARENT_SCOPE)
endfunction()
