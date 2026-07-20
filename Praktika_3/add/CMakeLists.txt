# Исходные файлы приложения
set(APP_SOURCES
    src/main.c
)

# Создаём исполняемый файл
add_executable(app ${APP_SOURCES})

target_compile_definitions(app PRIVATE
    BITS_ARRAY_ELEMENT_TYPE=${BITS_ARRAY_ELEMENT_TYPE}
    N=${N_BITS}
)

# Связываем с библиотекой
target_link_libraries(app
    big_num_lib
)

# Указываем папку с заголовками
target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

# Устанавливаем свойства
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/app
)

# Копируем заголовки при сборке (опционально)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(app PRIVATE -DNDEBUG)
endif()
