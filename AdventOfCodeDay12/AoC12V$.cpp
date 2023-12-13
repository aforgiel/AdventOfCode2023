// AdventOfCodeDay12.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

bool debug = false;

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay12/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay12/input.txt";
#endif

bool MatchPattern(char* buffer, const char* pattern);
bool FindPattern(char** buffer, const char* pattern);
void SkipSpace(char** buffer);

inline bool IsDigit(char c)
{
	return (c >= '0' && c <= '9');
}

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

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
const char* pComma = ",";

int progress = 0;
const int threshold = 1000000;

typedef struct Problem* ProblemPtr;
typedef struct Record* RecordPtr;

struct Problem {
	std::vector<Record> records;

	void Read(std::ifstream& input);
	void Print(void) const;
	int64_t FindSolution1(void);
	int64_t FindSolution2(void);
};

enum class Input : int {
	Start,

	N,
	NGG,
	NGB,
	NBG,
	NBB,
	B,
	BG,
	BB,
	M,
	MG,
	MB,
	MGG,
	MGB,
	MBG,
	MBB,
	E,
	EG,
	EB,

	// Duals
	DS,
	DB,
	DE,
	DF,

	EndOfList
};

typedef struct RecursiveInput* RecursiveInputPtr;
struct RecursiveInput {
	int64_t data[(int)Input::EndOfList];
};

struct Record {
	char* row;
	int size;
	std::vector<int> damages;
	int minSize;
	char first, last;

	void Read(std::string& input);
	void Print(void) const;
	int64_t CountArrangements(void);
	int64_t RecursiveProbability(Input position, int depth, int limit, RecursiveInput& input);
	int64_t CountUnfoldedArrangements(int copies);

	int64_t RecursiveCountV2(const char* buffer, const int position, const int index, std::vector<int>& listOfDamages, const int length, const int minLength);

	int64_t RecursiveCount(char* buffer, int position, std::vector<int>& listOfDamages, int length, int minLength);
	bool ProgressiveDamageValidation(char* buffer, std::vector<int>& listOfDamages, int length, int minLength);
};

void
Problem::Read(std::ifstream& input)
{
	std::string line;
	Record record;

	while (std::getline(input, line))
	{
		record.Read(line);
		records.push_back(record);
	}
}

void
Problem::Print(void) const
{
	int index = 0;
	for (Record record : records)
	{
		printf("[%03d] ", ++index);
		record.Print();
		printf("\n");
	}
}

int64_t
Problem::FindSolution1(void)
{
	int64_t result = 0;
	int index = 0;
	int64_t count;

	for (Record record : records)
	{
		printf("[%03d] ", ++index);
		record.Print();
		count = record.CountArrangements();
		printf(" => %zd\n", count);
		result += count;
		/*
				if (index == 20)
					break;
		*/
	}

	return result;
}

int64_t
Problem::FindSolution2(void)
{
	int64_t result = 0;
	int index = 0;
	int64_t count;

	for (Record record : records)
	{
		printf("[%03d] ", ++index);
		record.Print();
		count = record.CountUnfoldedArrangements(5);
		printf(" => %zd\n", count);
		if (index == 10)
			break;
		result += count;
	}

	return result;
}

void
Record::Read(std::string& input)
{
	char* tmp;
	int damage;

	tmp = const_cast<char*>(input.c_str());
	size = 0;
	while (*tmp != ' ' && *tmp != '\0')
	{
		size++;
		tmp++;
	}

	row = new char[size + 1];

	strncpy(row, input.c_str(), size);
	row[size] = '\0';
	first = row[0];
	last = row[size - 1];

	damages.clear();
	minSize = 0;
	while (*tmp != '\0')
	{
		sscanf(tmp, "%d", &damage);
		damages.push_back(damage);
		minSize += damage + 1;
		FindPattern(&tmp, pComma);
	}
	minSize--;
}

void
Record::Print(void) const
{
	printf("Row: %s, Damages:", row);
	for (int i : damages)
		printf(" %d", i);
	printf(", Size: %d, MinSize: %d", size, minSize);
}

int64_t
Record::CountArrangements(void)
{
	int64_t result;

	progress = 0;
	result = RecursiveCountV2(row, 0, 0, damages, size, minSize);
/*
	int64_t validation;
	validation = RecursiveCount(row, 0, damages, size, minSize);
	if (validation != result)
	{
		printf("\n!!! WRONG COUNT (%zd vs. %zd) !!!\n", result, validation);
		debug = true;
		result = RecursiveCountV2(row, 0, 0, damages, size, minSize);
		debug = false;
	}
*/
	return result;
}

#define V(c) (input.data[(int)Input::c])
#define DP(y,x) (V(y)==V(x)?0:(V(y)-V(x))*RecursiveProbability(Input::x,depth+1,limit,input))

int64_t
Record::RecursiveProbability(Input position, int depth, int limit, RecursiveInput& input)
{
	int64_t result;

	result = 0;

	return result;
}

int64_t
Record::CountUnfoldedArrangements(int copies)
{
	int64_t result;

	char* buffer;
	std::vector<int> list;
	char* tmp;
	int length, minLength;

	buffer = new char[copies * (size + 1)];
	list.clear();
	tmp = buffer;
	minLength = 0;
	for (int i = 0; i < copies; i++)
	{
		strncpy(tmp, row, size);
		tmp += size;
		*tmp = '?';
		tmp++;
		for (int j = 0; j < damages.size(); j++)
		{
			list.push_back(damages[j]);
			minLength += damages[j] + 1;
		}
	}
	tmp--;
	*tmp = '\0';
	length = (size + 1) * copies - 1;
	minLength--;

	printf(" buffer: %s, length: %d, minLength: %d, damages:",buffer,length, minLength);
	for (int i = 0; i < list.size(); i++)
		printf(" %d", list[i]);
	printf("\n");

	result = RecursiveCountV2(buffer, 0, 0, list, length, minLength);

	delete buffer;

	return result;
}

int64_t
Record::RecursiveCountV2(const char* buffer, const int position, const int index, std::vector<int>& listOfDamages, const int length, const int minLength)
{
	int count;
	int pos, i, min;
	char* tmp;

	if (progress > 0 && progress % threshold == 0)
		printf(".");
	progress++;

	tmp = const_cast<char*>(buffer) + position;
	pos = position;
	i = index;
	min = minLength;
	while (*tmp != '\0' && i < listOfDamages.size())
	{
#if COMMENT == true
		if (debug)
		{
			printf("\n");
			for (int j = 0; j < pos; j++)
				printf(" ");
			printf("[%d] buffer: %s", index, tmp);
		}
#endif
		// Step 1: skip operational spring
		while (*tmp != '\0' && *tmp == '.')
		{
			tmp++;
			pos++;
		}
		if (*tmp == '\0' || (length - pos) < min)
		{
#if COMMENT == true
			if (debug)
				printf(" => EOF1\n");
#endif
			return 0;
		}

		// Step 2: recursion case ?
		if (*tmp == '?')
		{
			int64_t result;
#if COMMENT == true
			if (debug)
				printf(" => Recursion [#]");
#endif
			* tmp = '#';
			result = RecursiveCountV2(buffer, pos, i, listOfDamages, length, min);
#if COMMENT == true
			if (debug)
				printf(" => Recursion [.]");
#endif
			* tmp = '.';
			result += RecursiveCountV2(buffer, pos, i, listOfDamages, length, min);
			*tmp = '?';
			return result;
		}

		// Step 3: Go through the damaged spring
		count = listOfDamages[i];
#if COMMENT == true
		if (debug)
			printf(" [%d] damage %d", i, count);
#endif
		while (*tmp != '\0' && count > 0)
		{
			if (*tmp == '.')
				break;
			count--;
			tmp++;
			pos++;
			min--;
		}
#if COMMENT == true
		if (debug)
			printf(" (%d,%c)", count, *tmp == '\0' ? 'E' : *tmp);
#endif
		if (count == 0)
			i++;

		if (*tmp == '\0' && i == listOfDamages.size())
		{
#if COMMENT == true
			if (debug)
				printf(" => END OF LINE MATCH\n");
#endif
			return 1;
		}

		if (*tmp != '\0' && (count > 0 || (length - pos) < min))
		{
#if COMMENT == true
			if (debug)
				printf(" => EOF2\n");
#endif
			return 0;
		}

		// Step 4: check the operational spring
		if (*tmp == '#')
		{
#if COMMENT == true
			if (debug)
				printf(" => EOF3\n");
#endif
			return 0;
		}
		tmp++;
		pos++;
		min--;
	}

	while (*tmp != '\0')
	{
		if (*tmp == '#')
		{
#if COMMENT == true
			if (debug)
				printf(" => WRONG ENDING\n");
#endif
			return 0;
		}
		tmp++;
	}

#if COMMENT == true
	if (debug)
		printf(" => MATCH\n");
#endif

	return 1;
}

int64_t
Record::RecursiveCount(char* buffer, int position, std::vector<int>& listOfDamages, int length, int minLength)
{
	int64_t result;

	if (progress % threshold == 0)
		printf(".");
	progress++;

	while (position < length && buffer[position] != '?')
		position++;

	if (position == length)
		return ProgressiveDamageValidation(buffer, listOfDamages, length, minLength) == true ? 1 : 0;

	result = 0;
	buffer[position] = '.';
	if (ProgressiveDamageValidation(buffer, listOfDamages, length, minLength))
		result = RecursiveCount(buffer, position + 1, listOfDamages, length, minLength);
	buffer[position] = '#';
	if (ProgressiveDamageValidation(buffer, listOfDamages, length, minLength))
		result += RecursiveCount(buffer, position + 1, listOfDamages, length, minLength);
	buffer[position] = '?';

	return result;
}

bool
Record::ProgressiveDamageValidation(char* buffer, std::vector<int>& listOfDamages, int length, int minLength)
{
	int position;
	char* tmp;
	int count;

	tmp = buffer;
	for (int damage : listOfDamages)
	{
		while (*tmp != '\0' && *tmp == '.' && *tmp != '?')
			tmp++;

		if (*tmp == '\0')
			return false;

		position = (int)(tmp - buffer);
		if ((length - position) < minLength)
			return false;

		if (*tmp == '?')
			return true;

		count = 0;
		while (*tmp == '#' && count < damage)
		{
			count++;
			minLength--;
			tmp++;
		}

		if (count < damage)
		{
			if (*tmp == '?')
				return true;
			else
				return false;
		}

		if (*tmp == '#')
			return false;

		minLength--;
	}

	while (*tmp != '\0' && *tmp != '?')
	{
		if (*tmp == '#')
			return false;
		tmp++;
	}

	return true;
}

std::string testInput[] =
{
	".?????...??.?????...?.?????...??.?????...??.?????...? 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1",
	"???##?###?????????##?###?????????##?###?????????##?###?????????##?###????? 1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4",
	".#.#???..??#???#???.#.#???..??#???#???.#.#???..??#???#???.#.#???..??#???#???.#.#???..??#???#?? 1,1,1,1,1,4,1,1,1,1,1,4,1,1,1,1,1,4,1,1,1,1,1,4,1,1,1,1,1,4",
	"?.??#...#???.??#...#???.??#...#???.??#...#???.??#...#? 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1",
	"???#??.???#??#.??????#??.???#??#.??????#??.???#??#.??????#??.???#??#.??????#??.???#??#.?? 1,3,1,5,1,1,3,1,5,1,1,3,1,5,1,1,3,1,5,1,1,3,1,5,1"
};

#define NTest 5

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Problem problem;

	printf("Advent of Code - Day 12\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	clockStart = clock();

	problem.Read(input);
	result = problem.FindSolution1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);
/*
	Record test;

	for (int i = 0; i < NTest; i++)
	{
		test.Read(testInput[i]);
		test.Print();
		printf("\t=> %zd\n", test.CountArrangements());
	}
*/

	clockStart = clock();

	result = problem.FindSolution2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}

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