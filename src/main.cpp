#include "ui/Menu.h"
#include "ui/Instructions.h"
#include "ui/Scores.h"
#include <ncurses.h>
#include "game/Game.h"

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    Menu menu;
    bool running = true;

    while (running) {
        int choice = menu.navigate();
        switch (choice) {
            case 0:
            {
                Game game;
                game.run();
                break;
            }
            case 1:
            {
                Instructions::show();
                break;
            }
            case 2:
            {
                Scores::showTop();
                break;
            }
            case 3:
            {
                clear();
                mvprintw(0, 0, "Saliendo del juego.\n");
                refresh();
                running = false;
                break;
            }
        }
    }

    endwin();
    return 0;
}