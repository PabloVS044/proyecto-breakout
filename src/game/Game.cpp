#include "Game.h"
#include "../ui/Scoreboard.h"
#include <ncurses.h>
#include <thread>
#include <chrono>

Game::Game() : running(false), gameMode(SINGLE_PLAYER), 
               ball(40, 20), paddle1(35, 22, 7), paddle2(35, 1, 7),
               threadManager(this), prevPaddle1X(-1), prevPaddle2X(-1),
               prevBallX(-1), prevBallY(-1),
               leftPressed(false), rightPressed(false), aPressed(false), dPressed(false) {
    renderer.setGameSync(&gameSync);
    threadManager.setGameSync(&gameSync);
}

Game::~Game() {
    cleanup();
}

void Game::selectGameMode() {
    clear();
    
    std::vector<std::string> modeOptions = {
        "1. Un jugador",
        "2. Dos jugadores"
    };
    
    size_t selectedMode = 0;
    int ch;
    
    while (true) {
        clear();
        mvprintw(8, 30, "SELECCIONA MODO DE JUEGO");
        
        for (size_t i = 0; i < modeOptions.size(); i++) {
            if (i == selectedMode) {
                mvprintw(static_cast<int>(10 + i), 28, "> %s", modeOptions[i].c_str());
            } else {
                mvprintw(static_cast<int>(10 + i), 30, "%s", modeOptions[i].c_str());
            }
        }
        
        mvprintw(13, 25, "Usa flechas o W/S para navegar, ENTER para seleccionar");
        refresh();
        
        ch = getch();
        if ((ch == 'w' || ch == 'W' || ch == KEY_UP) && selectedMode > 0) {
            selectedMode--;
        }
        if ((ch == 's' || ch == 'S' || ch == KEY_DOWN) && selectedMode < modeOptions.size() - 1) {
            selectedMode++;
        }
        if (ch == '\n' || ch == '\r' || ch == ' ') {
            gameMode = (selectedMode == 0) ? SINGLE_PLAYER : TWO_PLAYER;
            break;
        }
    }
}

void Game::init() {
    selectGameMode();
    
    clear();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
    
    initBlocks();
    renderer.initGameScreen();
    
    // Inicializar regiones de rendering usando setRegion
    gameSync.paddle1Region.setRegion(0, LINES - 3, COLS, LINES - 3);
    
    if (gameMode == TWO_PLAYER) {
        gameSync.paddle2Region.setRegion(0, 1, COLS, 1);
    }
    
    gameSync.ballRegion.setRegion(0, 0, COLS, LINES);
    gameSync.scoreRegion.setRegion(0, 0, COLS, 2);
    
    // Calcular región de bloques centrada
    const int blockCols = 10;
    const int blockWidth = 3;
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    gameSync.blocksRegion.setRegion(startXBlocks, 3, startXBlocks + blockCols * blockWidth, 8);
    
    // Ajustar posiciones de paddles según el modo
    paddle1.setPosition((COLS - paddle1.getWidth()) / 2, LINES - 3);
    if (gameMode == TWO_PLAYER) {
        paddle2.setPosition((COLS - paddle2.getWidth()) / 2, 1);
    }
    
    running = true;
    gameSync.gameRunning = true;
}

void Game::cleanup() {
    threadManager.stopThreads();
    nodelay(stdscr, FALSE);
}

void Game::initBlocks() {
    blockMatrix.clear();
    blockMatrix.resize(5, std::vector<bool>(10, true));
}

void Game::handleInput() {
    // Capturar múltiples teclas en buffer
    int ch;
    while ((ch = getch()) != ERR) {
        switch (ch) {
            case KEY_LEFT:
                leftPressed = true;
                break;
            case KEY_RIGHT:
                rightPressed = true;
                break;
            case 'a':
            case 'A':
                aPressed = true;
                break;
            case 'd':
            case 'D':
                dPressed = true;
                break;
            case 'q':
            case 27: // ESC
                running = false;
                gameSync.gameRunning = false;
                break;
            case ' ':
                scoreboard.addPoints(100);
                gameSync.scoreRegion.markDirty();
                break;
        }
    }
}

void Game::processKeyStates() {
    bool paddle1Moved = false;
    bool paddle2Moved = false;
    
    if (gameMode == SINGLE_PLAYER) {
        // En modo 1 jugador, flechas y A/D controlan el mismo paddle
        if (leftPressed.load() || aPressed.load()) {
            updatePaddle1Position(paddle1.getX() - 1);
            paddle1Moved = true;
        }
        if (rightPressed.load() || dPressed.load()) {
            updatePaddle1Position(paddle1.getX() + 1);
            paddle1Moved = true;
        }
    } else {
        // En modo 2 jugadores, controles independientes y simultáneos
        if (leftPressed.load()) {
            updatePaddle1Position(paddle1.getX() - 1);
            paddle1Moved = true;
        }
        if (rightPressed.load()) {
            updatePaddle1Position(paddle1.getX() + 1);
            paddle1Moved = true;
        }
        if (aPressed.load()) {
            updatePaddle2Position(paddle2.getX() - 1);
            paddle2Moved = true;
        }
        if (dPressed.load()) {
            updatePaddle2Position(paddle2.getX() + 1);
            paddle2Moved = true;
        }
    }
    
    // Resetear estados después de procesar (para permitir movimiento continuo)
    // No resetear inmediatamente para permitir movimiento fluido
}

void Game::resetKeyStates() {
    leftPressed = false;
    rightPressed = false;
    aPressed = false;
    dPressed = false;
}

void Game::updatePaddle1Position(int newX) {
    if (newX >= 1 && newX + paddle1.getWidth() < COLS - 1) {
        prevPaddle1X = paddle1.getX();
        paddle1.setPosition(newX, paddle1.getY());
        gameSync.paddle1Region.markDirty();
    }
}

void Game::updatePaddle2Position(int newX) {
    if (gameMode == TWO_PLAYER && newX >= 1 && newX + paddle2.getWidth() < COLS - 1) {
        prevPaddle2X = paddle2.getX();
        paddle2.setPosition(newX, paddle2.getY());
        gameSync.paddle2Region.markDirty();
    }
}

void Game::updateBallPosition(int newX, int newY) {
    prevBallX = ball.getX();
    prevBallY = ball.getY();
    ball.setPosition(newX, newY);
    gameSync.ballRegion.markDirty();
}

void Game::run() {
    init();
    
    // Calcular posición centrada para bloques
    const int blockCols = 10;
    const int blockWidth = 3;
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    
    // Dibujar elementos estáticos iniciales
    renderer.renderBlocks(blockMatrix, startXBlocks, 3);
    renderer.renderScore(scoreboard.getScore(), scoreboard.getHighScore());
    
    // Mostrar controles
    if (gameMode == SINGLE_PLAYER) {
        mvprintw(LINES-1, 2, "Flechas/A-D para mover | ESPACIO: +puntos | Q: salir");
    } else {
        mvprintw(LINES-1, 2, "J1: Flechas | J2: A-D | ESPACIO: +puntos | Q: salir");
    }
    
    // Renderizado inicial de paddles
    gameSync.paddle1Region.markDirty();
    if (gameMode == TWO_PLAYER) {
        gameSync.paddle2Region.markDirty();
    }
    
    refresh();
    
    // Iniciar hilos
    threadManager.startThreads();
    
    // Loop principal del juego
    while (running && gameSync.gameRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    cleanup();
}
