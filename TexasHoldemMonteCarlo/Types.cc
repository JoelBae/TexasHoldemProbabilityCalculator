#include "Types.h"

BestScoreHand::BestScoreHand(std::vector<TexasHoldemMonteCarlo::Card> hand, int score) : hand{hand}, score{score} {}

TexasHoldemMonteCarlo::Card::Card(int suit, int rank) : suit(suit), rank(rank) {}

bool TexasHoldemMonteCarlo::Card::operator==(const Card& other) const {
	return (suit == other.suit && rank == other.rank);
}