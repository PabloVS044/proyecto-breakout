#include "ui/Menu.h"
#include "ui/Instructions.h"
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
            case 0: // Iniciar partida
            {
                Game game;
                game.run();
                break;
            }
            case 1: // Instrucciones
            {
                Instructions::show();
                break;
            }
            case 2: // Puntajes
            {
                clear();
                mvprintw(0, 0, "Pantalla de puntajes (pendiente)...\n");
                mvprintw(1, 0, "Presiona cualquier tecla para continuar...");
                refresh();
                getch();
                break;
            }
            case 3: // Salir
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