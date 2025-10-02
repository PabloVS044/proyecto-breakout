#include "Instructions.h"
#include <ncurses.h>
#include <locale.h>

void Instructions::show() {
    clear();
    
    int centerX = COLS / 2;
    int startY = 1;
    
    // Banner principal con estilo arcade
  attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(startY++, centerX - 42, " .___                 __                             .__                  ");
    mvprintw(startY++, centerX - 42, " |   | ____   _______/  |________ __ __   ____  ____ |__| ____   ____   ");
    mvprintw(startY++, centerX - 42, " |   |/    \\ /  ___/\\   __\\_  __ \\  |  \\_/ ___\\/ ___\\|  |/  _ \\ /    \\  ");
    mvprintw(startY++, centerX - 42, " |   |   |  \\\\___ \\  |  |  |  | \\/  |  /\\  \\__\\  \\___|  (  <_> )   |  \\ ");
    mvprintw(startY++, centerX - 42, " |___|___|  /____  > |__|  |__|  |____/  \\___  >___  >__|\\____/|___|  / ");
    mvprintw(startY++, centerX - 42, "          \\/     \\/                          \\/    \\/               \\/  ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    
    startY += 2;
    
    // Sección OBJETIVO con estilo retro
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(startY++, centerX - 30, "+======================================================+");
    mvprintw(startY++, centerX - 30, "|  *****         OBJETIVO DEL JUEGO         *****   |");
    mvprintw(startY++, centerX - 30, "+======================================================+");
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    startY++;
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(startY++, centerX - 28, ">>  Destruir TODOS los bloques del nivel");
    mvprintw(startY++, centerX - 28, ">>  NO dejes caer la pelota");
    mvprintw(startY++, centerX - 28, ">>  Consigue la puntuacion mas alta");
    attroff(COLOR_PAIR(3) | A_BOLD);
    
    startY += 2;
    
    // CONTROLES con diseño de arcade
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(startY++, centerX - 30, "+======================================================+");
    mvprintw(startY++, centerX - 30, "|  ##################  CONTROLES  ################## |");
    mvprintw(startY++, centerX - 30, "+======================================================+");
    attroff(COLOR_PAIR(4) | A_BOLD);
    
    startY += 2;
    
    // JUGADOR 1 - Estilo neón
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(startY++, centerX - 35, "    +============================================+");
    mvprintw(startY++, centerX - 35, "    |  ########  JUGADOR 1  ########          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(5) | A_BOLD);
    
    attron(COLOR_PAIR(5));
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    |      <===  Flecha Izquierda              |");
    mvprintw(startY++, centerX - 35, "    |      ===>  Flecha Derecha                |");
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(5));
    
    startY += 2;
    
    // JUGADOR 2 - Estilo neón alternativo
    attron(COLOR_PAIR(6) | A_BOLD);
    mvprintw(startY++, centerX - 35, "    +============================================+");
    mvprintw(startY++, centerX - 35, "    |  ########  JUGADOR 2  ########          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(6) | A_BOLD);
    
    attron(COLOR_PAIR(6));
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    |      [A]  Mover Izquierda                |");
    mvprintw(startY++, centerX - 35, "    |      [D]  Mover Derecha                  |");
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(6));
    
    startY += 3;
    
    // Footer con animación
    attron(COLOR_PAIR(7) | A_BOLD);
    mvprintw(startY++, centerX - 32, "################################################################");
    attroff(COLOR_PAIR(7) | A_BOLD);
    
    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    mvprintw(startY++, centerX - 25, ">>> PRESIONA CUALQUIER TECLA PARA CONTINUAR <<<");
    attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    
    attron(COLOR_PAIR(7) | A_BOLD);
    mvprintw(startY, centerX - 32, "################################################################");
    attroff(COLOR_PAIR(7) | A_BOLD);
    
    refresh();
    getch();
}#include "Instructions.h"
#include <ncurses.h>
#include <locale.h>

void Instructions::show() {
    clear();
    
    int centerX = COLS / 2;
    int startY = 1;
    
    // Banner principal con estilo arcade
  attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(startY++, centerX - 42, " .___                 __                             .__                  ");
    mvprintw(startY++, centerX - 42, " |   | ____   _______/  |________ __ __   ____  ____ |__| ____   ____   ");
    mvprintw(startY++, centerX - 42, " |   |/    \\ /  ___/\\   __\\_  __ \\  |  \\_/ ___\\/ ___\\|  |/  _ \\ /    \\  ");
    mvprintw(startY++, centerX - 42, " |   |   |  \\\\___ \\  |  |  |  | \\/  |  /\\  \\__\\  \\___|  (  <_> )   |  \\ ");
    mvprintw(startY++, centerX - 42, " |___|___|  /____  > |__|  |__|  |____/  \\___  >___  >__|\\____/|___|  / ");
    mvprintw(startY++, centerX - 42, "          \\/     \\/                          \\/    \\/               \\/  ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    
    startY += 2;
    
    // Sección OBJETIVO con estilo retro
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(startY++, centerX - 30, "+======================================================+");
    mvprintw(startY++, centerX - 30, "|  *****         OBJETIVO DEL JUEGO         *****   |");
    mvprintw(startY++, centerX - 30, "+======================================================+");
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    startY++;
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(startY++, centerX - 28, ">>  Destruir TODOS los bloques del nivel");
    mvprintw(startY++, centerX - 28, ">>  NO dejes caer la pelota");
    mvprintw(startY++, centerX - 28, ">>  Consigue la puntuacion mas alta");
    attroff(COLOR_PAIR(3) | A_BOLD);
    
    startY += 2;
    
    // CONTROLES con diseño de arcade
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(startY++, centerX - 30, "+======================================================+");
    mvprintw(startY++, centerX - 30, "|  ##################  CONTROLES  ################## |");
    mvprintw(startY++, centerX - 30, "+======================================================+");
    attroff(COLOR_PAIR(4) | A_BOLD);
    
    startY += 2;
    
    // JUGADOR 1 - Estilo neón
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(startY++, centerX - 35, "    +============================================+");
    mvprintw(startY++, centerX - 35, "    |  ########  JUGADOR 1  ########          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(5) | A_BOLD);
    
    attron(COLOR_PAIR(5));
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    |      <===  Flecha Izquierda              |");
    mvprintw(startY++, centerX - 35, "    |      ===>  Flecha Derecha                |");
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(5));
    
    startY += 2;
    
    // JUGADOR 2 - Estilo neón alternativo
    attron(COLOR_PAIR(6) | A_BOLD);
    mvprintw(startY++, centerX - 35, "    +============================================+");
    mvprintw(startY++, centerX - 35, "    |  ########  JUGADOR 2  ########          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(6) | A_BOLD);
    
    attron(COLOR_PAIR(6));
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    |      [A]  Mover Izquierda                |");
    mvprintw(startY++, centerX - 35, "    |      [D]  Mover Derecha                  |");
    mvprintw(startY++, centerX - 35, "    |                                          |");
    mvprintw(startY++, centerX - 35, "    +============================================+");
    attroff(COLOR_PAIR(6));
    
    startY += 3;
    
    // Footer con animación
    attron(COLOR_PAIR(7) | A_BOLD);
    mvprintw(startY++, centerX - 32, "################################################################");
    attroff(COLOR_PAIR(7) | A_BOLD);
    
    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    mvprintw(startY++, centerX - 25, ">>> PRESIONA CUALQUIER TECLA PARA CONTINUAR <<<");
    attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    
    attron(COLOR_PAIR(7) | A_BOLD);
    mvprintw(startY, centerX - 32, "################################################################");
    attroff(COLOR_PAIR(7) | A_BOLD);
    
    refresh();
    getch();
}