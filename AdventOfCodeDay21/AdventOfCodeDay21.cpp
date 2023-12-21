// AdventOfCodeDay21.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <cstdlib>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay21/sample.txt";
const int MaxStepPart1 = 6;
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay21/input.txt";
const int MaxStepPart1 = 64;
#endif
const int MaxStepPart2 = 26501365;

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

typedef struct Garden* GardenPtr;
typedef struct Plot* PlotPtr;
typedef struct Record* RecordPtr;

struct Plot {
	int x;
	int y;

	void Print(void) const { printf("(%d,%d)", x, y); }
	bool operator==(const Plot& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	void Min(const Plot& input) { x = MIN(x, input.x); y = MIN(y, input.y); }
	void Max(const Plot& input) { x = MAX(x, input.x); y = MAX(y, input.y); }
};

struct PlotHash {
	size_t operator()(const Plot& plot) const {
		return plot.x + plot.y * 1024;
	}
};

struct Record {
	Plot plot;
	int step;

	void Print(void) const { printf("plot: "); plot.Print(); printf(" %d", step); }
	bool operator==(const Record& rhs) const {
		return plot == rhs.plot && step == rhs.step;
	}
	bool operator >(const Record& rhs) const {
		return step > rhs.step;
	}
};

struct RecordHash {
	size_t operator()(const Record& record) const {
		return (record.plot.x + record.plot.y * 1024) * 512 + record.step;
	}
};

typedef std::priority_queue<Record,std::vector<Record>, std::greater<Record>> PriorityQueue;

const int NoStep = -1;

struct Garden {
	std::vector<std::string> layout;
	int width;
	int height;
	int* steps;
	Plot start;

	void Read(std::ifstream& input);
	void Print(void) const;

	bool CheckCoordinates(const Plot & plot) const;
	char GetPlot(const Plot &plot) const;

	int GetSteps(const Plot& plot) const;
	bool SetSteps(const Plot& plot, int input);
	void ProcessRecord(PriorityQueue& queue, const Record& record);

	int64_t PrintSolutionPart1(int maxStep) const;
	int64_t FindSolutionPart1(void);

	int64_t FindSolutionPart2(int maxSteps);
};

void
Garden::Read(std::ifstream& input)
{
	std::string line;
	std::string::size_type found;

	height = 0;
	width = 0;

	while (std::getline(input, line))
	{
		layout.push_back(line);
		found = line.find("S");
		if (found != std::string::npos)
		{
			start.x = (int)found;
			start.y = height;
		}
		height++;
	}

	width = (int)layout[0].size();
	steps = new int[9 * width * height];
	for (int i = 0; i < 9 * width * height; i++)
		steps[i] = NoStep;
}

void
Garden::Print(void) const
{
	int index = 0;

	printf("Width: %d, Height: %d\n", width, height);
	for (auto line : layout)
		printf("\t[%3d] %s\n", index++, line.c_str());
	printf("Start: "); start.Print(); printf("\n");
}

bool
Garden::CheckCoordinates(const Plot & plot) const
{
	if (plot.x < -width || plot.x >= 2*width ||
		plot.y < -height || plot.y >= 2*height)
		return false;

	return true;
}

char
Garden::GetPlot(const Plot &plot) const
{
	return layout[((plot.y % height) + height) % height][((plot.x % width) + width) % width];
}

int
Garden::GetSteps(const Plot& plot) const
{
	if (!CheckCoordinates(plot))
		return std::numeric_limits<int>::max();

	return steps[plot.x + width + ( plot.y + height ) * 3 * width];
}

bool
Garden::SetSteps(const Plot& plot, int input)
{
	if (!CheckCoordinates(plot))
		return false;

	steps[plot.x + width + ( plot.y + height ) * 3 * width] = input;
	return true;
}

void
Garden::ProcessRecord(PriorityQueue& queue, const Record& record)
{
	Record nRecord;

	for (int i = 1; i <= 4; i++)
	{
		nRecord.plot = { record.plot.x + DX[i], record.plot.y + DY[i] };

		if( GetPlot(nRecord.plot)!='#' && GetSteps(nRecord.plot) == NoStep )
		{
			nRecord.step = record.step + 1;
			SetSteps(nRecord.plot, nRecord.step);
			queue.push(nRecord);
		}
	}
}

int64_t
Garden::PrintSolutionPart1(int maxSteps) const
{
	int64_t reach = 0;
	int steps;

	printf("Solution\n");
	for (int y = 0; y < height; y++)
	{
		printf("\t[%3d]", y);
		for (int x = 0; x < width; x++)
		{
			steps = GetSteps({ x,y });
			if (steps != NoStep &&
				(steps % 2) == (maxSteps % 2) &&
				steps <= maxSteps)
			{
				printf("O");
				reach++;
			}
			else
				printf("%c", GetPlot({ x, y }));
		}
		printf("\n");
	}

	printf("Reach: %zd\n",reach);

	return reach;
}

int64_t
Garden::FindSolutionPart1(void)
{
	int64_t result = 0;
	PriorityQueue queue;
	Record record;
	int64_t processedRecord = 0;

	record.plot = start;
	record.step = 0;
	queue.push(record);

	while (!queue.empty())
	{
		record = queue.top();
		queue.pop();
		ProcessRecord(queue, record);
		processedRecord++;
	}

	printf("Processed records: %zd\n", processedRecord);

	return PrintSolutionPart1(MaxStepPart1);
}

int64_t
Garden::FindSolutionPart2(int maxSteps)
{
	std::queue<Plot> edge;
	std::unordered_set<Plot,PlotHash> visited;
	Plot plot, nPlot;
	size_t size;

	int64_t counts[3];
	int64_t fsize;
	int step;
	int64_t * edges; // Number of plots on the edge (the number to be extrapolated to find the result)
	int64_t * d1; // First derivatives
	int64_t * d2; // Second derivatives

	visited.insert(start);
	edge.push(start);
	counts[0] = 0;
	counts[1] = 1;

	edges = new int64_t[width];
	d1 = new int64_t[width];
	d2 = new int64_t[width];

	step = 0;
	while(true)
	{
		size = edge.size();
		for (size_t q = 0; q < size; ++q)
		{
			plot = edge.front();
			edge.pop();
			for (int direction = 1; direction <= 4; direction++)
			{
				nPlot = { plot.x + DX[direction], plot.y + DY[direction] };
				if (GetPlot(nPlot) != '#' && visited.find(nPlot) == visited.end())
				{
					edge.push(nPlot);
					visited.insert(nPlot);
				}
			}
		}

		fsize = (int)edge.size();
		// Compute the number of reached plots, respecting the odd/even pattern:
		counts[2] = fsize + counts[0];
		counts[0] = counts[1];
		counts[1] = counts[2];

		// Compute the modulo growth rate:
		int ix = step % width;
		if (step >= width)
		{
			int64_t dx = fsize - edges[ix]; // First derivative
			d2[ix] = dx - d1[ix]; // Second derivative
			d1[ix] = dx;
		}
		edges[ix] = fsize;
		step++;

		// Check if the modulo growth is now stable, i.e the second derivative (d2) is null
		if (step >= 2 * width)
		{
			bool allZero = true;
			for( int i = 0; i<width;i++)
				if (d2[i] != 0)
				{
					allZero = false;
					break;
				}
			if (allZero)
				break;
		}
	}

	// Extrapolate:
	for (int i = step; i < maxSteps; i++)
	{
		int ix = i % width;
		edges[ix] += d1[ix];

		counts[2] = edges[ix] + counts[0];
		counts[0] = counts[1];
		counts[1] = counts[2];
	}

	delete[] edges;
	delete[] d1;
	delete[] d2;

	return counts[2];
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Garden problem;

	printf("Advent of Code - Day 21\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	problem.Read(input);
	problem.Print();

	clockStart = clock();

	result = problem.FindSolutionPart1();
	printf("result part 1: %zd\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = problem.FindSolutionPart2(MaxStepPart2);
	printf("result part 2: %zd\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}