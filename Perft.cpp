#include "BitBoards.h"
#include "ChessBoard.h"
#include "MoveGenerator.h"
#include <chrono>

int perft(ChessBoard& cb, int depth) {
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves);
    int total = 0;
    int size = end - moves;
    StateInfo si;
    if (depth == 1) {
        return size;
    } else {
        for (int i = 0; i < size; i++) {
            cb.doMove(moves[i], si);
            total += perft(cb, depth - 1);
            cb.undoMove(moves[i]);
            end--;
        }
        return total;
    }
}

int main(int arc, char** argv) {
    BitBoards::precomputeAttackSets();
    std::string startFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    StateInfo si;
    ChessBoard cb = ChessBoard(startFen, si);
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves);
    int size = end - moves;
    int curr = 0;
    long long total = 0;
    StateInfo st;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < size; i++) {
        cb.doMove(moves[i], st);
        curr = perft(cb, 3);
        std::cout << "From: " << getFrom(moves[i]) << " To: " << getTo(moves[i]) << " Num: " << curr << std::endl;
        cb.printBoard(cb.pieces());
        cb.undoMove(moves[i]);
        total += curr;
        end--;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << ms_int.count() << std::endl;
    std::cout << total << std::endl;
}

