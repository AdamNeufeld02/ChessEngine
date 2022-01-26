#include "Search.h"

Move Search::searchStart(ChessBoard& cb, int depth) {
    nodesSearched = 0;
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    Move bestMove = NOMOVE;
    int score = -infinity;
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -search(cb, -infinity, infinity, depth - 1);
        cb.undoMove(moves[i]);
        if (tempScore >= score) {
            bestMove = moves[i];
            score = tempScore;
        }
        nodesSearched++;
    }
    std::cout << score << std::endl;
    std::cout << nodesSearched << std::endl;
    return bestMove;
}

int Search::search(ChessBoard& cb, int alpha, int beta, int depth) {
    if (depth <= 0) {
        return quiesce(cb, alpha, beta);
    }
    Move moves[MAXMOVES];
    Move* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    StateInfo si;
    for (int i = 0; i < length; i++) {
        cb.doMove(moves[i], si);
        int tempScore = -search(cb, -beta, -alpha, depth - 1);
        cb.undoMove(moves[i]);
        nodesSearched++;
        if (tempScore >= beta) {
            return tempScore;
        }
        if (tempScore > alpha) {
            alpha = tempScore;
        }
    }
    return alpha;
}

int Search::quiesce(ChessBoard& cb, int alpha, int beta) {
    int eval = Evaluation::evaluate(cb);
    if (eval >= beta) {
        return beta;
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
        nodesSearched++;
        if (tempScore >= beta) {
            return beta;
        }
        if (tempScore > alpha) {
            alpha = tempScore;
        }
    }
    return alpha;
}