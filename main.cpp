#include "GameState.h"

int main(int argc, char** argv) {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
    GameState gs = GameState();
    gs.start();

    return 0;
}