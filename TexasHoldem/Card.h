#pragma once
class Card
{
public:
	int suit;
	int rank;
	bool operator == (const Card& other) const;
	Card(int suit, int rank);
};


