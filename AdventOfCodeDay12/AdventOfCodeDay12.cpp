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
#define COMMENT false

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

struct Record {
	char* row;
	int size;
	std::vector<int> damages;
	int minSize;

	void Read(std::string& input);
	void Print(void) const;
	int64_t CountArrangements(void);
	int64_t RecursiveCount(char* buffer, int position, std::vector<int>& listOfDamages, int length, int minLength);
	bool ProgressiveDamageValidation(char* buffer, std::vector<int>& listOfDamages, int length, int minLength);
	int64_t CountUnfoldedArrangements(int copies);
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
		printf("[%03d]", ++index);
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
	int64_t count;

	for (Record record : records)
	{
		printf("[%03d]", ++index);
		count = record.CountUnfoldedArrangements(5);
		printf(" => %zd\n", count);
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
	printf(", MinSize: %d\n", minSize);
}

int64_t
Record::CountArrangements(void)
{
	int64_t result;
	char* buffer;

	buffer = new char[size + 1];
	strcpy(buffer, row);
#if COMMENT == true
	printf("CountArrangements(%s,%d,%d)\n", buffer, size, minSize);
#endif
	result = RecursiveCount(buffer, 0, damages, size, minSize);
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
	{
		result = ProgressiveDamageValidation(buffer, listOfDamages, length, minLength) == true ? 1 : 0;
#if COMMENT == true
		printf("\t\t%s", buffer);
		if (result != 0)
			printf(" => MATCH");
		printf("\n");
#endif
		return result;
	}

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

#if COMMENT == true
	printf("\tProgressive validation: %s (", buffer);
	for (int i = 0; i < listOfDamages.size(); i++)
		printf(" %d", listOfDamages[i]);
	printf(")");
#endif

	tmp = buffer;
	for (int damage : listOfDamages)
	{
		while (*tmp != '\0' && *tmp == '.' && *tmp != '?')
			tmp++;

		if (*tmp == '\0')
		{
#if COMMENT == true
			printf(" => EOF\n");
#endif
			return false;
		}

		position = (int)( tmp - buffer);
		if ((length - position) < minLength)
		{
#if COMMENT == true
			printf(" => IMPOSSIBLE (size too small => length: %d, minLength: %d, position: %d)\n", length, minLength, position);
#endif
			return false;
		}

		if (*tmp == '?')
		{
#if COMMENT == true
			printf(" => POSSIBLE PROGRESSIVE (Possible)\n");
#endif
			return true;
		}

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
			{
#if COMMENT == true
				printf(" => POSSIBLE PROGRESSIVE (Uncomplete damage)\n");
#endif
				return true;
			}
			else
			{
#if COMMENT == true
				printf(" => IMPOSSIBLE (Too short)\n");
#endif
				return false;
			}
		}

		if (*tmp == '#')
		{
#if COMMENT == true
			printf(" => IMPOSSIBLE 2 (Continuous #)\n");
#endif
			return false;
		}

		minLength--;
	}

	while (*tmp != '\0' && *tmp != '?')
	{
		if (*tmp == '#')
		{
#if COMMENT == true
			printf(" => WRONG REMAINING\n");
#endif
			return false;
		}
		tmp++;
	}

#if COMMENT == true
	printf(" => POSSIBLE FINISH\n");
#endif

	return true;
}

int64_t
Record::CountUnfoldedArrangements(int copies)
{
	int64_t result;
	
	char* buffer;

	int64_t r1, r2,r3,r4;
	char first, last;
	char prev, next;

	buffer = new char[size + 2 + 1];
	r1 = r2 =r3=r4=0;
	first = row[0];
	last = row[size - 1];

	printf("row: %s, damage:", row);
	for (int damage : damages)
		printf(" %d", damage);
	printf(", size: %d, minSize: %d, first: %c, last: %c\n",size,minSize,first,last);

	// Case 1: nominal
	strncpy(buffer, row, size);
	buffer[size] = '\0';
	r1 = RecursiveCount(buffer, 0, damages, size, minSize);
	printf("\tcase 1: %s => %zd (nominal)\n", buffer,r1);

	// Case 2: begin
	buffer[size] = '?';
	buffer[size+1] = '\0';
	r2 = RecursiveCount(buffer, 0, damages, size + 1, minSize);
	printf("\tcase 2: %s => %zd (begin)\n", buffer, r2);

	// Case 3: end
	buffer[0] = '?';
	strncpy(buffer + 1, row, size);
	buffer[size + 1] = '\0';
	r3 = RecursiveCount(buffer, 0, damages, size + 1, minSize);
	printf("\tcase 3: %s => %zd (end)\n", buffer, r3);

	// Case 4: mid
	buffer[size+1] = '?';
	buffer[size + 2] = '\0';
	r4 = RecursiveCount(buffer, 0, damages, size + 2, minSize);
	printf("\tcase 4: %s => %zd (mid)\n", buffer, r4);

	result = r3*r2;
	for (int i = 2; i < copies; i++)
		result *= r4;

	return result;
}

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
	problem.Print();
	result = problem.FindSolution1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

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