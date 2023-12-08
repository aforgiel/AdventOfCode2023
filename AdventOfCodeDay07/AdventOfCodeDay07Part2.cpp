// AdventOfCodeDay07.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay07/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2023/AdventOfCodeDay07/input.txt";
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

inline bool IsSymbol(char c)
{
	return !IsDigit(c) && c != '.';
}

const char* pSpace = " ";

typedef struct Game* GamePtr;
typedef struct Hand* HandPtr;

struct Game {
	std::vector<Hand> hands;

	void Read(std::ifstream& input);
	void Print(void);
	void SortHands(void);
	int64_t FindSolution(void);
};

#define kHandSize 5
enum class Type : int {
	Void = 0,

	HighCard,
	OnePair,
	TwoPair,
	ThreeOfAKind,
	FullHouse,
	FourOfAKind,
	FiveOfAKind
};

const char* kTypeName[] = {
	"Void",

	"High Card",
	"One Pair",
	"Two Pair",
	"Three Of A Kind",
	"Full House",
	"Four Of A Kind",
	"Five Of A Kind"
};

struct Hand {
	int id;
	char cards[kHandSize + 1];
	int bid;
	Type type;

	void Read(std::string& line);
	void Print(void);
	Type ComputeType(void);

	bool operator <(const Hand& hand) const;
	bool operator <=(const Hand& hand) const;
	bool operator >(const Hand& hand) const;
	bool operator >=(const Hand& hand) const;
};

inline int CardRank(char card)
{
	switch (card)
	{
	case '2':
		return 1;
	case '3':
		return 2;
	case '4':
		return 3;
	case '5':
		return 4;
	case '6':
		return 5;
	case'7':
		return 6;
	case '8':
		return 7;
	case '9':
		return 8;
	case 'T':
		return 9;
	case 'J':
		return 0; // Joker are the weakest
	case 'Q':
		return 11;
	case 'K':
		return 12;
	case 'A':
		return 13;
	default:
		break;
	}
	return -1;
}

int CompareCards(char a, char b)
{
	if (CardRank(a) == CardRank(b))
		return 0;
	if (CardRank(a) > CardRank(b))
		return 1;

	return -1;
}

void
Game::Read(std::ifstream& input)
{
	std::string line;
	Hand hand;
	int index;

	index = 0;
	while (std::getline(input, line))
	{
		hand.Read(line);
		hand.id = ++index;
		hand.type = hand.ComputeType();
		hands.push_back(hand);
	}
}

void
Game::Print(void)
{
	for (Hand hand : hands)
		hand.Print();
}

void
Game::SortHands(void)
{
	std::sort(hands.begin(), hands.end(), [](Hand& a, Hand& b)
		{
			return a > b;
		});
}

int64_t
Game::FindSolution(void)
{
	int64_t result;
	int rank;

	result = 0;
	rank = (int)hands.size();
	for (Hand hand : hands)
	{
		result += hand.bid * rank;
		rank--;
	}

	return result;
}

void
Hand::Read(std::string& line)
{
	char* tmp;

	tmp = const_cast<char*>(line.c_str());
	for (int i = 0; i < kHandSize; i++)
	{
		cards[i] = *tmp;
		tmp++;
	}
	cards[kHandSize] = '\0';
	SkipSpace(&tmp);
	sscanf_s(tmp, "%d", &bid);
}

void
Hand::Print(void)
{
	printf("[%04d] \'%s\' bid: %03d, type: %s\n", id, cards, bid, kTypeName[(int)type]);
}

Type
Hand::ComputeType(void)
{
	std::map<char, int> tree;
	std::map<char, int>::iterator search;
	int joker;

	joker = 0;
	// Count the number of cards of each type:
	for (int i = 0; i < kHandSize; i++)
	{
		if (cards[i] == 'J')
			joker++;
		else
		{
			search = tree.find(cards[i]);
			if (search == tree.end())
				tree[cards[i]] = 1;
			else
				tree[cards[i]] = tree[cards[i]] + 1;
		}
	}

	if (joker == kHandSize)
		return Type::FiveOfAKind;

	int first, second;

	first = second = 0;
	for (const auto& n : tree)
	{
		if (n.second > first)
		{
			second = first;
			first = n.second;
		}
		else if (n.second > second)
			second = n.second;
	}

	first += joker;

	switch (first)
	{
	case 5:
		return Type::FiveOfAKind;
	case 4:
		return Type::FourOfAKind;
	case 3:
		if (second == 2)
			return Type::FullHouse;
		else
			return Type::ThreeOfAKind;
	case 2:
		if (second == 2)
			return Type::TwoPair;
		else
			return Type::OnePair;
	case 1:
		return Type::HighCard;
	default:
		break;
	}

	return Type::Void;
}

bool
Hand::operator <(const Hand& hand) const
{
	if (type < hand.type)
		return true;
	else if (type > hand.type)
		return false;

	// equal type case:
	int i;
	int compare;
	i = 0;
	while (i < kHandSize)
	{
		compare = CompareCards(cards[i], hand.cards[i]);
		if (compare < 0)
			return true;
		else if (compare > 0)
			return false;
		i++;
	}

	return id < hand.id;
}

bool
Hand::operator <=(const Hand& hand) const
{
	return !(hand < *this);
}

bool
Hand::operator >(const Hand& hand) const
{
	return (hand < *this);
}

bool
Hand::operator >=(const Hand& hand) const
{
	return !(*this < hand);
}

int main()
{
	std::ifstream input;
	clock_t clockStart, clockEnd;
	double time_taken;
	int64_t result;
	Game Game;

	printf("Advent of Code - Day 07\n");

	input.open(fileName);
	if (!input.is_open())
		return 0;
	printf("Opening file %s\n", fileName);

	result = 0;
	clockStart = clock();

	Game.Read(input);
	Game.SortHands();
	Game.Print();
	result = Game.FindSolution();

	printf("result: %I64d\n", result);

	clockEnd = clock();
	time_taken
		= double(clockEnd - clockStart)
		/ double(CLOCKS_PER_SEC);
	printf("Elapsed time: %f seconds\n", time_taken);

	input.close();

	return 0;
}