# Исходные файлы библиотеки
set(LIB_SOURCES
    src/big_num.c
)

# Заголовочные файлы
set(LIB_HEADERS
    include/big_num.h
)

# Создаём статическую библиотеку
add_library(big_num_lib STATIC ${LIB_SOURCES})

target_compile_definitions(big_num_lib PUBLIC
    BITS_ARRAY_ELEMENT_TYPE=${BITS_ARRAY_ELEMENT_TYPE}
    N=${N_BITS}
)

# Создаём динамическую библиотеку (опционально)
if(BUILD_SHARED_LIBS)
    add_library(big_num_shared SHARED ${LIB_SOURCES})
    set_target_properties(big_num_shared PROPERTIES
        OUTPUT_NAME big_num
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
    )
endif()

# Указываем папку с заголовками
target_include_directories(big_num_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(BUILD_SHARED_LIBS)
    target_include_directories(big_num_shared PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

# Устанавливаем свойства
set_target_properties(big_num_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(BUILD_SHARED_LIBS)
    set_target_properties(big_num_shared PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Экспортируем библиотеку для использования в других модулях
set(BIG_NUM_LIBRARIES big_num_lib PARENT_SCOPE)
