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

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay08/samplePart2.txt";
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

typedef struct Map* MapPtr;
typedef int32_t Node;
typedef struct Network* NetworkPtr;

struct Map {
	std::string instructions;
	char* instPosition;
	std::map<Node, Network> nodes;
	std::map<int, int>primes;

	void Read(std::ifstream& input);
	void Print(void) const;
	void ResetInstruction(void);
	int GetInstruction(void);
	void ComputePrimes(int max);
	void FillPrimeTable(int value);
	int64_t FindSolution(void);

};

#define LEFT 0
#define RIGHT 1

Node ReadNode(char** buffer)
{
	char tmp[4];
	std::sscanf(*buffer, "%3s", tmp);
	(*buffer) += 3;
	return *(int*)tmp;
}

struct Network {
	Node next[2];
	void Read(char* buffer);
	void Print(void) const;
};

const char* p1 = " = (";
const char* p2 = ", ";

void
Map::Read(std::ifstream& input)
{
	std::string line;
	Node node;
	Network network;
	char* tmp;

	std::getline(input, instructions);
	ResetInstruction();
	std::getline(input, line);

	while (std::getline(input, line))
	{
		tmp = const_cast<char*>(line.c_str());
		node = ReadNode(&tmp);
		FindPattern(&tmp, p1);
		network.Read(tmp);
		nodes[node] = network;
	}
}

void
Map::Print(void) const
{
	printf("Instructions: %s\n", instructions.c_str());
	for (const auto& n : nodes)
	{
		printf("[%s] ", (char*)&n.first);
		n.second.Print();
	}
}

void
Map::ResetInstruction(void)
{
	instPosition = const_cast<char*>(instructions.c_str());
}

int
Map::GetInstruction(void)
{
	int result;

	switch (*instPosition)
	{
	case 'L':
		result = 0;
		break;
	case 'R':
		result = 1;
		break;
	default:
		printf("Bad instruction: %c", *instPosition);
		exit(-1);
		break;
	}

	instPosition++;
	if (*instPosition == '\0')
		instPosition = const_cast<char*>(instructions.c_str());

	return result;
}

void
Map::ComputePrimes(int max)
{
	int index;
	primes.clear();
	printf("Primes:");
	index = 0;
	for (int counter = 2; counter <= max; counter++)
	{
		bool isPrime = true;
		for (int lower = 2; lower < counter; lower++) {
			if (counter % lower == 0) {
				isPrime = false;
				break;
			}
		}
		if (isPrime)
		{
			printf(" %d", counter);
			if (++index == 10)
			{
				printf("\n");
				index = 0;
			}
			primes[counter] = 0;
		}
	}
	printf("\n");
}

void
Map::FillPrimeTable(int value)
{
	for (const auto &n: primes)
		if (value % n.first == 0)
			primes[n.first] = 1;
}

int64_t
Map::FindSolution(void)
{
	int64_t result;
	int count, max;
	std::map<Node,int> input;
	char* tmp;
	Node node;

	// First, find all nodes that end with A
	for (const auto& n : nodes)
	{
		tmp = (char*)&n.first;
		if (tmp[2] == 'A')
			input[n.first] = 0;
	}

	// Then, compute the required number of steps to get to a Z
	max = 0;
	printf("Start:");
	for (const auto &n: input)
	{
		printf("\t%s", (char*)&n.first);
		count = 0;
		node = n.first;
		tmp = (char*)&node;
		ResetInstruction();
		while (tmp[2] != 'Z')
		{
			node = nodes[node].next[GetInstruction()];
			count++;

		}
		printf(":%d", count);
		input[n.first] = count;
		if (count > max)
			max = count;

	}

	// Build a table of prime numbers to decompose all step numbers
	printf("\n");
	printf("Max: %d\n", max);
	ComputePrimes((int)std::sqrt(max));

	// Decompose all step numbers
	for (const auto& n : input)
		FillPrimeTable(n.second);

	// Compute the solution:
	printf("Result: 1");
	result = 1;
	for (const auto& n : primes)
		if (n.second == 1)
		{
			printf(" x %d", n.first);
			result *= n.first;
		}
	printf(" = %zd\n", result);

	return result;
}

void
Network::Read(char* buffer)
{
	next[LEFT] = ReadNode(&buffer);
	FindPattern(&buffer, p2);
	next[RIGHT] = ReadNode(&buffer);
}

void
Network::Print(void) const
{
	printf("left: %s, right: %s\n", (char*)&next[LEFT], (char*)&next[RIGHT]);
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Map Map;

	printf("Advent of Code - Day 08\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	result = 0;
	clockStart = clock();

	Map.Read(input);
	Map.Print();
	result = Map.FindSolution();

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}