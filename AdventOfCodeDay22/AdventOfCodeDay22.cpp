// AdventOfCodeDay22.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <deque>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
#include <cstdlib>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay22/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay22/input.txt";
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

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int64_t GCD(int64_t a, int64_t b)
{
	if (b == 0)
		return a;
	return GCD(b, a % b);
}

int64_t LCM(std::vector<int64_t> group)
{
	int64_t result = group[0];

	for (int i = 1; i < group.size(); i++)
		result = (((group[i] * result)) /
			(GCD(group[i], result)));

	return result;
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

typedef struct Stack* StackPtr;
typedef struct Brick* BrickPtr;
typedef struct Point* PointPtr;

struct Point {
	int x;
	int y;
	int z;

	void Print(void) const { printf("(%d,%d,%d)", x, y, z); }
	void Read(char** buffer);
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	void Min(const Point& input) { x = MIN(x, input.x); y = MIN(y, input.y); z = MIN(z, input.z); }
	void Max(const Point& input) { x = MAX(x, input.x); y = MAX(y, input.y); z = MAX(z, input.z); }
};

struct PointHash {
	size_t operator()(const Point& plot) const {
		return plot.x + 599 * (plot.y + 569 * plot.z);
	}
};

struct Brick {
	int ID;
	Point min;
	Point max;

	std::set<BrickPtr> above;
	std::set<BrickPtr> below;
	int top;
	bool chainReaction;

	void Print(void) const {
		printf("[%04d]", ID); min.Print(); printf("~"); max.Print();
		printf(" below: "); for (BrickPtr ptr : below) printf("%d ", ptr->ID);
		printf("above: "); for (BrickPtr ptr : above) printf("%d ", ptr->ID);
	}
	void Read(std::string& line);

	void Add(std::set<BrickPtr>& group, BrickPtr brick);
	bool CanBeDisintegrated(void)const;
	int ChainReactionPotential(void)const;

	bool operator >(const Brick& rhs) const { return min.z > rhs.min.z; }
};

struct CompareBrickPtr {
	bool operator()(const BrickPtr lhs, const BrickPtr rhs) const { return *lhs > *rhs; }
};

typedef std::priority_queue<BrickPtr, std::vector<BrickPtr>, CompareBrickPtr> PriorityQueue;

struct Stack {
	std::vector<Brick> bricks;
	Point min;
	Point max;
	int xWidth;
	int yWidth;
	BrickPtr* floor;

	void Read(std::ifstream& input);
	void Print(void) const;

	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

void
Point::Read(char** buffer)
{
	sscanf(*buffer, "%d", &x);
	FindPattern(buffer, ",");
	sscanf(*buffer, "%d", &y);
	FindPattern(buffer, ",");
	sscanf(*buffer, "%d", &z);
	while (IsDigit(**buffer))
		(*buffer)++;
}

void
Brick::Read(std::string& line)
{
	char* tmp;
	Point point;

	min = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
	max = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	tmp = const_cast<char*>(line.c_str());
	point.Read(&tmp);
	min.Min(point);
	max.Max(point);
	tmp++;
	point.Read(&tmp);
	min.Min(point);
	max.Max(point);
}

void
Brick::Add(std::set<BrickPtr>& group, BrickPtr brick)
{
	auto search = group.find(brick);
	if (search != group.end())
		return;

	group.insert(brick);
}

bool
Brick::CanBeDisintegrated(void)const
{
	for (BrickPtr brick : above)
		if (brick->below.size() == 1)
			return false;

	return true;
}

int
Brick::ChainReactionPotential(void)const
{
	int result;

	result = 0;
	for (BrickPtr brick : above)
	{
		if (brick->chainReaction)
			continue;
		brick->chainReaction = true;
		for (BrickPtr ptr : brick->below)
			if (ptr->chainReaction == false)
				brick->chainReaction = false;

		if (brick->chainReaction)
			result += 1;
	}

	for (BrickPtr brick : above)
		if (brick->chainReaction)
			result += brick->ChainReactionPotential();

	return result;
}

void
Stack::Read(std::ifstream& input)
{
	std::string line;
	Brick brick;
	int ID;

	min = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
	max = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	ID = 1;
	while (std::getline(input, line))
	{
		brick.Read(line);
		brick.ID = ID;
		bricks.push_back(brick);
		min.Min(brick.min);
		max.Max(brick.max);
		ID++;
	}

	xWidth = max.x - min.x + 1;
	yWidth = max.y - min.y + 1;

	printf("floor min: %d,%d, max: %d,%d => xWidth: %d, y:Width: %d\n", min.x, min.y, max.x, max.y, xWidth, yWidth);
}

void
Stack::Print(void) const
{
	for (const Brick& brick : bricks)
	{
		brick.Print();
		printf("\n");
	}
}

int64_t
Stack::FindSolutionPart1(void)
{
	int64_t result = 0;
	PriorityQueue queue;
	BrickPtr brick, bottom;
	int top;

	floor = new BrickPtr[xWidth * yWidth];
	for (int i = 0; i < xWidth * yWidth; i++)
		floor[i] = NULL;

	for (Brick& brick : bricks)
		queue.push(&brick);

	while (!queue.empty())
	{
		brick = queue.top();
		queue.pop();
#if COMMENT == true
		brick->Print(); printf("\n");
#endif
		top = 0;
		// First pass: find the highest the brick can be
		for (int x = brick->min.x; x <= brick->max.x; x++)
			for (int y = brick->min.y; y <= brick->max.y; y++)
			{
				bottom = floor[x + y * xWidth];
				if (bottom != NULL)
				{
					if (bottom->top > top)
						top = bottom->top;
				}
			}
		// First pass: find the highest the brick can be
		brick->top = top + brick->max.z - brick->min.z + 1;
		for (int x = brick->min.x; x <= brick->max.x; x++)
			for (int y = brick->min.y; y <= brick->max.y; y++)
			{
				bottom = floor[x + y * xWidth];
				if (bottom != NULL)
				{
					if (bottom->top == top)
					{
						bottom->Add(bottom->above, brick);
						brick->Add(brick->below, bottom);
					}
				}
				floor[x + y * xWidth] = brick;
			}
#if COMMENT == true
		printf("      ");
		for (int x = min.x; x <= max.x; x++)
			printf("[%3d]", x);
		for (int y = min.y; y <= max.y; y++)
		{
			printf("\n[%3d] ", y);
			for (int x = min.x; x <= max.x; x++)
				printf(" %3d ", floor[x + y * xWidth] == NULL ? 0 : floor[x + y * xWidth]->top);
		}
		printf("\n");
#endif
	}

	for (Brick& brick : bricks)
	{
		brick.Print();
		if (brick.CanBeDisintegrated())
		{
			printf("=> Can be disintegrated");
			result++;
		}
		printf("\n");
	}

	return result;
}

int64_t
Stack::FindSolutionPart2(void)
{
	int64_t result = 0;
	int potential;

	for (Brick& brick : bricks)
	{
		for (Brick& ptr : bricks)
			ptr.chainReaction = false;

		brick.Print();
		brick.chainReaction = true;
		potential = brick.ChainReactionPotential();
		if (potential > 0)
		{
			printf("=> Would cause %d bricks to fall", potential);
			result += potential;
		}
		printf("\n");
	}

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Stack problem;

	printf("Advent of Code - Day 22\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	problem.Read(input);

	clockStart = clock();

	result = problem.FindSolutionPart1();

	printf("result part 1: %zd\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = problem.FindSolutionPart2();

	printf("result part 2: %zd\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}