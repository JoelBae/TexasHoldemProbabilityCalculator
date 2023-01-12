#include "MonteCarlo.h"
#include <unordered_map>
#include <stdint.h>

using namespace TexasHoldemMonteCarlo;

std::vector<Card> genCards(std::vector<Card> &holeCards, int numOpps, std::mt19937 &rng) {
    // generate pool of valid cards
    std::vector<Card> deck;
    for (int i = 1; i < 14; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (((i != holeCards[0].rank) || (j != holeCards[0].suit)) && ((i != holeCards[1].rank) || (j != holeCards[1].suit))) {
                deck.emplace_back(j, i);
            }
        }
    }
  
    // generate cards
    std::vector<Card> cards;
    int numCards = 5 + 2 * numOpps;
    std::shuffle(deck.begin(), deck.end(), rng);
    for (int i = 0; i < numCards; ++i) {
        cards.push_back(deck[i]);
    }
   
    return cards;
}

std::vector<std::vector<int>> comb(int N, int K)
{
    std::string bitmask(K, 1);
    bitmask.resize(N, 0);
    std::vector<std::vector<int>> combos;
    do {
        std::vector<int> combo;
        for (int i = 0; i < N; ++i)
        {
            if (bitmask[i]) combo.push_back(i);
        }
        combos.push_back(combo);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    return combos;
}

uint64_t mod15(uint64_t num) {
    num = (num >> 32) + (num & 0xFFFFFFFF);
    num = (num >> 16) + (num & 0xFFFF);
    num = (num >> 8) + (num & 0xFF);
    num = (num >> 4) + (num & 0xF);
    if (num < 15) {
        return num;
    }
    else if (num < 30) {
        return num - 15;
    }
    return num - 30;
}

BitFields cardToBinary(int rank, int count) {
    BitFields bf{};
    if (rank == 1) {
        bf.ranks = 0b100000000000000;
        bf.counts = 0b000100000000000000000000000000000000000000000000000000000000 << count;
    }
    else {
        bf.ranks = 0b000000000000100 << (rank - 2);
        uint64_t countBits = 0b000000000000000000000000000000000000000000000000000100000000;
        bf.counts = (countBits << (rank - 2) * 4) << count;
    }
    return bf;
}

int generateScore(std::vector<Card>& hand) {
    std::unordered_map<int, int> counts;
    BitFields bf = {};

    // translate hand to bit fields
    for (Card& card : hand) {
        BitFields singleCard = cardToBinary(card.rank, counts[card.rank]);
        ++counts[card.rank];
        bf.ranks |= singleCard.ranks;
        bf.counts |= singleCard.counts;
    }

    // determine if 4 card, full house, 3 card, 2 pair, pair, or high card
    uint64_t value = mod15(bf.counts);

    // determine if straight
    bool straight = false;
    bool lowStraight = false;
    unsigned int temp = bf.ranks & (~bf.ranks + 1);
    if (bf.ranks == 0b100000000111100) {
        lowStraight = true;
    }
    if (bf.ranks / temp == 0b11111) {
        straight = true;
    }

    // determine if flush
    bool flush = true;
    int suit = hand[0].suit;
    for (Card& card : hand) {
        if (card.suit != suit) {
            flush = false;
            break;
        }
    }

    // royal flush
    if (flush && bf.ranks == 0b111110000000000) {
        return 1;
    }
    // straight flush
    else if (straight && flush && !lowStraight) {
        return 2;
    }

    // straight flush
    else if (lowStraight && flush) {
        return 3;
    }
    // 4 card
    else if (value == 1) {
        return 4;
    }
    // full house
    else if (value == 10) {
        return 5;
    }
    // flush
    else if (flush) {
        return 6;
    }
    // straight
    else if (straight && !lowStraight) {
        return 7;
    }
    else if (lowStraight) {
        return 8;
    }
    // 3 card
    else if (value == 9) {
        return 9;
    }
    // two pair
    else if (value == 7) {
        return 10;
    }
    // pair
    else if (value == 6) {
        return 11;
    }
    // high card
    else {
        return 12;
    }
}

std::vector<Card>& tieBreaker(std::vector<Card>& hand1, std::vector<Card>& hand2) {
    std::vector<int> ranks1;
    std::vector<int> ranks2;
    std::unordered_map<int, int> counts1;
    std::unordered_map<int, int> counts2;

    for (int i = 0; i < 5; ++i) {
        ++counts1[hand1[i].rank];
        ++counts2[hand2[i].rank];
    }


    for (auto const& c : counts1) {
        for (int i = 0; i < c.second; ++i) {
            ranks1.push_back(c.first);
        }
    }

    for (auto const& c : counts2) {
        for (int i = 0; i < c.second; ++i) {
            ranks2.push_back(c.first);
        }
    }

    std::sort(ranks1.begin(), ranks1.end());
    std::sort(ranks2.begin(), ranks2.end());

    int score1 = 0;
    int score2 = 0;
    for (int i = 0; i < 5; ++i) {
        score1 |= ((ranks1[i] == 1 ? 14 : ranks1[i]) << (16 - i * 4));
        score2 |= ((ranks2[i] == 1 ? 14 : ranks2[i]) << (16 - i * 4));
    }

    if (score1 > score2) {
        return hand1;
    }
    else {
        return hand2;
    }
}

BestScoreHand getBest(std::vector<Card>& firstHand, std::vector<Card>& secondHand) {
    firstHand.insert(firstHand.end(), secondHand.begin(), secondHand.end());
    std::vector<std::vector<int>> combos = comb(7, 5);
    std::vector<Card> bestHand;
    int bestScore = 13; // 13 is arbitrary, upper limit to score is 12
    for (std::vector<int> combo : combos) {
        std::vector<Card> hand;
        for (int i : combo) {
            hand.push_back(firstHand[i]);
        }
        int score = generateScore(hand);
        if (score < bestScore) {
            bestScore = score;
            bestHand = hand;
        }
        else if (score == bestScore) {
            bestHand = tieBreaker(bestHand, hand);
        }
    }
    BestScoreHand best(bestHand, bestScore);
    return best;
}

bool simulate(std::vector<Card> &cards, std::vector<Card> holeCards, int numOpps) {

    std::vector<std::vector<Card>> opps;
    std::vector<Card> communityCards;

    // add cards to opponent hands
    for (int i = 0; i < numOpps; ++i) {
        Card first = cards.back();
        cards.pop_back();
        Card second = cards.back();
        cards.pop_back();
        std::vector<Card> hand = { first, second };
        opps.push_back(hand);
    }

    // add community cards
    for (int i = 0; i < 5; ++i) {
        communityCards.push_back(cards.back());
        cards.pop_back();
    }

    BestScoreHand userBest = getBest(holeCards, communityCards);

    for (std::vector<Card> opp : opps) {
        BestScoreHand oppBest = getBest(opp, communityCards);
        if (userBest.score > oppBest.score) {
            return false;
        }
        else if (userBest.score == oppBest.score) {
            if (tieBreaker(userBest.hand, oppBest.hand) == oppBest.hand) {
                return false;
            }
        }
    }
    return true;
}

double TexasHoldemMonteCarlo::computeMonteCarlo(std::vector<Card> holeCards, int numOpps, int trials, std::mt19937 *rng) {
    if (rng == nullptr) {
        std::random_device rd{};
        std::mt19937 mersenneTwister{rd()};
        rng = &mersenneTwister;
    }
    int wins = 0;
    for (int i = 0; i < trials; ++i) {
        std::vector<Card> cards = genCards(holeCards, numOpps, *rng);
        if (simulate(cards, holeCards, numOpps)) {
            ++wins;
        }
    }
    return wins / (trials * 1.0) * 100;
}