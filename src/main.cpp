#include "ui/Menu.h"
#include "Instructions.h"
#include <ncurses.h>

int main() {
    initscr();            // Inicializa ncurses
    noecho();             // No mostrar teclas
    cbreak();             // Captura teclas al vuelo
    keypad(stdscr, TRUE); // Habilita flechas
    curs_set(0);          // Oculta cursor

    Menu menu;
    bool running = true;

    while (running) {
        int choice = menu.navigate();
        switch (choice) {
            case 0: // Iniciar partida
                clear();
                mvprintw(0, 0, "Iniciando partida (pendiente implementar)...\n");
                mvprintw(1, 0, "Presiona cualquier tecla para continuar...");
                refresh();
                getch();
                break;
            case 1: // Instrucciones
                Instructions::show();
                break;
            case 2: // Puntajes
                clear();
                mvprintw(0, 0, "Pantalla de puntajes (pendiente)...\n");
                mvprintw(1, 0, "Presiona cualquier tecla para continuar...");
                refresh();
                getch();
                break;
            case 3: // Salir
                clear();
                mvprintw(0, 0, "Saliendo del juego.\n");
                refresh();
                running = false;
                break;
        }
    }

    endwin(); // Cierra ncurses
    return 0;
}