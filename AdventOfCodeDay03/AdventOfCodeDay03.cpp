// AdventOfCodeDay03.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay03/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay03/input.txt";
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

inline bool IsDigit(char c)
{
    return (c >= '0' && c <= '9');
}

inline bool IsSymbol(char c)
{
    return !IsDigit(c) && c != '.';
}

typedef struct Schematic* SchematicPtr;
typedef struct Part* PartPtr;
typedef struct Gear* GearPtr;

struct Schematic {
    std::vector<char*> rows;
    std::vector<PartPtr> parts;
    std::vector<GearPtr> gears;
    int width;
    int height;

    void Read(FILE* file);
    void Print(void);
    bool IsSymbol(int x, int y);
    bool IsAdjacentToSymbol(int x, int y);
    bool IsGear(int x, int y);
    int CountAdjacentParts(GearPtr gear);
    int64_t ScanParts(void);
    int64_t ScanGears(void);
};

struct Part {
    int number;
    int x;
    int y;
    int length;
    bool adjacent;
};

struct Gear {
    int x;
    int y;
    std::vector<PartPtr> parts;
};

void
Schematic::Read(FILE* file)
{
    char buffer[256];
    char* tmp;

    width = 0;
    height = 0;
    rows.clear();

    while (ReadLine(file, buffer) > 0)
    {
        width = strlen(buffer);
        tmp = new char[width + 1];
        strcpy(tmp, buffer);
        rows.push_back(tmp);
        height++;
    }
}

void
Schematic::Print(void)
{
    int i;
    printf("Width: %d, Height: %d\n", width, height);
    i = 0;
    for (char* tmp : rows)
        printf("[%03d] %s\n", ++i, tmp);

    printf("Parts: %zd\n", parts.size());
    i = 0;
    for (PartPtr part : parts)
        printf("[%03d] (%d,%d) %d%s\n", ++i, part->x, part->y, part->number, part->adjacent ? "(A)" : "");

    int64_t sum;
    sum = 0;
    printf("Gears: %zd\n", gears.size());
    i = 0;
    for (GearPtr gear : gears)
    {
        printf("[%03d] (%d,%d) %zd", ++i, gear->x, gear->y, gear->parts.size());
        for (PartPtr part : gear->parts)
            printf(" %d", part->number);
        if (gear->parts.size() == 2)
            sum += gear->parts[0]->number * gear->parts[1]->number;
        printf("\n");
    }
    printf("Sum of ratios: %zd\n", sum);
}

bool
Schematic::IsSymbol(int x, int y)
{
    if (x < 0 || x >= width ||
        y < 0 || y >= height)
        return false;

    return ::IsSymbol(rows[y][x]);
}

bool
Schematic::IsAdjacentToSymbol(int x, int y)
{
    int i;
    for (i = -1; i < 2; i++)
        if (IsSymbol(x + i, y - 1) || IsSymbol(x + i, y + 1))
            return true;

    if (IsSymbol(x - 1, y) || IsSymbol(x + 1, y))
        return true;

    return false;
}

int64_t
Schematic::ScanParts(void)
{
    int x, y;
    PartPtr part;
    char tmp;
    int64_t result;
    int number;

    result = 0;

    for (y = 0; y < height; y++)
    {
        printf("[%03d]", y + 1);
        number = 0;
        part = NULL;
        for (x = 0; x < width; x++)
        {
            tmp = rows[y][x];

            if (IsDigit(tmp))
            {
                if (number == 0)
                {
                    part = new Part();
                    part->x = x;
                    part->y = y;
                    parts.push_back(part);
                }

                number = number * 10 + tmp - '0';
                part->number = number;
                part->length++;
                if (IsAdjacentToSymbol(x, y))
                    part->adjacent = true;

                if (x == (width - 1))
                {
                    printf(" %d", part->number);
                    if (part->adjacent)
                    {
                        printf("(A)");
                        result += number;
                    }
                }
            }
            else
            {
                if (number != 0)
                {
                    printf(" %d", part->number);
                    if (part->adjacent)
                    {
                        printf("(A)");
                        result += number;
                    }
                    number = 0;
                    part = NULL;
                }
                if (part != NULL)
                    part->adjacent = false;
            }
        }
        printf("\n");
    }

    return result;
}

bool
Schematic::IsGear(int x, int y)
{
    if (x < 0 || x >= width ||
        y < 0 || y >= height)
        return false;

    return rows[y][x] == '*';
}

int
Schematic::CountAdjacentParts(GearPtr gear)
{
    int minx, maxx;

    gear->parts.clear();
    for (PartPtr part : parts)
    {
        minx = part->x - 1;
        if (minx < 0)
            minx = 0;

        maxx = part->x + part->length;
        if (maxx >= width)
            maxx = width - 1;

        if (part->y == (gear->y - 1) || part->y == (gear->y + 1))
            if (gear->x >= minx && gear->x <= maxx)
                gear->parts.push_back(part);
        if (part->y == gear->y)
            if (gear->x == maxx || gear->x == minx)
                gear->parts.push_back(part);
    }

    return gear->parts.size();
}

int64_t
Schematic::ScanGears(void)
{
    int x, y;
    int64_t result;
    int64_t ratio;
    GearPtr gear;

    result = 0;

    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            if (IsGear(x, y))
            {
                gear = new Gear();
                gear->x = x;
                gear->y = y;
                gear->parts.clear();
                gears.push_back(gear);

                if (CountAdjacentParts(gear) == 2)
                {
                    ratio = gear->parts[0]->number * gear->parts[1]->number;
                    printf("Gear: (%d,%d) %d x %d = %zd\n", x, y, gear->parts[0]->number, gear->parts[1]->number, ratio);
                    result += ratio;
                }
            }

    return result;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    SchematicPtr schematic;
    int64_t result;

    printf("Advent of Code - Day 03\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    clockStart = clock();
    schematic = new Schematic();
    schematic->Read(input);
    fclose(input);
    result = schematic->ScanParts();

    printf("result 1: %I64d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    clockStart = clock();
    result = schematic->ScanGears();
    schematic->Print();

    printf("result 2: %I64d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}