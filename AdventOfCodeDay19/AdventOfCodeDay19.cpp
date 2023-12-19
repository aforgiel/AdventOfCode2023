// AdventOfCodeDay19.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay19/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay19/input.txt";
#endif

enum class Category : int {
	ExtremelyCoolLooking,		// 1
	Musical,					// 2
	Aerodynamic,				// 3
	Shiny,						// 4

	EndOfList
};

inline Category GetCategory(char c)
{
	switch (c)
	{
	case 'x':
		return Category::ExtremelyCoolLooking;
	case 'm':
		return Category::Musical;
	case 'a':
		return Category::Aerodynamic;
	case 's':
		return Category::Shiny;
	}
	//Default...
	return Category::ExtremelyCoolLooking;
}

const char CategorySymbol[(int)Category::EndOfList] = { 'x','m','a','s' };

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

typedef struct Process* ProcessPtr;
typedef struct Workflow* WorkflowPtr;
typedef struct Instruction* InstructionPtr;
typedef struct Part* PartPtr;
typedef struct PartRange* PartRangePtr;

struct Part {
	int ratings[(int)Category::EndOfList];
	bool accepted;

	void Read(std::string & line );
	void Print(void)const;
};

struct PartRange {
	bool valid;
	int minRatings[(int)Category::EndOfList];
	int maxRatings[(int)Category::EndOfList];

	void Init(void);
	void Print(void)const;
};

enum class Operator : int {
	Lower,
	Greater,
	Connect
};

struct Instruction {
	Operator op;
	Category category;
	int value;
	std::string destination;

	void Read(char ** tmp);
	void Print(void)const;
	void NextWorkflow(Part& part, std::string& nextWorkflow) const;
	bool ProcessPart(Part& part, std::string & nextWorkflow ) const;
	void NextRangeWorkflow(PartRange& partRange, std::string& nextWorkflow) const;
	void ProcessPartRange(PartRange& partRange, PartRange &nextPartRange, std::string& nextWorkflow) const;
};

struct Workflow {
	std::string label;
	std::vector<Instruction> instructions;

	void Read(std::string& line);
	void Print(void)const;
	bool ProcessPart( Part &part, std::string& nextWorkflow) const;
	void ProcessPartRange(PartRange& partRange, const std::map<std::string, Workflow> & workflows, std::vector<PartRange> & acceptedPartRanges ) const;
};

struct Process {
	std::map<std::string,Workflow> workflows;
	std::vector<Part> parts;

	void Read(std::ifstream& input);
	void Print(void) const;

	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

void
Part::Read(std::string& line)
{
	char* tmp;
	Category category;
	int value;

	for (int i = 0; i < 4; i++)
		ratings[i] = 0;
	accepted = false;

	tmp = const_cast<char*>(line.c_str());
	tmp++;
	while (*tmp != '}')
	{
		category = GetCategory(*tmp);
		tmp++;
		tmp++;
		sscanf(tmp, "%d", &value);
		ratings[(int)category] = value;
		while (*tmp != ',' && *tmp != '}')
			tmp++;
		if (*tmp == ',')
			tmp++;
	}
}

void
Part::Print(void)const
{
	for (int i = 0; i < 4; i++)
		printf("%c = %d ", CategorySymbol[i], ratings[i]);
}

void
PartRange::Init(void)
{
	for (int i = 0; i < 4; i++)
	{
		minRatings[i] = 1;
		maxRatings[i] = 4000;
	}
	valid = true;
}

void
PartRange::Print(void)const
{
	for (int i = 0; i < 4; i++)
		printf("%c = [%d,%d] ", CategorySymbol[i], minRatings[i], maxRatings[i]);
}

const char* pDestination = ":";

void
Instruction::Read(char** input)
{
	char buffer[10];
	char* tmp;

	destination.clear();

	tmp = buffer;
	while (isalpha(**input))
	{
		*tmp = **input;
		tmp++;
		(*input)++;
	}
	*tmp = '\0';
	switch (**input)
	{
	case '>':
		category = GetCategory(buffer[0]);
		op = Operator::Greater;
		break;
	case '<':
		category = GetCategory(buffer[0]);
		op = Operator::Lower;
		break;
	default:
		op = Operator::Connect;
		destination = std::string(buffer);
		return;
	}

	(*input)++;
	sscanf(*input, "%d", &value);
	FindPattern(input, pDestination);
	tmp = buffer;
	while (isalpha(**input))
	{
		*tmp = **input;
		tmp++;
		(*input)++;
	}
	*tmp = '\0';
	destination = std::string(buffer);
	(*input)++;
}

void
Instruction::Print(void)const
{
	switch (op)
	{
	case Operator::Greater:
	case Operator::Lower:
		printf("%c %c %d => %s\n", CategorySymbol[(int)category], op == Operator::Greater ? '>' : '<', value, destination.c_str());
		break;
	case Operator::Connect:
		printf("%s\n", destination.c_str());
		break;
	}
}

void
Instruction::NextWorkflow(Part& part, std::string& nextWorkflow) const
{
	if (destination == "A")
	{
		part.accepted = true;
		nextWorkflow = "";
		return;
	}
	else if (destination == "R")
	{
		part.accepted = false;
		nextWorkflow = "";
		return;
	}
	nextWorkflow = destination;
}

bool
Instruction::ProcessPart(Part& part, std::string& nextWorkflow) const
{
	switch (op)
	{
	case Operator::Lower:
		if (part.ratings[(int)category] < value)
		{
			NextWorkflow(part, nextWorkflow);
			return true;
		}
		break;
	case Operator::Greater:
		if (part.ratings[(int)category] > value)
		{
			NextWorkflow(part, nextWorkflow);
			return true;
		}
		break;
	case Operator::Connect:
		NextWorkflow(part, nextWorkflow);
		return true;
	}

	return false;
}

void
Instruction::NextRangeWorkflow(PartRange& nextPartRange, std::string& nextWorkflow) const
{
	if (destination == "A")
	{
		nextPartRange.valid = true;
		nextWorkflow = "";
		return;
	}
	else if (destination == "R")
	{
		nextPartRange.valid = false;
		nextWorkflow = "";
		return;
	}

	nextPartRange.valid = true;
	nextWorkflow = destination;
}

void
Instruction::ProcessPartRange(PartRange& partRange, PartRange& nextPartRange, std::string& nextWorkflow) const
{
	nextPartRange = partRange;
	nextWorkflow = "";

	switch (op)
	{
	case Operator::Lower:
		nextPartRange.maxRatings[(int)category] = value - 1;
		if (nextPartRange.minRatings[(int)category] > nextPartRange.maxRatings[(int)category])
			nextPartRange.valid = false;
		else
			NextRangeWorkflow(nextPartRange, nextWorkflow);
		partRange.minRatings[(int)category] = value;
		if (partRange.minRatings[(int)category] > partRange.maxRatings[(int)category])
			partRange.valid = false;
		return;
	case Operator::Greater:
		nextPartRange.minRatings[(int)category] = value + 1;
		if (nextPartRange.minRatings[(int)category] > nextPartRange.maxRatings[(int)category])
			nextPartRange.valid = false;
		else
			NextRangeWorkflow(nextPartRange, nextWorkflow);
		partRange.maxRatings[(int)category] = value;
		if (partRange.minRatings[(int)category] > partRange.maxRatings[(int)category])
			partRange.valid = false;
		return;
	case Operator::Connect:
		if (destination == "A")
		{
			nextPartRange.valid = true;
			nextWorkflow = "";
			return;
		}
		else if (destination == "R")
		{
			nextPartRange.valid = false;
			nextWorkflow = "";
			return;
		}
		nextWorkflow = destination;
		return;
	}
}

void
Workflow::Read(std::string& line)
{
	char* tmp;
	Instruction instruction;

	label.clear();
	instructions.clear();

	tmp = const_cast<char*>(line.c_str());
	while (*tmp != '{')
	{
		label += *tmp;
		tmp++;
	}
	tmp++;
	while (*tmp != '}')
	{
		instruction.Read(&tmp);
		instructions.push_back(instruction);
	}
}

void
Workflow::Print(void)const
{
	int index = 0;
	printf("label: %s\n", label.c_str());
	for (const Instruction & instruction : instructions)
	{
		printf("\t[%d] ", ++index);
		instruction.Print();
	}
}

bool
Workflow::ProcessPart(Part& part, std::string& nextWorkflow) const
{
#if COMMENT == true
	printf(" => %s", label.c_str());
#endif
	for (const Instruction & instruction : instructions)
		if (instruction.ProcessPart(part, nextWorkflow))
		{
			if (nextWorkflow == "")
			{
#if COMMENT == true
				printf(" => %s\n", part.accepted ? "A" :"R");
#endif
				return true;
			}
			return false;
		}

#if COMMENT == true
	printf("Workflow error\n");
#endif

	return true;
}

void
Workflow::ProcessPartRange(PartRange& partRange, const std::map<std::string, Workflow> & workflows, std::vector<PartRange>& acceptedPartRanges) const
{
#if COMMENT == true
	printf("ProcessPartRange( %s, ", label.c_str()); partRange.Print(); printf(")\n");
#endif

	PartRange nextPartRange;
	std::string nextWorkflow;

	nextWorkflow = "";

	for (const Instruction & instruction : instructions)
	{
		instruction.ProcessPartRange(partRange, nextPartRange, nextWorkflow);
#if COMMENT == true
		printf("\tpartRange: "); partRange.Print(); printf("\n");
		printf("\tnextPartRange: "); nextPartRange.Print(); printf("\n");
		printf("\tnetxWorkflow: %s\n",nextWorkflow.c_str());
#endif
		if (nextWorkflow != "" && nextPartRange.valid )
			const_cast<std::map<std::string, Workflow>&>(workflows)[nextWorkflow].ProcessPartRange(nextPartRange, workflows, acceptedPartRanges);
		else
		{
			if (nextPartRange.valid)
			{
#if COMMENT == true
				printf("\t\t=> Accepted: "); nextPartRange.Print(); printf(")\n");
#endif
				acceptedPartRanges.push_back(nextPartRange);
			}
		}
		if (!partRange.valid)
			break;
	}
}

void
Process::Read(std::ifstream& input)
{
	std::string line;
	Workflow workflow;
	Part part;

	while (std::getline(input, line) && !line.empty())
	{
		workflow.Read(line);
		workflows[workflow.label] = workflow;
	}

	while (std::getline(input, line))
	{
		part.Read(line);
		parts.push_back(part);
	}
}

void
Process::Print(void) const
{
	int index = 0;
	printf("Workflows: %zd\n", workflows.size());
	for (const auto& n : workflows)
	{
		printf("[%d] ", ++index);
		n.second.Print();
	}
	index = 0;
	printf("Parts: %zd\n", parts.size());
	for (Part part:parts)
	{
		printf("[%d] ", ++index);
		part.Print();
		printf("\n");
	}
}

int64_t
Process::FindSolutionPart1(void)
{
	int64_t result;
	std::string workflow;
	int index;

	result = 0;

	index = 0;
	for (Part part : parts)
	{
		part.accepted = false;
		workflow = "in";
#if COMMENT == true
		printf("[%03d] ", ++index);
		part.Print();
#endif
		while( ! workflows[workflow].ProcessPart(part, workflow) ) {}
		if (part.accepted)
			for (int i = 0; i < 4; i++)
				result += part.ratings[i];
	}

	return result;
}

int64_t
Process::FindSolutionPart2(void)
{
	int64_t result;
	PartRange seed;
	std::vector<PartRange> acceptedPartRanges;
	int64_t combinations;
	int index;

	seed.Init();

	workflows["in"].ProcessPartRange(seed, workflows, acceptedPartRanges);

	result = 0;
	index = 0;
	for (PartRange & range : acceptedPartRanges)
	{
#if COMMENT == true
		printf("[%03d] ", ++index);	range.Print(); printf("\n");
#endif
		combinations = 1;
		for (int i = 0; i < 4; i++)
			combinations *= range.maxRatings[i] - range.minRatings[i]+1;
		result += combinations;
	}

	return result;
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Process process;

	printf("Advent of Code - Day 19\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	process.Read(input);

	clockStart = clock();

	result = process.FindSolutionPart1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = process.FindSolutionPart2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}