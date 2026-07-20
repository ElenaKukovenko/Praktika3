# Ищем библиотеку для тестирования
# В данном случае используем простые assert

# Исходные файлы тестов
set(TEST_SOURCES
    test_big_num.c
)

# Создаём исполняемый файл для тестов
add_executable(test_big_num ${TEST_SOURCES})

# Связываем с библиотекой
target_link_libraries(test_big_num
    big_num_lib
)

# Указываем папку с заголовками
target_include_directories(test_big_num PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

# Добавляем тест в CTest
add_test(NAME BigNumTest COMMAND test_big_num)

# Устанавливаем свойства
set_target_properties(test_big_num PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Опции для тестов
target_compile_definitions(test_big_num PRIVATE -DTEST)

# Добавляем дополнительные тесты (если есть несколько файлов)
# add_test(NAME TestAddition COMMAND test_addition)
# add_test(NAME TestSubtraction COMMAND test_subtraction)
# add_test(NAME TestMultiplication COMMAND test_multiplication)
# add_test(NAME TestDivision COMMAND test_division)
