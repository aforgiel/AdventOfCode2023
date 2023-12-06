// AdventOfCodeDay06.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay06/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay06/input.txt";
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
	while (**buffer == ' ' )
		(*buffer)++;
}

inline bool IsDigit(char c)
{
	return (c >= '0' && c <= '9');
}

inline bool IsSymbol(char c)
{
	return !IsDigit(c) && c != '.';
}

typedef struct Competition* CompetitionPtr;
typedef struct Race* RacePtr;

struct Competition {
	std::vector<Race> races;

	void Read(std::ifstream& input);
	void Print(void);
	int64_t FindSolution(void);
};

struct Race {
	int time;
	int distance;

	void Print(void);
	bool ComputeSolutions(int& min, int& max);
};

const char* pSpace = " ";
const char* pTime = "Time: ";
const char* pDistance = "Distance: ";

void
Competition::Read(std::ifstream& input)
{
	std::string line;
	char* tmp;
	Race race;
	int time, distance;

	std::getline(input, line);
	tmp = const_cast<char*>(line.c_str());
	FindPattern(&tmp, pTime);
	SkipSpace(&tmp);
	while (*tmp)
	{
		sscanf_s(tmp, "%d", &time);
		race.time = time;
		races.push_back(race);
		FindPattern(&tmp, pSpace);
		SkipSpace(&tmp);
	}

	std::getline(input, line);
	tmp = const_cast<char*>(line.c_str());
	FindPattern(&tmp, pDistance);
	SkipSpace(&tmp);
	auto it = races.begin();
	while (*tmp)
	{
		sscanf_s(tmp, "%d", &distance);
		it->distance = distance;
		FindPattern(&tmp, pSpace);
		SkipSpace(&tmp);
		it++;
	}
}

void
Competition::Print(void)
{
	int index;
	index = 0;
	for (Race race : races)
	{
		printf("[%02d] ", ++index);
		race.Print();
	}
}

int64_t
Competition::FindSolution(void)
{
	int64_t result;
	int min, max;

	result = 1;
	for (Race race : races)
		if( race.ComputeSolutions(min, max) )
			result *= (max - min + 1);
	
	return result;
}

void
Race::Print(void)
{
	int min, max;
	ComputeSolutions(min, max);
	printf("time: %d, distance: %d, [%d,%d] = %d\n", time, distance, min, max, max - min + 1);
}

bool
Race::ComputeSolutions(int& min, int& max)
{
	double tmp, det;
	double x1, x2;

	tmp = (double)(time * time - 4 * distance);
	if (tmp < 0)
	{
		min = max = 0;
		return false;
	}

	det = std::sqrt(tmp);

	x1 = (double(time) - det) / 2;
	x2 = (double(time) + det) / 2;

	printf("\tx1: %f, x2: %f\n", x1, x2);

	min = (int)std::floor(x1+1);
	if (std::floor(x2) == x2)
		x2--;
	max = (int)std::floor(x2);

	return true;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Competition Competition;

	printf("Advent of Code - Day 06\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	result = 0;
	clockStart = clock();

	Competition.Read(input);
	Competition.Print();
	result = Competition.FindSolution();

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}