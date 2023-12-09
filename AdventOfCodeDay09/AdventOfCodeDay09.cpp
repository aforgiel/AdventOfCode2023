// AdventOfCodeDay09.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay09/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay09/input.txt";
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

const char* pSpace = " ";

typedef struct OASIS* OASISPtr;
typedef struct Report* ReportPtr;

struct OASIS {
	std::vector<ReportPtr> reports;

	void Read(std::ifstream& input);
	void Print(void) const;
	int64_t FindSolution1(void);
	int64_t FindSolution2(void);
};

struct Report {
	std::vector<int> values;

	void Read(std::string& input);
	void Print(void) const;
	int PredictNext(std::vector<int>& input);
	std::vector<int>* GetReverseOrderValues(void);
};

void
OASIS::Read(std::ifstream& input)
{
	std::string line;
	ReportPtr report;

	while (std::getline(input, line))
	{
		report = new Report();
		report->Read(line);
		reports.push_back(report);
	}
}

void
OASIS::Print(void) const
{
	int index = 0;
	for (ReportPtr report : reports)
	{
		printf("[%03d]", ++index);
		report->Print();
	}
}

int64_t
OASIS::FindSolution1(void)
{
	int64_t result = 0;
	int index = 0;
	int next;

	for (ReportPtr report : reports)
	{
#if COMMENT == true
		printf("[%03d] ", ++index);
		report->Print();
#endif
		next = report->PredictNext(report->values);
#if COMMENT == true
		printf("=> %d\n", next);
#endif
		result += next;
	}

	return result;
}

int64_t
OASIS::FindSolution2(void)
{
	int64_t result = 0;
	int index = 0;
	int prev;
	std::vector<int>* reverseValues;

	for (ReportPtr report : reports)
	{
#if COMMENT == true
		printf("[%03d] ", ++index);
		report->Print();
#endif
		reverseValues = report->GetReverseOrderValues();
		prev = report->PredictNext(*reverseValues);
#if COMMENT == true
		printf("=> %d\n", prev);
#endif
		result += prev;
		delete reverseValues;
	}

	return result;
}

void
Report::Read(std::string& input)
{
	char* tmp;
	int value;

	tmp = const_cast<char*>(input.c_str());
	while (*tmp != '\0')
	{
		sscanf_s(tmp, "%d", &value);
		values.push_back(value);
		FindPattern(&tmp, pSpace);
	}
}

void
Report::Print(void) const
{
	for (int value : values)
		printf(" %d", value);
	printf("\n");
}

int
Report::PredictNext(std::vector<int>& input)
{
	std::vector<int> derivation;
	int delta;
	int index = 0;
	int prev = 0;
	bool allZero = true;

	for (auto it = begin(input); it != end(input); ++it)
	{
		switch (index)
		{
		case 0:
			prev = *it;
			break;
		default:
			delta = *it - prev;
#if COMMENT == true
			printf("%d ", delta);
#endif
			derivation.push_back(delta);
			if (delta != 0)
				allZero = false;
			prev = *it;
			break;
		}
		index++;
	}
#if COMMENT == true
	printf("\n");
#endif

	if (allZero == true)
		return prev;

	return prev + PredictNext(derivation);
}

std::vector<int>*
Report::GetReverseOrderValues(void)
{
	std::vector<int>* result;

	result = new std::vector<int>();
	for (auto it = rbegin(values); it != rend(values); ++it)
		result->push_back(*it);

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	OASIS oasis;

	printf("Advent of Code - Day 09\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	clockStart = clock();

	oasis.Read(input);
	oasis.Print();
	result = oasis.FindSolution1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = oasis.FindSolution2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}