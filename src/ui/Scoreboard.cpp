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
    // Marco decorativo superior
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(y, x, "+==================================================+");
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    // Línea del scoreboard con diseño arcade
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(y + 1, x, "|");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    // SCORE actual
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(y + 1, x + 2, "### SCORE:");
    attroff(COLOR_PAIR(3) | A_BOLD);
    
    attron(COLOR_PAIR(3) | A_BOLD | A_REVERSE);
    mvprintw(y + 1, x + 13, " %06d ", score);
    attroff(COLOR_PAIR(3) | A_BOLD | A_REVERSE);
    
    // Separador visual
    attron(COLOR_PAIR(2));
    mvprintw(y + 1, x + 22, "|||");
    attroff(COLOR_PAIR(2));
    
    // HIGH SCORE
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(y + 1, x + 26, "*** HIGH SCORE:");
    attroff(COLOR_PAIR(4) | A_BOLD);
    
    attron(COLOR_PAIR(4) | A_BOLD | A_REVERSE);
    mvprintw(y + 1, x + 42, " %06d ", highScore);
    attroff(COLOR_PAIR(4) | A_BOLD | A_REVERSE);
    
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(y + 1, x + 50, "|");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    // Marco decorativo inferior
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(y + 2, x, "+==================================================+");
    attroff(COLOR_PAIR(2) | A_BOLD);
}