#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <ncurses.h>

class Scoreboard {
private:
    int score;
    int highScore;

public:
    Scoreboard();
    void addPoints(int points);
    void reset();
    void render(int y, int x) const;
};

#endif // SCOREBOARD_H
