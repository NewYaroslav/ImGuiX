# cmake/link_local_libs.cmake

function(link_all_local_libs target)
    # Определяем расширения библиотек по платформе
    if(MSVC)
        file(GLOB LOCAL_LIBS "${CMAKE_BINARY_DIR}/lib/*.lib")
    else()
        file(GLOB LOCAL_LIBS "${CMAKE_BINARY_DIR}/lib/*.a")
    endif()

    # Исключаем sfml-main как бесполезный
    list(FILTER LOCAL_LIBS EXCLUDE REGEX ".*sfml-main.*")

    message(STATUS "[ImGuiX] Linking ${target} with local libs:")
    foreach(lib ${LOCAL_LIBS})
        message(STATUS "  -> ${lib}")
    endforeach()

    target_link_libraries(${target} PRIVATE ${LOCAL_LIBS})
endfunction()