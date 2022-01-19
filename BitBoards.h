#ifndef BITBOARDS_H
#define BITBOARDS_H
#include <cstdint>
#include "Types.h"
#include <iostream>

// Contains definitions and operations to do with bitboards including the calculation of bitboard attack sets


typedef uint64_t bitBoard;

enum Direction {
    NORTH = 8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,
    NORTHEAST = NORTH + EAST,
    SOUTHEAST = SOUTH + EAST,
    NORTHWEST = NORTH + WEST,
    SOUTHWEST = SOUTH + WEST
};

// MagicSquare struct
struct MagicSquare {
    bitBoard* attacks;
    bitBoard occMask;
    bitBoard magic;
    int shift;
};

namespace BitBoards {
    // precomputes attack sets and stores them for fast lookup during runtime
    void precomputeAttackSets();
    // sets the bits in a given attack mask corresponding to the index provided
    bitBoard setOccupancy(int index, int bitsInMask, bitBoard attackMask);
    // genarates an attack occupancy mask for the given square for either a rook or bishop
    bitBoard genOccMask(int square, int rook);
    void initRookMagics();
    void initBishopMagics();
    void initBetweenBB();
    bitBoard computeBetweenBB(int sq1, int sq2, Direction dir);
    // computes rook attack from a given square and occupancy
    bitBoard computeRookAttack(int square, bitBoard occ);
    // computes Bishop attack from a given square and occupancy
    bitBoard computeBishopAttack(int square, bitBoard occ);
    // computes a knight attack from a given square
    bitBoard computeKnightAttack(int square);
    // computes a king attack from a given square
    bitBoard computeKingAttack(int square);
    // computes a pawn attack for a given square and colour
    bitBoard computePawnAttack(int square, int white);
}

// from https://github.com/official-stockfish/Stockfish/blob/master/src/bitboard.h
constexpr bitBoard FileABB = 0x0101010101010101ULL;
constexpr bitBoard FileBBB = FileABB << 1;
constexpr bitBoard FileCBB = FileABB << 2;
constexpr bitBoard FileDBB = FileABB << 3;
constexpr bitBoard FileEBB = FileABB << 4;
constexpr bitBoard FileFBB = FileABB << 5;
constexpr bitBoard FileGBB = FileABB << 6;
constexpr bitBoard FileHBB = FileABB << 7;

constexpr bitBoard Rank1BB = 0xFF;
constexpr bitBoard Rank2BB = Rank1BB << (8 * 1);
constexpr bitBoard Rank3BB = Rank1BB << (8 * 2);
constexpr bitBoard Rank4BB = Rank1BB << (8 * 3);
constexpr bitBoard Rank5BB = Rank1BB << (8 * 4);
constexpr bitBoard Rank6BB = Rank1BB << (8 * 5);
constexpr bitBoard Rank7BB = Rank1BB << (8 * 6);
constexpr bitBoard Rank8BB = Rank1BB << (8 * 7);

// Pre-generated magic numbers for rook attacks
static constexpr bitBoard rookMagicNums[64] = {
    11565248328107303040ULL, 12123725398701785089ULL, 900733188335206529ULL, 72066458867205152ULL,
    144117387368072224ULL, 216203568472981512ULL, 9547631759814820096ULL, 2341881152152807680ULL,
    140740040605696ULL, 2316046545841029184ULL, 72198468973629440ULL, 81205565149155328ULL,
    146508277415412736ULL, 703833479054336ULL, 2450098939073003648ULL, 576742228899270912ULL,
    36033470048378880ULL, 72198881818984448ULL, 1301692025185255936ULL, 90217678106527746ULL,
    324684134750365696ULL, 9265030608319430912ULL, 4616194016369772546ULL, 2199165886724ULL,
    72127964931719168ULL, 2323857549994496000ULL, 9323886521876609ULL, 9024793588793472ULL,
    562992905192464ULL, 2201179128832ULL, 36038160048718082ULL, 36029097666947201ULL,
    4629700967774814240ULL, 306244980821723137ULL, 1161084564161792ULL, 110340390163316992ULL,
    5770254227613696ULL, 2341876206435041792ULL, 82199497949581313ULL, 144120019947619460ULL,
    324329544062894112ULL, 1152994210081882112ULL, 13545987550281792ULL, 17592739758089ULL,
    2306414759556218884ULL, 144678687852232706ULL, 9009398345171200ULL, 2326183975409811457ULL,
    72339215047754240ULL, 18155273440989312ULL, 4613959945983951104ULL, 145812974690501120ULL,
    281543763820800ULL, 147495088967385216ULL, 2969386217113789440ULL, 19215066297569792ULL,
    180144054896435457ULL, 2377928092116066437ULL, 9277424307650174977ULL, 4621827982418248737ULL,
    563158798583922ULL, 5066618438763522ULL, 144221860300195844ULL, 281752018887682ULL
    };
// Pre-generated magic numbers for bishop attacks
static constexpr bitBoard bishopMagicNums[64] = {
    18018831494946945ULL, 1134767471886336ULL, 2308095375972630592ULL, 27308574661148680ULL,
    9404081239914275072ULL, 4683886618770800641ULL, 216245358743802048ULL, 9571253153235970ULL,
    27092002521253381ULL, 1742811846410792ULL, 8830470070272ULL, 9235202921558442240ULL,
    1756410529322199040ULL, 1127005325142032ULL, 1152928124311179269ULL, 2377913937382869017ULL,
    2314850493043704320ULL, 4684324174200832257ULL, 77688339246880000ULL, 74309421802472544ULL,
    8649444578941734912ULL, 4758897525753456914ULL, 18168888584831744ULL, 2463750540959940880ULL,
    9227893366251856128ULL, 145276341141897348ULL, 292821938185734161ULL, 5190965918678714400ULL,
    2419567834477633538ULL, 2308272929927873024ULL, 18173279030480900ULL, 612771170333492228ULL,
    4611976426970161409ULL, 2270508834359424ULL, 9223442681551127040ULL, 144117389281722496ULL,
    1262208579542270208ULL, 13988180992906560530ULL, 4649975687305298176ULL, 9809420809726464128ULL,
    1153222256471056394ULL, 2901448468860109312ULL, 40690797321924624ULL, 4504295814726656ULL,
    299204874469892ULL, 594838215186186752ULL, 7210408796106130432ULL, 144405467744964672ULL,
    145390656058359810ULL, 1153203537948246016ULL, 102002796048417802ULL, 9243919728426124800ULL,
    2455024885924167748ULL, 72066815467061280ULL, 325424741529814049ULL, 1175584649085829253ULL,
    18720594346444812ULL, 584352516473913920ULL, 1441151883179198496ULL, 4919056693802862608ULL,
    1161950831810052608ULL, 2464735771073020416ULL, 54610562058947072ULL, 580611413180448ULL
};

// The bitboard of squares between two squares. If squares are not in a straight line the between bitboard is only the second index
extern bitBoard betweenBB[64][64];
extern bitBoard squares[64];
extern bitBoard pawnAttacks[2][64];
extern bitBoard knightAttacks[64];
extern bitBoard kingAttacks[64];
extern bitBoard rookAttacks[102400];
extern bitBoard bishopAttacks[5248];

extern MagicSquare rookMagics[64];
extern MagicSquare bishopMagics[64];


template<Direction dir>
constexpr bitBoard shift(bitBoard bb) {
    return dir == NORTH ? bb << 8 : dir == NORTHEAST ? bb << 9
        :  dir == EAST ? bb << 1 : dir == SOUTHEAST ? bb >> 7
        :  dir == SOUTH ? bb >> 8 : dir == SOUTHWEST ? bb >> 9
        :  dir == WEST ? bb >> 1 : dir == NORTHWEST ? bb << 7
        : 0;
}

inline int getLSBIndex(bitBoard bb) {
    return __builtin_ctzll(bb);
}

inline int popLSB(bitBoard& bb) {
    int sq = getLSBIndex(bb);
    bb &= bb-1;
    return sq;
}

inline int countBits(bitBoard bb) {
    return __builtin_popcountll(bb);
}

inline void setBit(bitBoard& bb, int index) {
    bb = bb | ((bitBoard)1 << index);
}

inline bitBoard moreThanOne(bitBoard bb) {
    return bb & (bb - 1);
}

template<PieceType pt>
inline bitBoard genAttacksBB(int square, bitBoard occ) {
    MagicSquare ms;
    switch (pt)
    {
    case KNIGHT:
        return knightAttacks[square];
    case BISHOP:
        ms = bishopMagics[square];
        occ &= ms.occMask;
        return ms.attacks[(occ * ms.magic) >> ms.shift];
    case ROOK:
        ms = rookMagics[square];
        occ &= ms.occMask;
        return ms.attacks[(occ * ms.magic) >> ms.shift];
    case QUEEN:
        return genAttacksBB<BISHOP>(square, occ) | genAttacksBB<ROOK>(square, occ);
    case KING:
        return kingAttacks[square];
    }
}

template<PieceType pt>
inline bitBoard genAttacksBB(int square) {
    switch (pt)
    {
    case KNIGHT:
        return knightAttacks[square];
    case BISHOP:
        return bishopMagics[square].attacks[0];
    case ROOK:
        return rookMagics[square].attacks[0];
    case QUEEN:
        return genAttacksBB<ROOK>(square) | genAttacksBB<BISHOP>(square);
    case KING:
        return kingAttacks[square];
    }
}

inline bitBoard getBetweenBB(int sq1, int sq2) {
    return betweenBB[sq1][sq2];
}

#endif