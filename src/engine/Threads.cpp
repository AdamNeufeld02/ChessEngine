#include "Threads.h"

Thread::Thread(int id_) {
    std::unique_lock<std::mutex> lk(mtx);
    pTT.set(120000);
    reset();
    id = id_;
    // Set searching to true here so the threadpool constructor will wait for all threads to be inside of the 
    // idle loop before continuing
    searching = true;
    mtx.unlock();
    t = std::thread(&idle, this);
}

// Clears the threads searchstats and move history Called on startup and should be called on new game
void Thread::reset() {
    pTT.clear();
    searching = false;
    bestMove = NOMOVE;
    bestScore = -Infinity;
    depthReached = 0;
    nodesSearched = 0;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                history[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < PIECENB; i++) {
        for (int j = 0; j < 64; j++) {
            counterMoves[i][j] = NOMOVE;
        }
    }
}

void Thread::idle() {
    while (true) {
        std::unique_lock<std::mutex> lk(mtx);
        searching = false;
        cv.notify_all();
        cv.wait(lk, [&]{return searching;});
        if (quit) return;
        mtx.unlock();
        searchStart();
    }
}

void Thread::wakeAndSearch() {
    std::unique_lock<std::mutex> lk(mtx);
    searching = true;
    cv.notify_one();
}

void Thread::destroy() {
    std::unique_lock<std::mutex> lk(mtx);
    quit = true;
    searching = true;
    cv.notify_all();
    mtx.unlock();
    t.join();
}

void Thread::waitForSearch() {
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [&]{ return !searching; });
}

void MainThread::checkTime() {
    auto curr = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start).count();
    if (elapsed >= alloted) {
        parent->stopAllThreads();
    }
}

ThreadPool::ThreadPool(int size_) {
    if (size_ > 0) {
        size = size_;
        threads = (Thread**)malloc(sizeof(Thread*) * (size));
        threads[0] = new MainThread(0);
        static_cast<MainThread*>(threads[0])->parent = this;
        threads[0]->penalty = 1;
        threads[0]->bonus = 5;
        for (int i = 1; i < size; i++) {
            threads[i] = new Thread(i);
            threads[i]->parent = this;
            threads[i]->penalty = 1 + i % 2;
            threads[i]->bonus = 5 + i % 3;
        }

        // Init reduction table
        for (int i = 0; i < MAXMOVES; i++) {
            Search::reductions[i] = std::log(i);
        }
    }
    // Wait for all threads to be ready before returning
    waitForAllThreads();
}

void ThreadPool::startSearching(ChessBoard& cb, int time) {
    // Make sure no threads are searching
    waitForAllThreads();
    // Init main thread
    MainThread* mt = static_cast<MainThread*>(threads[0]);
    mt->alloted = time;
    mt->start = std::chrono::steady_clock::now();
    for (int i = 0; i < size; i++) {
        threads[i]->bestMove = NOMOVE;
        threads[i]->bestScore = -Infinity;
        threads[i]->cb.copy(cb);
        threads[i]->nodesSearched = 0;
        threads[i]->depthReached = 0;
        threads[i]->cb.thisThread = threads[i];
    }
    wakeAllThreads();
}

void ThreadPool::wakeAllThreads() {
    for (int i = 0; i < size; i++) {
        threads[i]->wakeAndSearch();
    }
}

void ThreadPool::waitForAllThreads() {
    for (int i = 0; i < size; i++) {
        threads[i]->waitForSearch();
    }
}

void ThreadPool::stopAllThreads() {
    for (int i = 0; i < size; i++) {
        threads[i]->searching = false;
    }
}

SearchInfo ThreadPool::getBestThread() {
    SearchInfo si;
    si.depth = 0;
    si.best = NOMOVE;
    si.nodesSearched = 0;
    si.score = -Infinity;
    Thread* bestThread = threads[0];

    std::map<Move, int64_t> votes;
    int minScore = threads[0]->bestScore;
    for (int i = 1; i < size; i++) {
        minScore = std::min(minScore, threads[i]->bestScore);
    }

    for (int i = 0; i < size; i++) {

        votes[threads[i]->bestMove] += std::max((threads[i]->bestScore - minScore), 1) + 5 * threads[i]->depthReached;
        
        if (votes[threads[i]->bestMove] > votes[bestThread->bestMove]) {
            bestThread = threads[i];
        }

        si.nodesSearched += threads[i]->nodesSearched;
    }

    si.depth = bestThread->depthReached;
    si.best = bestThread->bestMove;
    si.score = bestThread->bestScore;

    return si;
}

// Destroys the threadpool all threads should already be idle
void ThreadPool::destroy() {
    for (int i = 0; i < size; i++) {
        threads[i]->destroy();
        delete threads[i];
    }
    free(threads);
}