/*
Реализация арифметики больших чисел

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "big_num.h"

//Вернём i-ый N-битный элемент из битового массива.
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, unsigned int i)
{
    //Сколько бит в одном слове
    const unsigned int BITS_IN_WORD = sizeof(BitsArrayElementType) * 8;

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
