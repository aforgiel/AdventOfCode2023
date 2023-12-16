// AdventOfCodeDay16.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay16/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay16/input.txt";
#endif

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

enum class Direction : int {
	Up,
	Down,
	Left,
	Right,
	EndOfList
};

const char DirectionMask[(int)Direction::EndOfList] = { 0b0001, 0b0010, 0b0100, 0b1000 };

const int DX[(int)Direction::EndOfList] = { 0,0,-1,1 };
const int DY[(int)Direction::EndOfList] = { -1,1,0,0 };

enum class TileType : int {
	Empty,					// '.'
	MirrorUp,				// '/'
	MirrorDown,				// '\'
	SplitterVertical,		// '|'
	SplitterHorizontal,		// '-'
	EndOfList
};

const char NextDirection[(int)TileType::EndOfList][(int)Direction::EndOfList] = {
	{ 0b0001, 0b0010, 0b0100, 0b1000 },	// '.'

	{ 0b1000, 0b0100, 0b0010, 0b0001 },	// '/'
	{ 0b0100, 0b1000, 0b0001, 0b0010 },	// '\'

	{ 0b0001, 0b0010, 0b0011, 0b0011 },	// '|'
	{ 0b1100, 0b1100, 0b0100, 0b1000 }	// '-'
};


typedef struct Contraption* ContraptionPtr;
typedef struct Move* MovePtr;

struct Contraption {
	std::vector<std::string> layout;
	int width;
	int height;
	char* tracking;

	void Read(std::ifstream& input);
	void Print(void) const;
	char GetTile(int x, int y);

	void ResetTracking(void);
	void TrackBeam(int x, int y, Direction direction, std::deque<Move>& moves);

	int Score(int x, int y, Direction direction);

	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

struct Move {
	int x;
	int y;
	Direction direction;
};

void
Contraption::Read(std::ifstream& input)
{
	std::string line;

	tracking = NULL;
	height = 0;
	width = 0;

	while (std::getline(input, line))
	{
		layout.push_back(line);
		height++;
	}

	width = (int)layout[0].size();

	ResetTracking();
}

void
Contraption::Print(void) const
{
	int index = 0;
	printf("Width: %d, Height: %d\n", width, height);
	for (auto line : layout)
		printf("[%03d] %s\n", ++index, line.c_str());
}

char
Contraption::GetTile(int x, int y)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return '?';

	return layout[y][x];
}

void
Contraption::ResetTracking(void)
{
	if (tracking == NULL)
		tracking = new char[height * width];

	for (int i = 0; i < height * width; i++)
		tracking[i] = 0;
}

void
Contraption::TrackBeam(int x, int y, Direction direction, std::deque<Move>& moves)
{
	char tile;
	char* history;
	char operationMask;

	tile = GetTile(x, y);
	history = &tracking[x + y * width];
#if COMMENT == true
	printf("(%d,%d) '%c' " BYTE_TO_BINARY_PATTERN "\n", x, y, tile, BYTE_TO_BINARY(*history));
#endif
	if (tile == '?' || ((*history) & DirectionMask[(int)direction]) > 0)
		return;

	*history = (*history) | DirectionMask[(int)direction];

	operationMask = 0;
	switch (tile)
	{
	case '.':
		operationMask = NextDirection[(int)TileType::Empty][(int)direction];
		break;
	case '/':
		operationMask = NextDirection[(int)TileType::MirrorUp][(int)direction];
		break;
	case '\\':
		operationMask = NextDirection[(int)TileType::MirrorDown][(int)direction];
		break;
	case '|':
		operationMask = NextDirection[(int)TileType::SplitterVertical][(int)direction];
		break;
	case '-':
		operationMask = NextDirection[(int)TileType::SplitterHorizontal][(int)direction];
		break;
	}

	for (int i = 0; i < (int)Direction::EndOfList; i++)
		if ((operationMask & (1 << i)) > 0)
		{
			Move move;
			move.x = x + DX[i];
			move.y = y + DY[i];
			move.direction = (Direction)i;
			moves.push_back(move);
		}
}

int
Contraption::Score(int x, int y, Direction direction)
{
	int result;
	std::deque< Move> moves;
	Move move;

	move.x = x;
	move.y = y;
	move.direction = direction;
	moves.clear();
	moves.push_back(move);
	result = 0;

	ResetTracking();

	while (moves.size() > 0)
	{
		move = moves.front();
		moves.pop_front();
		TrackBeam(move.x, move.y, move.direction, moves);
	}

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			result += tracking[x + y * width] > 0;

	return result;
}



int64_t
Contraption::FindSolutionPart1(void)
{
	int64_t result = 0;

	result = Score(0, 0, Direction::Right);

	return result;
}

int64_t
Contraption::FindSolutionPart2(void)
{
	int64_t result = 0;
	int score;

	for (int x = 0; x < width; x++)
	{
		score = Score(x, 0, Direction::Down);
		if (score > result)
			result = score;
		score = Score(x, height - 1, Direction::Up);
		if (score > result)
			result = score;
	}

	for (int y = 0; y < height; y++)
	{
		score = Score(0, y, Direction::Right);
		if (score > result)
			result = score;
		score = Score(width - 1, y, Direction::Left);
		if (score > result)
			result = score;
	}

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Contraption problem;

	printf("Advent of Code - Day 16\n");

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