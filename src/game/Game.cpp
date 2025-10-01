#include "Game.h"
#include "../ui/Scoreboard.h"
#include <ncurses.h>
#include <thread>
#include <chrono>

Game::Game() : running(false), gameMode(SINGLE_PLAYER), 
               scoreboard(),
               renderer(),
               threadManager(this),
               gameSync(),
               ball(40, 20), 
               paddle1(35, 22, 7), 
               paddle2(35, 1, 7),
               prevPaddle1X(-1), prevPaddle2X(-1),
               prevBallX(-1), prevBallY(-1),
               ballStarted(false),
               gameOver(false),
               gameWon(false),
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
    const int blockRows = 8;
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    gameSync.blocksRegion.setRegion(startXBlocks, 3, startXBlocks + blockCols * blockWidth, 3 + blockRows);
    
    // Ajustar posiciones de paddles según el modo
    paddle1.setPosition((COLS - paddle1.getWidth()) / 2, LINES - 3);
    if (gameMode == TWO_PLAYER) {
        paddle2.setPosition((COLS - paddle2.getWidth()) / 2, 1);
    }
    
    resetBall();

    running = true;
    gameSync.gameRunning = true;
}


void Game::cleanup() {
    threadManager.stopThreads();
    nodelay(stdscr, FALSE);
}

void Game::initBlocks() {
    blockMatrix.clear();
    // Aumentar a 8 filas de bloques para más desafío
    blockMatrix.resize(8, std::vector<bool>(10, true));
}


void Game::handleInput() {
    int ch;
    // No resetear las teclas aquí - mantener el estado para movimiento continuo
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
            case 'p':
            case 'P':
                if (!ballStarted) {
                    ballStarted = true;
                    gameSync.ballRegion.markDirty();
                }
                break;
            case ' ':
                scoreboard.addPoints(100); 
                gameSync.scoreRegion.markDirty();
                break;
            case 'q':
            case 27:
                running = false;
                gameSync.gameRunning = false;
                break;
        }
    }
}

void Game::processKeyStates() {
    // No permitir movimiento de paddles hasta que se lance la pelota
    if (!ballStarted) {
        return;
    }
    
    bool paddle1Moved = false;
    bool paddle2Moved = false;
    
    if (gameMode == SINGLE_PLAYER) {
        // En modo 1 jugador, flechas y A/D controlan el mismo paddle
        if (leftPressed.load() || aPressed.load()) {
            if (paddle1.tryMoveLeft()) {
                gameSync.paddle1Region.markDirty();
                paddle1Moved = true;
            }
        }
        if (rightPressed.load() || dPressed.load()) {
            if (paddle1.tryMoveRight()) {
                gameSync.paddle1Region.markDirty();
                paddle1Moved = true;
            }
        }
    } else {
        // En modo 2 jugadores, controles independientes y simultáneos
        if (leftPressed.load()) {
            if (paddle1.tryMoveLeft()) {
                gameSync.paddle1Region.markDirty();
                paddle1Moved = true;
            }
        }
        if (rightPressed.load()) {
            if (paddle1.tryMoveRight()) {
                gameSync.paddle1Region.markDirty();
                paddle1Moved = true;
            }
        }
        if (aPressed.load()) {
            if (paddle2.tryMoveLeft()) {
                gameSync.paddle2Region.markDirty();
                paddle2Moved = true;
            }
        }
        if (dPressed.load()) {
            if (paddle2.tryMoveRight()) {
                gameSync.paddle2Region.markDirty();
                paddle2Moved = true;
            }
        }
    }
    (void)paddle1Moved;
    (void)paddle2Moved;
    
}

void Game::resetKeyStates() {
    leftPressed = false;
    rightPressed = false;
    aPressed = false;
    dPressed = false;
}

void Game::updateGameLogic() {
    if (!running || !gameSync.gameRunning || gameOver || gameWon) {
        return;
    }
    
    // Actualizar física de la pelota
    updateBallPhysics();
}

void Game::resetBall() {
    
    ball.setPosition(paddle1.getX() + paddle1.getWidth()/2, paddle1.getY() - 1);
    ball.setVelocity(1, -1);
    ballStarted = false;
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

void Game::showGameOver() {
    cleanup();
    clear();
    
    int centerY = LINES / 2;
    int centerX = COLS / 2;
    
    mvprintw(centerY - 2, centerX - 10, "¡GAME OVER!");
    mvprintw(centerY - 1, centerX - 15, "Puntuación final: %d", scoreboard.getScore());
    mvprintw(centerY + 1, centerX - 12, "Presiona cualquier tecla");
    mvprintw(centerY + 2, centerX - 12, "para volver al menú");
    
    refresh();
 
    nodelay(stdscr, FALSE); 
    getch();
    nodelay(stdscr, TRUE);
}

void Game::showGameWon() {
    cleanup();
    clear();
    
    int centerY = LINES / 2;
    int centerX = COLS / 2;
    
    mvprintw(centerY - 3, centerX - 15, "¡¡¡FELICIDADES!!!");
    mvprintw(centerY - 2, centerX - 15, "¡HAS GANADO EL JUEGO!");
    mvprintw(centerY, centerX - 15, "Puntuación final: %d", scoreboard.getScore());
    mvprintw(centerY + 2, centerX - 12, "Presiona cualquier tecla");
    mvprintw(centerY + 3, centerX - 12, "para volver al menú");
    
    refresh();
    
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
}

void Game::updateBallPhysics() {
    if (!ballStarted) {
        ball.setPosition(paddle1.getX() + paddle1.getWidth()/2, paddle1.getY() - 1);
        gameSync.ballRegion.markDirty();
        return;
    }
    
    // Solo mover si ha pasado suficiente tiempo
    if (!ball.shouldMove()) {
        return;
    }
    
    // Resetear el timer y mover la pelota
    ball.resetMoveTimer();
    
    // Mover la pelota
    int currentX = ball.getX();
    int currentY = ball.getY();
    int newX = currentX + ball.getVelocityX();
    int newY = currentY + ball.getVelocityY();
    
    // Colisión con paredes laterales
    if (newX <= 1) {
        ball.reverseX();
        newX = 2; // Asegurar que no se quede en el borde
    } else if (newX >= COLS - 2) {
        ball.reverseX();
        newX = COLS - 3; // Asegurar que no se quede en el borde
    }
    
    // Colisión con pared superior (solo en modo 1 jugador)
    if (gameMode == SINGLE_PLAYER && newY <= 1) {
        ball.reverseY();
        newY = 2; // Asegurar que no se quede en el borde
    }
    
    // Colisión con pared superior en modo 2 jugadores = pierde jugador 2
    if (gameMode == TWO_PLAYER && newY <= 1) {
        gameOver = true;
        running = false;
        gameSync.gameRunning = false;
        return;
    }
    
    // Colisión con pared inferior = pierde jugador 1
    if (newY >= LINES - 2) {
        gameOver = true;
        running = false;
        gameSync.gameRunning = false;
        return;
    }
    
    updateBallPosition(newX, newY);
    checkCollisions();
}


void Game::checkCollisions() {
    // Verificar colisión con paddle1 (inferior)
    if (checkPaddleCollision(paddle1)) {
        ball.reverseY();
        
        // Mover la pelota una posición arriba para evitar que se superponga con el paddle
        int newY = paddle1.getY() - 1;
        ball.setPosition(ball.getX(), newY);
        
        // Ajustar dirección horizontal según posición en paddle
        int paddleCenter = paddle1.getX() + paddle1.getWidth() / 2;
        int ballOffset = ball.getX() - paddleCenter;
        
        if (ballOffset < -1) {
            ball.setVelocity(-1, ball.getVelocityY());
        } else if (ballOffset > 1) {
            ball.setVelocity(1, ball.getVelocityY());
        } else {
            ball.setVelocity(0, ball.getVelocityY());
        }
        
        gameSync.paddle1Region.markDirty(); // Redibujar paddle
        gameSync.ballRegion.markDirty();
        return; // Evitar múltiples colisiones en el mismo frame
    }
    
    // Verificar colisión con paddle2 (superior) en modo 2 jugadores
    if (gameMode == TWO_PLAYER && checkPaddleCollision(paddle2)) {
        ball.reverseY();
        
        // Mover la pelota una posición abajo para evitar que se superponga con el paddle
        int newY = paddle2.getY() + 1;
        ball.setPosition(ball.getX(), newY);
        
        // Ajustar dirección horizontal según posición en paddle2
        int paddleCenter = paddle2.getX() + paddle2.getWidth() / 2;
        int ballOffset = ball.getX() - paddleCenter;
        
        if (ballOffset < -1) {
            ball.setVelocity(-1, ball.getVelocityY());
        } else if (ballOffset > 1) {
            ball.setVelocity(1, ball.getVelocityY());
        } else {
            ball.setVelocity(0, ball.getVelocityY());
        }
        
        gameSync.paddle2Region.markDirty(); // Redibujar paddle
        gameSync.ballRegion.markDirty();
        return; // Evitar múltiples colisiones en el mismo frame
    }

    checkBlockCollisions();
}

bool Game::checkPaddleCollision(const Paddle& paddle) {
    int ballY = ball.getY();
    int ballX = ball.getX();
    int paddleY = paddle.getY();
    int paddleX = paddle.getX();
    int paddleWidth = paddle.getWidth();
    int ballVelY = ball.getVelocityY();
    
    // Verificar si la pelota está en la misma línea que el paddle
    bool sameRow = (ballY == paddleY);
    // Verificar si la pelota está dentro del rango horizontal del paddle
    bool inRange = (ballX >= paddleX && ballX < paddleX + paddleWidth);
    
    // Solo colisionar si la pelota se está moviendo hacia el paddle
    // Para paddle1 (inferior): la pelota debe estar moviéndose hacia abajo (velocityY > 0)
    // Para paddle2 (superior): la pelota debe estar moviéndose hacia arriba (velocityY < 0)
    bool correctDirection = false;
    if (paddleY > LINES / 2) {
        // Paddle inferior (paddle1)
        correctDirection = (ballVelY > 0);
    } else {
        // Paddle superior (paddle2)
        correctDirection = (ballVelY < 0);
    }
    
    return sameRow && inRange && correctDirection;
}

void Game::checkBlockCollisions() {
    const int blockCols = 10;
    const int blockWidth = 3;
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    
    int ballX = ball.getX();
    int ballY = ball.getY();
    int ballVelY = ball.getVelocityY();
    
    for (int row = 0; row < static_cast<int>(blockMatrix.size()); row++) {
        for (int col = 0; col < blockCols; col++) {
            if (!blockMatrix[row][col]) continue; 
            
            int blockX = startXBlocks + col * blockWidth;
            int blockY = 3 + row;
            
            // Verificar colisión con el bloque
            if (ballY == blockY && ballX >= blockX && ballX < blockX + blockWidth) {
                
                // Destruir el bloque
                blockMatrix[row][col] = false;
                renderer.clearBlock(blockX, blockY);
                
                // Añadir puntos según la fila (como en el original)
                // Filas superiores dan más puntos
                int points = 10 * (8 - row);
                scoreboard.addPoints(points);
                
                // Rebotar la pelota
                ball.reverseY();
                
                // Mover la pelota fuera del bloque para evitar colisiones múltiples
                if (ballVelY > 0) {
                    ball.setPosition(ballX, blockY - 1);
                } else {
                    ball.setPosition(ballX, blockY + 1);
                }
                
                gameSync.scoreRegion.markDirty();
                gameSync.blocksRegion.markDirty();
                gameSync.ballRegion.markDirty();
                
                // Verificar si se ganó el juego
                if (checkWinCondition()) {
                    gameWon = true;
                    running = false;
                    gameSync.gameRunning = false;
                }
                
                return; // Solo una colisión por frame
            }
        }
    }
}

bool Game::checkWinCondition() {
    for (const auto& row : blockMatrix) {
        for (bool block : row) {
            if (block) return false; // Aún hay bloques
        }
    }
    return true; // Todos los bloques destruidos
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
        mvprintw(LINES-1, 2, "P: LANZAR PELOTA | Flechas/A-D: mover (despues de lanzar) | Q: salir");
    } else {
        mvprintw(LINES-1, 2, "P: LANZAR | J1: Flechas | J2: A-D (despues de lanzar) | Q: salir");
    }
    
    // Renderizado inicial de paddles
    gameSync.paddle1Region.markDirty();
    gameSync.ballRegion.markDirty(); 
    if (gameMode == TWO_PLAYER) {
        gameSync.paddle2Region.markDirty();
    }
    
    refresh();
    
    // Iniciar hilos
    threadManager.startThreads();
    
    // Loop principal del juego - los hilos manejan la lógica y renderizado
    while (running && gameSync.gameRunning && !gameOver && !gameWon) {
        // Solo esperar y verificar estado del juego
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Detener hilos antes de mostrar mensajes finales
    threadManager.stopThreads();
    
    if (gameWon) {
        showGameWon();
    } else if (gameOver) {
        showGameOver();
    } else {
        cleanup();
    }
}
