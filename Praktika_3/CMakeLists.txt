cmake_minimum_required(VERSION 3.10)

project(Praktika3 VERSION 1.0.0)

# Устанавливаем стандарт C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Настраиваемые параметры
set(BITS_ARRAY_ELEMENT_TYPE "unsigned int" CACHE STRING "Type of word in bit array")
set(N_BITS 32 CACHE STRING "Number of bits per element")

# Опции сборки
option(BUILD_TESTS "Build tests" ON)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

# Включаем папки с исходниками
include_directories(${CMAKE_SOURCE_DIR}/lib/include)

# Добавляем поддиректории
add_subdirectory(lib)
add_subdirectory(app)

# Тесты
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Информация о проекте
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
