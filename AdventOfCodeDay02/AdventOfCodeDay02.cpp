// AdventOfCodeDay15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE true
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay02/sample2.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay02/input.txt";
#endif

const char* digits[] =
{
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

bool FindPattern(char** buffer, const char* pattern)
{
    char* ptrPattern;
    bool result;

    ptrPattern = const_cast<char*>(pattern);
    result = false;

    while ((**buffer) != '\0' && *ptrPattern != '\0')
    {
        result = (*ptrPattern == **buffer);
        if (result == false)
            return false;

        ptrPattern++;
        (*buffer)++;
    }

    if (*ptrPattern == '\0')
        return true;

    return false;
}

int FindTextDigit(char** buffer)
{
    char* tmp;
    int index;

    index = 0;
    for( index = 0; index < 9; index++ )
    {
        tmp = *buffer;
        if (FindPattern(&tmp, digits[index]))
        {
            *buffer = tmp;
            return index + 1;
        }
    }

    return 0;
}

int ReadLine(FILE* file, char* buffer)
{
    char* ptr;
    int result;

    ptr = buffer;
    result = 0;

    while (fread(ptr, 1, 1, file) > 0)
    {
        result++;
        if (*ptr == '\n')
        {
            *ptr = NULL;
            return result;
        }
        ptr++;
    }

    *ptr = '\0';
    return result;
}

const char* kXXXPattern = "dfdf=";

inline bool IsDigit( char c )
{
    return (c >= '0' && c <= '9');
}

bool ReadData1(FILE* file, int64_t * result )
{
    char buffer[256];
    char * tmp;
    int firstDigit;
    int aDigit;
    int calibration;
    int index;
    int digit;
    int textDigit;

    index = 0;
    while (ReadLine(file, buffer) > 0)
    {
        firstDigit = 0;
        aDigit = 0;
        textDigit = 0;
        tmp = buffer;
        printf("[%004d] buffer: %s,", index, buffer);
        while( * tmp != '\0' )
        {
            if (IsDigit(*tmp))
            {
                digit = *tmp - '0';
                printf(" => d: %d", digit);
                if (firstDigit == 0)
                    firstDigit = digit;
                aDigit = digit;
            }
            tmp++;
        }

        calibration = firstDigit * 10 + aDigit;

        printf( " calibration: %d\n", calibration );
        *result += calibration;
        index++;
    }

    return true;
}

bool ReadData2(FILE* file, int64_t * result )
{
    char buffer[256];
    char * tmp;
    int firstDigit;
    int aDigit;
    int calibration;
    int index;
    int digit;
    int textDigit;
    char* textTmp;

    index = 0;
    while (ReadLine(file, buffer) > 0)
    {
        firstDigit = 0;
        aDigit = 0;
        textDigit = 0;
        tmp = buffer;
        printf("[%004d] buffer: %s,", index, buffer);
        while( * tmp != '\0' )
        {
            if (IsDigit(*tmp))
            {
                digit = *tmp - '0';
                printf(" => d: %d", digit);
                if (firstDigit == 0)
                    firstDigit = digit;
                aDigit = digit;
            }
            else
            {
                textTmp = tmp;
                textDigit = FindTextDigit(&textTmp);
                if (textDigit > 0)
                {
                    printf(" => t: %d", textDigit);
                    if (firstDigit == 0)
                        firstDigit = textDigit;
                    aDigit = textDigit;
                }
            }
            tmp++;
        }

        calibration = firstDigit * 10 + aDigit;

        printf( " calibration: %d\n", calibration );
        *result += calibration;
        index++;
    }

    return true;
}

void PrintData( void )
{
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    int64_t result;

    printf("Advent of Code - Day 02\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    clockStart = clock();
    result = 0;
    ReadData1(input, &result);
    printf("result 1: %I64d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    clockStart = clock();
    result = 0;
    ReadData2(input, &result);
    printf("result 2: %I64d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}