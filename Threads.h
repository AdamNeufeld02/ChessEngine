#ifndef THREADS_H
#define THREADS_H
#include "Types.h"
#include "Evaluation.h"
#include "ChessBoard.h"
#include "Search.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>

struct SearchInfo {
    int depth;
    int nodesSearched;
    int score;
    Move best;
};

class Thread;

class ThreadPool {
    public:
    ThreadPool(int size_);
    void waitForAllThreads();
    // Searches the given chessboard for the allotedTime
    void startSearching(ChessBoard& cb, int time);
    void wakeAllThreads();
    void stopAllThreads();
    void destroy();
    SearchInfo getBestThread();
    int getNextDepth(int iterDepth);

    private:
    int size;
    Thread** threads;
};

class Thread {
    public:
    Thread(int id);
    void reset();
    void wakeAndSearch();
    void waitForSearch();
    void destroy();

    std::mutex mtx;
    std::condition_variable cv;
    std::thread t;
    bool searching, quit = false;
    pawnTT pTT;

    // The main history move table indexed by colour and the from then to square of a move
    int history[2][64][64];
    // The counter move history storing moves indexed by the Piece and to square of a move
    Move counterMoves[PIECENB][64];

    protected:
    friend class ThreadPool;
    void idle();
    void searchStart();
    int aspirationSearch(Stack* ss, int center, int depth);
    int rootSearch(Stack* ss, int alpha, int beta, int depth);
    template<NodeType type>
    int search(Stack* ss, int alpha, int beta, int depth);
    template<NodeType type>
    int quiesce(Stack* ss, int alpha, int beta);

    void updateHistory(Move* quiets, int quietCount, Move best);

    int id;

    int bonus;
    int penalty;
    int currDepth;
    
    Move bestMove;
    int bestScore;
    int depthReached;
    int nodesSearched;
    ChessBoard cb;
    Move pv[MAXDEPTH];
    
    ThreadPool* parent;
    
};

class MainThread : public Thread {
    using Thread::Thread;

    public:
    void checkTime();

    protected:
    friend class ThreadPool;
    int alloted;
    std::chrono::steady_clock::time_point start;

};

#endif