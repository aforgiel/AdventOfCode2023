// AdventOfCodeDay05.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay05/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay05/input.txt";
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

inline bool IsDigit(char c)
{
    return (c >= '0' && c <= '9');
}

inline bool IsSymbol(char c)
{
    return !IsDigit(c) && c != '.';
}

typedef struct Almanac * AlmanacPtr;
typedef struct SmartLUT* SmartLUTPtr;
typedef struct LUTEntry* LUTEntryPtr;

struct Almanac {
    std::vector<int64_t> seeds;
    std::vector<SmartLUTPtr> steps;

    void Read(std::ifstream& input);
    void Print(void);
    int64_t FindLocation(int64_t seed);
    int64_t FindLowestLocation(void);
};

struct SmartLUT {
    std::string name;
    std::map<int64_t, LUTEntry> entries;

    void Read(std::ifstream &input);
    void Print(void);
    int64_t FindDestination(int64_t source);
};

struct LUTEntry {
    int64_t destination;
    int64_t range;
};

const char* pSpace = " ";
const char* pSeeds = "seeds:";
const char* pSteps[] = {
    "seed-to-soil map:",
    "soil-to-fertilizer map:",
    "fertilizer-to-water map:",
    "water-to-light map:",
    "light-to-temperature map:",
    "temperature-to-humidity map:",
    "humidity-to-location map:"
};

void
Almanac::Read(std::ifstream& input)
{
    std::string line;
    char* tmp;
    int64_t seed;
    int step;
    SmartLUTPtr smartLUT;

    std::getline(input, line);
    tmp = const_cast<char*>(line.c_str());
    FindPattern(&tmp, pSeeds);
    while (FindPattern(&tmp, pSpace))
    {
        sscanf_s(tmp, "%zd", &seed);
        seeds.push_back(seed);
    }

    step = 0;
    while (input && step < 7)
    {
        std::getline(input, line);
        tmp = const_cast<char*>(line.c_str());

        if (FindPattern(&tmp, pSteps[step]))
        {
            smartLUT = new SmartLUT();
            smartLUT->name = pSteps[step];
            smartLUT->Read(input);
            steps.push_back(smartLUT);
            step++;
        }
    }
}

void
Almanac::Print(void)
{
    printf("Seeds:");
    for( int64_t seed: seeds)
        printf(" %zd", seed);
    printf("\n");
    for (SmartLUTPtr smartLUT : steps)
        smartLUT->Print();
}

int64_t
Almanac::FindLocation(int64_t seed)
{
    int step;
    int64_t result;

    result = seed;
    for (step = 0; step < 7; step++)
        result = steps[step]->FindDestination(result);

    return result;
}

int64_t
Almanac::FindLowestLocation(void)
{
    int64_t location;
    int64_t result;
    result = std::numeric_limits<int64_t>::max();
    for (int64_t seed : seeds)
    {
        location = FindLocation(seed);
        if (location < result)
            result = location;
    }

    return result;
}

void
SmartLUT::Read(std::ifstream& input)
{
    std::string line;
    char* tmp;
    int64_t source, destination, range;
    LUTEntry entry;

    while (input)
    {
        std::getline(input, line);
        tmp = const_cast<char*>(line.c_str());
        if (*tmp == '\0')
            break;

        sscanf_s(tmp, "%zd %zd %zd", &destination, &source, &range);
        entry.destination = destination;
        entry.range = range;
        entries[source] = entry;
    };
}

void
SmartLUT::Print(void)
{
    printf("SmartLUT: %s\n", name.c_str());
    for (const auto& n : entries)
        printf("source: %zd, destination: %zd, range: %zd\n", n.first, n.second.destination, n.second.range);
}

int64_t
SmartLUT::FindDestination(int64_t source)
{
    for (const auto& n : entries)
        if (source >= n.first && source < n.first + n.second.range)
            return n.second.destination + source - n.first;

    return source;
}

int main()
{
    std::ifstream input;
    clock_t clockStart, clockEnd;
    double time_taken;
    int64_t result;
    Almanac almanac;

    printf("Advent of Code - Day 05\n");

    input.open(fileName);
    if (!input.is_open())
        return 0;
    printf("Opening file %s\n", fileName);

    result = 0;
    clockStart = clock();

    almanac.Read(input);
    almanac.Print();

    result = almanac.FindLowestLocation();
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

    input.close();

    return 0;
}