#include "BitBoards.h"
#include "ChessBoard.h"
#include "MoveGenerator.h"
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

std::string indexToStr(int index) {
    int rank = index / 8;
    int file = index % 8;
    std::string r;
    std::string f;
    switch (rank)
    {
    case 0:
        r = "1";
        break;
    case 1:
        r = "2";
        break;
    case 2:
        r = "3";
        break;
    case 3:
        r = "4";
        break;
    case 4:
        r = "5";
        break;
    case 5:
        r = "6";
        break;
    case 6:
        r = "7";
        break;
    case 7:
        r = "8";
        break;
    }
    switch (file)
    {
    case 0:
        f = "a";
        break;
    case 1:
        f = "b";
        break;
    case 2:
        f = "c";
        break;
    case 3:
        f = "d";
        break;
    case 4:
        f = "e";
        break;
    case 5:
        f = "f";
        break;
    case 6:
        f = "g";
        break;
    case 7:
        f = "h";
        break;
    }
    f.append(r);
    return f;
}

std::string moveToStr(Move move) {
    std::string from = indexToStr(getFrom(move));
    std::string to = indexToStr(getTo(move));
    from.append(to);
    return from;
}

int main(int arc, char** argv) {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
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
        std::cout << moveToStr(moves[i].move) << ": " << curr << std::endl;
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

