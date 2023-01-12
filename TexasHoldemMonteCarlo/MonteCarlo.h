#include <iostream>
#include <random>
#include "Types.h"

namespace TexasHoldemMonteCarlo {
    double computeMonteCarlo(std::vector<TexasHoldemMonteCarlo::Card> holeCards, int numOpps, int trials, std::mt19937 *rng);
}