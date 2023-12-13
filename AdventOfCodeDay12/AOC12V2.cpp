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

#define TEST_MODE true
#define COMMENT true

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
	int64_t RecursiveCount(char* buffer, int position, std::vector<int>& listOfDamages, int length, int minLength);
	bool ProgressiveDamageValidation(char* buffer, std::vector<int>& listOfDamages, int length, int minLength);
	int64_t RecursiveProbability(Input position, int depth, int limit, RecursiveInput& input);
	int64_t CountUnfoldedArrangements(int copies);

	int64_t RecursiveCountV2(char* buffer, int position, int index, std::vector<int>& listOfDamages, int length, int minLength);
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
	}

	return result;
}

int64_t
Problem::FindSolution2(void)
{
	int64_t result = 0;
	int index = 0;

	for (Record record : records)
		result += record.CountUnfoldedArrangements(5);

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
	printf(", MinSize: %d", minSize);
}

int64_t
Record::CountArrangements(void)
{
	int64_t result;
	char* buffer;

	buffer = new char[size + 1];
	strcpy(buffer, row);
//	result = RecursiveCount(buffer, 0, damages, size, minSize);
	progress = 0;
	result = RecursiveCountV2(buffer, 0, 0, damages, size, minSize);
	delete buffer;

	return result;
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

#define V(c) (input.data[(int)Input::c])
#define DP(x) (V(x)==N2?0:(V(x)-N2)*RecursiveProbability(Input::x,depth+1,limit,input))
#define DP2(y,x) (V(y)==V(x)?0:(V(y)-V(x))*RecursiveProbability(Input::x,depth+1,limit,input))

int64_t
Record::RecursiveProbability(Input position, int depth, int limit, RecursiveInput& input)
{
	int64_t result;
	int64_t N2;

	result = 0;

	if (depth == limit)
	{
		for (int i = 0; i < depth; i++)
			printf(" ");
		switch (position)
		{
		case Input::DB:
		case Input::DS:
			printf("%zd\n", V(N));
			return V(N);

		default:
			printf("0\n");
			return 0;
		}
	}

	N2 = V(N) * V(N);

	result = V(N) * RecursiveProbability(Input::DS, depth + 1, limit, input);
	result += DP(DS);
	result += DP2(DE, DS);
	if (position != Input::Start)
		result += DP2(DB, DS);

	for (int i = 0; i < depth; i++)
		printf(" ");
	printf("%zd\n", result);
/*
	switch (position)
	{
	case Input::Start:
		result = V(N) * RecursiveProbability(Input::DB, depth + 1, limit, input);
		result += DP(DS);
		result += DP2(DE, DS);
		break;

	case Input::DE:
	case Input::DF:
		result = V(N) * RecursiveProbability(Input::DB, depth + 1, limit, input);
		result += DP(DF);
		result += DP(DS);
		result += DP2(DF, DS);

	default:
		break;
	}
*/
	return result;
}

int64_t
Record::CountUnfoldedArrangements(int copies)
{
	int64_t result;
	char* buffer;
	bool startSensitivity, endSensitivity;
	RecursiveInput input;
	std::vector<int> doubleDamages;

	result = 0;
/*

	buffer = new char[2 * size + 3 + 1];

	// [XXX]
	buffer[0] = '\0';
	startSensitivity = endSensitivity = false;
	strncpy(buffer + 1, row, size);
	buffer[size + 1] = '\0';
	V(N) = RecursiveCount(buffer + 1, 0, damages, size, minSize);

	doubleDamages = damages;
	for (int i = 0; i < damages.size(); i++)
		doubleDamages.push_back(damages[i]);

	// DS = [XXX][?][XXX]
	buffer[size + 1] = '?';
	strncpy(buffer + size + 2, row, size);
	buffer[2 * size + 2] = '\0';
	V(DS) = RecursiveCount(buffer + 1, 0, doubleDamages, 2 * size + 1, 2 * minSize + 1);

	// DE = [XXX][?][XXX][?]
	buffer[2 * size + 2] = '?';
	buffer[2 * size + 3] = '\0';
	V(DE) = RecursiveCount(buffer + 1, 0, doubleDamages, 2 * size + 2, 2 * minSize + 1);

	// DB = [?][XXX][?][XXX]
	buffer[0] = '?';
	buffer[2 * size + 2] = '\0';
	V(DB) = RecursiveCount(buffer, 0, doubleDamages, 2 * size + 2, 2 * minSize + 1);

	// DF = [XXX][?][XXX][?]
	buffer[2 * size + 2] = '?';
	buffer[2 * size + 3] = '\0';
	V(DF) = RecursiveCount(buffer, 0, doubleDamages, 2 * size + 3, 2 * minSize + 1);

	printf("row: %s\n", row);
	printf("\tN: %zd, DS: %zd, DB: %zd, DE: %zd, DF: %zd\n", V(N), V(DS), V(DB), V(DE), V(DF));
	result = RecursiveProbability(Input::Start, 0, 4, input);
	printf("\t=> %zd\n",result);

	for (int i = 0; i < 4; i++)
	{
		if (row[0] == '?')
		{
			if ((i & 1) == 0)
				buffer[0] = '.';
			else
				buffer[0] = '#';
		}
		if (row[size - 1] == '?')
		{
			if ((i & 2) == 0)
				buffer[size - 1] = '.';
			else
				buffer[size - 1] = '#';
		}

		V(NGG + i) = RecursiveCount(buffer, 0, damages, size, minSize);
		printf("\t%s => %zd\n", buffer, V(NGG + i));
	}

	// Case 2: begin
	strncpy(buffer, row, size);
	buffer[size+1] = '\0';
	buffer[size] = '?';
	V(B) = RecursiveCount(buffer, 0, damages, size, minSize);
	for (int i = 0; i < 2; i++)
	{
		if ((i & 1) == 0)
			buffer[size] = '.';
		else
			buffer[size] = '#';

		V(BG + i) = RecursiveCount(buffer, 0, damages, size, minSize);
		printf("\t%s => %zd\n", buffer, V(BG + i));
	}

	// Case 3: end
	strncpy(buffer+1, row, size);
	buffer[size + 1] = '\0';
	buffer[0] = '?';
	V(E) = RecursiveCount(buffer, 0, damages, size, minSize);
	for (int i = 0; i < 2; i++)
	{
		if ((i & 1) == 0)
			buffer[0] = '.';
		else
			buffer[0] = '#';

		V(EG + i) = RecursiveCount(buffer, 0, damages, size, minSize);
		printf("\t%s => %zd\n", buffer, V(EG+ i));
	}

	// Case 4: mid
	strncpy(buffer + 1, row, size);
	buffer[0] = '?';
	buffer[size+1] = '?';
	buffer[size + 2] = '\0';
	V(M) = RecursiveCount(buffer, 0, damages, size, minSize);
	for (int i = 0; i < 4; i++)
	{
		if (row[0] == '?')
		{
			if ((i & 1) == 0)
				buffer[0] = '.';
			else
				buffer[0] = '#';
		}
		if (row[size - 1] == '?')
		{
			if ((i & 2) == 0)
				buffer[size - 1] = '.';
			else
				buffer[size - 1] = '#';
		}
		V(MGG + i) = RecursiveCount(buffer, 0, damages, size, minSize);
		printf("\t%s => %zd\n", buffer, V(MGG + i));
	}
*/
	/*
	#if COMMENT == true
		printf("row: %s (", row);
		for (int i = 0; i < damages.size(); i++)
		{
			if (i > 0)
				printf(", ");
			printf("%d", damages[i]);
		}
		printf("), input: (");
		for (int i = 0; i < (int)Input::EndOfList; i++)
		{
			if (i != 0)
				printf(" ,");
			printf("%zd", input.data[i]);
		}
		printf(") => %zd\n", result);
	#endif
	*/
	return result;
}

int64_t
Record::RecursiveCountV2(char* buffer, int position, int index, std::vector<int>& listOfDamages, int length, int minLength)
{
	int64_t result;
	int count;

	if (progress>0 && progress % threshold == 0)
		printf(".");
	progress++;

	result = 0;
	while (*buffer != '\0' && index < listOfDamages.size())
	{
#if COMMENT == true
		printf("\n");
		for (int i = 0; i < position; i++)
			printf(" ");
		printf("[%d] buffer: %s", index, buffer);
#endif
		// Step 1: skip operational spring
		while (*buffer != '\0' && *buffer == '.')
		{
			buffer++;
			position++;
		}
		if (*buffer == '\0' || (length - position) < minLength)
		{
#if COMMENT == true
			printf(" => EOF1\n");
#endif
			return 0;
		}

		// Step 2: recursion case ?
		if (*buffer == '?')
		{
#if COMMENT == true
			printf(" => Recursion [.]\n");
#endif
			*buffer = '.';
			result = RecursiveCountV2(buffer, position, index, listOfDamages, length, minLength);
#if COMMENT == true
			printf(" => Recursion [#]\n");
#endif
			*buffer = '#';
			result += RecursiveCountV2(buffer, position, index, listOfDamages, length, minLength);
			*buffer = '?';
			return result;
		}

		// Step 3: Go through the damaged spring
		count = listOfDamages[index];
		while (*buffer != '\0' && count > 0)
		{
			if (*buffer == '.')
				break;
			buffer++;
			position++;
			count--;
		}
		if (*buffer == '\0' || count > 0 || (length - position) < minLength)
		{
#if COMMENT == true
			printf(" => EOF2\n");
#endif
			return 0;
		}

		// Step 4: check the operational spring
		if (*buffer == '#')
		{
#if COMMENT == true
			printf(" => EOF3\n");
#endif
			return 0;
		}

		index++;
	}

	return 1;
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

		clockStart = clock();

	result = problem.FindSolution2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);
*/
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