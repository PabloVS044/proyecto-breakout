#include "Instructions.h"
#include <ncurses.h>

void Instructions::show() {
    clear();
    mvprintw(0, 0, "===== INSTRUCCIONES =====\n");
    mvprintw(1, 0, "Objetivo: Destruir todos los bloques sin dejar caer la pelota.\n\n");
    mvprintw(3, 0, "Controles:\n");
    mvprintw(4, 0, "Jugador 1 -> Flechas izquierda/derecha\n");
    mvprintw(5, 0, "Jugador 2 -> Teclas A (izquierda), D (derecha)\n\n");
    mvprintw(7, 0, "Presiona cualquier tecla para regresar al menu...\n");
    refresh();
    getch();
}