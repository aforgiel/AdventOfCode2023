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
	while (**buffer == ' ')
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
	int64_t time;
	int64_t distance;

	void Print(void);
	int64_t Distance(int64_t time);
	bool ComputeSolutions(int64_t& min, int64_t& max);
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
	int64_t time, distance;
	char buffer[256];
	char* ptr;

	std::getline(input, line);
	tmp = const_cast<char*>(line.c_str());
	FindPattern(&tmp, pTime);
	ptr = buffer;
	while (*tmp)
	{
		SkipSpace(&tmp);
		*ptr = *tmp;
		ptr++;
		tmp++;
	}
	*ptr = '\0';
	sscanf_s(buffer, "%zd", &time);
	race.time = time;

	std::getline(input, line);
	tmp = const_cast<char*>(line.c_str());
	FindPattern(&tmp, pDistance);
	ptr = buffer;
	while (*tmp)
	{
		SkipSpace(&tmp);
		*ptr = *tmp;
		ptr++;
		tmp++;
	}
	*ptr = '\0';
	sscanf_s(buffer, "%zd", &distance);
	race.distance = distance;
	races.push_back(race);
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
	int64_t min, max;

	result = 1;
	for (Race race : races)
		if (race.ComputeSolutions(min, max))
			result *= (max - min + 1);

	return result;
}

void
Race::Print(void)
{
	int64_t min, max;
	ComputeSolutions(min, max);
	printf("time: %zd, distance: %zd, [%zd,%zd] = %zd\n", time, distance, min, max, max - min + 1);
}

int64_t
Race::Distance(int64_t t)
{
	return t * (time - t);
}


bool
Race::ComputeSolutions(int64_t& min, int64_t& max)
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

	min = (int64_t)std::floor(x1 + 1);
	if (std::floor(x2) == x2)
		x2--;
	max = (int64_t)std::floor(x2);

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