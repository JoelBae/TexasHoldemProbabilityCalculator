#pragma once
#include "Card.h"
#include <vector>

class BestScoreHand
{
public:
	std::vector<Card> hand;
	int score;
	BestScoreHand(std::vector<Card> hand, int score);
};

