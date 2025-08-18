# cmake/deps/nlohmann_json.cmake
function(imguix_use_or_fetch_nlohmann_json out_target)
  # 0) Система
  find_package(nlohmann_json CONFIG QUIET)
  if(TARGET nlohmann_json::nlohmann_json)
    set(${out_target} nlohmann_json::nlohmann_json PARENT_SCOPE)
    return()
  endif()

  # 1) Режимы
  set(mode "${IMGUIX_DEPS_JSON_MODE}")
  if(mode STREQUAL "INHERIT" OR mode STREQUAL "")
    set(mode "${IMGUIX_DEPS_MODE}")
  endif()
  if(mode STREQUAL "SYSTEM")
    message(FATAL_ERROR "nlohmann_json not found in SYSTEM mode")
  endif()

  # 2) Сабмодуль
  if(EXISTS "${CMAKE_SOURCE_DIR}/libs/json/CMakeLists.txt")
    add_subdirectory("${CMAKE_SOURCE_DIR}/libs/json"
                     "${CMAKE_BINARY_DIR}/libs/json" EXCLUDE_FROM_ALL)
  else()
    # 3) (опционально) FetchContent — если разрешишь сеть
    # include(FetchContent)
    # FetchContent_Declare(nlohmann_json
    #   GIT_REPOSITORY https://github.com/nlohmann/json.git
    #   GIT_TAG        v3.11.3
    # )
    # FetchContent_MakeAvailable(nlohmann_json)
    message(FATAL_ERROR "nlohmann_json: no system package and no submodule at libs/json")
  endif()

  # 4) Нормализуем имя цели → импортное
  if(TARGET nlohmann_json::nlohmann_json)
    set(${out_target} nlohmann_json::nlohmann_json PARENT_SCOPE)
  elseif(TARGET nlohmann_json)
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
    set(${out_target} nlohmann_json::nlohmann_json PARENT_SCOPE)
  else()
    message(FATAL_ERROR "nlohmann_json target not found after add_subdirectory()")
  endif()
endfunction()
