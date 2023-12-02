// AdventOfCodeDay02.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay02/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay02/input.txt";
#endif

bool MatchPattern(char* buffer, const char* pattern)
{
    char* ptrPattern;
    bool result;

    ptrPattern = const_cast<char*>(pattern);
    result = false;

    while ((*buffer) != '\0' && *ptrPattern != '\0')
    {
        result = (*ptrPattern == *buffer);
        if (result == false)
            return false;

        ptrPattern++;
        buffer++;
    }

    if (*ptrPattern == '\0')
        return true;

    return false;
}

bool FindPattern(char** buffer, const char* pattern)
{
    char* ptrPattern;
    bool result;

    ptrPattern = const_cast<char*>(pattern);
    result = false;

    while (**buffer != '\0' && *ptrPattern != '\0')
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

    *ptr = '\0';
    return result;
}

const char* header = "Game ";
const char* startGame = ": ";
const char* nextColor = ", ";
const char* nextSubset = "; ";
const char* space = " ";

#define R 0
#define G 1
#define B 2

const char* colors[] = {
    "red",
    "green",
    "blue"
};

const int maxs[] = { 12, 13, 14 };

bool ReadData1(FILE* file, int64_t* result)
{
    char buffer[256];
    char* tmp;
    char* ncolor;
    char* nsubset;
    int index;
    int subset;

    int color[3];
    int i;

    int game;
    int cube;
    bool possible;

    index = 0;
    while (ReadLine(file, buffer) > 0)
    {
        tmp = buffer;
        printf("[%004d] input: \"%s\"", index,buffer);

        FindPattern(&tmp, header);
        sscanf_s(tmp, "%d", &game);
        printf(" game: %d\n", game);
        FindPattern(&tmp, startGame );
        possible = true;
        subset = 0;
        while (*tmp != '\0')
        {
            color[R] = color[G] = color[B] = 0;

            // Read subset:
            while (*tmp != '\0')
            {
                sscanf_s(tmp, "%d", &cube);
                FindPattern(&tmp, space);
                for (i = 0; i < 3; i++)
                    if (MatchPattern(tmp, colors[i]))
                        color[i] = cube;
                ncolor = tmp;
                nsubset = tmp;
                FindPattern(&ncolor, nextColor);
                FindPattern(&nsubset, nextSubset);
                if (ncolor >= nsubset)
                {
                    tmp = nsubset;
                    break;
                }
                tmp = ncolor;
            }

            printf("\tsubset %d: {%d,%d,%d}", subset, color[R], color[G], color[B]);

            if (color[R] > maxs[R] ||
                color[G] > maxs[G] ||
                color[B] > maxs[B])
            {
                printf(" => impossible\n");
                possible = false;
            }
            else
                printf("\n");
            subset++;
        }

        if (possible)
        {
            printf("\t=> possible\n");
            *result += game;
        }

        index++;
    }

    return true;
}

bool ReadData2(FILE* file, int64_t* result)
{
    char buffer[256];
    char* tmp;
    char* ncolor;
    char* nsubset;
    int index;
    int subset;

    int maxColor[3];
    int color[3];
    int i;

    int game;
    int cube;
    int score;

    index = 0;
    while (ReadLine(file, buffer) > 0)
    {
        tmp = buffer;
        printf("[%004d] input: \"%s\"", index,buffer);

        FindPattern(&tmp, header);
        sscanf_s(tmp, "%d", &game);
        printf(" game: %d\n", game);
        FindPattern(&tmp, startGame );
        maxColor[R] = maxColor[G] = maxColor[B] = 1;
        subset = 0;
        while (*tmp != '\0')
        {
            color[R] = color[G] = color[B] = 0;

            // Read subset:
            while (*tmp != '\0')
            {
                sscanf_s(tmp, "%d", &cube);
                FindPattern(&tmp, space);
                for (i = 0; i < 3; i++)
                    if (MatchPattern(tmp, colors[i]))
                        color[i] = cube;
                ncolor = tmp;
                nsubset = tmp;
                FindPattern(&ncolor, nextColor);
                FindPattern(&nsubset, nextSubset);
                if (ncolor >= nsubset)
                {
                    tmp = nsubset;
                    break;
                }
                tmp = ncolor;
            }

            printf("\tsubset %d: {%d,%d,%d}\n", subset, color[R], color[G], color[B]);

            for (i = 0; i < 3; i++)
                if (color[i] > maxColor[i])
                    maxColor[i] = color[i];
            subset++;
        }

        score = maxColor[R] * maxColor[G] * maxColor[B];
        printf("\tminimum set {%d,%d,%d} => score: %d\n", maxColor[R], maxColor[G], maxColor[B],score);
        *result += score;

        index++;
    }

    return true;
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