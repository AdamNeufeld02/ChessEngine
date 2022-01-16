#include "GameState.h"

int main(int argc, char** argv) {
    BitBoards::precomputeAttackSets();
    GameState gs = GameState();
    gs.start();

    return 0;
}