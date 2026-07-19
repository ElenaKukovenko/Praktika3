/*
Библиотека для работы с большими числами

Куковенко Елена Евгеньевна
Группа МК-101
*/

#ifndef BIG_NUM_H
#define BIG_NUM_H

#include <stdlib.h>

// тип слова битового массива
typedef unsigned int BitsArrayElementType;

// битовый массив представляется массивом слов
typedef BitsArrayElementType* BitsArray;

// тип, в который поместиться любой N-битный кусочек
typedef unsigned long long BitsArrayMaxType;

// большое число представляется битовым массивом
typedef BitsArray BigNum;

// размер элементов большого числа
#define N	32

#define IN
#define OUT

// возвращает i-ый N-битный элемент из битового массива
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, unsigned int i);

// помещает i-ый N-битный элемент в битовый массив
void BitsArraySet(BitsArray bitsArray, unsigned int i, BitsArrayMaxType value);

// выделяет память для хранения большого числа
// и инициализирует его из строки шестнадцатиричных символов.
BigNum GetBigNumByStr(IN const char* str, OUT size_t* bigNumSize);

// выделяет память для хранения большого числа
BigNum AllocBigNum(size_t bigNumSize);

void PrintBigNum(IN BigNum bigNum, size_t bigNumSize);

void AddBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);
void SubBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);
void MulBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);
void DivBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);

#endif 
