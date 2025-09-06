# Provides: imguix_use_or_fetch_pfd(<OUT_VAR>)
# Exports target: pfd::pfd (header-only)
function(imguix_use_or_fetch_pfd OUT_VAR)
    # Try system package first if user chose AUTO/SYSTEM
    if(NOT DEFINED IMGUIX_DEPS_PFD_MODE)
        set(IMGUIX_DEPS_PFD_MODE "INHERIT")
    endif()
    set(_mode "${IMGUIX_DEPS_PFD_MODE}")
    if(_mode STREQUAL "INHERIT")
        set(_mode "${IMGUIX_DEPS_MODE}")
    endif()

    set(_found FALSE)
    if(_mode STREQUAL "AUTO" OR _mode STREQUAL "SYSTEM")
        # vcpkg provides CONFIG package
        find_package(portable-file-dialogs CONFIG QUIET)
        if(TARGET portable-file-dialogs::portable-file-dialogs)
            add_library(pfd::pfd ALIAS portable-file-dialogs::portable-file-dialogs)
            set(${OUT_VAR} pfd::pfd PARENT_SCOPE)
            set(_found TRUE)
        endif()
    endif()
    if(_found)
        return()
    endif()

    # BUNDLED (or AUTO fallback)
    set(_PFD_DIR "${PFD_DIR}")
    if(NOT _PFD_DIR)
        set(_PFD_DIR "${PROJECT_SOURCE_DIR}/libs/portable-file-dialogs")
    endif()
    if(NOT EXISTS "${_PFD_DIR}/portable-file-dialogs.h")
        message(FATAL_ERROR "portable-file-dialogs not found at ${_PFD_DIR}. "
                            "Set PFD_DIR or add submodule libs/portable-file-dialogs.")
    endif()

    add_library(pfd INTERFACE)
    add_library(pfd::pfd ALIAS pfd)
    target_include_directories(pfd INTERFACE
        $<BUILD_INTERFACE:${_PFD_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    # header-only: никаких линковок
    set(${OUT_VAR} pfd::pfd PARENT_SCOPE)
endfunction()
