#include "Search.h"
int Search::currDepth;
int Search::nodesSearched;
bool Search::abort;
std::chrono::steady_clock::time_point Search::start;
int Search::alloted;
TransposTable Search::tTable;

void Search::init() {
    tTable.clear();
}

Move Search::searchStart(ChessBoard& cb, int time) {
    nodesSearched = 0;
    int topScore;

    start = std::chrono::steady_clock::now();
    abort = false;
    alloted = time * 1000;

    Move bestFound = NOMOVE;
    int iterEval = 0;
    int depthReached = 0;

    for (int depth = 1; depth < 64; depth++) {
        currDepth = depth;
        topScore = -infinity;
        ScoredMove moves[MAXMOVES];
        ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
        int length = end - moves;
        MovePick mp = MovePick(moves, length, bestFound, cb);
        Move curr = mp.getNext();
        Move topMove = moves[0].move;

        StateInfo si;
        while (curr != NOMOVE) {
            if (abort)  {
                std::cout << "Eval: " << (double)iterEval / mgVals[PAWN] << std::endl;
                std::cout << "Nodes Visited: " << nodesSearched << std::endl;
                std::cout << "Depth: " << depthReached << std::endl;
                return bestFound;
            }

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
        bestFound = topMove;
        depthReached = depth;
        iterEval = topScore;
    }

    
    std::cout << "Eval: " << (double)iterEval / mgVals[PAWN] << std::endl;
    std::cout << "Nodes Visited: " << nodesSearched << std::endl;
    return bestFound;
}

void Search::checkTime() {
    auto curr = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start).count();
    abort = elapsed >= alloted;
}

int Search::search(ChessBoard& cb, int alpha, int beta, int depth) {
    nodesSearched++;
    Move best = NOMOVE;
    int topScore;

    if (!abort && !(nodesSearched & 4095)) {
        checkTime();
    }

    if (abort) {
        return 0;
    }

    if (tTable.probe(cb.key(), depth, beta, &topScore, &best)) return topScore;

    if (depth <= 0) {
        return quiesce(cb, alpha, beta);
    }
    topScore = -infinity;
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

    MovePick mp = MovePick(moves, length, best, cb);
    Move curr = mp.getNext();
    StateInfo si;
    while (curr != NOMOVE) {
        cb.doMove(curr, si);
        int tempScore = -search(cb, -beta, -alpha, depth - 1);
        cb.undoMove(curr);
        if (tempScore >= beta) {
            tTable.addEntry(cb.key(), curr, tempScore, depth, Lower);
            return tempScore;
        }
        if (tempScore > topScore) {
            topScore = tempScore;
            best = curr;
            if (tempScore > alpha) {
                alpha = tempScore;
            }
        }
        curr = mp.getNext();
    }
    tTable.addEntry(cb.key(), best, topScore, depth, Exact);
    return topScore;
}

int Search::quiesce(ChessBoard& cb, int alpha, int beta) {
    nodesSearched++;

    if (!abort && !(nodesSearched & 4095)) {
        checkTime();
    }
    if (abort) {
        return 0;
    }

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
    MovePick mp = MovePick(moves, length, NOMOVE, cb);
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