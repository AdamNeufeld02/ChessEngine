#include "Search.h"
int Search::currDepth;
Move Search::bestFound;
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

    start = std::chrono::steady_clock::now();
    abort = false;
    alloted = time * 1000;

    Stack ss[MAXDEPTH];
    Move pv[MAXDEPTH];
    for (int i = 0; i < MAXDEPTH; i++) {
        ss[i].ply = i;
        ss[i].killers[0] = NOMOVE;
        ss[i].killers[1] = NOMOVE;
    }
    ss->pv = pv;
    ss->pv[0] = NOMOVE;

    bestFound = NOMOVE;

    int depthReached = 0;

    int val = searchRoot(cb, ss, -Infinity, Infinity, 1);
    // Iterative Deepening:
    // Repeatedly search with increasing depth up until max depth or time runs out
    // Use the results of the previous search for move ordering
    for (int depth = 2; depth < MAXDEPTH; depth++) {
        val = widenSearch(cb, ss, val, depth);
        if (abort) {
            std::cout << "Total Nodes Visited: " << nodesSearched << std::endl;
            return bestFound;
        }
        depthReached = depth;
        std::cout << "Eval: " << (double)val / mgVals[PAWN] << std::endl;
        std::cout << "Nodes Visited: " << nodesSearched << std::endl;
        std::cout << "Depth: " << depthReached << std::endl;
        std::cout << "PV: ";
        for (int i = 0; i < depth; i++) {
            if (pv[i] == NOMOVE) break;
            std::cout << Misc::moveToString(pv[i]);
            std::cout << " ";
        }
        std::cout << std::endl;
        std::cout << "--------------------" << std::endl;
    }

    return bestFound;
}

void Search::checkTime() {
    auto curr = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start).count();
    abort = elapsed >= alloted;
}

int Search::widenSearch(ChessBoard& cb, Stack* ss, int val, int depth) {
    int alpha = val - aspiration;
    int beta = val + aspiration;
    // Aspiration Windows:
    // We can use the results of the previous iteration to guide us in the next iteration
    // by limiting the window [alpha, beta] if this fails we must research with a full window.
    int temp = searchRoot(cb, ss, alpha, beta, depth);
    if (temp <= alpha || temp >= beta) {
        std::cout << "Re-Searching" << std::endl;
        temp = searchRoot(cb, ss, -Infinity, Infinity, depth);
    }
    // Update PV on non aborted searches
    if (!abort) {
        bestFound = ss->pv[0];
    }
    return temp;
}

int Search::searchRoot(ChessBoard& cb, Stack* ss, int alpha, int beta, int depth) {
    currDepth = depth;
    int topScore = -Infinity;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    MovePick mp = MovePick(moves, length, bestFound, ss->killers, cb);
    Move curr = mp.getNext();
    
    Move topMove = NOMOVE;

    Move pv[MAXDEPTH];
    (ss+1)->pv = pv;
    StateInfo si;
    int moveCount = 1;
    int value = -Infinity;
    while (curr != NOMOVE) {

        if (abort)  {
            return 0;
        }

        cb.doMove(curr, si);

        if (moveCount > 1) {
            value = -search<NonPV>(cb, ss+1, -(alpha+1), -alpha, depth-1);
        }

        if (moveCount == 1 || (value > alpha && value < beta)) {
            (ss+1)->pv = pv;
            (ss+1)->pv[0] = NOMOVE;
            value = -search<PV>(cb, ss+1, -beta, -alpha, depth-1);
        }
        
        cb.undoMove(curr);
        if (value >= beta) {
            return value;
        }
        if (value > topScore) {
            topScore = value;
            if (topScore > alpha) {
                alpha = topScore;
                topMove = curr;
                updatePv(ss->pv, curr, (ss+1)->pv);
            }
        }
        nodesSearched++;
        moveCount++;
        curr = mp.getNext();
    }
    return topScore;
}

template<NodeType type>
int Search::search(ChessBoard& cb, Stack* ss, int alpha, int beta, int depth) {
    // Check the time every 4096 nodes searched
    if (!abort && !(nodesSearched & 4095)) {
        checkTime();
    }

    if (abort) {
        return 0;
    }

    nodesSearched++;
    Move best = NOMOVE;
    int topScore;
    
    Move pv[MAXDEPTH];
    (ss+1)->pv = pv;
    bool isPV = type != NonPV;

    bool hit;
    zobristKey key = cb.key();
    Entry* ttEntry = tTable.probe(key, &hit);
    int staticEval = NoValue;

    if (hit && ttEntry->getDepth() >= depth && !isPV) {
        Type ttType = ttEntry->getType();
        int ttScore = ttEntry->getScore();
        if (ttType == Lower && ttScore >= beta) {
            return ttScore;
        } else if (ttScore < beta && (ttType == Upper || ttType == Exact)) {
            return ttScore;
        }
    }
    if (hit) {
        best = ttEntry->getMove();
        staticEval = ttEntry->getEval();
    }

    // Drop into quiescense when we reach the required depth
    if (depth <= 0) {
        return quiesce<type>(cb, alpha, beta);
    }
    topScore = -Infinity;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;

    if (length == 0) {
        if (cb.checkers()) {
            return -Infinity + (currDepth - depth);
        } else {
            return Draw;
        }
    }

    // Razoring:
    // If A Node is doing particularly bad we can drop into quiscence
    // If it fails low we can stop searching.
    if (!(isPV) && depth < 3 && !cb.checkers()) {
        if (staticEval == NoValue) {
            staticEval = Evaluation::evaluate(cb);
        }
        if (staticEval + 150 * depth * depth < alpha) {
            int val = quiesce<NonPV>(cb, alpha, alpha+1);
            if (val < alpha) return val;
        }
    }

    if (type == RootPV) {
        best = (ss - ss->ply)->pv[ss->ply];
    }

    MovePick mp = MovePick(moves, length, best, ss->killers, cb);
    
    Move curr = mp.getNext();

    int value;
    int moveCount = 1;
    StateInfo si;
    while (curr != NOMOVE) {
        __builtin_prefetch(tTable.getEntry(cb.keyAfter(curr)));
        cb.doMove(curr, si);
        // If we are in a non pv node or have already searched the first move of a PV node
        // We do a null window search on the move. If that fails low we know it wont surpass alpha
        if (!isPV || moveCount > 1) {
            value = -search<NonPV>(cb, ss+1, -(alpha+1), -alpha, depth - 1);
        }

        // If this is the first move of a pv node or the previous null window search failed high
        // We must do a full window search to get the exact value.
        if (isPV && (moveCount == 1 || (value > alpha && value < beta))) {
            (ss+1)->pv = pv;
            (ss+1)->pv[0] = NOMOVE;
            value = -search<PV>(cb, ss+1, -beta, - alpha, depth - 1);
        }

        cb.undoMove(curr);
        if (abort) return 0;
        if (value > topScore) {
            topScore = value;
            if (value > alpha) {
                best = curr;
                if (isPV) {
                    alpha = value;
                    updatePv(ss->pv, curr, (ss+1)->pv);
                }
            }
            if (value >= beta) {
                tTable.addEntry(key, curr, value, staticEval, depth, Lower);
                // Update killers
                if (!cb.pieceOn(getTo(curr)) && !(getFlag(curr) == PROMOTION)) {
                    if (ss->killers[0] != curr) {
                        ss->killers[1] = ss->killers[0];
                        ss->killers[0] = curr;
                    }
                }
                return value;
            }
        }
        curr = mp.getNext();
        moveCount++;
    }
    Type bound = isPV && best ? Exact : Upper; 
    tTable.addEntry(key, best, topScore, staticEval, depth, bound);
    return topScore;
}

template<NodeType type>
int Search::quiesce(ChessBoard& cb, int alpha, int beta) {
    if (!abort && !(nodesSearched & 4095)) {
        checkTime();
    }
    if (abort) {
        return 0;
    }

    bool isPV = type != NonPV;

    bool hit;
    zobristKey key = cb.key();
    Entry* ttEntry = tTable.probe(key, &hit);
    int staticEval = NoValue;
    Move best = NOMOVE;
    int topScore;

    if (hit && !isPV) {
        Type ttType = ttEntry->getType();
        int ttScore = ttEntry->getScore();
        if (ttType == Lower && ttScore >= beta) {
            return ttScore;
        } else if (ttScore < beta && (ttType == Upper || ttType == Exact)) {
            return ttScore;
        }
    }
    if (hit) {
        best = ttEntry->getMove();
        staticEval = ttEntry->getEval();
    }

    nodesSearched++;

    if (staticEval == NoValue) {
        staticEval = Evaluation::evaluate(cb);
    }

    topScore = staticEval;

    if (staticEval >= beta) {
        return staticEval;
    }
    if (staticEval > alpha) {
        if (isPV) {
            alpha = staticEval;
        }
    }

    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, true);
    int length = end - moves;
    Move blank[2] = {NOMOVE, NOMOVE};
    MovePick mp = MovePick(moves, length, best, blank, cb);
    Move curr = mp.getNext();
    StateInfo si;

    while (curr != NOMOVE) {
        __builtin_prefetch(tTable.getEntry(cb.keyAfter(curr)));
        cb.doMove(curr, si);
        int tempScore = -quiesce<type>(cb, -beta, -alpha);
        cb.undoMove(curr);
        if (abort) return 0;
        if (tempScore >= beta) {
            tTable.addEntry(key, curr, tempScore, staticEval, 0, Lower);
            return tempScore;
        }
        if (tempScore > topScore) {
            topScore = tempScore;
            if(tempScore > alpha) {
                best = curr;
                if (isPV) {
                    alpha = tempScore;
                }
            }
        }
        curr = mp.getNext();
    }
    tTable.addEntry(key, best, topScore, staticEval, 0, Upper);
    return topScore;
}

void Search::updatePv(Move* pv, Move move, Move* childPv) {
    for (*pv++ = move; childPv && *childPv != NOMOVE; )
        *pv++ = *childPv++;
    *pv = NOMOVE;
}