# ===== deps/sfml.cmake =====
# Purpose: Provide SFML targets (Graphics, Window, System) from submodule or system.
# Inputs:  IMGUIX_SFML_SRC_DIR
# Outputs: out_targets receives "SFML::Graphics;SFML::Window;SFML::System"
# Notes:   Ensures idempotent addition.

# Resolve SFML dependency
# Params:
# - out_targets: variable to receive semicolon-separated target list
# Behavior:
# - Reuses existing targets, adds submodule, or finds system package
# Usage:
#   imguix_use_or_fetch_sfml(SFML_TARGETS)
# Idempotent: guarded by global property
function(imguix_use_or_fetch_sfml out_targets)
    # Already available?
    if(TARGET SFML::Graphics AND TARGET SFML::Window AND TARGET SFML::System)
        set(${out_targets} "SFML::Graphics;SFML::Window;SFML::System" PARENT_SCOPE)
        return()
    endif()

    # Added before via this helper?
    get_property(_sfml_added GLOBAL PROPERTY IMGUIX_SFML_ADDED)
    if(_sfml_added)
        if(TARGET SFML::Graphics AND TARGET SFML::Window AND TARGET SFML::System)
            set(${out_targets} "SFML::Graphics;SFML::Window;SFML::System" PARENT_SCOPE)
            return()
        else()
            message(FATAL_ERROR "IMGUIX: SFML marked as added, but SFML targets are missing.")
        endif()
    endif()

    # Source dir can be overridden; default to project submodule
    if(NOT DEFINED IMGUIX_SFML_SRC_DIR OR IMGUIX_SFML_SRC_DIR STREQUAL "")
        set(IMGUIX_SFML_SRC_DIR "${PROJECT_SOURCE_DIR}/libs/SFML")
    endif()

    if(EXISTS "${IMGUIX_SFML_SRC_DIR}/CMakeLists.txt")
        # Unique binary dir to avoid collisions with any other add_subdirectory()
        set(_SFML_BIN "${CMAKE_BINARY_DIR}/_deps/sfml-build")

        # Minimal component config
        set(SFML_BUILD_GRAPHICS       ON  CACHE BOOL "" FORCE)
        set(SFML_BUILD_WINDOW         ON  CACHE BOOL "" FORCE)
        set(SFML_BUILD_AUDIO          OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_NETWORK        OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_EXAMPLES       OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_DOC            OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_TEST_SUITE     OFF CACHE BOOL "" FORCE)
        set(SFML_USE_SYSTEM_DEPS      OFF CACHE BOOL "" FORCE)
        set(SFML_ENABLE_PCH           OFF CACHE BOOL "" FORCE)
        # Respect BUILD_SHARED_LIBS from the top project; don't override here.

        add_subdirectory("${IMGUIX_SFML_SRC_DIR}" "${_SFML_BIN}")

        # Mark as added to keep idempotency
        set_property(GLOBAL PROPERTY IMGUIX_SFML_ADDED TRUE)

        # Expect SFML namespace targets (v3)
        set(${out_targets} "SFML::Graphics;SFML::Window;SFML::System" PARENT_SCOPE)
        return()
    else()
        # Fallback to system package
        find_package(SFML REQUIRED COMPONENTS Graphics Window System)
        set(${out_targets} "SFML::Graphics;SFML::Window;SFML::System" PARENT_SCOPE)
        return()
    endif()
endfunction()
