# cmake/deps/freetype.cmake
# Выдаёт: out_target = freetype (или Freetype::Freetype)
function(imguix_use_or_find_freetype out_target)
  # 0) Уже есть цель (например, её собрал SFML)
  if(TARGET Freetype::Freetype)
    set(${out_target} Freetype::Freetype PARENT_SCOPE)
    return()
  endif()
  if(TARGET freetype)
    set(${out_target} freetype PARENT_SCOPE)
    return()
  endif()

  # 1) Попробуем пакет из системы
  find_package(Freetype QUIET)
  if(Freetype_FOUND OR TARGET Freetype::Freetype)
    set(${out_target} Freetype::Freetype PARENT_SCOPE)
    return()
  endif()

  # 2) Режимы (INHERIT => глобальный)
  set(mode "${IMGUIX_DEPS_FREETYPE_MODE}")
  if(mode STREQUAL "INHERIT" OR mode STREQUAL "")
    set(mode "${IMGUIX_DEPS_MODE}")
  endif()
  if(mode STREQUAL "SYSTEM")
    message(FATAL_ERROR "Freetype not found in SYSTEM mode")
  endif()

  # 3) Сабмодуль?
  if(EXISTS "${CMAKE_SOURCE_DIR}/libs/freetype/CMakeLists.txt")
    add_subdirectory("${CMAKE_SOURCE_DIR}/libs/freetype"
                     "${CMAKE_BINARY_DIR}/libs/freetype" EXCLUDE_FROM_ALL)
    # Официальные цели фритайпа:
    if(TARGET Freetype::Freetype)
      set(${out_target} Freetype::Freetype PARENT_SCOPE)
    elseif(TARGET freetype)
      set(${out_target} freetype PARENT_SCOPE)
    else()
      message(FATAL_ERROR "Freetype submodule added but target not found")
    endif()
    return()
  endif()

  # 4) FetchContent (как последний шанс)
  include(FetchContent)
  set(_ft_tag "${IMGUIX_FREETYPE_GIT_TAG}")
  if(NOT _ft_tag)
    set(_ft_tag "VER-2-13-2")  # актуальную метку при необходимости поменять
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
