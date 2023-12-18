// AdventOfCodeDay17.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay17/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay17/input.txt";
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

inline bool IsDigit(char c)
{
	return (c >= '0' && c <= '9');
}

typedef struct Map* MapPtr;
typedef struct Tile* TilePtr;
typedef struct Record* RecordPtr;

const int MaxCumulativeHeat = std::numeric_limits<int>::max();

struct Tile {
	int x;
	int y;

	bool operator==(const Tile& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
};

struct Record {
	Tile tile;
	Direction direction;
	int step;
	int cumulativeHeat;

	bool operator==(const Record& rhs) const {
		return tile == rhs.tile && direction == rhs.direction && step == rhs.step;
	}
	bool operator >(const Record& rhs) const {
		return cumulativeHeat > rhs.cumulativeHeat;
	}
};

struct RecordHash {
	size_t operator()(const Record& record) const {
		return (1 + record.tile.x + record.tile.y * 1024) * 512 + (int)record.direction * 32 + record.step;
	}
};

typedef std::priority_queue<Record, std::vector<Record>, std::greater<Record>> PriorityQueue;

struct Map {
	std::vector<std::string> layout;
	int width;
	int height;
	std::unordered_map<Record, int, RecordHash> records;

	void Read(std::ifstream& input);
	void Print(void) const;

	bool CheckCoordinates(int x, int y) const;
	int GetHeat(int x, int y) const;
	int GetCumulativeHeat(const Record& record)const;
	bool OnEdge(const Record& record, Direction direction);

	void ProcessRecord(PriorityQueue& queue, Record& record, int minStraightStep, int maxStraightStep);

	bool GetMinRecord(int x, int y, Record& record, int minStraightStep, int maxStraightStep) const;
	bool GetPathRecord(Record& record, int minStraightStep, int maxStraightStep) const;
	void PrintSolution(int sx, int sy, int minStraightStep, int maxStraightStep) const;

	int64_t FindSolution(int minStraightStep, int maxStraightStep);
};

void
Map::Read(std::ifstream& input)
{
	std::string line;

	height = 0;
	width = 0;

	while (std::getline(input, line))
	{
		layout.push_back(line);
		height++;
	}

	width = (int)layout[0].size();
}

const char* NullLabel = "     ";

void
Map::Print(void) const
{
	int index = 0;

	printf("Width: %d, Height: %d\n", width, height);
	for (auto line : layout)
		printf("\t[%3d] %s\n", index++, line.c_str());

#if COMMENT == true
	Record record;
	printf("Cumulative heat map:\n");
	index = 0;
	printf("\t     ");
	for (int i = 0; i < width; i++)
		printf("[%3d]", index++);
	printf("\n");
	index = 0;
	for (int i = 0; i < width * height; i++)
	{
		if (i % width == 0)
			printf("\t[%3d]", index++);
		if (GetMinRecord(i % width, i / width, record, 10))
			printf("%4d ", record.cumulativeHeat);
		else
			printf(NullLabel);
		if (i % width == width - 1)
			printf("\n");
	}
#endif
}

bool
Map::CheckCoordinates(int x, int y) const
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
		return false;

	return true;
}

int
Map::GetHeat(int x, int y) const
{
	if (!CheckCoordinates(x, y))
		return MaxCumulativeHeat;

	return layout[y][x] - '0';
}

int
Map::GetCumulativeHeat(const Record& record)const
{
	auto it = records.find(record);
	if (it != records.end())
		return it->second;
	return MaxCumulativeHeat;
}

bool
Map::OnEdge(const Record& record, Direction direction)
{
	switch (direction)
	{
	case Direction::Up:
		return record.tile.y == 0;
		break;
	case Direction::Right:
		return record.tile.x == (width - 1);
		break;
	case Direction::Down:
		return record.tile.y == (height - 1);
	case Direction::Left:
		return record.tile.x == 0;
	}

	return false;
}

void
Map::ProcessRecord(PriorityQueue& queue, Record& record, int minStraightStep, int maxStraightStep)
{
	Record nRecord;
	int nCumulativeHeat, altCumulativeHeat;
	bool valid;

#if COMMENT == true
	printf("ProcessRecord: (%d,%d) %c(%d) %d\n", record.tile.x, record.tile.y, DirectionSymbol[(int)record.direction], record.step, record.cumulativeHeat);
#endif

	for (int i = 1; i <= 4; i++)
	{
		nRecord.tile.x = record.tile.x + DX[i];
		nRecord.tile.y = record.tile.y + DY[i];

		if (CheckCoordinates(nRecord.tile.x, nRecord.tile.y) && i != (int)OppositeDirection[(int)record.direction])
		{
			nRecord.direction = (Direction)i;
			nRecord.cumulativeHeat = MaxCumulativeHeat;
			valid = false;

			altCumulativeHeat = record.cumulativeHeat + GetHeat(nRecord.tile.x, nRecord.tile.y);

			if (i == (int)record.direction || record.direction == Direction::All)
			{
				nRecord.step = record.step + 1;
				if (record.step < maxStraightStep)
					valid = true;
			}
			else
				if (record.step >= minStraightStep)
				{
					nRecord.step = 1;
					valid = true;
				}


			if (OnEdge(nRecord, nRecord.direction) && (nRecord.step < minStraightStep))
				valid = false;

			nCumulativeHeat = GetCumulativeHeat(nRecord);
			if (valid && altCumulativeHeat < nCumulativeHeat)
			{
#if COMMENT == true
				printf("\t%d > %d => set (%d,%d) %c (%d)\n", nCumulativeHeat, altCumulativeHeat, nRecord.tile.x, nRecord.tile.y, DirectionSymbol[(int)nRecord.direction], nRecord.step);
#endif
				nRecord.cumulativeHeat = altCumulativeHeat;
				records[nRecord] = altCumulativeHeat;
				queue.push(nRecord);
			}
		}
	}
}

bool
Map::GetMinRecord(int x, int y, Record& record, int minStraightStep, int maxStraightStep) const
{
	Record tmp;
	int minCumulativeHeat;

	tmp.tile.x = x;
	tmp.tile.y = y;
	minCumulativeHeat = MaxCumulativeHeat;
	auto mit = records.end();

#if COMMENT == true
	printf("GetMinRecord: (%d,%d)\n", x, y);
#endif
	for (int direction = 1; direction <= 5; direction++)
	{
		tmp.direction = (Direction)direction;
		for (int step = minStraightStep; step <= maxStraightStep; step++)
		{
			tmp.step = step;
			auto it = records.find(tmp);
			if (it != records.end() && it->second < minCumulativeHeat)
			{
#if COMMENT == true
				printf("\t%c (%d) => %d\n", DirectionSymbol[direction], step, it->second);
#endif
				minCumulativeHeat = it->second;
				mit = it;
			}
		}
	}

	if (mit != records.end())
	{
		record = mit->first;
#if COMMENT == true
		printf("\tFOUND %c (%d) => %d\n", DirectionSymbol[(int)record.direction], record.step, record.cumulativeHeat);
#endif
		return true;
	}

	return false;
}

bool
Map::GetPathRecord(Record& record, int minStraightStep, int maxStraightStep) const
{
	Record tmp;
	int minCumulativeHeat;

	tmp.tile.x = record.tile.x - DX[(int)record.direction];
	tmp.tile.y = record.tile.y - DY[(int)record.direction];

	if (record.step > 1)
	{
		tmp.step = record.step - 1;
		if (tmp.tile.x == 0 && tmp.tile.y == 0)
			tmp.direction = Direction::All;
		else
			tmp.direction = record.direction;
		auto it = records.find(tmp);
		if (it != records.end())
		{
			record = it->first;
			return true;
		}
		return false;
	}

	minCumulativeHeat = MaxCumulativeHeat;
	auto mit = records.end();

	for (int direction = 1; direction <= 5; direction++)
		if (direction != (int)OppositeDirection[(int)record.direction] && direction != (int)record.direction)
		{
			tmp.direction = (Direction)direction;
			for (int step = minStraightStep; step <= maxStraightStep; step++)
			{
				tmp.step = step;
				auto it = records.find(tmp);
				if (it != records.end() && it->second < minCumulativeHeat)
				{
					minCumulativeHeat = it->second;
					mit = it;
				}
			}
		}

	if (mit == records.end())
		return false;

	record = mit->first;

	return true;
}

void
Map::PrintSolution(int sx, int sy, int minStraightStep, int maxStraightStep) const
{
	int index;
	std::vector<std::string> solutions;
	Record record;
	int solution;

	if (!GetMinRecord(sx, sy, record, minStraightStep, maxStraightStep))
	{
		printf("No solution\n");
		return;
	}

	printf("end: (%d, %d) %c %d\n", record.tile.x, record.tile.y, DirectionSymbol[(int)record.direction], record.step);

	solutions.clear();
	for (std::string line : layout)
		solutions.push_back(line);

	index = 0;
	solution = 0;
	while (!(record.tile == Tile{ 0,0 }))
	{
		if (!IsDigit(solutions[record.tile.y][record.tile.x]))
		{
			printf("Loop error at (%d,%d)\n", record.tile.x, record.tile.y);
			break;
		}

		solution += GetHeat(record.tile.x, record.tile.y);
		solutions[record.tile.y][record.tile.x] = DirectionSymbol[(int)record.direction];

		if (!GetPathRecord(record, minStraightStep, maxStraightStep))
		{
			printf("Path error at (%d,%d)\n", record.tile.x, record.tile.y);
			break;
		}
	}

	printf("Path to (%d,%d) => %d\n", sx, sy, solution);
	index = 0;
	for (std::string line : solutions)
		printf("\t[%3d] %s\n", index++, line.c_str());
}

int64_t
Map::FindSolution(int minStraightStep, int maxStraightStep)
{
	PriorityQueue queue;
	Record record;

	int64_t recordProcessed = 0;

	records.clear();

	record.tile.x = 0;
	record.tile.y = 0;
	record.direction = Direction::All;
	record.step = 1;
	record.cumulativeHeat = 0;
	records[record] = 0;
	queue.push(record);

	while (!queue.empty())
	{
		record = queue.top();
		queue.pop();

		recordProcessed++;

		ProcessRecord(queue, record, minStraightStep, maxStraightStep);
	}

	printf("\nProcessed records: %zd\n", recordProcessed);

	if (!GetMinRecord(width - 1, height - 1, record, minStraightStep, maxStraightStep))
		return MaxCumulativeHeat;

	return record.cumulativeHeat;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Map map;

	printf("Advent of Code - Day 17\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	map.Read(input);

	clockStart = clock();

	result = map.FindSolution(1, 3);

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	map.PrintSolution(map.width - 1, map.height - 1, 1, 3);

	clockStart = clock();

	result = map.FindSolution(4, 10);

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	map.PrintSolution(map.width - 1, map.height - 1, 4, 10);

	input.close();

	return 0;
}