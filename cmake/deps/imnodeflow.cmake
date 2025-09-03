# cmake/deps/imnodeflow.cmake
# Provides: imguix_use_or_fetch_imnodeflow(<OUT_VAR>)
# Exports target: imnodeflow::imnodeflow

function(imguix_use_or_fetch_imnodeflow OUT_VAR)
    # Only bundled flow (no official package yet)
    set(_IMNODEFLOW_DIR "${IMNODEFLOW_DIR}")
    if(NOT _IMNODEFLOW_DIR)
        set(_IMNODEFLOW_DIR "${PROJECT_SOURCE_DIR}/libs/ImNodeFlow")
    endif()

    if(NOT EXISTS "${_IMNODEFLOW_DIR}/src/ImNodeFlow.cpp")
        message(FATAL_ERROR "ImNodeFlow not found at ${_IMNODEFLOW_DIR}. "
                            "Set IMNODEFLOW_DIR or add submodule libs/ImNodeFlow.")
    endif()

    add_library(imnodeflow STATIC
        "${_IMNODEFLOW_DIR}/src/ImNodeFlow.cpp"
    )
    target_compile_options(imnodeflow PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/FIcstdint>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-include cstdint>
    )
    add_library(imnodeflow::imnodeflow ALIAS imnodeflow)

    # ImNodeFlow depends only on Dear ImGui
    target_link_libraries(imnodeflow PUBLIC imgui::imgui)

    target_include_directories(imnodeflow PUBLIC
        "${_IMNODEFLOW_DIR}/include"
    )

    # Recommended by upstream sample: enable ImGui math operators
    target_compile_definitions(imnodeflow PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

    set_target_properties(imnodeflow PROPERTIES
        CXX_STANDARD 17
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${_IMNODEFLOW_DIR}/include/ImNodeFlow.h"
    )

    set(${OUT_VAR} imnodeflow::imnodeflow PARENT_SCOPE)
endfunction()
