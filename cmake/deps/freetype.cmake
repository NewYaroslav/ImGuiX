# ===== deps/freetype.cmake =====
# Purpose: Provide Freetype target from system, submodule, or FetchContent.
# Inputs:  IMGUIX_DEPS_MODE, IMGUIX_DEPS_FREETYPE_MODE, IMGUIX_FREETYPE_GIT_TAG
# Outputs: out_target receives Freetype::Freetype or freetype
# Notes:   Uses system package when available.

# Resolve Freetype dependency
# Params:
# - out_target: variable to receive target name
# Behavior:
# - Reuses existing target, finds system package, or fetches from source
# Usage:
#   imguix_use_or_find_freetype(FREETYPE_TARGET)
# Idempotent: reuse target if already created
function(imguix_use_or_find_freetype out_target)
  # 0) Existing target (e.g., provided by SFML)
  if(TARGET Freetype::Freetype)
    set(${out_target} Freetype::Freetype PARENT_SCOPE)
    return()
  endif()
  if(TARGET freetype)
    set(${out_target} freetype PARENT_SCOPE)
    return()
  endif()

  # 1) Try system package
  find_package(Freetype QUIET)
  if(Freetype_FOUND OR TARGET Freetype::Freetype)
    set(${out_target} Freetype::Freetype PARENT_SCOPE)
    return()
  endif()

  # 2) Modes (INHERIT uses global mode)
  set(mode "${IMGUIX_DEPS_FREETYPE_MODE}")
  if(mode STREQUAL "INHERIT" OR mode STREQUAL "")
    set(mode "${IMGUIX_DEPS_MODE}")
  endif()
  if(mode STREQUAL "SYSTEM")
    message(FATAL_ERROR "Freetype not found in SYSTEM mode")
  endif()

  # 3) Submodule
  if(EXISTS "${CMAKE_SOURCE_DIR}/libs/freetype/CMakeLists.txt")
    add_subdirectory("${CMAKE_SOURCE_DIR}/libs/freetype"
                     "${CMAKE_BINARY_DIR}/libs/freetype" EXCLUDE_FROM_ALL)
    # Official Freetype targets
    if(TARGET Freetype::Freetype)
      set(${out_target} Freetype::Freetype PARENT_SCOPE)
    elseif(TARGET freetype)
      set(${out_target} freetype PARENT_SCOPE)
    else()
      message(FATAL_ERROR "Freetype submodule added but target not found")
    endif()
    return()
  endif()

  # 4) FetchContent as last resort
  include(FetchContent)
  set(_ft_tag "${IMGUIX_FREETYPE_GIT_TAG}")
  if(NOT _ft_tag)
    set(_ft_tag "VER-2-13-2")  # update tag if needed
  endif()
  FetchContent_Declare(
    freetype_ext
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG        ${_ft_tag}
  )
  FetchContent_MakeAvailable(freetype_ext)

  if(TARGET Freetype::Freetype)
    set(${out_target} Freetype::Freetype PARENT_SCOPE)
  elseif(TARGET freetype)
    set(${out_target} freetype PARENT_SCOPE)
  else()
    message(FATAL_ERROR "Freetype fetched but target not found")
  endif()
endfunction()
