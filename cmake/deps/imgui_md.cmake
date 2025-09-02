# Provides: imguix_use_or_fetch_imgui_md(<OUT_VAR>)
# Exports target: imgui_md::imgui_md
function(imguix_use_or_fetch_imgui_md OUT_VAR)
    set(_IMGUI_MD_DIR "${IMGUI_MD_DIR}")
    if(NOT _IMGUI_MD_DIR)
        set(_IMGUI_MD_DIR "${PROJECT_SOURCE_DIR}/libs/imgui_md")
    endif()
    if(NOT EXISTS "${_IMGUI_MD_DIR}/imgui_md.cpp")
        message(FATAL_ERROR "imgui_md not found at ${_IMGUI_MD_DIR}.")
    endif()

    # --- resolve md4c ---
    set(MD4C_TGT "")
    set(_MD4C_INC "")  # NEW: будем хранить путь к заголовкам md4c

    # 1) system md4c (vcpkg и т.п.)
    find_path(MD4C_INCLUDE_DIR NAMES md4c.h PATH_SUFFIXES include)
    find_library(MD4C_LIBRARY NAMES md4c)
    if(MD4C_INCLUDE_DIR AND MD4C_LIBRARY)
        add_library(md4c_sys UNKNOWN IMPORTED)
        set_target_properties(md4c_sys PROPERTIES
            IMPORTED_LOCATION "${MD4C_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MD4C_INCLUDE_DIR}")
        set(MD4C_TGT md4c_sys)
        set(_MD4C_INC "${MD4C_INCLUDE_DIR}")         # NEW
    endif()

    # 2) bundled md4c
    if(NOT MD4C_TGT)
        set(_MD4C_DIR "${MD4C_DIR}")
        if(NOT _MD4C_DIR)
            set(_MD4C_DIR "${PROJECT_SOURCE_DIR}/libs/md4c")
        endif()

        if(EXISTS "${_MD4C_DIR}/CMakeLists.txt")
            add_subdirectory("${_MD4C_DIR}" "${CMAKE_BINARY_DIR}/_deps/md4c")
            if(TARGET md4c)
                set(MD4C_TGT md4c)
                # Попытка вытащить include из таргета (на случай, если понадобится)
                get_target_property(_tmp_inc md4c INTERFACE_INCLUDE_DIRECTORIES)
                if(_tmp_inc) 
                    set(_MD4C_INC "${_tmp_inc}") 
                endif()
            elseif(TARGET MD4C::MD4C)
                set(MD4C_TGT MD4C::MD4C)
                # Обычно у них правильный include внутри таргета, но подстрахуемся:
                set(_MD4C_INC "${_MD4C_DIR}/src")
            else()
                message(FATAL_ERROR "md4c target not found after add_subdirectory(${_MD4C_DIR})")
            endif()
        else()
            # прямой .c (обычно в src/)
            if(EXISTS "${_MD4C_DIR}/md4c.c")
                set(_MD4C_SRC "${_MD4C_DIR}/md4c.c")
                set(_MD4C_INC "${_MD4C_DIR}")
            elseif(EXISTS "${_MD4C_DIR}/src/md4c.c")
                set(_MD4C_SRC "${_MD4C_DIR}/src/md4c.c")
                set(_MD4C_INC "${_MD4C_DIR}/src")
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

    # --- imgui_md ---
    add_library(imgui_md STATIC "${_IMGUI_MD_DIR}/imgui_md.cpp")
    add_library(imgui_md::imgui_md ALIAS imgui_md)
    target_include_directories(imgui_md PUBLIC "${_IMGUI_MD_DIR}")

    # Fallback: если _MD4C_INC не определён — вычисляем по факту расположения заголовка
    if(NOT _MD4C_INC)
        if(DEFINED MD4C_INCLUDE_DIR AND EXISTS "${MD4C_INCLUDE_DIR}/md4c.h")
            set(_MD4C_INC "${MD4C_INCLUDE_DIR}")
        elseif(DEFINED _MD4C_DIR AND EXISTS "${_MD4C_DIR}/src/md4c.h")
            set(_MD4C_INC "${_MD4C_DIR}/src")
        elseif(DEFINED _MD4C_DIR AND EXISTS "${_MD4C_DIR}/md4c.h")
            set(_MD4C_INC "${_MD4C_DIR}")
        else()
            message(FATAL_ERROR "imgui_md: cannot locate md4c.h; set MD4C_DIR to md4c root (with src/md4c.h).")
        endif()
    endif()

    # чтобы imgui_md.cpp видел "md4c.h"
    target_include_directories(imgui_md PRIVATE "${_MD4C_INC}")

    # md4c тянем PUBLIC — чтобы у потребителей тоже были его usage-reqs
    target_link_libraries(imgui_md PUBLIC imgui::imgui ${MD4C_TGT})

    set_target_properties(imgui_md PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_IMGUI_MD_DIR}/imgui_md.h"
    )

    message(STATUS "imgui_md: using md4c includes at: ${_MD4C_INC}")

    set(${OUT_VAR} imgui_md::imgui_md PARENT_SCOPE)
endfunction()

