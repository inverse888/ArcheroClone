#include "Game.h"
#include <iostream>

int main() {
    Game game;
    if (game.init()) {
        game.run();
    }
    return 0;
}
