#include "Player.h"
int main() {
    BitBoards::precomputeAttackSets();
    Evaluation::init();
    Zobrist::init();
    Search::init();
    Player player = Player();
    player.start();
}