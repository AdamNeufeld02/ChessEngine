#include "Player.h"

Player::Player() {
    engine1 = new ThreadPool(8);
    engine2 = new ThreadPool(8);
    engine1Colour = BLACK;
    states = std::unique_ptr<std::deque<StateInfo>>(new std::deque<StateInfo>(1));
    states->emplace_back();
    cb = new ChessBoard(startingFen, states->back());
}

void Player::start() {
    while (true) {
        newGame();
    }
}

void Player::newGame() {
    states->emplace_back();
    cb->fenToBoard(startingFen, states->back());
    fens.clear();
    engine1Colour = BLACK;
    selfPlay();
}

void Player::selfPlay() {
    ScoredMove moves[MAXMOVES];
    ScoredMove* end;
    SearchInfo si;
    while (true) {
        if (cb->isDraw()) {
            result = 1;
            break;
        }
        end = MoveGenerator::generateMoves(*cb, moves, false);
        if (end == moves) {
            if (cb->checkers()) {
                result = cb->colourToMove() == WHITE ? 0 : 2;
                break;
            } else {
                result = 1;
                break;
            }
        }

        if (engine1Colour == cb->colourToMove()) {
            engine1->startSearching(*cb, 1);
            engine1->waitForAllThreads();
            si = engine1->getBestThread();
        } else {
            engine2->startSearching(*cb, 1);
            engine2->waitForAllThreads();
            si = engine2->getBestThread();
        }
        states->emplace_back();
        cb->doMove(si.best, states->back());
        fens.push_back(cb->boardToFen());
    }
    writeResults();
}

void Player::writeResults() {
    std::string finalRes;
    if (result == 0) {
        finalRes = " [0.0]\n";
    } else if (result == 1) {
        finalRes = " [0.5]\n";
    } else if (result == 2) {
        finalRes = " [1.0]\n";
    } else {
        std::cout << "Error on write" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ofstream oFile;
    oFile.open(RESULTFILE, std::ofstream::out | std::ofstream::app);

    for (std::string fen : fens) {
        oFile << fen;
        oFile << finalRes;
    }
    oFile.close();
}