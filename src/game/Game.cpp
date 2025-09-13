#include "Game.h"
#include "ui/Scoreboard.h"
#include <ncurses.h>
#include <thread>
#include <chrono>

Game::Game() {
    running = true;
    scoreboard = Scoreboard();
}

void Game::run() {
    // Paddle
    const int paddleWidth = 7;
    int paddleY = LINES - 3;
    int paddleX = (COLS - paddleWidth) / 2;

    // Bloques
    const int blockRows = 5;
    const int blockCols = 10;
    const int blockWidth = 3; // cada bloque ocupa 3 columnas
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    char blocks[blockRows][blockCols];

    for(int i=0;i<blockRows;i++)
        for(int j=0;j<blockCols;j++)
            blocks[i][j] = 1; // bloque visible

    // Configuración ncurses
    clear();
    nodelay(stdscr, TRUE);  // getch no bloquea
    keypad(stdscr, TRUE);   // habilita flechas
    curs_set(0);
    noecho();

    // Dibujar borde y bloques solo una vez
    box(stdscr, 0, 0);
    for(int i=0;i<blockRows;i++)
        for(int j=0;j<blockCols;j++)
            if(blocks[i][j] == 1)
                mvaddch(2+i, startXBlocks + j*blockWidth, '['),
                mvaddch(2+i, startXBlocks + j*blockWidth + 1, '#'),
                mvaddch(2+i, startXBlocks + j*blockWidth + 2, ']');

    // Mostrar puntaje
    scoreboard.render(0, 2);
    mvprintw(LINES-2, 2, "Flechas para mover, ESPACIO para puntos, q para salir");
    refresh();

    while (running) {
        int ch = getch();

        // Borrar paddle anterior
        for(int i=0;i<paddleWidth;i++)
            mvaddch(paddleY, paddleX + i, ' ');

        // Mover paddle
        if (ch == KEY_LEFT && paddleX > 1) paddleX--;
        if (ch == KEY_RIGHT && paddleX < COLS - paddleWidth - 1) paddleX++;
        if (ch == 'q') running = false;
        if (ch == ' ') scoreboard.addPoints(100); // prueba

        // Dibujar paddle en nueva posición
        for(int i=0;i<paddleWidth;i++)
            mvaddch(paddleY, paddleX + i, '=');

        // Actualizar puntaje
        scoreboard.render(0, 2);

        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
