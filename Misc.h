#ifndef MISC_H
#define MISC_H
#include "BitBoards.h"
#include "Types.h"
#include <string>

namespace Misc{
    // Function that was used to generate magics for rooks and bishops.
    // Heavily inspired by the routine found on https://www.chessprogramming.org/Looking_for_Magics
    bitBoard generateMagicNumber(int square, int rook);
    unsigned int XORShift32Rand();
    // generates 64 bit number with low number of set bits (ideal candidate)
    bitBoard getMagicNumberCandidate();
    bitBoard genRand64();
    std::string indexToString(int idx);
    std::string moveToString(Move move);
}

#endif