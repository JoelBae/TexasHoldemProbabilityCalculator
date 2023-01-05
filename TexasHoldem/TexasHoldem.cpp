// TexasHoldem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "BestScoreHand.h"
#include "Card.h"
#include <algorithm>
#include <random>
#include <string>
#include <unordered_map>
#include <chrono>
#include <bitset>
#include <memory>
#include <fstream>



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

struct BitFields {
    unsigned int ranks : 15;
    unsigned long long counts : 60;
};



unsigned long long mod15(unsigned long long num) {
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
        unsigned long long countBits = 0b000000000000000000000000000000000000000000000000000100000000;
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
    unsigned long long value = mod15(bf.counts);

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

int computeMonteCarlo(std::vector<Card> &holeCards, int numOpps, int trials, std::mt19937 &rng) {
    int wins = 0;
    for (int i = 0; i < trials; ++i) {
        std::vector<Card> cards = genCards(holeCards, numOpps, rng);
        if (simulate(cards, holeCards, numOpps)) {
            ++wins;
        }
    }
    return wins;
}

int main()
{
    char final;
    do {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::vector<Card> holeCards;
        for (int i = 0; i < 2; ++i) {
            char suit;
            int suitVal;
            int rank;
            std::cout << "Choose Card:" << std::endl;
            std::cout << "  Pick Suit: (H)earts, (D)iamonds, (C)lubs, (S)pades ";
            std::cin >> suit;
            std::cout << std::endl;
            std::cout << "Choose Card:" << std::endl;
            std::cout << "  Pick Rank: 1 - 13 (1 = ACE, 13 = KING) ";
            std::cin >> rank;
            std::cout << std::endl;
            if (suit == 'H') {
                suitVal = 0;
            }
            else if (suit == 'D') {
                suitVal = 1;
            }
            else if (suit == 'C') {
                suitVal = 2;
            }
            else if (suit == 'S') {
                suitVal = 3;
            }
            holeCards.emplace_back(suitVal, rank);
        }
        int numOpps;
        std::cout << "Pick Number of Opponents: ";
        std::cin >> numOpps ;
        std::cout << std::endl;
        std::cout << "COMPUTING PROBABILITY..." << std::endl;
        auto start = std::chrono::steady_clock::now();
        float result = computeMonteCarlo(holeCards, numOpps, 100000, rng) / 100000.0;
        std::cout << "The result is: " << result << "%" << std::endl;
        auto stop = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
        std::cout << "In " << duration.count() << " Seconds" << std::endl;
        std::cout << "Press R to restart. Press X to Exit ";
        std::cin >> final;
        std::cout << std::endl;
    } while (final == 'R');

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
