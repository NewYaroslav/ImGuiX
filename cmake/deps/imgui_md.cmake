# Provides: imguix_use_or_fetch_imgui_md(<OUT_VAR>)
# Exports target: imgui_md::imgui_md
function(imguix_use_or_fetch_imgui_md OUT_VAR)
    # --- paths ---
    set(_IMGUI_MD_DIR "${IMGUI_MD_DIR}")
    if(NOT _IMGUI_MD_DIR)
        set(_IMGUI_MD_DIR "${PROJECT_SOURCE_DIR}/libs/imgui_md")
    endif()
    if(NOT EXISTS "${_IMGUI_MD_DIR}/imgui_md.cpp")
        message(FATAL_ERROR "imgui_md not found at ${_IMGUI_MD_DIR}.")
    endif()

    # --- resolve md4c (system or bundled) ---
    set(MD4C_TGT "")
    set(_MD4C_INC "")

    # 1) Try system md4c (vcpkg/OS packages)
    find_path(MD4C_INCLUDE_DIR NAMES md4c.h PATH_SUFFIXES include)
    find_library(MD4C_LIBRARY NAMES md4c)
    if(MD4C_INCLUDE_DIR AND MD4C_LIBRARY)
        add_library(md4c_sys UNKNOWN IMPORTED)
        set_target_properties(md4c_sys PROPERTIES
            IMPORTED_LOCATION "${MD4C_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MD4C_INCLUDE_DIR}"
        )
        set(MD4C_TGT md4c_sys)
        set(_MD4C_INC "${MD4C_INCLUDE_DIR}")
    endif()

    # 2) Bundled md4c (submodule or sources)
    if(NOT MD4C_TGT)
        set(_MD4C_DIR "${MD4C_DIR}")
        if(NOT _MD4C_DIR)
            set(_MD4C_DIR "${PROJECT_SOURCE_DIR}/libs/md4c")
        endif()

        if(IMGUIX_MD4C_USE_SUBDIR AND EXISTS "${_MD4C_DIR}/CMakeLists.txt")
            add_subdirectory("${_MD4C_DIR}" "${CMAKE_BINARY_DIR}/_deps/md4c")
            if(TARGET MD4C::MD4C)
                set(MD4C_TGT MD4C::MD4C)
            elseif(TARGET md4c)
                set(MD4C_TGT md4c)
            else()
                message(FATAL_ERROR "md4c target not found after add_subdirectory(${_MD4C_DIR})")
            endif()
            # try to read include dir from target; else fallback to src/ or root
            get_target_property(_tmp_inc ${MD4C_TGT} INTERFACE_INCLUDE_DIRECTORIES)
            if(_tmp_inc)
                list(GET _tmp_inc 0 _MD4C_INC)
            elseif(EXISTS "${_MD4C_DIR}/src/md4c.h")
                set(_MD4C_INC "${_MD4C_DIR}/src")
            else()
                set(_MD4C_INC "${_MD4C_DIR}")
            endif()
        else()
            # compile directly from sources (md4c.c in src/ or root)
            if(EXISTS "${_MD4C_DIR}/src/md4c.c")
                set(_MD4C_SRC "${_MD4C_DIR}/src/md4c.c")
                set(_MD4C_INC "${_MD4C_DIR}/src")
            elseif(EXISTS "${_MD4C_DIR}/md4c.c")
                set(_MD4C_SRC "${_MD4C_DIR}/md4c.c")
                set(_MD4C_INC "${_MD4C_DIR}")
            else()
                message(FATAL_ERROR "md4c.c not found in ${_MD4C_DIR} or ${_MD4C_DIR}/src.")
            endif()
            enable_language(C)
            add_library(md4c STATIC "${_MD4C_SRC}")
            target_include_directories(md4c PUBLIC "${_MD4C_INC}")
            set_target_properties(md4c PROPERTIES POSITION_INDEPENDENT_CODE ON)
            set(MD4C_TGT md4c)
        endif()
    endif()

    # --- imgui_md target ---
    add_library(imgui_md STATIC "${_IMGUI_MD_DIR}/imgui_md.cpp")
    add_library(imgui_md::imgui_md ALIAS imgui_md)

    target_include_directories(imgui_md
        PUBLIC
            $<BUILD_INTERFACE:${_IMGUI_MD_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    
    target_include_directories(imgui_md
        PRIVATE "${_MD4C_INC}"
    )

    target_link_libraries(imgui_md PUBLIC imgui::imgui ${MD4C_TGT})

    set_target_properties(imgui_md PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_IMGUI_MD_DIR}/imgui_md.h"
    )

    message(STATUS "imgui_md: using md4c target: ${MD4C_TGT}")
    message(STATUS "imgui_md: using md4c includes at: ${_MD4C_INC}")

    set(${OUT_VAR} imgui_md::imgui_md PARENT_SCOPE)
endfunction()
