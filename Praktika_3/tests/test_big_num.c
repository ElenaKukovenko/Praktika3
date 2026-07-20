/*
Тесты для арифметики больших чисел

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "../lib/include/big_num.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================

// Сравнивает два числа и возвращает 1 если равны, иначе 0
static int are_equal(BigNum a, BigNum b, size_t sizeA, size_t sizeB)
{
    if (a == NULL || b == NULL) {
        return 0;
    }

    // Если размеры разные, но меньший может быть заполнен нулями
    size_t maxSize = (sizeA > sizeB) ? sizeA : sizeB;

    for (size_t i = 0; i < maxSize; i++) {
        BitsArrayMaxType va = (i < sizeA) ? BitsArrayGet(a, i) : 0;
        BitsArrayMaxType vb = (i < sizeB) ? BitsArrayGet(b, i) : 0;

        if (va != vb) {
            return 0;
        }
    }
    return 1;
}

// Создаёт число из строки и возвращает его
static BigNum create_num(const char* str, size_t* size)
{
    return GetBigNumByStr(str, size);
}

// Освобождает память
static void safe_free(BigNum num)
{
    if (num != NULL) {
        free(num);
    }
}

// ============================================
// ТЕСТ 1: Создание числа из строки
// ============================================
static void test_creation()
{
    printf("Test 1: Creation from string... ");

    size_t size;
    BigNum num = GetBigNumByStr("1234", &size);

    assert(num != NULL);
    assert(size > 0);

    safe_free(num);
    printf("PASSED\n");
}

// ============================================
// ТЕСТ 2: Сложение простых чисел
// ============================================
static void test_addition_simple()
{
    printf("Test 2: Addition (5 + 3 = 8)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("5", &s1);
    BigNum b = GetBigNumByStr("3", &s2);

    sr = (s1 > s2) ? s1 + 1 : s2 + 1;
    BigNum res = AllocBigNum(sr);

    AddBigNum(a, b, res, s1, s2);

    size_t s_expected;
    BigNum expected = GetBigNumByStr("8", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 3: Сложение с переполнением
// ============================================
static void test_addition_overflow()
{
    printf("Test 3: Addition with overflow (F + 1 = 10)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("F", &s1);
    BigNum b = GetBigNumByStr("1", &s2);

    sr = (s1 > s2) ? s1 + 1 : s2 + 1;
    BigNum res = AllocBigNum(sr);

    AddBigNum(a, b, res, s1, s2);

    size_t s_expected;
    BigNum expected = GetBigNumByStr("10", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 4: Вычитание простых чисел
// ============================================
static void test_subtraction_simple()
{
    printf("Test 4: Subtraction (A - 5 = 5)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("A", &s1);
    BigNum b = GetBigNumByStr("5", &s2);

    sr = (s1 > s2) ? s1 : s2;
    BigNum res = AllocBigNum(sr);

    SubBigNum(a, b, res, s1, s2);

    size_t s_expected;
    BigNum expected = GetBigNumByStr("5", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 5: Вычитание с заёмом
// ============================================
static void test_subtraction_borrow()
{
    printf("Test 5: Subtraction with borrow (5 - A = negative)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("5", &s1);
    BigNum b = GetBigNumByStr("A", &s2);

    sr = (s1 > s2) ? s1 : s2;
    BigNum res = AllocBigNum(sr);

    SubBigNum(a, b, res, s1, s2);

    // Для N=32: 5 - A = -5 = 0xFFFFFFFB
    // Для N=4:  5 - A = -5 = 0xB
    size_t s_expected;
    BigNum expected;

#if N == 4
    expected = GetBigNumByStr("B", &s_expected);
#elif N == 8
    expected = GetBigNumByStr("FB", &s_expected);
#elif N == 16
    expected = GetBigNumByStr("FFFB", &s_expected);
#elif N == 32
    expected = GetBigNumByStr("FFFFFFFB", &s_expected);
#else
    expected = GetBigNumByStr("FFFFFFFFFFFFFFFB", &s_expected);
#endif

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 6: Умножение простых чисел
// ============================================
static void test_multiplication_simple()
{
    printf("Test 6: Multiplication (A * 5 = 32)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("A", &s1);
    BigNum b = GetBigNumByStr("5", &s2);

    sr = s1 + s2;
    BigNum res = AllocBigNum(sr);

    MulBigNum(a, b, res, s1, s2);

    size_t s_expected;
    BigNum expected = GetBigNumByStr("32", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 7: Умножение с переполнением
// ============================================
static void test_multiplication_overflow()
{
    printf("Test 7: Multiplication with overflow (F * F = E1)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("F", &s1);
    BigNum b = GetBigNumByStr("F", &s2);

    sr = s1 + s2;
    BigNum res = AllocBigNum(sr);

    MulBigNum(a, b, res, s1, s2);

    size_t s_expected;
    BigNum expected = GetBigNumByStr("E1", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 8: Деление простых чисел
// ============================================
static void test_division_simple()
{
    printf("Test 8: Division (A / 3 = 3)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("A", &s1);
    BigNum b = GetBigNumByStr("3", &s2);

    sr = s1;
    BigNum res = AllocBigNum(sr);

    DivBigNum(a, b, res, s1, s2);

    size_t s_expected;
    BigNum expected = GetBigNumByStr("3", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 9: Деление с остатком
// ============================================
static void test_division_remainder()
{
    printf("Test 9: Division with remainder (F / 5 = 3)... ");

    size_t s1, s2, sr;
    BigNum a = GetBigNumByStr("F", &s1);
    BigNum b = GetBigNumByStr("5", &s2);

    sr = s1;
    BigNum res = AllocBigNum(sr);

    DivBigNum(a, b, res, s1, s2);

    // 15 / 5 = 3
    size_t s_expected;
    BigNum expected = GetBigNumByStr("3", &s_expected);

    assert(are_equal(res, expected, sr, s_expected));

    safe_free(a);
    safe_free(b);
    safe_free(res);
    safe_free(expected);

    printf("PASSED\n");
}

// ============================================
// ТЕСТ 10: Умножение и деление (обратная проверка)
// ============================================
static void test_mul_div_inverse()
{
    printf("Test 10: Multiplication and division inverse (A * 5 / 5 = A)... ");

    size_t s1, s2, sm, sd;
    BigNum a = GetBigNumByStr("A", &s1);
    BigNum b = GetBigNumByStr("5", &s2);

    // A * 5
    sm = s1 + s2;
    BigNum mul = AllocBigNum(sm);
    MulBigNum(a, b, mul, s1, s2);

    // (A * 5) / 5
    sd = sm;
    BigNum div = AllocBigNum(sd);
    DivBigNum(mul, b, div, sm, s2);

    // Должно быть равно A
    assert(are_equal(div, a, sd, s1));

    safe_free(a);
    safe_free(b);
    safe_free(mul);
    safe_free(div);

    printf("PASSED\n");
}

// ============================================
// MAIN
// ============================================
int main()
{
    printf("\n========================================\n");
    printf("  RUNNING TESTS\n");
    printf("  Kukovenko Elena Evgenievna, MK-101\n");
    printf("========================================\n\n");

    int passed = 0;
    int total = 10;

    test_creation(); passed++;
    test_addition_simple(); passed++;
    test_addition_overflow(); passed++;
    test_subtraction_simple(); passed++;
    test_subtraction_borrow(); passed++;
    test_multiplication_simple(); passed++;
    test_multiplication_overflow(); passed++;
    test_division_simple(); passed++;
    test_division_remainder(); passed++;
    test_mul_div_inverse(); passed++;

    printf("\n========================================\n");
    printf("  TESTS COMPLETED: %d/%d PASSED\n", passed, total);
    printf("========================================\n\n");

    if (passed == total) {
        printf("ALL TESTS PASSED!\n\n");
        return 0;
    }
    else {
        printf("SOME TESTS FAILED!\n\n");
        return 1;
    }
}
