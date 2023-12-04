// AdventOfCodeDay04.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay04/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay04/input.txt";
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

typedef struct Collection* CollectionPtr;
typedef struct Card* CardPtr;
typedef struct NumberType* NumberTypePtr;

struct Collection {
    std::vector<CardPtr> cards;
    void Read(FILE* file);
    void Print(void);
    int64_t Score(void);
};

struct Card {
    int id;
    std::map<int,NumberTypePtr> numbers;

    void Read(char *buffer);
    void Print(void);
    NumberTypePtr AddNumber(int number);
    void AddWinningNumber(int number);
    void AddDrawNumber(int number);
    int64_t Score(void);
};

#define WinningMask 2
#define DrawMask 1
#define MatchMask 3

struct NumberType {
    int mask;
};

void
Collection::Read(FILE* file)
{
    CardPtr card;

    char buffer[256];
    while (ReadLine(file, buffer))
    {
        card = new Card();
        card->Read(buffer);
        cards.push_back(card);
    }
}

void
Collection::Print(void)
{
    int index;
    index = 0;
    printf("Cards: %zd\n", cards.size());
    for (CardPtr card : cards)
    {
        printf("[%03d] ", ++index);
        card->Print();
    }
    printf("\nScore: %zd\n", Score());
}

int64_t
Collection::Score(void)
{
    int64_t result;
    result = 0;
    for (CardPtr card : cards)
        result += card->Score();
    return result;
}


const char * header = "Card ";
const char* separator = ":";
const char* space = " ";

void
Card::Read(char* buffer)
{
    char* tmp;
    tmp = buffer;
    int number;
    FindPattern(&tmp, header);
    sscanf_s(tmp, "%d", &id);
    FindPattern(&tmp, separator);
    while (FindPattern(&tmp, space))
    {
        if (*tmp == '|')
            break;
        sscanf_s(tmp, "%d", &number);
        AddWinningNumber(number);
    }
    while (FindPattern(&tmp, space))
    {
        sscanf_s(tmp, "%d", &number);
        AddDrawNumber(number);
    }
}

void
Card::Print(void)
{
    printf("id: %d, score: %zd, numbers:", id, Score());
    for (const auto& n : numbers)
        printf(" %d (%d)", n.first, n.second->mask);
    printf("\n");
}

NumberTypePtr
Card::AddNumber(int number)
{
    auto search = numbers.find(number);
    if (search == numbers.end())
    {
        NumberTypePtr result;

        result = new NumberType();
        result->mask = 0;
        numbers[number] = result;
        return result;
    }

    return search->second;
}

void
Card::AddWinningNumber(int number)
{
    NumberTypePtr value;
    value = AddNumber(number);
    value->mask = value->mask | WinningMask;
}

void
Card::AddDrawNumber(int number)
{
    NumberTypePtr value;
    value = AddNumber(number);
    value->mask = value->mask | DrawMask;
}

int64_t
Card::Score(void)
{
    int64_t result;
    result = 0;
    for( const auto&n:numbers)
        if(n.second->mask == MatchMask)
            result = result == 0 ? 1: result * 2;

    return result;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    int64_t result;
    Collection collection;

    printf("Advent of Code - Day 04\n");

    error = fopen_s(&input, fileName, "r");
    printf("Opening file %s => %d\n", fileName, error);
    if (error != 0)
        return error;

    result = 0;
    clockStart = clock();

    collection.Read(input);
    collection.Print();
    result = collection.Score();

    printf("result 1: %I64d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    result = 0;
    clockStart = clock();

    printf("result 2: %I64d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}