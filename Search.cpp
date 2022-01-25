#include "Search.h"

Move Search::searchStart(ChessBoard& cb, int depth) {
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves);
    int length = end - moves;
    std::cout << length << std::endl;
    Move bestMove = NOMOVE;
    int score = -infinity;
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -search(cb, depth - 1);
        cb.undoMove(moves[i]);
        std::cout << tempScore << std::endl;
        if (tempScore > score) {
            bestMove = moves[i];
            score = tempScore;
        }
    }
    return bestMove;
}

int Search::search(ChessBoard& cb, int depth) {
    if (depth <= 0) {
        return Evaluation::evaluate(cb);
    }
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves);
    int length = end - moves;
    int topScore = -infinity;
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -search(cb, depth - 1);
        if (tempScore > topScore) {
            topScore = tempScore;
        }
        cb.undoMove(moves[i]);
    }
    return topScore;
}