#ifndef PLAYER_H
#define PLAYER_H

#include "ChessBoard.h"
#include "Threads.h"
#include <vector>
#include <deque>
#include <fstream>

const static std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

#define RESULTFILE "Fens.txt"

class Player
{
private:
    ThreadPool* engine1;
    ThreadPool* engine2;
    Colour engine1Colour;
    ChessBoard* cb;
    std::unique_ptr<std::deque<StateInfo>> states;
    std::vector<std::string> fens;
    int result;
    void newGame();
    void selfPlay();
    void writeResults();
    
public:
    Player();
    void start();
    
};


#endif