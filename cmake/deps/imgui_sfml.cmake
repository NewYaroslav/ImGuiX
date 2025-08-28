# cmake/deps/imgui_sfml.cmake
# Provide ImGui-SFML target and return it as both SFML-wrapper and IMGUI target.
# Usage:
#   imguix_use_or_fetch_imgui_sfml(IMGUI_SFML_TARGET OUT_IMGUI_TARGET)
# Output:
#   IMGUI_SFML_TARGET = ImGui-SFML
#   OUT_IMGUI_TARGET  = ImGui-SFML
function(imguix_use_or_fetch_imgui_sfml out_sfml out_imgui)
    # Resolve dirs
    if(NOT DEFINED IMGUI_DIR OR IMGUI_DIR STREQUAL "")
        set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui")
    endif()
    if(NOT DEFINED IMGUI_SFML_DIR OR IMGUI_SFML_DIR STREQUAL "")
        set(IMGUI_SFML_DIR "${PROJECT_SOURCE_DIR}/libs/imgui-sfml")
    endif()

    # Create ImGui-SFML if not exists
    set(_created FALSE)
    if(NOT TARGET ImGui-SFML)
        add_library(ImGui-SFML STATIC
            "${IMGUI_SFML_DIR}/imgui-SFML.cpp"
            "${IMGUI_DIR}/imgui.cpp"
            "${IMGUI_DIR}/imgui_draw.cpp"
            "${IMGUI_DIR}/imgui_widgets.cpp"
            "${IMGUI_DIR}/imgui_tables.cpp"
            # "${IMGUI_DIR}/imgui_demo.cpp"
            "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp"
        )
        add_library(ImGui-SFML::ImGui-SFML ALIAS ImGui-SFML)

        # Public includes for consumers
        target_include_directories(ImGui-SFML PUBLIC
            $<BUILD_INTERFACE:${IMGUI_SFML_DIR}>
            $<INSTALL_INTERFACE:include>
        )
        # Treat ImGui headers as system for warnings
        target_include_directories(ImGui-SFML SYSTEM PUBLIC
            $<BUILD_INTERFACE:${IMGUI_DIR}>
        )

        # OpenGL and SFML are required for compilation of this target
        find_package(OpenGL REQUIRED)

        # Get SFML targets (use existing, helper, or system package)
        if(TARGET SFML::Graphics AND TARGET SFML::Window AND TARGET SFML::System)
            set(_SFML_TARGETS_LOCAL SFML::Graphics SFML::Window SFML::System)
        elseif(COMMAND imguix_use_or_fetch_sfml)
            imguix_use_or_fetch_sfml(_SFML_TARGETS_LOCAL)
        else()
            find_package(SFML REQUIRED COMPONENTS Graphics Window System)
            set(_SFML_TARGETS_LOCAL SFML::Graphics SFML::Window SFML::System)
        endif()
        target_link_libraries(ImGui-SFML PUBLIC ${_SFML_TARGETS_LOCAL} OpenGL::GL)

        # Win32 extra (as in upstream)
        if(WIN32 AND MINGW)
            target_link_libraries(ImGui-SFML PUBLIC imm32)
        endif()

        # IMGUI_USER_CONFIG (default to upstream imconfig-SFML.h)
        if(NOT DEFINED IMGUI_SFML_CONFIG_DIR OR IMGUI_SFML_CONFIG_DIR STREQUAL "")
            set(IMGUI_SFML_CONFIG_DIR "${IMGUI_SFML_DIR}")
        endif()
        if(NOT DEFINED IMGUI_SFML_CONFIG_NAME OR IMGUI_SFML_CONFIG_NAME STREQUAL "")
            set(IMGUI_SFML_CONFIG_NAME "imconfig-SFML.h")
        endif()
        
        if(DEFINED IMGUIX_USER_CONFIG_DIR AND DEFINED IMGUIX_USER_CONFIG_NAME)
            target_include_directories(ImGui-SFML PUBLIC
                $<BUILD_INTERFACE:${IMGUIX_USER_CONFIG_DIR}>
            )
            target_compile_definitions(ImGui-SFML PUBLIC
                IMGUI_USER_CONFIG="${IMGUIX_USER_CONFIG_NAME}"
            )
        else()
            target_include_directories(ImGui-SFML PUBLIC
                $<BUILD_INTERFACE:${IMGUI_SFML_CONFIG_DIR}>
            )
            target_compile_definitions(ImGui-SFML PUBLIC
                IMGUI_USER_CONFIG="${IMGUI_SFML_CONFIG_NAME}"
            )
        endif()

        set(_created TRUE)
    endif()

    # Optional FreeType integration on this target
    if(IMGUIX_IMGUI_FREETYPE)
        if(EXISTS "${IMGUI_DIR}/misc/freetype/imgui_freetype.cpp")
            # Avoid double-adding: guard by a target property
            get_target_property(_ft_added ImGui-SFML IMGUIX_FT_ADDED)
            if(NOT _ft_added)
                target_sources(ImGui-SFML PRIVATE
                    "${IMGUI_DIR}/misc/freetype/imgui_freetype.cpp"
                )
                target_include_directories(ImGui-SFML PRIVATE
                    "${IMGUI_DIR}/misc/freetype"
                )
                target_compile_definitions(ImGui-SFML PUBLIC IMGUI_ENABLE_FREETYPE)

                # Link freetype (prefer project helper)
                if(COMMAND imguix_use_or_find_freetype)
                    imguix_use_or_find_freetype(_FREETYPE_TGT)
                else()
                    find_package(Freetype REQUIRED)
                    set(_FREETYPE_TGT Freetype::Freetype)
                endif()
                target_link_libraries(ImGui-SFML PRIVATE ${_FREETYPE_TGT})

                # Mark as added to be idempotent
                set_target_properties(ImGui-SFML PROPERTIES IMGUIX_FT_ADDED TRUE)
            endif()
        else()
            message(FATAL_ERROR "imgui_freetype.cpp not found in ${IMGUI_DIR}/misc/freetype")
        endif()
    endif()

    # Return targets
    set(${out_sfml} "ImGui-SFML" PARENT_SCOPE)
    set(${out_imgui} "ImGui-SFML" PARENT_SCOPE)
endfunction()
