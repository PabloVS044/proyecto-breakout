#include "ui/Menu.h"
#include "ui/Instructions.h"
#include "ui/Scores.h"
#include <ncurses.h>
#include "game/Game.h"

int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Inicializar pares de colores
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

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