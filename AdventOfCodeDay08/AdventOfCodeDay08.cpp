// AdventOfCodeDay08.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

#define TEST_MODE true
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay08/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay08/input.txt";
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

const char* pSpace = " ";

typedef struct Game* GamePtr;
typedef struct Hand* HandPtr;

struct Game {
	std::vector<Hand> hands;

	void Read(std::ifstream& input);
	void Print(void);
	int64_t FindSolution(void);
};

struct Hand {
	void Read(std::string& line);
	void Print(void);
};

void
Game::Read(std::ifstream& input)
{
	std::string line;
	Hand hand;

	while (std::getline(input, line))
	{
		hand.Read(line);
		hands.push_back(hand);
	}
}

void
Game::Print(void)
{
	for (Hand hand : hands)
		hand.Print();
}

int64_t
Game::FindSolution(void)
{
	int64_t result;

	result = 0;

	return result;
}

void
Hand::Read(std::string& line)
{
	char* tmp;

	tmp = const_cast<char*>(line.c_str());
}

void
Hand::Print(void)
{
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Game Game;

	printf("Advent of Code - Day 08\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	result = 0;
	clockStart = clock();

	Game.Read(input);
	Game.Print();
	result = Game.FindSolution();

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}