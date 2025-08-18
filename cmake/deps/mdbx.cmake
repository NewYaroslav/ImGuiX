# cmake/deps/mdbx.cmake
# imguix_use_or_fetch_mdbx(<out_target>)
function(imguix_use_or_fetch_mdbx out_target)
    # ---- Effective mode ----
    set(mode "${IMGUIX_DEPS_MDBX_MODE}")
    if(mode STREQUAL "INHERIT")
        set(mode "${IMGUIX_DEPS_MODE}")
    endif()

    # ---- Idempotent: if already have a target -> return ----
    if(TARGET mdbx::mdbx)
        set(${out_target} mdbx::mdbx PARENT_SCOPE)
        return()
    elseif(TARGET mdbx-static)
        add_library(mdbx::mdbx ALIAS mdbx-static)
        set(${out_target} mdbx::mdbx PARENT_SCOPE)
        return()
    elseif(TARGET mdbx)
        add_library(mdbx::mdbx ALIAS mdbx)
        set(${out_target} mdbx::mdbx PARENT_SCOPE)
        return()
    endif()

    # ---- Try SYSTEM/AUTO via find_package ----
    if(NOT mode STREQUAL "BUNDLED")
        # CONFIG first
        find_package(MDBX CONFIG QUIET)
        if(NOT TARGET mdbx::mdbx)
            # Some distros don't provide namespaced target; try MODULE
            find_package(MDBX QUIET MODULE)
        endif()

        if(TARGET mdbx::mdbx)
            set(${out_target} mdbx::mdbx PARENT_SCOPE)
            return()
        elseif(TARGET mdbx-static)
            add_library(mdbx::mdbx ALIAS mdbx-static)
            set(${out_target} mdbx::mdbx PARENT_SCOPE)
            return()
        elseif(TARGET mdbx)
            add_library(mdbx::mdbx ALIAS mdbx)
            set(${out_target} mdbx::mdbx PARENT_SCOPE)
            return()
        endif()
    endif()

    if(mode STREQUAL "SYSTEM")
        message(FATAL_ERROR "MDBX not found in SYSTEM mode")
    endif()

    # ---- BUNDLED/AUTO: submodule? ----
    set(_MDBX_SRC "${PROJECT_SOURCE_DIR}/libs/libmdbx")
    if(EXISTS "${_MDBX_SRC}/CMakeLists.txt")
        # Build options BEFORE add_subdirectory
        set(MDBX_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
        set(MDBX_BUILD_TOOLS          OFF CACHE BOOL "" FORCE)
        if(IMGUIX_SDK_BUNDLE_DEPS)
            # чтобы install() сабпроекта поставил статическую либу (+заголовки у них тоже инсталлятся)
            set(MDBX_INSTALL_STATIC ON CACHE BOOL "" FORCE)
        endif()

        # Unique binary dir
        add_subdirectory("${_MDBX_SRC}" "${CMAKE_BINARY_DIR}/_deps/mdbx-build")

        if(TARGET mdbx-static)
            add_library(mdbx::mdbx ALIAS mdbx-static)
        elseif(TARGET mdbx)
            add_library(mdbx::mdbx ALIAS mdbx)
        else()
            message(FATAL_ERROR "libmdbx submodule added but no known target (mdbx / mdbx-static) was created")
        endif()

        set(${out_target} mdbx::mdbx PARENT_SCOPE)
        return()
    endif()

    # ---- Fallback: FetchContent ----
    include(FetchContent)
    # Options must be set BEFORE MakeAvailable
    set(MDBX_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
    set(MDBX_BUILD_TOOLS          OFF CACHE BOOL "" FORCE)
    if(IMGUIX_SDK_BUNDLE_DEPS)
        set(MDBX_INSTALL_STATIC ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_Declare(
        libmdbx
        GIT_REPOSITORY https://github.com/erthink/libmdbx.git
        GIT_TAG stable
    )
    FetchContent_MakeAvailable(libmdbx)

    if(TARGET mdbx-static)
        add_library(mdbx::mdbx ALIAS mdbx-static)
    elseif(TARGET mdbx)
        add_library(mdbx::mdbx ALIAS mdbx)
    else()
        message(FATAL_ERROR "Unknown libmdbx target name after FetchContent; check upstream CMake")
    endif()

    set(${out_target} mdbx::mdbx PARENT_SCOPE)
endfunction()
