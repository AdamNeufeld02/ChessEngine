#include "GameState.h"
#include "Zobrist.h"
#include "Search.h"

int main(int argc, char** argv) {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
    Zobrist::init();
    Search::init();
    GameState gs = GameState();
    gs.start();

    return 0;
}