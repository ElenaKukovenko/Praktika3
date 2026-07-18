/*
Реализация арифметики больших чисел

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "big_num.h"
#include <string.h>  // для memset
#include <stdlib.h>  // для malloc (уже есть в big_num.h)

//Вернём i-ый N-битный элемент из битового массива.
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, unsigned int i){

    BitsArrayMaxType MASK;
    if (N == 64) {
        MASK = ~0ULL;
    }
    else {
        MASK = (1ULL << N) - 1;
    }
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

    BitsArrayMaxType MASK;
    if (N == 64) {
        MASK = ~0ULL;
    }
    else {
        MASK = (1ULL << N) - 1;
    }
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
