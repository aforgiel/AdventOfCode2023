// AdventOfCodeDay13.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay13/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay13/input.txt";
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

typedef struct Problem* ProblemPtr;
typedef struct Pattern* PatternPtr;

struct Problem {
	std::vector<Pattern> patterns;

	void Read(std::ifstream& input);
	void Print(void) const;
	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

#define VERTICAL 0
#define HORIZONTAL 1

#define NullAxis -1

struct Pattern {
	std::vector<std::string>lines;
	int width;
	int height;

	void Read(std::ifstream& input);
	void Print(void) const;
	int FindReflectionAxis(int direction);
	int FindFixedReflectionAxis(int direction);
	char Value(int x, int y);
	void SetValue(int x, int y, char value);
	char MirrorValue(int x, int y, int direction, int reflectionAxis);
	void MirrorCoordinates(int x, int y, int direction, int reflectionAxis, int* xm, int* ym);
};

void
Problem::Read(std::ifstream& input)
{
	std::string line;
	Pattern pattern;

	while (input)
	{
		pattern.Read(input);
		patterns.push_back(pattern);
	}
}

void
Problem::Print(void) const
{
	int index = 0;
	for (Pattern pattern : patterns)
	{
		printf("[%03d] ", ++index);
		pattern.Print();
	}
}

int64_t
Problem::FindSolutionPart1(void)
{
	int64_t result = 0;
	int direction;
	int reflectionAxis;
	int index = 0;

	for (Pattern pattern : patterns)
	{
		for (direction = VERTICAL; direction <= HORIZONTAL; direction++)
		{
			reflectionAxis = pattern.FindReflectionAxis(direction);
			if (reflectionAxis != NullAxis)
			{
				printf("[%03d] %s %d\n", ++index, direction == VERTICAL ? "Vertical" : "Horizontal", reflectionAxis + 1);
				result += (direction == VERTICAL ? 1 : 100) * (reflectionAxis + 1);
				break;
			}
		}
	}

	return result;
}

int64_t
Problem::FindSolutionPart2(void)
{
	int64_t result = 0;
	int direction;
	int reflectionAxis;
	int fixedDirection;
	int fixedReflectionAxis;
	int index = 0;

	for (Pattern pattern : patterns)
	{
		fixedReflectionAxis = std::numeric_limits<int>::max();
		fixedDirection = -1;
		for (direction = VERTICAL; direction <= HORIZONTAL; direction++)
		{
			reflectionAxis = pattern.FindFixedReflectionAxis(direction);
			if (reflectionAxis != NullAxis && reflectionAxis < fixedReflectionAxis)
			{
				fixedDirection = direction;
				fixedReflectionAxis = reflectionAxis;
			}
		}
		printf("[%03d] %s (%d) %d\n", ++index, fixedDirection == VERTICAL ? "Vertical" : "Horizontal", fixedDirection, fixedReflectionAxis + 1);
		result += (fixedDirection == VERTICAL ? 1 : 100) * (fixedReflectionAxis + 1);
	}

	return result;
}

void
Pattern::Read(std::ifstream& input)
{
	std::string line;

	lines.clear();
	while (std::getline(input, line))
	{
		if (line.size() == 0)
			break;
		lines.push_back(line);
	}
	width = (int)lines[0].size();
	height = (int)lines.size();
}

void
Pattern::Print(void) const
{
	int index = 0;
	printf("Width: %d, Height: %d\n", width, height);
	for (std::string line : lines)
		printf("\t[%03d] %s\n", ++index, line.c_str());
}

int
Pattern::FindReflectionAxis(int direction)
{
	int candidate;
	int max;
	char value, mirrorValue;
	int result;
	bool found;

	switch (direction)
	{
	case VERTICAL:
		max = width - 1;
		break;
	case HORIZONTAL:
		max = height - 1;
		break;
	default:
		return NullAxis;
	}

	result = NullAxis;
	found = false;
	candidate = 0;
	while (candidate < max)
	{
		found = true;
		for (int x = 0; x < width && found; x++)
			for (int y = 0; y < height && found; y++)
			{
				value = Value(x, y);
				mirrorValue = MirrorValue(x, y, direction, candidate);

				if (value != '?' && mirrorValue != '?')
					if (value != mirrorValue)
						found = false;
			}
		if (found)
			return candidate;
		candidate++;
	}

	return NullAxis;
}

int
Pattern::FindFixedReflectionAxis(int direction)
{
	int candidate;
	int max;
	char value, mirrorValue;
	int result;
	bool found;
	int smudge;
	int xs, ys;

	switch (direction)
	{
	case VERTICAL:
		max = width - 1;
		break;
	case HORIZONTAL:
		max = height - 1;
		break;
	default:
		return NullAxis;
	}

	result = NullAxis;
	found = false;
	candidate = 0;
	while (candidate < max)
	{
		found = true;
		smudge = 0;
		for (int x = 0; x < width && found; x++)
			for (int y = 0; y < height && found; y++)
			{
				value = Value(x, y);
				mirrorValue = MirrorValue(x, y, direction, candidate);

				if (value != '?' && mirrorValue != '?')
					if (value != mirrorValue)
					{
						smudge++;
						if (smudge == 1)
						{
							if (value == '#')
							{
								xs = x;
								ys = y;
							}
							else
								MirrorCoordinates(x, y, direction, candidate, &xs, &ys);

							// Fix the smudge:
							SetValue(xs, ys, '.');
						}
						if (smudge > 1)
						{
							//Restore the smudge:
							SetValue(xs, ys, '#');
							mirrorValue = MirrorValue(x, y, direction, candidate);
							found = false;
						}
					}
			}
		if (found && smudge == 1)
		{
			return candidate;
		}
		candidate++;
	}

	return NullAxis;
}

char
Pattern::Value(int x, int y)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return '?';

	return lines[y][x];
}

void
Pattern::SetValue(int x, int y, char value)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return;

	lines[y][x] = value;
}

char
Pattern::MirrorValue(int x, int y, int direction, int reflectionAxis)
{
	int xm, ym;

	MirrorCoordinates(x, y, direction, reflectionAxis, &xm, &ym);

	if (xm < 0 || xm >= width ||
		ym < 0 || ym >= height)
		return '?';

	return lines[ym][xm];
}

void
Pattern::MirrorCoordinates(int x, int y, int direction, int reflectionAxis, int* xm, int* ym)
{
	switch (direction)
	{
	case VERTICAL:
		*xm = 2 * reflectionAxis - x + 1;
		*ym = y;
		break;
	case HORIZONTAL:
		*xm = x;
		*ym = 2 * reflectionAxis - y + 1;
		break;
	default:
		break;
	}
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Problem problem;

	printf("Advent of Code - Day 13\n");

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