#include "Search.h"

Move Search::searchStart(ChessBoard& cb, int depth) {
    nodesSearched = 0;
    currDepth = depth;
    int topScore = -infinity;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    MovePick mp = MovePick(moves, length, cb);
    Move curr = mp.getNext();
    Move topMove = moves[0].move;
    StateInfo si;
    while (curr != NOMOVE) {
        cb.doMove(curr, si);
        int tempScore = -search(cb, -infinity, -topScore, depth - 1);
        cb.undoMove(curr);
        if (tempScore > topScore) {
            topScore = tempScore;
            topMove = curr;
        }
        nodesSearched++;
        curr = mp.getNext();
    }
    std::cout << "Eval: " << (double)topScore / mgVals[PAWN] << std::endl;
    std::cout << "Nodes Visited: " << nodesSearched << std::endl;
    return topMove;
}

int Search::search(ChessBoard& cb, int alpha, int beta, int depth) {
    nodesSearched++;
    if (depth <= 0) {
        return quiesce(cb, alpha, beta);
    }
    int topScore = -infinity;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    if (length == 0) {
        if (cb.checkers()) {
            return -infinity + (currDepth - depth);
        } else {
            return draw;
        }
    }
    MovePick mp = MovePick(moves, length, cb);
    Move curr = mp.getNext();
    StateInfo si;
    while (curr != NOMOVE) {
        cb.doMove(curr, si);
        int tempScore = -search(cb, -beta, -alpha, depth - 1);
        cb.undoMove(curr);
        if (tempScore >= beta) {
            return tempScore;
        }
        if (tempScore > topScore) {
            topScore = tempScore;
            if (tempScore > alpha) {
                alpha = tempScore;
            }
        }
        curr = mp.getNext();
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
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, true);
    int length = end - moves;
    MovePick mp = MovePick(moves, length, cb);
    Move curr = mp.getNext();
    StateInfo si;

    while (curr != NOMOVE) {
        cb.doMove(curr, si);
        int tempScore = -quiesce(cb, -beta, -alpha);
        cb.undoMove(curr);
        if (tempScore >= beta) {
            return tempScore;
        }
        if (tempScore > eval) {
            eval = tempScore;
            if(tempScore > alpha) {
                alpha = tempScore;
            }
        }
        curr = mp.getNext();
    }
    return eval;
}