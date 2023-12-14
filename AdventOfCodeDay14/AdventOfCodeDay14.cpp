// AdventOfCodeDay14.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#define COMMENT true

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay14/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay14/input.txt";
#endif

enum class Direction :int {
	South,
	North,
	East,
	West,
	EndOfList
};

typedef struct Platform* PlatformPtr;

struct Platform {
	std::vector<std::string> lines;
	int width;
	int height;

	void Read(std::ifstream& input);
	void Print(void) const;
	char Value(int x, int y);
	void SetValue(int x, int y, char value);
	bool MoveRocks(Direction direction);
	int ValueScore(Direction direction, int x, int y);
	int Score(Direction direction);
	int64_t FindSolutionPart1(void);
	void SpinCycle(void);
	int64_t FindSolutionPart2(void);
};

void
Platform::Read(std::ifstream& input)
{
	std::string line;

	width = 0;
	height = 0;

	while (std::getline(input, line))
		lines.push_back(line);

	height = (int)lines.size();
	width = (int)lines[0].size();
}

void
Platform::Print(void) const
{
	int index = 0;
	printf("Width: %d, Height: %d\n", width, height);
	for (std::string line : lines)
		printf("[%03d] %s\n", ++index, line.c_str());
}

char
Platform::Value(int x, int y)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return '?';

	return lines[y][x];
}

void
Platform::SetValue(int x, int y, char value)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return;

	lines[y][x] = value;
}

const int DX[(int)Direction::EndOfList] = { 0,0,1,-1 };
const int DY[(int)Direction::EndOfList] = { 1,-1,0,0 };

bool
Platform::MoveRocks(Direction direction)
{
	bool result;
	int x, y;
	int ax, bx, ay, by;

	// x => ax * x + bx
	// y => ay * y + by
	switch (direction)
	{
	case Direction::South:
		ax = 1;
		bx = 0;
		ay = 1;
		by = 0;
		break;
	case Direction::North:
		ax = 1;
		bx = 0;
		ay = -1;
		by = height - 1;
		break;
	case Direction::East:
		ax = 1;
		bx = 0;
		ay = 1;
		by = 0;
		break;
	case Direction::West:
		ax = -1;
		bx = width - 1;
		ay = 1;
		by = 0;
		break;
	default:
		return false;
	}

	result = false;
	for (int n = 0; n < width * height; n++)
	{
		x = n % width;
		y = int(n / width);
		x = ax * x + bx;
		y = ay * y + by;
		if (Value(x, y) == 'O')
		{
			if (Value(x + DX[(int)direction], y + DY[(int)direction]) == '.')
			{
				SetValue(x + DX[(int)direction], y + DY[(int)direction], 'O');
				SetValue(x, y, '.');
				result = true;
			}
		}
	}

	return result;
}

int
Platform::ValueScore(Direction direction, int x, int y)
{
	switch (Value(x, y))
	{
	case 'O':
		switch (direction)
		{
		case Direction::South:
			return y + 1;
		case Direction::North:
			return height - y;
		case Direction::East:
			return x + 1;
		case Direction::West:
			return width - x;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return 0;
}

int
Platform::Score(Direction direction)
{
	int result = 0;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			result += ValueScore(direction, x, y);

	return result;
}

int64_t
Platform::FindSolutionPart1(void)
{
	int64_t result = 0;

	while (MoveRocks(Direction::North)) {}

	Print();

	return Score(Direction::North);;
}

void
Platform::SpinCycle(void)
{
	while (MoveRocks(Direction::North)) {}
	while (MoveRocks(Direction::West)) {}
	while (MoveRocks(Direction::South)) {}
	while (MoveRocks(Direction::East)) {}
}

#define NullPosition -1
#define PatternDiscoveryThreshold 2
#define MinPatternLength 2

typedef struct Entry* EntryPtr;
struct Entry {
	std::vector<int> positions;
};

int64_t
Platform::FindSolutionPart2(void)
{
	int64_t result = 0;
	std::map<int, Entry> values;
	std::map<int, Entry>::iterator search;
	std::vector<int> scores;
	int patternStart, patternLength;
	bool patternFound;
	int index = 0;
	int score;

	patternFound = false;
	while (!patternFound)
	{
		SpinCycle();
		score = Score(Direction::North);
		scores.push_back(Score(Direction::North));

		search = values.find(score);
		if (search == values.end())
		{
			printf("[%03d] New candidate: %d\n", index, score);
			Entry entry;

			entry.positions.clear();
			entry.positions.push_back(index);
			values[score] = entry;
		}
		else
		{
			values[score].positions.push_back(index);
			// Score is already present, search for a pattern:
			printf("[%03d] Existing candidate: %d (", index, score);
			for (int position : values[score].positions)
				printf(" %d", position);
			printf(" )");
			if (values[score].positions.size() <= PatternDiscoveryThreshold)
			{
				printf(" => Not enough data\n");
			}
			else
			{
				int lastLength = 0;
				int step = 0;
				int lastPosition;
				int length;

				patternFound = true;

				printf(" => Cycles:");
				auto it = values[score].positions.rbegin();
				lastPosition = *it;
				while (it != values[score].positions.rend() && step <= PatternDiscoveryThreshold)
				{
					length = lastPosition - *it;
					if (step == 1)
						lastLength = length;
					if (step >= 1)
						printf(" %d", length);
					if (step > 1)
					{
						if (length != lastLength)
						{
							printf(" => No rythm (%d)\n", index);
							patternFound = false;
							break;
						}
					}

					lastPosition = *it;
					step++;
					it++;
				}

				if (patternFound)
				{
					if (length <= 1)
					{
						printf(" => TOO SHORT\n");
						patternFound = false;
					}
					else
					{
						printf(" => Search for pattern at position %d\n", index - 2 * length);
						for (int i = index - 2 * length; i < index - length; i++)
						{
							printf("\t[%03d] %d, [%03d] %d\n", i, scores[i], i + length, scores[i + length]);
							if (scores[i] != scores[i + length])
							{
								printf("\t\t=> BAD\n");
								patternFound = false;
								break;
							}
						}

						if (patternFound)
						{
							patternStart = index - 2 * length;
							patternLength = length;
							printf("\t\t=> FOUND (Start: %d, Length: %d)\n", patternStart, patternLength);
						}
					}
				}
			}
		}

		index++;
	}

	result = scores[patternStart + (1000000000 - (int64_t)patternStart) % patternLength - 1];

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Platform platform;

	printf("Advent of Code - Day 14\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	platform.Read(input);
	platform.Print();

	clockStart = clock();

	result = platform.FindSolutionPart1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = platform.FindSolutionPart2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}