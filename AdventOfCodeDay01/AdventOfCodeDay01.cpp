// AdventOfCodeDay15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE true
#define COMMENT false

#if( TEST_MODE == true )
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay01/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay01/input.txt";
#endif

typedef struct TPoint* TPointPtr;
struct TPoint {
    int64_t x;
    int64_t y;
};

void InitPoint(TPoint& point, TPointPtr source = NULL)
{
    if (source == NULL)
    {
        point.x = 0;
        point.y = 0;

        return;
    }

    point.x = source->x;
    point.y = source->y;
}

void PrintPoint(const TPoint& data)
{
    printf("(%lld,%lld)", data.x, data.y);
}

int64_t Distance(const TPoint& a, const TPoint& b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

bool FindPattern(char** buffer, const char* pattern)
{
    char* ptrPattern;
    bool result;

    ptrPattern = const_cast<char*>(pattern);
    result = false;

    while ((*buffer) != NULL && *ptrPattern != NULL)
    {
        result = (*ptrPattern == **buffer);
        if (result == true)
            ptrPattern++;
        else
        {
            *buffer -= ptrPattern - pattern;
            ptrPattern = const_cast<char*>(pattern);
        }
        (*buffer)++;
    }

    return result;
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

    return result;
}

const char* kXXXPattern = "dfdf=";

bool ReadData(FILE* file )
{
    char buffer[256];

    while (ReadLine(file, buffer) > 0)
    {
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
    char buffer[200];
    int counter;

    printf("Advent of Code - Day 01\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    // Step 1

    counter = 1;
    ReadData(input);
    PrintData();

    clockStart = clock();

#if( TEST_MODE == true)

#else

#endif
    printf("result 1: %lld\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    // Step 2
    clockStart = clock();

#if( TEST_MODE == true)

#else

#endif
    printf("Result 2: %lld\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}