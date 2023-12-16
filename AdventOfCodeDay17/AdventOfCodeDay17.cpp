// AdventOfCodeDay17.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <map>
#include <deque>
#include <algorithm>
#include <cmath>
#include <cstdlib>

#define TEST_MODE true
#define COMMENT true

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay17/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay17/input.txt";
#endif

typedef struct Problem* ProblemPtr;

struct Problem {
	void Read(std::ifstream& input);
	void Print(void) const;

	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
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
Problem::FindSolutionPart1(void)
{
	int64_t result = 0;

	return result;
}

int64_t
Problem::FindSolutionPart2(void)
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

	printf("Advent of Code - Day 17\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	problem.Read(input);
	problem.Print();

	clockStart = clock();

	result = problem.FindSolutionPart1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = problem.FindSolutionPart2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}