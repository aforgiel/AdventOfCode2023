// AdventOfCodeDay15.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#define COMMENT true

#if( TEST_MODE == true)
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay15/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay15/input.txt";
#endif

#define NumberOfBoxes 256

typedef struct Sequence* SequencePtr;
typedef struct Lense* LensePtr;
typedef struct Box* BoxPtr;
typedef struct Step* StepPtr;

struct Lense {
	std::string label;
	int length;
};

struct Box {
	std::vector<Lense> lenses;
	void Clear(void);
	bool IsEmpty(void);
	void Print(void) const;
	void RemoveLense(std::string label);
	void InsertLense(std::string label, int length);
	int Score(void);
};

enum class Instruction : int {
	Void,
	Remove,
	Insert
};

struct Step {
	std::string input;
	std::string label;
	Instruction instruction;
	int focalLength;

	void Read(char** tmp);
	void Print(void) const;
	int HASH(const char* input) const;
	void Execute(std::array<Box, NumberOfBoxes>& boxes);
};

struct Sequence {
	std::array<Box, NumberOfBoxes> boxes;
	std::vector<Step> steps;

	void Read(std::ifstream& input);
	void Print(void) const;
	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

void
Sequence::Read(std::ifstream& input)
{
	std::string line;
	Step step;

	steps.clear();
	while (std::getline(input, line))
	{
		char* tmp;
		tmp = const_cast<char*>(line.c_str());
		while ((*tmp) != '\0')
		{
			step.Read(&tmp);
			steps.push_back(step);
		}
	}

	for (Box box : boxes)
		box.Clear();
}

void
Sequence::Print(void) const
{
	int index = 0;
	printf("Steps: %d\n", (int)steps.size());
	for (Step step : steps)
	{
		printf("[%04d] ", ++index);
		step.Print();
	}

	index = 0;
	printf("Boxes: %d\n", (int)boxes.size());
	for (Box box : boxes)
	{
		if (!box.IsEmpty())
		{
			printf("[%03d] ", index);
			box.Print();
		}
		index++;
	}
}

int64_t
Sequence::FindSolutionPart1(void)
{
	int64_t result = 0;

	for (Step step : steps)
		result += step.HASH(step.input.c_str());

	return result;
}

int64_t
Sequence::FindSolutionPart2(void)
{
	int64_t result = 0;
	int index;

	for (Step step : steps)
		step.Execute(boxes);

	for (index = 0; index < boxes.size(); index++)
		result += (index + 1) * boxes[index].Score();

	Print();

	return result;
}

void
Box::Clear(void)
{
	lenses.clear();
}

bool
Box::IsEmpty(void)
{
	return lenses.size() == 0;
}

void
Box::Print(void) const
{
	for (Lense lense : lenses)
		printf(" [%s %d]", lense.label.c_str(), lense.length);
	printf("\n");
}

void
Box::RemoveLense(std::string label)
{
	for (auto it = lenses.begin(); it != lenses.end(); ++it)
		if ((*it).label == label)
		{
			lenses.erase(it);
			return;
		}
}

void
Box::InsertLense(std::string label, int length)
{
	for (auto it = lenses.begin(); it != lenses.end(); ++it)
		if ((*it).label == label)
		{
			(*it).length = length;
			return;
		}

	Lense lense;

	lense.label = label;
	lense.length = length;
	lenses.push_back(lense);
}

int
Box::Score(void)
{
	int result;
	int index;

	result = 0;
	for (index = 0; index < lenses.size(); index++)
		result += (index + 1) * lenses[index].length;

	return result;
}

void
Step::Read(char** tmp)
{
	input.clear();
	label.clear();
	focalLength = 0;
	// Read the label:
	while (**tmp != '\0' && **tmp != '-' && **tmp != '=')
	{
		label += **tmp;
		input += **tmp;
		(*tmp)++;
	}

	// Read the instruction:
	switch (**tmp)
	{
	case '-':
		instruction = Instruction::Remove;
		break;
	case '=':
		instruction = Instruction::Insert;
		input += **tmp;
		(*tmp)++;
		sscanf(*tmp, "%d", &focalLength);
		break;
	default:
		instruction = Instruction::Void;
		break;
	}

	while (**tmp != '\0' && **tmp != ',')
	{
		input += **tmp;
		(*tmp)++;
	}

	if (**tmp == ',')
		(*tmp)++;
}

void
Step::Print(void) const
{
	printf("%s => %d, label: %s, instruction: %d, length: %d, box: %d\n",
		input.c_str(), HASH(input.c_str()),
		label.c_str(), (int)instruction, focalLength, HASH(label.c_str()));
}

int
Step::HASH(const char* input) const
{
	int result;
	char* tmp;

	tmp = const_cast<char*>(input);
	result = 0;
	while (*tmp != '\0')
	{
		result += (int)*tmp;
		result *= 17;
		result = result % 256;
		tmp++;
	}

	return result;
}

void
Step::Execute(std::array<Box, NumberOfBoxes>& boxes)
{
	int ID;

	ID = HASH(label.c_str());
	switch (instruction)
	{
	case Instruction::Remove:
		boxes[ID].RemoveLense(label);
		break;
	case Instruction::Insert:
		boxes[ID].InsertLense(label, focalLength);
		break;
	default:
		break;
	}
}


int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Sequence problem;

	printf("Advent of Code - Day 15\n");

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