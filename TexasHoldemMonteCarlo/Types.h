#pragma once
#include <vector>

namespace TexasHoldemMonteCarlo {
    struct Card {
        int suit;
        int rank;
        bool operator == (const Card& other) const;
        Card(int suit, int rank);
    };
}

struct BestScoreHand {
    std::vector<TexasHoldemMonteCarlo::Card> hand;
    int score;
    BestScoreHand(std::vector<TexasHoldemMonteCarlo::Card> hand, int score);
};

struct BitFields {
    unsigned int ranks : 15;
    uint64_t counts : 60;
};
