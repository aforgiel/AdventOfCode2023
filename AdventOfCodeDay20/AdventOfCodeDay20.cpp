// AdventOfCodeDay20.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay20/sample2.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay20/input.txt";
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

enum class Pulse : int {
	Low,
	High,
	EndOfList
};

const char* PulseName[(int)Pulse::EndOfList] = { "Low", "High" };

typedef struct Network* NetworkPtr;
typedef struct Module* ModulePtr;
typedef struct Message* MessagePtr;
struct Message {
	ModulePtr origin;
	Pulse pulse;
	ModulePtr destination;
	void Print(void) const;
};
typedef std::queue<Message> MessageQueue;

struct Module {
	std::string label;
	std::vector<ModulePtr> ins;
	std::vector<std::string> outNames;
	std::vector<ModulePtr> outs;
	int64_t cycle;
	void Connect(Network& network);
	virtual void Reset(void) {}
	virtual void Process(Message& message, MessageQueue& queue) = 0;
	virtual void SendMessage(Message& message, MessageQueue& queue)
	{
		for (ModulePtr out : outs)
		{
			message.destination = out;
			queue.push(message);
		}
	}
	void Read(const char* buffer);
	virtual void Print(void) const
	{
		printf("%s", label.c_str());
		printf(" in: ");
		for (int i = 0; i < ins.size(); i++)
			printf("%s%s", i > 0 ? ", " : "", ins[i]->label.c_str());
		printf(" | out: ");
		for (int i = 0; i < outs.size(); i++)
			printf("%s%s", i > 0 ? ", " : "", outs[i]->label.c_str());
		printf("\n");
	}
	virtual int64_t GetCycle(Network& network) { return 1; }
	virtual Pulse GetPulse(void) const = 0;
};

void
Message::Print(void) const
{
	printf("%s -%s-> %s\n", origin == nullptr ? "button" : origin->label.c_str(), pulse == Pulse::Low ? "low" : "high", destination->label.c_str());
}

struct Broadcaster : Module {
	virtual void Process(Message& message, MessageQueue& queue)
	{
		Message output;
		output.origin = this;
		output.pulse = message.pulse;
		SendMessage(output, queue);
	}
	virtual Pulse GetPulse(void) const { return Pulse::Low; };
};

struct TestModule : Module {
	virtual void Process(Message& message, MessageQueue& queue) {}
	virtual void Print(void) const { printf("TEST "); Module::Print(); }
	virtual int64_t GetCycle(Network& network) { return ins[0]->GetCycle(network); }
	virtual Pulse GetPulse(void) const { return ins[0]->GetPulse(); }
};

struct FlipFlop : Module {
	bool status = false;
	virtual void Reset(void) { status = false; }
	virtual void Process(Message& message, MessageQueue& queue)
	{
		Message output;
		if (message.pulse == Pulse::High)
			return;
		status = !status;
		output.origin = this;
		output.pulse = GetPulse();
		SendMessage(output, queue);
	}
	virtual void Print(void) const { printf("%%"); Module::Print(); }
	virtual int64_t GetCycle(Network& network);
	virtual Pulse GetPulse(void) const { return status ? Pulse::High : Pulse::Low; }
};

struct Conjunction : Module {
	std::map<ModulePtr, Pulse> memory;
	Pulse GetMemory(ModulePtr module) const
	{
		auto search = memory.find(module);
		if (search == memory.end())
			return Pulse::Low;
		return search->second;
	}
	void SetMemory(ModulePtr module, Pulse pulse) { memory[module] = pulse; }
	virtual void Reset(void)
	{
		for (ModulePtr in : ins)
			SetMemory(in, Pulse::Low);
	}
	virtual void Process(Message& message, MessageQueue& queue)
	{
		Message output;
		SetMemory(message.origin, message.pulse);
		output.origin = this;
		output.pulse = GetPulse();
		SendMessage(output, queue);
	}
	virtual void Print(void) const { printf("&"); Module::Print(); }
	virtual int64_t GetCycle(Network& network);
	virtual Pulse GetPulse(void) const
	{
		bool allHigh = true;
		for (ModulePtr in : ins)
			if (GetMemory(in) == Pulse::Low)
				return Pulse::High;
		return Pulse::Low;
	}
};

struct Network {
	std::map<std::string, ModulePtr> modules;

	void Read(std::ifstream& input);
	void Print(void) const;

	ModulePtr GetModule(std::string& label);
	void Reset(void);
	int64_t GetCycle(std::string label, Pulse target);

	int64_t FindSolutionPart1(void);
	int64_t FindSolutionPart2(void);
};

void
Module::Read(const char* buffer)
{
	char* tmp;
	std::string name;

	tmp = const_cast<char*>(buffer);
	label.clear();
	while (isalpha(*tmp))
	{
		label += *tmp;
		tmp++;
	}
	FindPattern(&tmp, " -> ");
	while (*tmp != '\0')
	{
		name.clear();
		while (isalpha(*tmp))
		{
			name += *tmp;
			tmp++;
		}
		outNames.push_back(name);
		FindPattern(&tmp, ", ");
	}
}

void
Module::Connect(Network& network)
{
	ModulePtr module;

	for (std::string name : outNames)
	{
		module = network.GetModule(name);
		if (module)
			outs.push_back(module);
	}

	for (ModulePtr module : outs)
		module->ins.push_back(this);
}

void
Network::Read(std::ifstream& input)
{
	std::string line;
	ModulePtr module;
	char* buffer;

	while (std::getline(input, line))
	{
		switch (line[0])
		{
		case '%':
			module = new FlipFlop();
			buffer = const_cast<char*>(line.c_str()) + 1;
			break;
		case '&':
			module = new Conjunction();
			buffer = const_cast<char*>(line.c_str()) + 1;
			break;
		default:
			module = new Broadcaster();
			buffer = const_cast<char*>(line.c_str());
			break;
		}
		module->Read(buffer);
		modules[module->label] = module;
	}

	for (auto& module : modules)
		module.second->Connect(*this);
}

void
Network::Print(void) const
{
	int index = 0;
	for (const auto& module : modules)
	{
		printf("[%03d] ", ++index);
		module.second->Print();
	}
}

ModulePtr
Network::GetModule(std::string& label)
{
	auto search = modules.find(label);
	if (search != modules.end())
		return search->second;

	ModulePtr module = new TestModule();
	module->label = label;
	modules[label] = module;

	return module;
}

void
Network::Reset(void)
{
	for (auto& module : modules)
		module.second->Reset();
}

int64_t
Network::FindSolutionPart1(void)
{
	int64_t result = 0;
	MessageQueue queue;
	Message first;
	Message message;
	int64_t lowCount, highCount;
	int index;
	int iteration;

	iteration = 0;
	lowCount = highCount = 0;

	do {
		iteration++;
#if COMMENT == true
		printf("[%03d] Low: %zd, High: %zd\n", iteration, lowCount, highCount);
#endif
		first.origin = nullptr;
		first.pulse = Pulse::Low;
		first.destination = modules["broadcaster"];
		queue.push(first);
		index = 0;
		while (!queue.empty())
		{
			message = queue.front();
			queue.pop();
#if COMMENT == true
			printf("\t[%03d] ", ++index); message.Print();
#endif
			if (message.pulse == Pulse::Low)
				lowCount++;
			else
				highCount++;
			message.destination->Process(message, queue);
		}
	} while (iteration != 1000);

	printf("=> Low: %zd, High: %zd\n", lowCount, highCount);

	result = lowCount * highCount;

	return result;
}

int64_t
FlipFlop::GetCycle(Network& network)
{
	if (cycle != 0)
		return cycle;

	cycle = network.GetCycle(label, Pulse::High);

	return cycle;
}

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

int64_t
Conjunction::GetCycle(Network& network)
{
	if (cycle != 0)
		return cycle;

	if (dynamic_cast<FlipFlop*>(ins[0]) != nullptr)
	{
		cycle = 0;
		for (ModulePtr in : ins)
			cycle += in->GetCycle(network);
	}
	else if (dynamic_cast<Conjunction*>(ins[0]) != nullptr)
	{
		std::vector<int64_t> cycles;
		for (ModulePtr in : ins)
			cycles.push_back(in->GetCycle(network));
		cycle = LCM(cycles);
	}

	return cycle;
}

int64_t
Network::GetCycle(std::string label, Pulse target)
{
	MessageQueue queue;
	Message first;
	Message message;
	int64_t result = 0;

	Reset();

	first.origin = nullptr;
	first.pulse = Pulse::Low;
	first.destination = modules["broadcaster"];

	while (true)
	{
		result++;
		queue.push(first);
		while (!queue.empty())
		{
			message = queue.front();
			queue.pop();
			message.destination->Process(message, queue);
			if (message.destination->label == label && message.destination->GetPulse() == target)
				return result;
		}
	}

	return 0;
}

int64_t
Network::FindSolutionPart2(void)
{
	for (auto& it : modules)
		it.second->cycle = 0;

	return modules["rx"]->GetCycle(*this);
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Network network;

	printf("Advent of Code - Day 20\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	network.Read(input);
	network.Print();

	clockStart = clock();

	result = network.FindSolutionPart1();

	printf("result part 1: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	clockStart = clock();

	result = network.FindSolutionPart2();

	printf("result part 2: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}