#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <ncurses.h>

class Scoreboard {
public:
    int score;
    
private:
    int highScore;

public:
    Scoreboard();
    void addPoints(int points);
    void reset();
    void render(int y, int x) const;
    int getScore() const { return score; }
    int getHighScore() const { return highScore; }
};

#endif
