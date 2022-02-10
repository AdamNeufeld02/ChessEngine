#include "BitBoards.h"
#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "Misc.h"
#include "Zobrist.h"
#include <chrono>

int perft(ChessBoard& cb, int depth) {
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int total = 0;
    int size = end - moves;
    StateInfo si;
    if (depth == 1) {
        return size;
    } else {
        for (int i = 0; i < size; i++) {
            cb.doMove(moves[i].move, si);
            total += perft(cb, depth - 1);
            cb.undoMove(moves[i].move);
            end--;
        }
        return total;
    }
}

int main(int arc, char** argv) {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
    Zobrist::init();
    std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    StateInfo si;
    ChessBoard cb = ChessBoard(startFen, si);
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int size = end - moves;
    int curr = 0;
    long long total = 0;
    StateInfo st;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < size; i++) {
        cb.doMove(moves[i].move, st);
        curr = perft(cb, 6);
        std::cout << Misc::moveToString(moves[i].move) << ": " << curr << std::endl;
        cb.undoMove(moves[i].move);
        total += curr;
        end--;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Total Time(ms): " << ms_int.count() << std::endl;
    std::cout << "Nodes: " << total << std::endl;
    std::cout << "NPS: " << (total * 1000) / ms_int.count() << std::endl;
}

