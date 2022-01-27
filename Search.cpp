#include "Search.h"

Move Search::searchStart(ChessBoard& cb, int depth) {
    nodesSearched = 0;
    int topScore = -infinity;
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    Move topMove = moves[0];
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -search(cb, -infinity, -topScore, depth - 1);
        cb.undoMove(moves[i]);
        if (tempScore > topScore) {
            topScore = tempScore;
            topMove = moves[i];
        }
        nodesSearched++;
    }
    std::cout << "Eval: " << topScore << std::endl;
    std::cout << "Nodes Visited: " << nodesSearched << std::endl;
    return topMove;
}

int Search::search(ChessBoard& cb, int alpha, int beta, int depth) {
    nodesSearched++;
    if (depth <= 0) {
        return quiesce(cb, alpha, beta);
    }
    int topScore = -infinity;
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    if (length == 0) {
        if (cb.checkers()) {
            return -infinity;
        } else {
            return draw;
        }
    }
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -search(cb, -beta, -alpha, depth - 1);
        cb.undoMove(moves[i]);
        if (tempScore >= beta) {
            return tempScore;
        }
        if (tempScore > topScore) {
            topScore = tempScore;
            if (tempScore > alpha) {
                alpha = tempScore;
            }
        }
    }
    return topScore;
}

int Search::quiesce(ChessBoard& cb, int alpha, int beta) {
    nodesSearched++;
    int eval = Evaluation::evaluate(cb);
    if (eval >= beta) {
        return eval;
    }
    if (eval > alpha) {
        alpha = eval;
    }
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves, true);
    int length = end - moves;
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -quiesce(cb, -beta, -alpha);
        cb.undoMove(moves[i]);
        if (tempScore >= beta) {
            return tempScore;
        }
        if (tempScore > eval) {
            eval = tempScore;
            if (tempScore > alpha) {
                alpha = tempScore;
            }
        }
    }
    return eval;
}