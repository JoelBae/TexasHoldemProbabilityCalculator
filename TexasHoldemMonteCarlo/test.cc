#include "MonteCarlo.h"
#include <chrono>

int main()
{
    char final;
    do {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::vector<TexasHoldemMonteCarlo::Card> holeCards;
        int i = 0;
        while (i < 2) {
            char suit;
            int suitVal;
            int rank;
            std::cout << "Choose Card:" << std::endl;
            std::cout << "  Pick Suit: (H)earts, (D)iamonds, (C)lubs, (S)pades ";
            std::cin >> suit;
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
            else {
                std::cout << "Invalid Input" << std::endl;
                continue;
            }
            std::cout << std::endl;
            std::cout << "  Pick Rank: 1 - 13 (1 = ACE, 13 = KING) ";
            std::cin >> rank;
            if (1 > rank || rank > 14) {
                std::cout << "Invalid Input" << std::endl;
                continue;
            }
            std::cout << std::endl;
            if (holeCards.size() == 0 || (holeCards.size() > 0 && (holeCards[0].suit != suitVal || holeCards[0].rank != rank))) {
                holeCards.emplace_back(suitVal, rank);
                ++i;
            }
       
        }
        int numOpps;
        std::cout << "Pick Number of Opponents: ";
        std::cin >> numOpps ;
        std::cout << std::endl;
        std::cout << "COMPUTING PROBABILITY..." << std::endl;
        auto start = std::chrono::steady_clock::now();
        double result = TexasHoldemMonteCarlo::computeMonteCarlo(holeCards, numOpps, 100000);
        std::cout << "The result is: " << result << "%" << std::endl;
        auto stop = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
        std::cout << "In " << duration.count() << " Seconds" << std::endl;
        std::cout << "Press R to restart. Press X to Exit ";
        std::cin >> final;
        std::cout << std::endl;
    } while (final == 'R');

}
