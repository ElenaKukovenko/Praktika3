/*
Реализация арифметики больших чисел

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "big_num.h"
#include <string.h>  // для memset
#include <stdlib.h>  // для malloc (уже есть в big_num.h)


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

//Вернём i-ый N-битный элемент из битового массива.
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, unsigned int i){
    BitsArrayMaxType MASK = create_mask();
    //Где начинается наш элемент
    unsigned long long bitOffset = (unsigned long long)i * N;
    //В каком слове находится начало
    unsigned int wordIndex = bitOffset / BITS_IN_WORD;
    //На каком месте внутри этого слова
    unsigned int bitInWord = bitOffset % BITS_IN_WORD;

    if (bitInWord + N <= BITS_IN_WORD) {
        // Весь элемент в одном слове

        //Берём слово
        BitsArrayElementType word = bitsArray[wordIndex];

        //Сдвигаем, чтобы нужные биты оказались в начале
        BitsArrayMaxType shifted = word >> bitInWord;

        //Накладываем маску, чтобы оставить только N бит
        BitsArrayMaxType result = shifted & MASK;

        //Возвращаем результат
        return result;
    }
    else {
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


// помещает i-ый N-битный элемент в битовый массив
void BitsArraySet(BitsArray bitsArray, unsigned int i, BitsArrayMaxType value) {

    //Где начинается наш элемент
    unsigned long long bitOffset = (unsigned long long)i * N;
    //В каком слове находится начало
    unsigned int wordIndex = bitOffset / BITS_IN_WORD;
    //На каком месте внутри этого слова
    unsigned int bitInWord = bitOffset % BITS_IN_WORD;

    BitsArrayMaxType MASK = create_mask();
    BitsArrayMaxType limitedValue = value & MASK;

    if (bitInWord + N <= BITS_IN_WORD) {
        //Берём текущее слово
        BitsArrayElementType word = bitsArray[wordIndex];

        //Создаём маску для очистки места
        //Нужно обнулить биты с bitInWord по bitInWord + N
        BitsArrayElementType clearMask = (BitsArrayElementType)~(MASK << bitInWord);

        //Очищаем место
        BitsArrayElementType clearedWord = word & clearMask;

        //Записываем новое значение
        BitsArrayElementType newWord = clearedWord | (BitsArrayElementType)(limitedValue << bitInWord);

        //Сохраняем обратно в массив
        bitsArray[wordIndex] = newWord;
    }
    else {
        //Сколько бит в первом и втором слове
        unsigned int bitsFromFirst = BITS_IN_WORD - bitInWord;
        unsigned int bitsFromSecond = N - bitsFromFirst;

        //Разделяем значение на две части
        BitsArrayMaxType part1 = limitedValue & ((1ULL << bitsFromFirst) - 1);
        BitsArrayMaxType part2 = limitedValue >> bitsFromFirst;

        //Записываем в первое слово (биты в конце)
        BitsArrayElementType word1 = bitsArray[wordIndex];
        BitsArrayElementType clearMask1 = (BitsArrayElementType)~((1ULL << bitsFromFirst) - 1);
        BitsArrayElementType clearedWord1 = word1 & clearMask1;
        BitsArrayElementType newWord1 = clearedWord1 | (BitsArrayElementType)part1;
        bitsArray[wordIndex] = newWord1;

        //Записываем во второе слово (биты в начале)
        BitsArrayElementType word2 = bitsArray[wordIndex + 1];
        BitsArrayElementType clearMask2 = (BitsArrayElementType)~((1ULL << bitsFromSecond) - 1);
        BitsArrayElementType clearedWord2 = word2 & clearMask2;
        BitsArrayElementType newWord2 = clearedWord2 | (BitsArrayElementType)part2;
        bitsArray[wordIndex + 1] = newWord2;
    }

}


// выделяет память для хранения большого числа
BigNum AllocBigNum(size_t bigNumSize) {
    if (bigNumSize == 0) {
        return NULL;
    }
    //Сколько памяти необходимо
    unsigned long long totalBits = (unsigned long long)bigNumSize * N;
    size_t wordsNeeded = (size_t)((totalBits + BITS_IN_WORD - 1) / BITS_IN_WORD);
    BigNum bigNum = (BigNum)malloc(wordsNeeded * sizeof(BitsArrayElementType));
    if (bigNum == NULL) {
        return NULL;
    }
    memset(bigNum, 0, wordsNeeded * sizeof(BitsArrayElementType));
    return bigNum;
}


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

// выделяет память для хранения большого числа
// и инициализирует его из строки шестнадцатиричных символов.
BigNum GetBigNumByStr(IN const char* str, OUT size_t* bigNumSize) {
    //Проверка входных данных
    if (str == NULL || bigNumSize == NULL) {
        return NULL;
    }
    //Подсчёт длины строки  
    size_t strLen = strlen(str);
    if (strLen == 0) {
        return NULL;
    }
    //Вычисление размера большого числа
    unsigned long long totalBits = (unsigned long long)strLen * 4;
    *bigNumSize = (size_t)((totalBits + N - 1) / N);
    //Выделение памяти
    BigNum num = AllocBigNum(*bigNumSize);
    if (num == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < strLen; i++) {
        // Берём символ с конца строки
        char c = str[strLen - 1 - i];
        unsigned int value = hex_char_to_value(c);

        // Вычисляем позицию для записи
        unsigned int bitPos = i * 4;           // Каждый символ = 4 бита
        unsigned int elementIndex = bitPos / N; // Индекс N-битного элемента
        unsigned int bitInElement = bitPos % N; // Позиция внутри элемента

        // Читаем текущее значение элемента
        BitsArrayMaxType current = BitsArrayGet(num, elementIndex);

        // Очищаем 4 бита в нужной позиции
        BitsArrayMaxType CLEAR_MASK = 0xF;  // 4 бита = 1111
        current &= ~(CLEAR_MASK << bitInElement);

        // Записываем новое значение (4 бита)
        current |= (BitsArrayMaxType)value << bitInElement;

        // Сохраняем обратно
        BitsArraySet(num, elementIndex, current);
    }
    return num;
}


void PrintBigNum(IN BigNum bigNum, size_t bigNumSize) {
    if (bigNum == NULL || bigNumSize == 0) {
        return;
    }
    // Смотрим, сколько hex-цифр в одном элементе
    // N бит / 4 бита на цифру = N/4, округление вверх
    unsigned int hexDigits = (N + 3) / 4;
    int started = 0;  // Флаг: начали ли вывод

    // Последний элемент - самый старший
    for (size_t i = bigNumSize; i > 0; i--) {
        BitsArrayMaxType value = BitsArrayGet(bigNum, i - 1);
        // Выводим этот элемент
        for (unsigned int j = hexDigits; j > 0; j--) {
            // Берём 4 бита
            unsigned int shift = (j - 1) * 4;
            unsigned int digit = (value >> shift) & 0xF;
            // Если digit != 0 или уже начали вывод или это последний элемент
            if (digit != 0 || started || (i == 1 && j == 1)) {
                started = 1;
                printf("%X", digit);
            }
        }
    }
}

void AddBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size) {
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

void SubBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size) {
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }
    size_t maxSize = (bigNum1Size > bigNum2Size) ? bigNum1Size : bigNum2Size;
    BitsArrayMaxType MASK = create_mask();
    BitsArrayMaxType zaem = 0;  // Перенос (0 или 1)

    for (size_t i = 0; i < maxSize; i++) {
        // Читаем элементы (если есть)
        BitsArrayMaxType a = (i < bigNum1Size) ? BitsArrayGet(bigNum1, i) : 0;
        BitsArrayMaxType b = (i < bigNum2Size) ? BitsArrayGet(bigNum2, i) : 0;

        // Складываем
        BitsArrayMaxType sub  = a - b - zaem;

        if (sub > MASK) {
            // diff стал отрицательным
            sub = sub + (MASK + 1);
            zaem = 1;
        }

        // Записываем результат (младшие N бит)
        BitsArraySet(res, i, sub & MASK);
    }
    //if (zaem) {
    //    
    //}
}

void MulBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size) {
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }
    BitsArrayMaxType MASK = create_mask();
    for (size_t j = 0; j < bigNum1Size; j++) {
        BitsArrayMaxType b = BitsArrayGet(bigNum1, j);
        BitsArrayMaxType carry = 0;

        // Умножаем на каждый элемент первого числа
        for (size_t i = 0; i < bigNum2Size; i++) {
            BitsArrayMaxType a = BitsArrayGet(bigNum2, i);
            BitsArrayMaxType product = a * b + carry;

            // Читаем текущее значение в позиции (i + j)
            BitsArrayMaxType current = BitsArrayGet(res, i + j);

            // Складываем с результатом
            BitsArrayMaxType sum = current + (product & MASK);

            // Вычисляем перенос
            carry = (product >> N) + (sum >> N);

            // Записываем результат
            BitsArraySet(res, i + j, sum & MASK);
        }

        // Если остался перенос, записываем его
        if (carry) {
            BitsArraySet(res, bigNum2Size + j, carry);
        }
    }
}



//void DivBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);
