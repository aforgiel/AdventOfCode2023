// AdventOfCodeDay10.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay10/samplePart2.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay10/input.txt";
#endif

typedef struct Map* MapPtr;

#define VERTICAL 0b1001
#define HORIZONTAL 0b0110

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

struct Map {
	std::vector<std::string> lines;

	int width;
	int height;

	int startX;
	int startY;

	MapPtr MakeClean(void);
	void Read(std::ifstream& input);
	void Print(void) const;
	char GetTile(int x, int y) const;
	void SetTile(int x, int y, char tile);
	int GetTileCode(char tile) const;
	char GetNextTile(int& x, int& y, int& to) const;
	int64_t FindSolution1(MapPtr cleanMap);
	int64_t FindSolution2(void);
};

MapPtr
Map::MakeClean(void)
{
	MapPtr result;
	char* buffer;

	result = new Map();
	buffer = new char[width + 1];

	result->width = width;
	result->height = height;

	for (int x = 0; x < width; x++)
		buffer[x] = '.';
	buffer[width] = '\0';

	for (int y = 0; y < height; y++)
		result->lines.push_back(std::string(buffer));

	return result;
}

void
Map::Read(std::ifstream& input)
{
	std::string line;
	bool found;

	width = 0;
	height = 0;

	while (std::getline(input, line))
	{
		lines.push_back(line);
		if (width == 0)
			width = (int)line.size();
		height++;
	}

	found = false;
	for (int y = 0; y < height && !found; y++)
		for (int x = 0; x < width && !found; x++)
			if (GetTile(x, y) == 'S')
			{
				startX = x;
				startY = y;
				found = true;
				break;
			}
}

void
Map::Print(void) const
{
	int index = 0;
	printf("width: %d, height: %d\n", width, height);
	for (int y = -1; y <= height; y++)
	{
		for (int x = -1; x <= width; x++)
			printf("%c", GetTile(x, y));
		printf("\n");
	}
}

char
Map::GetTile(int x, int y) const
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return '.';

	return lines[y].c_str()[x];
}

void
Map::SetTile(int x, int y, char tile)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return;

	*const_cast<char*>(&lines[y].c_str()[x]) = tile;
}

int
Map::GetTileCode(char tile) const
{
	/*
	*	X0X
	*	1X2
	*	X3X
	*
	*	=> 0b3210
	*/

	switch (tile)
	{
	case '|':
		return VERTICAL;
	case '-':
		return HORIZONTAL;
	case 'L':
		return 0b0101;
	case 'J':
		return 0b0011;
	case '7':
		return 0b1010;
	case 'F':
		return 0b1100;

	case 'S':
		return 0b1111; // Connect ot every sides

	default:
		break;
	}

	return 0b0000;
}

const int deltaX[4] = { 0,-1,1,0 };
const int deltaY[4] = { -1,0,0,1 };

const int from[4] = {
	0b0111,
	0b1011,
	0b1101,
	0b1110 };

const int to[4] = {
	0b1000,
	0b0100,
	0b0010,
	0b0001 };

char
Map::GetNextTile(int& x, int& y, int& to) const
{
	char result;
	int tileCode;
	int mask;
	int i;

	x += deltaX[to];
	y += deltaY[to];

	result = GetTile(x, y);
	tileCode = GetTileCode(result);
	mask = tileCode & from[to];
#if COMMENT == true
	printf("\tto: %d => [%d,%d] %c (" BYTE_TO_BINARY_PATTERN "), from: " BYTE_TO_BINARY_PATTERN ", mask: " BYTE_TO_BINARY_PATTERN "\n",
		to, x, y, result, BYTE_TO_BINARY(tileCode), BYTE_TO_BINARY(from[to]), BYTE_TO_BINARY(mask));
#endif

	i = 0;
	while (!(mask & 1) && i < 4)
	{
		mask = mask >> 1;
		i++;
	}

	to = i;

	return result;
}

int64_t
Map::FindSolution1(MapPtr cleanMap)
{
	int64_t result = 0;

	int x, y;
	int nextCode;
	int i;

	x = startX;
	y = startY;
	printf("Start: [%d,%d]\n", x, y);

	for (i = 0; i < 4; i++)
	{
		nextCode = GetTileCode(GetTile(x + deltaX[i], y + deltaY[i]));
#if COMMENT == true
		printf("\t[%d,%d] %c (" BYTE_TO_BINARY_PATTERN ") mask: " BYTE_TO_BINARY_PATTERN,
			x + deltaX[i], y + deltaY[i], GetTile(x + deltaX[i], y + deltaY[i]), BYTE_TO_BINARY(nextCode), BYTE_TO_BINARY(to[i]));
#endif
		if ((to[i] & nextCode) > 0)
		{
#if COMMENT == true
			printf(" => Connect\n");
#endif
			break;
		}
#if COMMENT == true
		printf("\n");
#endif
	}

	do {
#if COMMENT == true
		printf("[%zd] %c\n", result, GetTile(x, y));
#endif
		cleanMap->SetTile(x, y, GetTile(x, y));
		result++;

	} while (GetNextTile(x, y, i) != 'S');

	result /= 2;

	return result;
}

inline bool IsBorder(char c)
{
	switch (c)
	{
	case '|':
	case '-':
	case 'L':
	case 'J':
	case '7':
	case 'F':
	case 'S':
		return true;
	default:
		break;
	}
	return false;
}

int64_t
Map::FindSolution2(void)
{
	int64_t result = 0;
	int count;
	int x, y;
	int ray;
	char tmp;

	// First pass:
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			if (!IsBorder(GetTile(x, y)))
			{
				count = 0;
				for (ray = 1; (x + ray) <= width && (y + ray) <= height; ray++)
				{
					tmp = GetTile(x + ray, y + ray);
					if (tmp != 'L' && tmp != '7' && IsBorder(tmp))
						count++;
				}

				if (count % 2 == 0)
					SetTile(x, y, 'O');
				else
				{
					SetTile(x, y, 'I');
					result++;
				}
			}

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Map map;
	MapPtr cleanMap;

	printf("Advent of Code - Day 10\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	map.Read(input);
	map.Print();
	cleanMap = map.MakeClean();

	clockStart = clock();

	result = map.FindSolution1(cleanMap);

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	cleanMap->Print();

	clockStart = clock();

	result = cleanMap->FindSolution2();

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	cleanMap->Print();

	delete cleanMap;

	input.close();

	return 0;
}