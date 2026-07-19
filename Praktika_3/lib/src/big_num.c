/*
Реализация арифметики больших чисел

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "big_num.h"
#include <string.h>  // для memset
#include <stdlib.h>  // для malloc
#include <stdio.h>   // для printf

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================

// Создаёт маску для N бит
static BitsArrayMaxType create_mask(void)
{
    BitsArrayMaxType MASK;
    if (N == 64) {
        MASK = ~0ULL;
    }
    else {
        MASK = (1ULL << N) - 1;
    }
    return MASK;
}

// ============================================
// РАБОТА С БИТОВЫМ МАССИВОМ
// ============================================

// Вернём i-ый N-битный элемент из битового массива.
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, size_t i)
{
    BitsArrayMaxType MASK = create_mask();

    // Где начинается наш элемент
    unsigned long long bitOffset = (unsigned long long)i * N;
    // В каком слове находится начало
    size_t wordIndex = bitOffset / BITS_IN_WORD;
    // На каком месте внутри этого слова
    size_t bitInWord = bitOffset % BITS_IN_WORD;

    if (bitInWord + N <= BITS_IN_WORD) {
        // Весь элемент в одном слове

        // Берём слово
        BitsArrayElementType word = bitsArray[wordIndex];

        // Сдвигаем, чтобы нужные биты оказались в начале
        BitsArrayMaxType shifted = word >> bitInWord;

        // Накладываем маску, чтобы оставить только N бит
        BitsArrayMaxType result = shifted & MASK;

        // Возвращаем результат
        return result;
    }
    else {
        // Элемент в двух словах
        unsigned int bitsFromFirst = BITS_IN_WORD - bitInWord;
        unsigned int bitsFromSecond = N - bitsFromFirst;

        BitsArrayElementType word1 = bitsArray[wordIndex];
        BitsArrayMaxType part1 = (word1 >> bitInWord) & ((1ULL << bitsFromFirst) - 1);

        BitsArrayElementType word2 = bitsArray[wordIndex + 1];
        BitsArrayMaxType part2 = word2 & ((1ULL << bitsFromSecond) - 1);

        BitsArrayMaxType result = part1 | (part2 << bitsFromFirst);
        return result;
    }
}

// Помещает i-ый N-битный элемент в битовый массив
void BitsArraySet(BitsArray bitsArray, size_t i, BitsArrayMaxType value)
{
    // Где начинается наш элемент
    unsigned long long bitOffset = (unsigned long long)i * N;
    // В каком слове находится начало
    unsigned int wordIndex = bitOffset / BITS_IN_WORD;
    // На каком месте внутри этого слова
    unsigned int bitInWord = bitOffset % BITS_IN_WORD;

    BitsArrayMaxType MASK = create_mask();
    BitsArrayMaxType limitedValue = value & MASK;

    if (bitInWord + N <= BITS_IN_WORD) {
        // Запись в одно слово

        // Берём текущее слово
        BitsArrayElementType word = bitsArray[wordIndex];

        // Создаём маску для очистки места
        // Нужно обнулить биты с bitInWord по bitInWord + N
        BitsArrayElementType clearMask = (BitsArrayElementType)~(MASK << bitInWord);

        // Очищаем место
        BitsArrayElementType clearedWord = word & clearMask;

        // Записываем новое значение
        BitsArrayElementType newWord = clearedWord | (BitsArrayElementType)(limitedValue << bitInWord);

        // Сохраняем обратно в массив
        bitsArray[wordIndex] = newWord;
    }
    else {
        // Запись в два слова

        // Сколько бит в первом и втором слове
        unsigned int bitsFromFirst = BITS_IN_WORD - bitInWord;
        unsigned int bitsFromSecond = N - bitsFromFirst;

        // Разделяем значение на две части
        BitsArrayMaxType part1 = limitedValue & ((1ULL << bitsFromFirst) - 1);
        BitsArrayMaxType part2 = limitedValue >> bitsFromFirst;

        // Записываем в первое слово (биты в конце)
        BitsArrayElementType word1 = bitsArray[wordIndex];
        BitsArrayElementType clearMask1 = (BitsArrayElementType)~((1ULL << bitsFromFirst) - 1);
        BitsArrayElementType clearedWord1 = word1 & clearMask1;
        BitsArrayElementType newWord1 = clearedWord1 | (BitsArrayElementType)part1;
        bitsArray[wordIndex] = newWord1;

        // Записываем во второе слово (биты в начале)
        BitsArrayElementType word2 = bitsArray[wordIndex + 1];
        BitsArrayElementType clearMask2 = (BitsArrayElementType)~((1ULL << bitsFromSecond) - 1);
        BitsArrayElementType clearedWord2 = word2 & clearMask2;
        BitsArrayElementType newWord2 = clearedWord2 | (BitsArrayElementType)part2;
        bitsArray[wordIndex + 1] = newWord2;
    }
}

// ============================================
// УПРАВЛЕНИЕ ПАМЯТЬЮ
// ============================================

// Выделяет память для хранения большого числа
BigNum AllocBigNum(size_t bigNumSize)
{
    if (bigNumSize == 0) {
        return NULL;
    }

    // Сколько памяти необходимо
    unsigned long long totalBits = (unsigned long long)bigNumSize * N;
    size_t wordsNeeded = (size_t)((totalBits + BITS_IN_WORD - 1) / BITS_IN_WORD);

    BigNum bigNum = (BigNum)malloc(wordsNeeded * sizeof(BitsArrayElementType));
    if (bigNum == NULL) {
        return NULL;
    }

    memset(bigNum, 0, wordsNeeded * sizeof(BitsArrayElementType));
    return bigNum;
}

// ============================================
// ПРЕОБРАЗОВАНИЕ HEX-СИМВОЛОВ
// ============================================

// Преобразует hex-символ в число (0-15)
static unsigned int hex_char_to_value(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

// ============================================
// РАБОТА СО ЗНАКОМ (ДОПОЛНИТЕЛЬНЫЙ КОД)
// ============================================

// Проверяет, является ли число отрицательным
// (старший бит старшего элемента = 1)
int is_negative(BigNum num, size_t size)
{
    if (num == NULL || size == 0) {
        return 0;
    }

    // Проверяем все элементы с конца, пока не найдём ненулевой
    for (size_t i = size; i > 0; i--) {
        BitsArrayMaxType value = BitsArrayGet(num, i - 1);

        if (value != 0) {
            // Нашли значащий элемент → проверяем старший бит
            BitsArrayMaxType sign_bit;
            if (N == 64) {
                sign_bit = 0x8000000000000000ULL;
            }
            else {
                sign_bit = 1ULL << (N - 1);
            }
            return (value & sign_bit) != 0;
        }
    }

    return 0;  // Все элементы = 0 → число = 0
}

// Преобразует число в дополнительный код (меняет знак)
// Инвертирует все биты и добавляет 1
void NegateBigNum(BigNum num, size_t size){
    if (num == NULL || size == 0) {
        return;
    }

    BitsArrayMaxType MASK = create_mask();
    BitsArrayMaxType carry = 1;  // Добавляем 1 после инвертирования

    // Инвертируем все биты
    for (size_t i = 0; i < size; i++) {
        BitsArrayMaxType value = BitsArrayGet(num, i);
        BitsArrayMaxType inverted = (~value) & MASK;
        BitsArraySet(num, i, inverted);
    }

    // Добавляем 1
    for (size_t i = 0; i < size && carry; i++) {
        BitsArrayMaxType current = BitsArrayGet(num, i);
        BitsArrayMaxType sum = current + carry;

        if (sum > MASK) {
            BitsArraySet(num, i, 0);
            carry = 1;
        }
        else {
            BitsArraySet(num, i, sum);
            carry = 0;
        }
    }
}

// ============================================
// СОЗДАНИЕ ЧИСЛА ИЗ СТРОКИ
// ============================================

// Выделяет память для хранения большого числа
// и инициализирует его из строки шестнадцатиричных символов.
// Поддерживает отрицательные числа (начинаются с '-')
BigNum GetBigNumByStr(IN const char* str, OUT size_t* bigNumSize)
{
    if (str == NULL || bigNumSize == NULL) {
        return NULL;
    }

    // Проверяем наличие минуса
    int negative = 0;
    if (str[0] == '-') {
        negative = 1;
        str++;  // Пропускаем минус
    }

    size_t strLen = strlen(str);
    if (strLen == 0) {
        return NULL;
    }

    // Вычисляем размер
    unsigned long long totalBits = (unsigned long long)strLen * 4;
    *bigNumSize = (size_t)((totalBits + N - 1) / N);

    // Выделяем память
    BigNum num = AllocBigNum(*bigNumSize);
    if (num == NULL) {
        return NULL;
    }

    // Заполняем биты
    for (size_t i = 0; i < strLen; i++) {
        char c = str[strLen - 1 - i];
        unsigned int value = hex_char_to_value(c);

        unsigned int bitPos = i * 4;
        unsigned int elementIndex = bitPos / N;
        unsigned int bitInElement = bitPos % N;

        BitsArrayMaxType current = BitsArrayGet(num, elementIndex);
        current &= ~((BitsArrayMaxType)0xF << bitInElement);
        current |= (BitsArrayMaxType)value << bitInElement;
        BitsArraySet(num, elementIndex, current);
    }

    // Если число отрицательное → переводим в дополнительный код
    if (negative) {
        NegateBigNum(num, *bigNumSize);
    }

    return num;
}

// ============================================
// ВЫВОД ЧИСЛА
// ============================================

// Копирование числа
static void copy_big_num(BigNum dest, BigNum src, size_t size)
{
    if (dest == NULL || src == NULL) {
        return;
    }

    for (size_t i = 0; i < size; i++) {
        BitsArrayMaxType value = BitsArrayGet(src, i);
        BitsArraySet(dest, i, value);
    }
}

// Выводит большое число в шестнадцатеричном виде.
// Отрицательные числа выводятся с минусом.
void PrintBigNum(IN BigNum bigNum, size_t bigNumSize){
    if (bigNum == NULL || bigNumSize == 0) {
        return;
    }

    // Проверяем знак
    int negative = is_negative(bigNum, bigNumSize);

    BigNum temp = NULL;
    if (negative) {
        printf("-");

        // Находим реальный размер числа (без лишних нулей)
        size_t realSize = bigNumSize;
        while (realSize > 1 && BitsArrayGet(bigNum, realSize - 1) == 0) {
            realSize--;
        }

        // Если число -0 (быть не должно, но на всякий случай)
        if (realSize == 0) {
            printf("0");
            return;
        }

        // Создаём копию нужного размера
        temp = AllocBigNum(realSize);
        if (temp == NULL) {
            return;
        }

        for (size_t i = 0; i < realSize; i++) {
            BitsArraySet(temp, i, BitsArrayGet(bigNum, i));
        }

        NegateBigNum(temp, realSize);
        bigNum = temp;
        bigNumSize = realSize;
    }

    // Сколько hex-цифр в одном элементе
    unsigned int hexDigits = (N + 3) / 4;
    int started = 0;

    for (size_t i = bigNumSize; i > 0; i--) {
        BitsArrayMaxType value = BitsArrayGet(bigNum, i - 1);

        for (unsigned int j = hexDigits; j > 0; j--) {
            unsigned int shift = (j - 1) * 4;
            unsigned int digit = (value >> shift) & 0xF;

            if (digit != 0 || started || (i == 1 && j == 1)) {
                started = 1;
                printf("%X", digit);
            }
        }
    }

    if (!started) {
        printf("0");
    }

    if (temp != NULL) {
        free(temp);
    }
}

// ============================================
// АРИФМЕТИЧЕСКИЕ ОПЕРАЦИИ
// ============================================

// Сложение двух больших чисел
void AddBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res,
    size_t bigNum1Size, size_t bigNum2Size)
{
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }

    size_t maxSize = (bigNum1Size > bigNum2Size) ? bigNum1Size : bigNum2Size;
    BitsArrayMaxType MASK = create_mask();
    BitsArrayMaxType carry = 0;  // Перенос (0 или 1)

    for (size_t i = 0; i < maxSize; i++) {
        // Читаем элементы (если есть)
        BitsArrayMaxType a = (i < bigNum1Size) ? BitsArrayGet(bigNum1, i) : 0;
        BitsArrayMaxType b = (i < bigNum2Size) ? BitsArrayGet(bigNum2, i) : 0;

        // Складываем
        BitsArrayMaxType sum = a + b + carry;

        // Вычисляем перенос
        carry = (sum >> N) & 1;

        // Записываем результат (младшие N бит)
        BitsArraySet(res, i, sum & MASK);
    }

    if (carry) {
        BitsArraySet(res, maxSize, 1);
    }
}

// Вычитание двух больших чисел
void SubBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res,
    size_t bigNum1Size, size_t bigNum2Size)
{
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }

    size_t maxSize = (bigNum1Size > bigNum2Size) ? bigNum1Size : bigNum2Size;
    BitsArrayMaxType MASK = create_mask();
    BitsArrayMaxType zaem = 0;  // Заём (0 или 1)

    for (size_t i = 0; i < maxSize; i++) {
        // Читаем элементы (если есть)
        BitsArrayMaxType a = (i < bigNum1Size) ? BitsArrayGet(bigNum1, i) : 0;
        BitsArrayMaxType b = (i < bigNum2Size) ? BitsArrayGet(bigNum2, i) : 0;

        // Вычитаем
        BitsArrayMaxType sub = a - b - zaem;

        if (sub > MASK) {
            // Результат стал отрицательным
            sub = sub + (MASK + 1);
            zaem = 1;
        }
        else {
            zaem = 0;
        }

        // Записываем результат (младшие N бит)
        BitsArraySet(res, i, sub & MASK);
    }
    // zaem == 1 означает, что результат отрицательный
    // (сохраняется в дополнительном коде)
}

// Умножение двух больших чисел
void MulBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res,
    size_t bigNum1Size, size_t bigNum2Size)
{
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }

    // Запоминаем знаки
    int neg1 = is_negative(bigNum1, bigNum1Size);
    int neg2 = is_negative(bigNum2, bigNum2Size);
    int result_negative = neg1 ^ neg2;  // Разные знаки → отрицательный

    // Создаём копии и делаем их положительными
    BigNum a = AllocBigNum(bigNum1Size);
    BigNum b = AllocBigNum(bigNum2Size);
    if (a == NULL || b == NULL) {
        free(a);
        free(b);
        return;
    }

    copy_big_num(a, bigNum1, bigNum1Size);
    copy_big_num(b, bigNum2, bigNum2Size);

    if (neg1) NegateBigNum(a, bigNum1Size);
    if (neg2) NegateBigNum(b, bigNum2Size);

    // Обнуляем результат
    size_t resSize = bigNum1Size + bigNum2Size;
    for (size_t i = 0; i < resSize; i++) {
        BitsArraySet(res, i, 0);
    }

    // Умножаем положительные числа
    BitsArrayMaxType MASK = create_mask();

    for (size_t j = 0; j < bigNum1Size; j++) {
        BitsArrayMaxType bj = BitsArrayGet(b, j);
        BitsArrayMaxType carry = 0;

        for (size_t i = 0; i < bigNum2Size; i++) {
            BitsArrayMaxType ai = BitsArrayGet(a, i);
            BitsArrayMaxType product = ai * bj + carry;

            BitsArrayMaxType current = BitsArrayGet(res, i + j);
            BitsArrayMaxType sum = current + (product & MASK);

            carry = (product >> N) + (sum >> N);
            BitsArraySet(res, i + j, sum & MASK);
        }

        if (carry) {
            BitsArraySet(res, bigNum2Size + j, carry & MASK);
        }
    }

    // Если результат должен быть отрицательным
    if (result_negative) {
        NegateBigNum(res, resSize);
    }

    free(a);
    free(b);
}

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ДЕЛЕНИЯ
// ============================================

// Проверяет, равно ли число нулю
int is_zero(BigNum num, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (BitsArrayGet(num, i) != 0) {
            return 0;
        }
    }
    return 1;
}

// Сравнение двух чисел
static int compare_big_nums(BigNum a, BigNum b, size_t sizeA, size_t sizeB)
{
    size_t maxSize = (sizeA > sizeB) ? sizeA : sizeB;

    for (size_t i = maxSize; i > 0; i--) {
        BitsArrayMaxType va = (i <= sizeA) ? BitsArrayGet(a, i - 1) : 0;
        BitsArrayMaxType vb = (i <= sizeB) ? BitsArrayGet(b, i - 1) : 0;

        if (va > vb) return 1;
        if (va < vb) return -1;
    }
    return 0;  // Числа равны
}

// Увеличение числа на 1
static void add_one_to_big_num(BigNum num, size_t size)
{
    BitsArrayMaxType carry = 1;  // Начинаем с 1

    for (size_t i = 0; i < size && carry; i++) {
        BitsArrayMaxType current = BitsArrayGet(num, i);
        BitsArrayMaxType sum = current + carry;

        // Если переполнение
        if (sum > create_mask()) {
            BitsArraySet(num, i, 0);
            carry = 1;
        }
        else {
            BitsArraySet(num, i, sum);
            carry = 0;
        }
    }

    if (carry) {
        printf("Warning: overflow when adding 1\n");
    }
}

// ============================================
// ДЕЛЕНИЕ
// ============================================

// Деление двух больших чисел (целая часть)
void DivBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res,
    size_t bigNum1Size, size_t bigNum2Size)
{
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }

    // Проверка деления на 0
    if (is_zero(bigNum2, bigNum2Size)) {
        return;
    }

    // Запоминаем знаки
    int neg1 = is_negative(bigNum1, bigNum1Size);
    int neg2 = is_negative(bigNum2, bigNum2Size);
    int result_negative = neg1 ^ neg2;

    // Создаём копии и делаем их положительными
    BigNum a = AllocBigNum(bigNum1Size);
    BigNum b = AllocBigNum(bigNum2Size);
    if (a == NULL || b == NULL) {
        free(a);
        free(b);
        return;
    }

    copy_big_num(a, bigNum1, bigNum1Size);
    copy_big_num(b, bigNum2, bigNum2Size);

    if (neg1) NegateBigNum(a, bigNum1Size);
    if (neg2) NegateBigNum(b, bigNum2Size);

    // Если делимое < делитель, результат = 0
    if (compare_big_nums(a, b, bigNum1Size, bigNum2Size) < 0) {
        free(a);
        free(b);
        return;
    }

    // Деление положительных чисел
    BigNum remainder = AllocBigNum(bigNum1Size);
    copy_big_num(remainder, a, bigNum1Size);

    size_t quotientSize = bigNum1Size;
    BigNum quotient = AllocBigNum(quotientSize);

    while (compare_big_nums(remainder, b, bigNum1Size, bigNum2Size) >= 0) {
        SubBigNum(remainder, b, remainder, bigNum1Size, bigNum2Size);
        add_one_to_big_num(quotient, quotientSize);
    }

    copy_big_num(res, quotient, quotientSize);

    // Если результат должен быть отрицательным
    if (result_negative && !is_zero(res, quotientSize)) {
        NegateBigNum(res, quotientSize);
    }

    free(a);
    free(b);
    free(remainder);
    free(quotient);
}