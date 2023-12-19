// AdventOfCodeDay18.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay18/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay18/input.txt";
#endif

enum class Direction : int {
	Void = 0,

	Up,			// 1
	Right,		// 2
	Down,		// 3
	Left,		// 4

	All,

	EndOfList
};

const char DirectionSymbol[(int)Direction::EndOfList] = { ' ', '^', '>', 'v', '<', '?' };
const Direction OppositeDirection[(int)Direction::EndOfList] = { Direction::Void,Direction::Down,Direction::Left, Direction::Up,Direction::Right,Direction::All };

const int DX[(int)Direction::EndOfList] = { 0,0,1,0,-1,0 };
const int DY[(int)Direction::EndOfList] = { 0,-1,0,1,0,0 };

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

typedef struct DigPlan* DigPlanPtr;
typedef struct Instruction* InstructionPtr;
typedef struct Point* PointPtr;

struct Point {
	int64_t x;
	int64_t y;

	void Print(void)const;
	bool operator==(const Point& rhs) const { return x == rhs.x && y == rhs.y; }
};

struct Instruction {
	Direction direction;
	int length;
	std::string color;

	void Read(std::string& line);
	void Print(void)const;
	void Move(Point &point) const;
	void Decode(Direction& direction, int& length) const;
};

struct DigPlan {
	std::vector<Instruction> instructions;
	int width;
	int height;
	Point start;
	Point digger;
	char* map;

	void Read(std::ifstream& input);
	void Print(void) const;
	void FindSetup(void);
	void DigCube(Point point, char c);
	void ExecuteInstruction(Instruction instruction);
	char GetCube(Point point) const;
	void DigTrench(void);
	int DigLagoon(void);

	int64_t ComputeArea(void);
	int64_t ComputeCorrectedArea(void);

	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

void
Point::Print(void)const
{
	printf("(%zd,%zd)", x, y);
}

const char* pColorStart = " (";

void
Instruction::Read(std::string& line)
{
	char* tmp;
	char colorBuffer[10];

	tmp = const_cast<char*>(line.c_str());
	switch (*tmp)
	{
	case 'U':
		direction = Direction::Up;
		break;
	case 'R':
		direction = Direction::Right;
		break;
	case 'D':
		direction = Direction::Down;
		break;
	case 'L':
		direction = Direction::Left;
		break;
	default:
		direction = Direction::Void;
		break;
	}
	tmp++;

	sscanf(tmp, "%d", &length);
	FindPattern(&tmp, pColorStart);
	strncpy(colorBuffer, tmp, 7);
	colorBuffer[7] = '\0';
	color = std::string(colorBuffer);
}

void
Instruction::Print(void)const
{
	printf("D: %c, L: %d, C: %s\n", DirectionSymbol[(int)direction], length, color.c_str());
}

void
Instruction::Move(Point& point) const
{
	point.x += length * DX[(int)direction];
	point.y += length * DY[(int)direction];
}

inline int HexToDigital(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';

	if (c >= 'a' && c <= 'f')
		return 10 + c - 'a';

	return 0;
}

void
Instruction::Decode(Direction& direction, int& length) const
{
	char* tmp;
	int64_t mask;

	tmp = const_cast<char*>(color.c_str());
	tmp++; // Skip the #
	mask = 0;
	while (*tmp != '\0')
	{
		mask = mask << 4;
		mask = mask | HexToDigital(*tmp);
		tmp++;
	}

	switch( mask & 0b1111 )
	{
	case 0:
		direction = Direction::Right;
		break;
	case 1:
		direction = Direction::Down;
		break;
	case 2:
		direction = Direction::Left;
		break;
	case 3:
		direction = Direction::Up;
		break;
	default:
		direction = Direction::Void;
	}
	length = (int) mask >> 4;
}

void
DigPlan::Read(std::ifstream& input)
{
	std::string line;
	Instruction instruction;

	while (std::getline(input, line))
	{
		instruction.Read(line);
		instructions.push_back(instruction);
	}

	FindSetup();

	map = new char[width * height];
	memset(map, '.', width*height);

	DigTrench();
}

void
DigPlan::Print(void) const
{
	int index = 0;

#if COMMENT == true
	for (Instruction instruction : instructions)
	{
		printf("[%03d] ", ++index);
		instruction.Print();
	}
#endif
	printf("width: %d, height: %d, start: ", width, height); start.Print(); printf("\n");
	index = 0;
	for (int i = 0; i < width * height; i++)
	{
		if (i % width == 0)
			printf("\t[%3d] ", index++);
		printf("%c", map[i]);
		if (i % width == width - 1)
			printf("\n");
	}
}

void
DigPlan::FindSetup(void)
{
	Point min, max, tmp;

	min = { std::numeric_limits<int>::max(),std::numeric_limits<int>::max() };
	max = { std::numeric_limits<int>::min(),std::numeric_limits<int>::min() };

	tmp = { 0,0 };
	for (Instruction instruction : instructions)
	{
		instruction.Move(tmp);
		if (tmp.x > max.x)
			max.x = tmp.x;
		if (tmp.y > max.y)
			max.y = tmp.y;
		if (tmp.x < min.x)
			min.x = tmp.x;
		if (tmp.y < min.y)
			min.y = tmp.y;
	}

	printf("min: "); min.Print(); printf("\n");
	printf("max: "); max.Print(); printf("\n");

	width = (int)(max.x - min.x +1);
	height = (int)(max.y - min.y +1);

	start.x = -min.x;
	start.y = -min.y;
}

void
DigPlan::DigCube(Point point, char c )
{
	map[point.x + point.y * width] = c;
}

void
DigPlan::ExecuteInstruction(Instruction instruction)
{
	DigCube(digger, DirectionSymbol[(int)instruction.direction]);
	for (int i = 0; i < instruction.length; i++)
	{
		DigCube(digger, i == 0 ? DirectionSymbol[(int)instruction.direction] : '#');
		digger.x += DX[(int)instruction.direction];
		digger.y += DY[(int)instruction.direction];
	}
}

char
DigPlan::GetCube(Point point) const
{
	if (point.x < 0 || point.x >= width ||
		point.y < 0 || point.y >= height)
		return '.';

	return map[point.x + point.y * width];
}

void
DigPlan::DigTrench(void)
{
	digger = start;
	for (Instruction instruction : instructions)
		ExecuteInstruction(instruction);
}

int
DigPlan::DigLagoon(void)
{
	int result = 0;
	Point point,next;
	std::queue<Point> queue;
	char cube;

	point.x = 0;
	for (point.y = 0; point.y < height; point.y++)
		if (GetCube(point) != '.')
			break;
	point.x++;
	point.y++;
	queue.push(point);
	while (!queue.empty())
	{
		point = queue.front();
		queue.pop();
		cube = GetCube(point);
		if (cube == '.')
		{
			DigCube(point, 'O');
			for (int i = 1; i <= 4; i++)
			{
				next.x = point.x + DX[i];
				next.y = point.y + DY[i];
				queue.push(next);
			}
		}
	}

	for (point.y = 0; point.y < height; point.y++)
		for (point.x = 0; point.x < width; point.x++)
			if (GetCube(point) != '.')
				result++;
			
	return result;
}

int64_t
DigPlan::ComputeArea(void)
{
	std::vector<Point> vertices;
	Point point, point2;
	int64_t result, area, perimeter;
	int size;

	point = start;

	perimeter = 0;
	for (Instruction instruction : instructions)
	{
		instruction.Move(point);
		vertices.push_back(point);
		perimeter += instruction.length;
	}

	area = 0;
	point = vertices[0];
	size = (int)vertices.size();
	for (int i = 1; i <= size; i++)
	{
		point2 = vertices[i%size];
		area += point.x * point2.y - point2.x * point.y;
		point = point2;
	}
	area /= 2;

	result = area + perimeter / 2 + 1;

	return result;
}

int64_t
DigPlan::ComputeCorrectedArea(void)
{
	std::vector<Point> vertices;
	Direction direction;
	int length;
	Point point, point2;
	int64_t result, area, perimeter;
	int size;

	point = { 0,0 };
	perimeter = 0;
	for (Instruction instruction : instructions)
	{
		instruction.Decode(direction, length);
		point.x += length * DX[(int)direction];
		point.y += length * DY[(int)direction];
#if COMMENT == true
		printf("Direction: %c, length: %d => ", DirectionSymbol[(int)direction], length); point.Print(); printf("\n");
#endif
		vertices.push_back(point);
		perimeter += length;
	}

	area = 0;
	point = vertices[0];
	size = (int)vertices.size();
	for (int i = 1; i <= size; i++)
	{
		point2 = vertices[i % size];
		area += point.x * point2.y - point2.x * point.y;
		point = point2;
	}
	area /= 2;

	result = area + perimeter / 2 + 1;

	return result;
}

int64_t
DigPlan::FindSolutionPart1(void)
{
	DigLagoon();

	return ComputeArea();
}

int64_t
DigPlan::FindSolutionPart2(void)
{
	return ComputeCorrectedArea();
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	DigPlan digPlan;

	printf("Advent of Code - Day 18\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	digPlan.Read(input);
	digPlan.Print();

	clockStart = clock();

	result = digPlan.FindSolutionPart1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	digPlan.Print();

	clockStart = clock();

	result = digPlan.FindSolutionPart2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}