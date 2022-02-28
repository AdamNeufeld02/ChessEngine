#include "Search.h"
#include "Threads.h"
TransposTable Search::tTable;

void Search::init() {
    tTable.clear();
}

void Thread::searchStart() {
    nodesSearched = 0;
    bestMove = NOMOVE;
    depthReached = 0;

    Stack ss[MAXDEPTH];
    for (int i = 0; i < MAXDEPTH; i++) {
        ss[i].ply = i;
        ss[i].killers[0] = NOMOVE;
        ss[i].killers[1] = NOMOVE;
    }
    ss->pv = pv;
    ss->pv[0] = NOMOVE;

    

    int val = rootSearch(ss, -Infinity, Infinity, currDepth);
    if (searching) {
        bestMove = pv[0];
        bestScore = val;
        depthReached = 1;
    }
    // Iterative Deepening:
    // Repeatedly search with increasing depth up until max depth or time runs out
    // Use the results of the previous search for move ordering
    for (int depth = currDepth + 1; depth < MAXDEPTH; depth++) {
        depth = parent->getNextDepth(depth);
        currDepth = depth;
        val = aspirationSearch(ss, val, depth);
        if (!searching) {
            return;
        }
    }
}

int Thread::aspirationSearch(Stack* ss, int center, int depth) {
    
    int delta = Search::aspiration;
    int alpha = center - delta;
    int beta = center + delta;
    // Aspiration Windows:
    // We can use the results of the previous iteration to guide us in the next iteration
    // by limiting the window [alpha, beta] if this fails we must research with a full window.
    int temp = rootSearch(ss, alpha, beta, depth);
    while (true) {
        if (temp > alpha && temp < beta) break;
        else if (temp >= beta) beta += delta;
        else alpha -= delta;

        delta += delta + delta / 2;
        temp = rootSearch(ss, alpha, beta, depth);
    }
    // Update Thread info on completed searches
    if (searching) {
        bestMove = ss->pv[0];
        bestScore = temp;
        depthReached = depth;
    }
    return temp;
}

int Thread::rootSearch(Stack* ss, int alpha, int beta, int depth) {
    int topScore = -Infinity;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;
    MovePick mp = MovePick(moves, length, bestMove, NOMOVE, ss->killers, cb);
    Move curr = mp.getNext();
    
    Move topMove = NOMOVE;
    ss->didNull = false;
    ss->current = NOMOVE;
    Move pv[MAXDEPTH];
    (ss+1)->pv = pv;

    StateInfo si;
    int moveCount = 1;
    int value = -Infinity;
    while (curr != NOMOVE) {

        if (!searching)  {
            return 0;
        }
        ss->current = curr;
        cb.doMove(curr, si);

        if (moveCount > 1) {
            value = -search<NonPV>(ss+1, -(alpha+1), -alpha, depth-1);
        }

        if (moveCount == 1 || (value > alpha && value < beta)) {
            (ss+1)->pv = pv;
            (ss+1)->pv[0] = NOMOVE;
            value = -search<PV>(ss+1, -beta, -alpha, depth-1);
        }
        
        cb.undoMove(curr);

        if (!searching) {
            return 0;
        }

        if (value >= beta) {
            return value;
        }
        if (value > topScore) {
            topScore = value;
            if (topScore > alpha) {
                alpha = topScore;
                topMove = curr;
                Search::updatePv(ss->pv, curr, (ss+1)->pv);
            }
        }
        moveCount++;
        curr = mp.getNext();
    }
    nodesSearched++;
    return topScore;
}

template<NodeType type>
int Thread::search(Stack* ss, int alpha, int beta, int depth) {
    // Check the time every 4096 nodes searched
    if (id == 0 && !(nodesSearched & 4095) && searching) {
        static_cast<MainThread*>(this)->checkTime();
    }

    if (!searching) {
        return 0;
    }

    // Drop into quiescense when we reach the required depth
    if (depth <= 0) {
        return quiesce<type>(ss+1, alpha, beta);
    }

    nodesSearched++;
    Move best = NOMOVE;
    int topScore;
    
    Move pv[MAXDEPTH];
    (ss+1)->pv = pv;
    ss->current = NOMOVE;
    bool isPV = type != NonPV;

    bool hit;
    zobristKey key = cb.key();
    uint64_t ttData = Search::tTable.probe(key, &hit);
    int staticEval = NoValue;

    if (hit && unpackDepth(ttData) >= depth && !isPV) {
        Type ttType = unpackType(ttData);
        int ttScore = unpackScore(ttData);
        if (ttType == Lower && ttScore >= beta) {
            return ttScore;
        } else if (ttScore < beta && (ttType == Upper || ttType == Exact)) {
            return ttScore;
        }
    }
    if (hit) {
        best = unpackMove(ttData);
        staticEval = unpackEval(ttData);
    }

    topScore = -Infinity;
    ScoredMove moves[MAXMOVES];
    ScoredMove* end = MoveGenerator::generateMoves(cb, moves, false);
    int length = end - moves;

    if (length == 0) {
        if (cb.checkers()) {
            return -Infinity + (ss->ply);
        } else {
            return Draw;
        }
    }

    if (cb.isDraw()) return draw;

    if (staticEval == NoValue) {
        staticEval = Evaluation::evaluate(cb);
    }

    // Razoring:
    // If A Node is doing particularly bad we can drop into quiscence
    // If it fails low we can stop searching.
    if (!(isPV) && depth < 3 && !cb.checkers()) {
        if (staticEval + 150 * depth * depth < alpha) {
            int val = quiesce<NonPV>(ss+1, alpha, alpha+1);
            if (val < alpha) return val;
        }
    }

    StateInfo si;

    // Null Move Pruning:
    // If our position is so good that we can let the opponent move twice in row and still have a good position
    // Then we can return this good score
    if(!isPV && !(ss-1)->didNull && staticEval >= beta && !cb.checkers() && cb.nonPawnMaterial(cb.colourToMove())) {
        ss->didNull = true;
        cb.doNullMove(si);
        int nullVal = -search<NonPV>(ss+1, -beta, -beta+1, depth - 3);
        cb.undoNullMove();
        if (!searching) return 0;
        if (nullVal >= beta) {
            if (nullVal >= MateInMax || nullVal <= -MateInMax) {
                return beta;
            } else {
                return nullVal;
            }
        }
    }

    ss->didNull = false;

    if (type == RootPV) {
        best = (ss - ss->ply)->pv[ss->ply];
    }

    MovePick mp = MovePick(moves, length, best, (ss-1)->current, ss->killers, cb);
    
    Move curr = mp.getNext();

    int value;
    int moveCount = 1;
    int quietCount = 0;

    Move quiets[MAXMOVES];

    bool doFullDepth = false;
    
    while (curr != NOMOVE) {
        __builtin_prefetch(Search::tTable.getEntry(cb.keyAfter(curr)));
        ss->current = curr;
        cb.doMove(curr, si);

        if (!isPV && depth >= 2 && !cb.checkers() && moveCount > 1) {
            int R = 1;

            R += moveCount / 5;

            value = -search<NonPV>(ss+1, -(alpha+1), -alpha, depth - 1 - R);

            if (value > alpha) {
                doFullDepth = true;
            } else {
                doFullDepth = false;
            }
                
        } else {
            doFullDepth = !isPV || moveCount > 1;
        }

        //doFullDepth = !isPV || moveCount > 1;

        // If we are in a non pv node or have already searched the first move of a PV node
        // We do a null window search on the move. If that fails low we know it wont surpass alpha
        if (doFullDepth) {
            value = -search<NonPV>(ss+1, -(alpha+1), -alpha, depth - 1);
        }

        // If this is the first move of a pv node or the previous null window search failed high
        // We must do a full window search to get the exact value.
        if (isPV && (moveCount == 1 || (value > alpha && value < beta))) {
            (ss+1)->pv = pv;
            (ss+1)->pv[0] = NOMOVE;
            value = -search<PV>(ss+1, -beta, - alpha, depth - 1);
        }

        cb.undoMove(curr);
        if (!searching) return 0;
        if (value > topScore) {
            topScore = value;
            if (value > alpha) {
                best = curr;
                if (isPV) {
                    alpha = value;
                    Search::updatePv(ss->pv, curr, (ss+1)->pv);
                }
            }
            if (value >= beta) {
                Search::tTable.addEntry(key, curr, value, staticEval, depth, Lower);
                // Update killers
                if (!cb.pieceOn(getTo(curr)) && !(getFlag(curr) == PROMOTION)) {
                    if (ss->killers[0] != curr) {
                        ss->killers[1] = ss->killers[0];
                        ss->killers[0] = curr;
                    }
                }
                // Update CounterMoves
                counterMoves[cb.pieceOn(getTo((ss-1)->current))][getTo((ss-1)->current)] = curr;
                updateHistory(quiets, quietCount, curr);
                return value;
            }
        }

        // Save quiet moves to update history at end
        if (!cb.pieceOn(getTo(curr)) && !(getFlag(curr) == PROMOTION)) {
            quiets[quietCount++] = curr;
        }

        curr = mp.getNext();
        moveCount++;
    }
    Type bound = isPV && best ? Exact : Upper; 
    Search::tTable.addEntry(key, best, topScore, staticEval, depth, bound);
    updateHistory(quiets, quietCount, best);
    if (best) {
        counterMoves[cb.pieceOn(getTo((ss-1)->current))][getTo((ss-1)->current)] = best;
    }
    return topScore;
}

template<NodeType type>
int Thread::quiesce(Stack* ss, int alpha, int beta) {
    // Check the time every 4096 nodes searched
    if (id == 0 && !(nodesSearched & 4095) && searching) {
        static_cast<MainThread*>(this)->checkTime();
    }

    if (!searching) {
        return 0;
    }

    if (cb.isDraw()) return draw;

    bool isPV = type != NonPV;

    bool hit;
    zobristKey key = cb.key();
    uint64_t ttData = Search::tTable.probe(key, &hit);
    int staticEval = NoValue;
    Move best = NOMOVE;
    int topScore;

    if (hit && !isPV) {
        Type ttType = unpackType(ttData);
        int ttScore = unpackScore(ttData);
        if (ttType == Lower && ttScore >= beta) {
            return ttScore;
        } else if (ttScore < beta && (ttType == Upper || ttType == Exact)) {
            return ttScore;
        }
    }
    if (hit) {
        best = unpackMove(ttData);
        staticEval = unpackEval(ttData);
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
    MovePick mp = MovePick(moves, length, best, (ss-1)->current, ss->killers, cb);
    Move curr = mp.getNext();
    StateInfo si;

    while (curr != NOMOVE) {
        __builtin_prefetch(Search::tTable.getEntry(cb.keyAfter(curr)));
        ss->current = curr;
        cb.doMove(curr, si);
        int tempScore = -quiesce<type>(ss+1, -beta, -alpha);
        cb.undoMove(curr);
        if (!searching) return 0;
        if (tempScore >= beta) {
            Search::tTable.addEntry(key, curr, tempScore, staticEval, 0, Lower);
            counterMoves[cb.pieceOn(getTo((ss-1)->current))][getTo((ss-1)->current)] = curr;
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
    Search::tTable.addEntry(key, best, topScore, staticEval, 0, Upper);
    if (best) {
        counterMoves[cb.pieceOn(getTo((ss-1)->current))][getTo((ss-1)->current)] = best;
    }
    return topScore;
}


void Thread::updateHistory(Move* quiets, int quietCount, Move best) {
    Move curr;
    int to, from;
    int val;
    for (int i = 0; i < quietCount; i++) {
        curr = quiets[i];
        if (curr == best) continue;
        from = getFrom(curr);
        to = getTo(curr);
        val = history[colourOf(cb.pieceOn(from))][from][to];
        history[colourOf(cb.pieceOn(from))][from][to] = std::max(val - penalty, 0);
    }

    if (!cb.pieceOn(getTo(best)) && !(getFlag(best) == PROMOTION)) {
        from = getFrom(best);
        to = getTo(best);
        val = history[colourOf(cb.pieceOn(from))][from][to];
        history[colourOf(cb.pieceOn(from))][from][to] = std::min(val + bonus, 75);
    }
}

void Search::updatePv(Move* pv, Move move, Move* childPv) {
    for (*pv++ = move; childPv && *childPv != NOMOVE; )
        *pv++ = *childPv++;
    *pv = NOMOVE;
}