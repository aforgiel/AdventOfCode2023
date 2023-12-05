// AdventOfCodeDay05.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay05/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay05/input.txt";
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

inline bool IsDigit(char c)
{
	return (c >= '0' && c <= '9');
}

inline bool IsSymbol(char c)
{
	return !IsDigit(c) && c != '.';
}

typedef struct Almanac* AlmanacPtr;
typedef struct SmartLUT* SmartLUTPtr;
typedef struct LUTEntry* LUTEntryPtr;
typedef struct Range* RangePtr;

struct Almanac {
	std::map<int64_t, int64_t> seeds;
	std::vector<SmartLUTPtr> steps;

	void Read(std::ifstream& input);
	void Print(void);
	int64_t FindLocation(int64_t seed);

	std::vector<Range> FindDestinationRanges(int step, std::vector<Range>& ranges);
	int64_t FindLowestLocation(void);
};

struct SmartLUT {
	std::string name;
	std::map<int64_t, LUTEntry> entries;

	void Read(std::ifstream& input);
	void Print(void);
	int64_t FindDestination(int64_t source);
	std::vector<Range> FindDestinationRanges(Range& range);
};

struct LUTEntry {
	int64_t destination;
	int64_t range;
};

struct Range {
	int64_t min;
	int64_t max;
};

const char* pSpace = " ";
const char* pSeeds = "seeds:";
const char* pSteps[] = {
	"seed-to-soil",
	"soil-to-fertilizer",
	"fertilizer-to-water",
	"water-to-light",
	"light-to-temperature",
	"temperature-to-humidity",
	"humidity-to-location"
};

void
Almanac::Read(std::ifstream& input)
{
	std::string line;
	char* tmp;
	int64_t seed, range;
	int step;
	SmartLUTPtr smartLUT;

	std::getline(input, line);
	tmp = const_cast<char*>(line.c_str());
	FindPattern(&tmp, pSeeds);
	while (FindPattern(&tmp, pSpace))
	{
		sscanf_s(tmp, "%zd", &seed);
		FindPattern(&tmp, pSpace);
		sscanf_s(tmp, "%zd", &range);
		seeds[seed] = range;
	}

	step = 0;
	while (input && step < 7)
	{
		std::getline(input, line);
		tmp = const_cast<char*>(line.c_str());

		if (FindPattern(&tmp, pSteps[step]))
		{
			smartLUT = new SmartLUT();
			smartLUT->name = pSteps[step];
			smartLUT->Read(input);
			steps.push_back(smartLUT);
			step++;
		}
	}
}

void
Almanac::Print(void)
{
	printf("Seeds:");
	for (const auto& n : seeds)
		printf(" [%zd-%zd]", n.first, n.first + n.second - 1);

	printf("\n");
	for (SmartLUTPtr smartLUT : steps)
		smartLUT->Print();
}

int64_t
Almanac::FindLocation(int64_t seed)
{
	int step;
	int64_t result;

	result = seed;
	for (step = 0; step < 7; step++)
		result = steps[step]->FindDestination(result);

	return result;
}

std::vector<Range>
Almanac::FindDestinationRanges(int step, std::vector<Range> & ranges)
{
	std::vector<Range> result;
	std::vector<Range> tmp;

	result.clear();
	for (Range range : ranges)
	{
		tmp = steps[step]->FindDestinationRanges(range);
		result.insert(result.end(), tmp.cbegin(), tmp.cend());
	}

	return result;
}


int64_t
Almanac::FindLowestLocation(void)
{
	int64_t result;
	result = std::numeric_limits<int64_t>::max();
	std::vector<Range> input;
	Range seedRange;
	for (const auto& n : seeds)
	{
		seedRange.min = n.first;
		seedRange.max = n.first+ n.second-1;

		printf("FindLowestLocation[%zd-%zd]\n", seedRange.min, seedRange.max);

		input.clear();
		input.push_back(seedRange);
		for (int step = 0; step < 7; step++)
		{
			input = FindDestinationRanges(step, input);
#if COMMENT == true
			printf("\t=>");
			for (Range range : input)
				printf(" [%zd-%zd]", range.min, range.max);
			printf("\n");
#endif
		}

		printf("\t=>");
		for (Range range : input)
			printf(" [%zd-%zd]", range.min, range.max);
		printf("\n");

		for (Range range : input)
			if (range.min < result)
				result = range.min;
	}

	return result;
}

void
SmartLUT::Read(std::ifstream& input)
{
	std::string line;
	char* tmp;
	int64_t source, destination, range;
	LUTEntry entry;

	while (input)
	{
		std::getline(input, line);
		tmp = const_cast<char*>(line.c_str());
		if (*tmp == '\0')
			break;

		sscanf_s(tmp, "%zd %zd %zd", &destination, &source, &range);
		entry.destination = destination;
		entry.range = range;
		entries[source] = entry;
	};
}

void
SmartLUT::Print(void)
{
	printf("SmartLUT: %s\n", name.c_str());
	for (const auto& n : entries)
		printf("source: %zd, destination: %zd, range: %zd\n", n.first, n.second.destination, n.second.range);
}

int64_t
SmartLUT::FindDestination(int64_t source)
{
	for (const auto& n : entries)
		if (source >= n.first && source < n.first + n.second.range)
			return n.second.destination + source - n.first;

	return source;
}

std::vector<Range>
SmartLUT::FindDestinationRanges(Range& range)
{
	std::vector<Range> result;
	Range current;
	Range entry;
	Range destination;
	Range tmp;

#if COMMENT == true
	printf("\tFindDestinationRange(%s,[%zd-%zd])\n", name.c_str(), range.min, range.max);
#endif

	current = range;
	for (const auto& n : entries)
	{
		entry.min = n.first;
		entry.max = n.first + n.second.range - 1;
		destination.min = n.second.destination;
		destination.max = n.second.destination + n.second.range - 1;
#if COMMENT == true
		printf("\t\t[%zd-%zd] => [%zd-%zd]\n", entry.min, entry.max, destination.min, destination.max);
#endif
		if (current.min < entry.min)
		{
			if (current.max < entry.min)
			{
				// case 1: [CCC] [EEE]
				result.push_back(current);
				return result;
			}
			else
			{
				// case 2: [CCC[EEE]][EEE]
				tmp.min = current.min;
				tmp.max = entry.min - 1;
				result.push_back(tmp);
				current.min = entry.min;
			}
		}

		if (current.min <= entry.max )
		{
			if (current.max <= entry.max)
			{
				// case 3: [EEE[CCC]EEE]
				tmp.min = destination.min + current.min - entry.min;
				tmp.max = destination.min + current.max - entry.min;
				result.push_back(tmp);
				return result;
			}
			else
			{
				// case 4: [EEE[CCC]][CCC]
				tmp.min = destination.min + current.min - entry.min;
				tmp.max = destination.max;
				result.push_back(tmp);
				current.min = entry.max+1;
			}
		}
	}

	// case 5: [EEE] [CCC]
	result.push_back(current);

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Almanac almanac;

	printf("Advent of Code - Day 05\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	result = 0;
	clockStart = clock();

	almanac.Read(input);
	almanac.Print();
	result = almanac.FindLowestLocation();

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}