#include "Misc.h"

std::string Misc::indexToString(int idx) {
    static std::string rankMap[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    static std::string fileMap[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    int rank = idx / 8;
    int file = idx % 8;
    std::string ret = fileMap[file];
    return ret.append(rankMap[rank]);
}

std::string Misc::moveToString(Move move) {
    std::string from = indexToString(getFrom(move));
    std::string to = indexToString(getTo(move));
    from.append(to);
    return from;
}


unsigned int Misc::XORShift32Rand() {
    static unsigned int state = 1804289383; // seed
    // XORShift32 algorithm
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}
bitBoard Misc::genRand64() {
    bitBoard n1, n2, n3, n4;
    n1 = XORShift32Rand() & 0xFFFF;
    n2 = XORShift32Rand() & 0xFFFF;
    n3 = XORShift32Rand() & 0xFFFF;
    n4 = XORShift32Rand() & 0xFFFF;
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}
bitBoard Misc::getMagicNumberCandidate() {
    return genRand64() & genRand64() & genRand64();
}

bitBoard Misc::generateMagicNumber(int square, int rook) {
    bitBoard attackMask, attacks[4096], occupancies[4096], used[4096], magic;
    int bits, magicIndex, fail;
    // init attackmask
    attackMask = rook? BitBoards::genOccMask(square, 1) : BitBoards::genOccMask(square, 0);
    // count relevant bits
    bits = countBits(attackMask);
    // Calculate all occupancies and their corresponding attacks
    for (int i = 0; i < (1 << bits); i++) {
        occupancies[i] = BitBoards::setOccupancy(i, bits, attackMask);
        attacks[i] = rook? BitBoards::computeRookAttack(square, occupancies[i]) : BitBoards::computeBishopAttack(square, occupancies[i]);
    }
    for (int k = 0; k < 100000000; k++) {
        magic = getMagicNumberCandidate();
        // Discard irrelavant magic numbers
        if (countBits((attackMask * magic) & 0xFF00000000000000ULL) < 6) continue;
        for (int i = 0; i < 4096; i++) {
            used[i] = 0ULL;
        }
        fail = 0;
        for (int i = 0; !fail && i < (1 << bits); i++) {
            // Calculate Magic Index
            magicIndex = (occupancies[i] * magic) >> (64 - bits);
            // Mark the used indecies with the calculated attack
            if (used[magicIndex] == 0ULL) {
                used[magicIndex] = attacks[i];
            // if Collision is not appropriate discard magic number
            } else if (used[magicIndex] != attacks[i]) {
                fail = 1;
            }
        }
        if (!fail) {
            return magic;
        }
    }
    // No magic number found over __ amount of tries
    std::cout << "Error, No magic number found" <<std::endl;
    return -1;
}