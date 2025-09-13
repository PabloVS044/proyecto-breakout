#ifndef GAME_H
#define GAME_H

#include "ui/Scoreboard.h"

class Game {
private:
    bool running;
    Scoreboard scoreboard;

public:
    Game();
    void run();
};

#endif
