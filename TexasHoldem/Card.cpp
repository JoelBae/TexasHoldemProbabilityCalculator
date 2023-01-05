#include "Card.h"
Card::Card(int suit, int rank) : suit(suit), rank(rank) {}
bool Card::operator==(const Card& other) const {
	return (suit == other.suit && rank == other.rank);
}