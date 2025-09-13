#include "Scoreboard.h"
#include <ncurses.h>

Scoreboard::Scoreboard() : score(0), highScore(0) {}

void Scoreboard::addPoints(int points) {
    score += points;
    if (score > highScore) highScore = score;
}

void Scoreboard::reset() {
    score = 0;
}

void Scoreboard::render(int y, int x) const {
    mvprintw(y, x, "Score: %d   High Score: %d", score, highScore);
}
