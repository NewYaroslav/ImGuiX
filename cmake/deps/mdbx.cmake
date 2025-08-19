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
        
        # --- Ensure version file exists for libmdbx (CI-friendly) ---
        # Требуется: git-репозиторий ИЛИ корректный VERSION.json с полями:
        # git_describe, git_timestamp, git_tree, git_commit, semver.
        # --- Decide: use git metadata OR fallback VERSION.json (only one allowed) ---
        # --- Minimal: CI uses VERSION.json, local uses git ---
		
		# CI detector
		set(_IN_CI FALSE)
		if(DEFINED ENV{GITHUB_ACTIONS} AND "$ENV{GITHUB_ACTIONS}" STREQUAL "true")
		  set(_IN_CI TRUE)
		endif()

		find_program(GIT_EXECUTABLE git)

		# Попробуем заранее вытащить версию из git (если есть), чтобы использовать в JSON
		set(_tag "")
		set(_ver "0.0.0")
		set(_commit "")
		set(_tree "")
		set(_ts 0)

		if(GIT_EXECUTABLE)
		  execute_process(
			COMMAND ${GIT_EXECUTABLE} -C "${_MDBX_SRC}" describe --tags --abbrev=0
			OUTPUT_VARIABLE _tag OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE _rc_tag
		  )
		  if(_rc_tag EQUAL 0 AND NOT _tag STREQUAL "")
			string(REGEX REPLACE "^v" "" _ver "${_tag}")
		  endif()

		  execute_process(
			COMMAND ${GIT_EXECUTABLE} -C "${_MDBX_SRC}" rev-parse --verify HEAD
			OUTPUT_VARIABLE _commit OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE _rc_commit
		  )
		  execute_process(
			COMMAND ${GIT_EXECUTABLE} -C "${_MDBX_SRC}" show -s --format=%T HEAD
			OUTPUT_VARIABLE _tree OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE _rc_tree
		  )
		  execute_process(
			COMMAND ${GIT_EXECUTABLE} -C "${_MDBX_SRC}" log -1 --format=%ct
			OUTPUT_VARIABLE _ts OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE _rc_ts
		  )
		  if(NOT _rc_ts EQUAL 0 OR _ts STREQUAL "")
			set(_ts 0)
		  endif()
		endif()

		if(_IN_CI)
		  # Скрыть git, чтобы не было двух источников версии
		  if(EXISTS "${_MDBX_SRC}/.git")
			file(RENAME "${_MDBX_SRC}/.git" "${_MDBX_SRC}/.git.ci.bak")
			message(STATUS "libmdbx: CI mode → hid .git to avoid dual version sources")
		  endif()

		  # Сгенерировать VERSION.json только если его нет
		  if(NOT EXISTS "${_MDBX_SRC}/VERSION.json")
			set(_git_describe "v${_ver}")
			if(NOT _tag STREQUAL "")
			  set(_git_describe "${_tag}")
			endif()

			file(WRITE "${_MDBX_SRC}/VERSION.json"
			  "{\n"
			  "  \"git_describe\": \"${_git_describe}\",\n"
			  "  \"git_timestamp\": ${_ts},\n"
			  "  \"git_tree\": \"${_tree}\",\n"
			  "  \"git_commit\": \"${_commit}\",\n"
			  "  \"semver\": \"${_ver}\"\n"
			  "}\n")
			message(STATUS "libmdbx: wrote VERSION.json (semver=${_ver})")
		  endif()
		else()
		  # Локально: гарантируем, что JSON не мешает git-версии
		  if(EXISTS "${_MDBX_SRC}/VERSION.json")
			file(REMOVE "${_MDBX_SRC}/VERSION.json")
			message(STATUS "libmdbx: local mode → removed VERSION.json (use git metadata)")
		  endif()
		endif()
		# ---

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
