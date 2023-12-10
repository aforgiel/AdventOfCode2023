// AdventOfCodeDay11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <cmath>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay11/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay11/input.txt";
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

void SkipSpace(char** buffer)
{
	while (**buffer == ' ')
		(*buffer)++;
}

inline bool IsDigit(char c)
{
	return (c >= '0' && c <= '9');
}

const char* pSpace = " ";

typedef struct Problem* ProblemPtr;

struct Problem {
	void Read(std::ifstream& input);
	void Print(void) const;
	int64_t FindSolution1(void);
	int64_t FindSolution2(void);
};

void
Problem::Read(std::ifstream& input)
{
	std::string line;

	while (std::getline(input, line))
	{
	}
}

void
Problem::Print(void) const
{
	int index = 0;
}

int64_t
Problem::FindSolution1(void)
{
	int64_t result = 0;

	return result;
}

int64_t
Problem::FindSolution2(void)
{
	int64_t result = 0;

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Problem problem;

	printf("Advent of Code - Day 11\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	clockStart = clock();

	problem.Read(input);
	problem.Print();
	result = problem.FindSolution1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = problem.FindSolution2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}