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
#include <cstdlib>

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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
	((byte) & 0x80 ? '1' : '0'), \
	((byte) & 0x40 ? '1' : '0'), \
	((byte) & 0x20 ? '1' : '0'), \
	((byte) & 0x10 ? '1' : '0'), \
	((byte) & 0x08 ? '1' : '0'), \
	((byte) & 0x04 ? '1' : '0'), \
	((byte) & 0x02 ? '1' : '0'), \
	((byte) & 0x01 ? '1' : '0') 

const char* pSpace = " ";

typedef struct Map* MapPtr;
typedef struct Galaxy* GalaxyPtr;

#define X 0
#define Y 1

struct Map {
	std::map<int, int> coordinates[2];
	std::vector<Galaxy> galaxies;
	int max[2];

	void Read(std::ifstream& input);
	void Print(void) const;
	void AddGalaxy(int x, int y);
	int GetCoordinate(const std::map<int, int>& coordinates, int value) const;
	int Distance(Galaxy& g1, Galaxy& g2) const;
	void ExpandCoordinates(std::map<int, int>& coordinates, int max, int increment = 1);
	int64_t FindSolution(int increment);
};

struct Galaxy {
	int coordinate[2];
};

#define ExpX(x) GetCoordinate(coordinates[0],x)
#define ExpY(y) GetCoordinate(coordinates[1],y)

void
Map::Read(std::ifstream& input)
{
	std::string line;
	char* tmp;
	int x, y;

	max[X] = max[Y] = 0;

	y = 0;
	while (std::getline(input, line))
	{
		tmp = const_cast<char*>(line.c_str());
		x = 0;
		while (*tmp != '\0')
		{
			if (*tmp == '#')
				AddGalaxy(x, y);
			x++;
			tmp++;
		}
		y++;
	}
}

void
Map::Print(void) const
{
	printf("Max: %d, %d\n", max[X], max[Y]);
#if COMMENT == true
	for (int i = 0; i < 2; i++)
	{
		printf("%s:", i == X ? "X" : "Y");
		for (const auto& it : coordinates[i])
			printf(" %d=>%d", it.first, it.second);
		printf("\n");
	}
#endif

	int index = 0;
	for (Galaxy g : galaxies)
	{
		printf("[%03d] (%d,%d) => (%d,%d)\n", ++index,
			g.coordinate[X], g.coordinate[Y],
			ExpX(g.coordinate[X]), ExpY(g.coordinate[Y]));
	}
}

void
Map::AddGalaxy(int x, int y)
{
	Galaxy g;

	g.coordinate[X] = x;
	g.coordinate[Y] = y;

	galaxies.push_back(g);

	for (int i = 0; i < 2; i++)
	{
		auto search = coordinates[i].find(g.coordinate[i]);
		if (search == coordinates[i].end())
			coordinates[i][g.coordinate[i]] = g.coordinate[i];

		if (max[i] < g.coordinate[i])
			max[i] = g.coordinate[i];
	}
}

int
Map::GetCoordinate(const std::map<int, int>& coordinates, int value) const
{
	auto search = coordinates.find(value);
	if (search == coordinates.end())
		return -1;

	return search->second;
}

int
Map::Distance(Galaxy& g1, Galaxy& g2) const
{
	int result;

	result = 0;
	for (int i = 0; i < 2; i++)
		result += abs(GetCoordinate(coordinates[i], g1.coordinate[i]) - GetCoordinate(coordinates[i], g2.coordinate[i]));

	return result;
}

void
Map::ExpandCoordinates(std::map<int, int>& coordinates, int max, int increment)
{
	int value;
	int expended;
	std::map<int, int>::iterator it1, it2;

	// First, clear the map:
	for (auto& n : coordinates)
		n.second = n.first;

	// Then expand:
	for (value = 0; value <= max; value++)
	{
		it1 = coordinates.find(value);
		if (it1 == coordinates.end())
		{
#if COMMENT == true
			printf("\t%d:", value);
#endif
			for (expended = value + 1; expended <= max; expended++)
			{
				it2 = coordinates.find(expended);
				if (it2 != coordinates.end())
				{
					it2->second += increment;
#if COMMENT == true
					printf(" %d=>%d", expended, it2->second);
#endif
				}
			}
#if COMMENT == true
			printf("\n");
#endif
		}
	}
}


int64_t
Map::FindSolution(int increment)
{
	int64_t result = 0;

	for (int i = 0; i < 2; i++)
	{
#if COMMENT == true
		printf("Expension of %s\n", i == X ? "X" : "Y");
#endif
		ExpandCoordinates(coordinates[i], max[i], increment);
	}

	int i, j;

	for (i = 0; i < (galaxies.size() - 1); i++)
		for (j = i + 1; j < galaxies.size(); j++)
		{
#if COMMENT == true
			printf("[%d,%d] ", i + 1, j + 1);
			printf("%d\n", Distance(galaxies[i], galaxies[j]));
#endif
			result += Distance(galaxies[i], galaxies[j]);
		}

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Map problem;

	printf("Advent of Code - Day 11\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	clockStart = clock();

	problem.Read(input);
	result = problem.FindSolution(1);

	printf("result part 1: %I64d\n", result);

	problem.Print();

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = problem.FindSolution(999999);

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	problem.Print();

	input.close();

	return 0;
}