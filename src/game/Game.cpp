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
    blockMatrix.resize(5, std::vector<bool>(10, true));
}


void Game::handleInput() {
    int ch;
    leftPressed = false;
    rightPressed = false;
    aPressed = false;
    dPressed = false;
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
                    ballStarted = true; /
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
    (void)paddle1Moved;
    (void)paddle2Moved;
    
}

void Game::resetKeyStates() {
    leftPressed = false;
    rightPressed = false;
    aPressed = false;
    dPressed = false;
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

void Game::updateBallPhysics() {
    if (!ballStarted) {
       
        ball.setPosition(paddle1.getX() + paddle1.getWidth()/2, paddle1.getY() - 1);
        gameSync.ballRegion.markDirty();
        return;
    }
    
    // Mover la pelota
    int currentX = ball.getX();
    int currentY = ball.getY();
    int newX = currentX + ball.getVelocityX();
    int newY = currentY + ball.getVelocityY();
    
    // Colisión con paredes laterales
    if (newX <= 0 || newX >= COLS - 1) {
        ball.reverseX();
        newX = ball.getX(); // Mantener posición X actual
    }
    
    // Colisión con pared superior
    if (newY <= 0) {
        ball.reverseY();
        newY = ball.getY(); 
    }
    
    
    if (newY >= LINES - 1) {
        if (gameMode == SINGLE_PLAYER) {
            
            gameOver = true;
            running = false;
            gameSync.gameRunning = false;
            return;
        } else {
            ball.reverseY();
            return;
        }
    }
    
    updateBallPosition(newX, newY);
    checkCollisions();
}


void Game::checkCollisions() {
  
    if (checkPaddleCollision(paddle1)) {
        ball.reverseY();
        
     
        int paddleCenter = paddle1.getX() + paddle1.getWidth() / 2;
        int ballOffset = ball.getX() - paddleCenter;
        
        if (ballOffset < -1) {
            ball.setVelocity(-1, ball.getVelocityY());
        } else if (ballOffset > 1) {
            ball.setVelocity(1, ball.getVelocityY());
        } else {
            ball.setVelocity(0, ball.getVelocityY());
        }
    }
    

    if (gameMode == TWO_PLAYER && checkPaddleCollision(paddle2)) {
        ball.reverseY();
    }

    checkBlockCollisions();
}

bool Game::checkPaddleCollision(const Paddle& paddle) {
    return (ball.getY() >= paddle.getY() - 1 && ball.getY() <= paddle.getY() + 1) &&
           ball.getX() >= paddle.getX() && 
           ball.getX() < paddle.getX() + paddle.getWidth();
}

void Game::checkBlockCollisions() {
    const int blockCols = 10;
    const int blockWidth = 3;
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < blockCols; col++) {
            if (!blockMatrix[row][col]) continue; 
            
            int blockX = startXBlocks + col * blockWidth;
            int blockY = 3 + row;
            
            

            // Verificar colisión
            if (ball.getY() == blockY && 
                ball.getX() >= blockX && ball.getX() < blockX + blockWidth) {
                
                blockMatrix[row][col] = false;
                renderer.clearBlock(blockX, blockY);
                scoreboard.addPoints(10);
                ball.reverseY();
                gameSync.scoreRegion.markDirty();
                
                return; 
            }
            
            
        }
    }
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
    
    mvprintw(ball.getY(), ball.getX(), "O");

    // Mostrar controles
    if (gameMode == SINGLE_PLAYER) {
        mvprintw(LINES-1, 2, "Flechas/A-D para mover | P: lanzar pelota | ESPACIO: +puntos | Q: salir");
    } else {
        mvprintw(LINES-1, 2, "J1: Flechas | J2: A-D | P: lanzar pelota | ESPACIO: +puntos | Q: salir");
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
    
    // Loop principal del juego
    while (running && gameSync.gameRunning) {
        if (prevBallX != -1 && prevBallY != -1) {
            mvprintw(prevBallY, prevBallX, " ");
        }
        updateBallPhysics();

        mvprintw(ball.getY(), ball.getX(), "O");
        refresh();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    if (gameOver) {
        showGameOver();
    } else {
        cleanup();
    }
}
