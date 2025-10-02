#include "Game.h"
#include "../ui/Scoreboard.h"
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

Game::Game() : running(false), gameMode(SINGLE_PLAYER), 
               scoreboard(),
               renderer(),
               threadManager(this),
               gameSync(),
               ball(40, 20), 
               paddle1(35, 22, 9), 
               paddle2(35, 3, 9),
               prevPaddle1X(-1), prevPaddle2X(-1),
               prevBallX(-1), prevBallY(-1),
               ballStarted(false),
               gameOver(false),
               gameWon(false),
               currentLevel(1),
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
        "2. Dos jugadores",
        "3. Regresar al menu principal"
    };
    
    size_t selectedMode = 0;
    int ch;
    
    while (true) {
        clear();
        
        // Mostrar título del juego
        int centerX = COLS / 2;
        int startY = 2;
        mvprintw(startY++, centerX - 39, "##################################################################################");
        mvprintw(startY++, centerX - 39, "#       ____                             __                         __           #");
        mvprintw(startY++, centerX - 39, "#      /\\  _`\\                          /\\ \\                       /\\ \\__        #");
        mvprintw(startY++, centerX - 39, "#      \\ \\ \\L\\ \\   _ __     __      _   \\ \\ \\/\'\\      ___    __  __\\ \\ ,_\\       #");
        mvprintw(startY++, centerX - 39, "#       \\ \\  _ <\' /\\`\'__\\ /\'__`\\  /\'__`\\ \\ \\ , <     / __`\\ /\\ \\/\\ \\\\ \\ \\/       #");
        mvprintw(startY++, centerX - 39, "#        \\ \\ \\L\\ \\\\ \\ \\/ /\\  __/ /\\ \\L\\.\\ \\ \\ \\\\`\\  /\\ \\L\\ \\\\ \\ \\_\\ \\\\ \\ \\_      #");
        mvprintw(startY++, centerX - 39, "#         \\ \\____/ \\ \\_\\ \\ \\____\\\\ \\__/.\\_\\\\ \\_\\ \\_\\\\ \\____/ \\ \\____/ \\ \\__\\     #");
        mvprintw(startY++, centerX - 39, "#          \\/___/   \\/_/  \\/____/ \\/__/\\/_/ \\/_/\\/_/ \\/___/   \\/___/   \\/__/     #");
        mvprintw(startY++, centerX - 39, "##################################################################################");
        
        mvprintw(startY + 2, centerX - 12, "SELECCIONA MODO DE JUEGO");
        
        for (size_t i = 0; i < modeOptions.size(); i++) {
            if (i == selectedMode) {
                mvprintw(static_cast<int>(startY + 4 + i), centerX - 12, "> %s", modeOptions[i].c_str());
            } else {
                mvprintw(static_cast<int>(startY + 4 + i), centerX - 10, "%s", modeOptions[i].c_str());
            }
        }
        
        mvprintw(startY + 8, centerX - 25, "Usa flechas o W/S para navegar, ENTER para seleccionar");
        refresh();
        
        ch = getch();
        if ((ch == 'w' || ch == 'W' || ch == KEY_UP) && selectedMode > 0) {
            selectedMode--;
        }
        if ((ch == 's' || ch == 'S' || ch == KEY_DOWN) && selectedMode < modeOptions.size() - 1) {
            selectedMode++;
        }
        if (ch == '\n' || ch == '\r') {
            if (selectedMode == 2) {
                // Regresar al menú principal
                return;
            } else {
                gameMode = (selectedMode == 0) ? SINGLE_PLAYER : TWO_PLAYER;
                break;
            }
        }
        if (ch == 27) {
            return;
        }
    }
}

void Game::init() {
    selectGameMode();
    
    if (gameMode == SINGLE_PLAYER) {
        promptPlayerName();
    } else if (gameMode == TWO_PLAYER) {
        promptTwoPlayerNames();
    }
    
    clear();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
    
    initBlocks();
    
    // Configurar velocidad de la pelota
    ball.setMoveSpeed(120);
    
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
    initBlocksForLevel(currentLevel);
}

void Game::initBlocksForLevel(int level) {
    blockMatrix.clear();
    blockMatrix.resize(10, std::vector<bool>(10, false));
    
    switch(level) {
        case 1:
            for(int i = 2; i < 8; i++) {
                for(int j = 0; j < 10; j++) {
                    blockMatrix[i][j] = true;
                }
            }
            break;
            
        case 2:
            for(int i = 1; i < 9; i++) {
                blockMatrix[i][4] = true;
                blockMatrix[i][5] = true;
            }
            for(int j = 2; j < 8; j++) {
                blockMatrix[1][j] = true;
                blockMatrix[8][j] = true;
            }
            break;
            
        case 3:
            for(int i = 1; i < 7; i++) {
                blockMatrix[i][6] = true;
                blockMatrix[i][7] = true;
            }
            for(int j = 4; j < 8; j++) {
                blockMatrix[7][j] = true;
                blockMatrix[8][j] = true;
            }
            blockMatrix[6][3] = true;
            blockMatrix[7][3] = true;
            break;
            
        case 4:
            for(int i = 1; i < 9; i++) {
                blockMatrix[i][2] = true;
                blockMatrix[i][3] = true;
            }
            for(int j = 2; j < 7; j++) {
                blockMatrix[1][j] = true;
                blockMatrix[2][j] = true;
                blockMatrix[4][j] = true;
                blockMatrix[5][j] = true;
            }
            blockMatrix[2][6] = true;
            blockMatrix[3][6] = true;
            blockMatrix[4][6] = true;
            break;
            
        case 5:
            for(int i = 1; i < 9; i++) {
                blockMatrix[i][2] = true;
                blockMatrix[i][3] = true;
            }
            for(int j = 2; j < 6; j++) {
                blockMatrix[1][j] = true;
                blockMatrix[2][j] = true;
                blockMatrix[7][j] = true;
                blockMatrix[8][j] = true;
            }
            blockMatrix[2][6] = true;
            blockMatrix[3][7] = true;
            blockMatrix[4][7] = true;
            blockMatrix[5][7] = true;
            blockMatrix[6][7] = true;
            blockMatrix[7][6] = true;
            break;
    }
}


void Game::handleInput() {
    int ch;
    //  mantener el estado para movimiento continuo
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
    
    // Física de la pelota
    updateBallPhysics();
}

void Game::resetBall() {
    
    ball.setPosition(paddle1.getX() + paddle1.getWidth()/2, paddle1.getY() - 1);
    ball.setVelocity(0, -1);
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
    
    // ASCII Art de GAME OVER centrado
    mvprintw(centerY - 7, centerX - 14, "  ####    ##   #    # ###### ");
    mvprintw(centerY - 6, centerX - 14, " #    #  #  #  ##  ## #      ");
    mvprintw(centerY - 5, centerX - 14, " #      #    # # ## # #####  ");
    mvprintw(centerY - 4, centerX - 14, " #  ### ###### #    # #      ");
    mvprintw(centerY - 3, centerX - 14, " #    # #    # #    # #      ");
    mvprintw(centerY - 2, centerX - 14, "  ####  #    # #    # ###### ");
    
    mvprintw(centerY, centerX - 14, "  ####  #    # ###### #####  ");
    mvprintw(centerY + 1, centerX - 14, " #    # #    # #      #    # ");
    mvprintw(centerY + 2, centerX - 14, " #    # #    # #####  #####  ");
    mvprintw(centerY + 3, centerX - 14, " #    # #    # #      #   #  ");
    mvprintw(centerY + 4, centerX - 14, " #    #  #  #  #      #    # ");
    mvprintw(centerY + 5, centerX - 14, "  ####    ##   ###### #    # ");
    
    mvprintw(centerY + 7, centerX - 10, "Puntuación final: %d", scoreboard.getScore());
    mvprintw(centerY + 9, centerX - 12, "Presiona ENTER para continuar");
    
    // Guardar puntaje en CSV
    if (!playerName.empty()) {
        std::ofstream csv("scores.csv", std::ios::app);
        if (csv.is_open()) {
            if (gameMode == SINGLE_PLAYER) {
                csv << '"' << playerName << '"' << "," << scoreboard.getScore() << ",single\n";
            } else if (gameMode == TWO_PLAYER && !player2Name.empty()) {
                csv << '"' << playerName << '"' << "," << '"' << player2Name << '"' << "," << scoreboard.getScore() << ",multi\n";
            }
        }
    }
    
    refresh();
    
    // Esperar Enter
    nodelay(stdscr, FALSE); 
    int ch;
    do {
        ch = getch();
    } while (ch != '\n' && ch != '\r');
    nodelay(stdscr, TRUE);
}

void Game::showGameWon() {
    cleanup();
    clear();
    
    int centerY = LINES / 2;
    int centerX = COLS / 2;
    
    // ASCII Art "YOU WIN"
    mvprintw(centerY - 8, centerX - 23, "##   ##  #####  ##   ##    ##   ## #### ##   ##");
    mvprintw(centerY - 7, centerX - 23, " ## ##   ##  ## ##   ##    ##   ##  ##  ###  ##");
    mvprintw(centerY - 6, centerX - 23, "  ###    ##  ## ##   ##    ## # ##  ##  ## # ##");
    mvprintw(centerY - 5, centerX - 23, "   ##    ##  ## ##   ##    #######  ##  ##  ###");
    mvprintw(centerY - 4, centerX - 23, "   ##    ##  ## ##   ##    ### ###  ##  ##   ##");
    mvprintw(centerY - 3, centerX - 23, "   ##    #####   #####     ##   ## #### ##   ##");
    
    mvprintw(centerY - 1, centerX - 17, "¡¡¡FELICIDADES!!!");
    mvprintw(centerY, centerX - 17, "¡HAS COMPLETADO TODOS LOS NIVELES!");
    mvprintw(centerY + 2, centerX - 10, "Puntuación final: %d", scoreboard.getScore());
    mvprintw(centerY + 4, centerX - 12, "Presiona cualquier tecla");
    mvprintw(centerY + 5, centerX - 12, "para volver al menú");
    
    // Guardar puntaje en CSV
    if (!playerName.empty()) {
        std::ofstream csv("scores.csv", std::ios::app);
        if (csv.is_open()) {
            if (gameMode == SINGLE_PLAYER) {
                csv << '"' << playerName << '"' << "," << scoreboard.getScore() << ",single\n";
            } else if (gameMode == TWO_PLAYER && !player2Name.empty()) {
                csv << '"' << playerName << '"' << "," << '"' << player2Name << '"' << "," << scoreboard.getScore() << ",multi\n";
            }
        }
    }
    
    refresh();
    
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
}

void Game::updateBallPhysics() {
    if (!ballStarted) {
        // Posición de la pelota según el paddle
        int newBallX = paddle1.getX() + paddle1.getWidth()/2;
        int newBallY = paddle1.getY() - 1;
        
        // Solo marcar como dirty si la posición cambió
        if (ball.getX() != newBallX || ball.getY() != newBallY) {
            ball.setPosition(newBallX, newBallY);
            gameSync.ballRegion.markDirty();
        }
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
        newX = 2;
    } else if (newX >= COLS - 2) {
        ball.reverseX();
        newX = COLS - 3;
    }
    
    // Colisión con pared superior (solo en modo 1 jugador)
    if (gameMode == SINGLE_PLAYER && newY <= 1) {
        ball.reverseY();
        newY = 2;
    }
    
    // Colisión con pared superior en modo 2 jugadores = game over
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
        // Dividir el paddle en 5 secciones
        int paddleLeft = paddle1.getX();
        int paddleWidth = paddle1.getWidth();
        int ballX = ball.getX();
        
        // Calcular posición relativa en el paddle (0.0 = izquierda, 1.0 = derecha)
        float relativePos = static_cast<float>(ballX - paddleLeft) / static_cast<float>(paddleWidth);
        
        // Mapear a velocidades: extremos dan ángulo más pronunciado
        int newVelX;
        if (relativePos < 0.2f) {
            // Extremo izquierdo - rebote fuerte a la izquierda
            newVelX = -2;
        } else if (relativePos < 0.4f) {
            // Izquierda moderada
            newVelX = -1;
        } else if (relativePos < 0.6f) {
            // Centro - rebote recto
            newVelX = 0;
        } else if (relativePos < 0.8f) {
            // Derecha moderada
            newVelX = 1;
        } else {
            // Extremo derecho - rebote fuerte a la derecha
            newVelX = 2;
        }
        
        ball.setVelocity(newVelX, ball.getVelocityY());
        
        gameSync.paddle1Region.markDirty();
        gameSync.ballRegion.markDirty();
        return;
    }
    
    // Verificar colisión con paddle2 (superior) en modo 2 jugadores
    if (gameMode == TWO_PLAYER && checkPaddleCollision(paddle2)) {
        ball.reverseY();
        
        // Mover la pelota una posición abajo para evitar que se superponga con el paddle
        int newY = paddle2.getY() + 1;
        ball.setPosition(ball.getX(), newY);
        
        // Ajustar dirección horizontal según posición en paddle2
        // Dividir el paddle en 5 secciones
        int paddleLeft = paddle2.getX();
        int paddleWidth = paddle2.getWidth();
        int ballX = ball.getX();
        
        // Calcular posición relativa en el paddle (0.0 = izquierda, 1.0 = derecha)
        float relativePos = static_cast<float>(ballX - paddleLeft) / static_cast<float>(paddleWidth);
        
        // Mapear a velocidades: extremos dan ángulo más pronunciado
        int newVelX;
        if (relativePos < 0.2f) {
            // Extremo izquierdo - rebote fuerte a la izquierda
            newVelX = -2;
        } else if (relativePos < 0.4f) {
            // Izquierda moderada
            newVelX = -1;
        } else if (relativePos < 0.6f) {
            // Centro - rebote recto
            newVelX = 0;
        } else if (relativePos < 0.8f) {
            // Derecha moderada
            newVelX = 1;
        } else {
            // Extremo derecho - rebote fuerte a la derecha
            newVelX = 2;
        }
        
        ball.setVelocity(newVelX, ball.getVelocityY());
        
        gameSync.paddle2Region.markDirty();
        gameSync.ballRegion.markDirty();
        return;
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
    
    // Solo colisionar si la pelota hacia el paddle
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
            int blockY = 5 + row;
            
            // Verificar colisión con el bloque
            if (ballY == blockY && ballX >= blockX && ballX < blockX + blockWidth) {
                
                // Destruir el bloque
                blockMatrix[row][col] = false;
                renderer.clearBlock(blockX, blockY);
                
                // Añadir puntos según la fila
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
                
                // Verificar si se ganó el nivel
                if (checkWinCondition()) {
                    if (currentLevel < MAX_LEVELS) {
                        nextLevel();
                    } else {
                        gameWon = true;
                        running = false;
                        gameSync.gameRunning = false;
                    }
                }
                
                return; // Solo una colisión por frame
            }
        }
    }
}

bool Game::checkWinCondition() {
    for (const auto& row : blockMatrix) {
        for (bool block : row) {
            if (block) return false;
        }
    }
    return true;
}

void Game::nextLevel() {
    if (currentLevel < MAX_LEVELS) {
        currentLevel++;
        initBlocksForLevel(currentLevel);
        
        // Reiniciar posición de la pelota
        ballStarted = false;
        ball.setPosition(paddle1.getX() + paddle1.getWidth()/2, paddle1.getY() - 1);
        ball.setVelocity(0, 0);
        
        // Velocidad por nivel
        int newSpeed = 120 - (currentLevel - 1) * 10;
        if (newSpeed < 15) newSpeed = 15;
        ball.setMoveSpeed(newSpeed);
        
        // Mostrar mensaje de nivel
        clear();
        int centerY = LINES / 2;
        int centerX = COLS / 2;
        mvprintw(centerY, centerX - 10, "¡NIVEL %d COMPLETADO!", currentLevel - 1);
        mvprintw(centerY + 1, centerX - 8, "Iniciando nivel %d...", currentLevel);
        mvprintw(centerY + 2, centerX - 8, "¡Velocidad aumentada!");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    } else {
        gameWon = true;
        running = false;
        gameSync.gameRunning = false;
    }
}


void Game::run() {
    init();
    
    // Calcular posición centrada para bloques
    const int blockCols = 10;
    const int blockWidth = 3;
    int startXBlocks = (COLS - blockCols * blockWidth) / 2;
    
    // Dibujar elementos estáticos iniciales
    renderer.renderBlocks(blockMatrix, startXBlocks, 5);
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

void Game::promptPlayerName() {
    // Interfaz simple para capturar el nombre usando ncurses
    echo();
    curs_set(1);
    nodelay(stdscr, FALSE);
    clear();
    mvprintw(LINES/2 - 1, (COLS/2) - 12, "Ingresa tu nombre:");
    mvprintw(LINES/2 + 1, (COLS/2) - 20, "(ENTER para confirmar, máx 16 chars)");
    refresh();

    char buffer[64];
    int maxLen = 16;
    int idx = 0;
    int ch;
    int inputY = LINES/2;
    int inputX = (COLS/2) - 12;
    move(inputY, inputX);
    while ((ch = getch()) != '\n' && ch != '\r') {
        if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (idx > 0) {
                idx--;
                buffer[idx] = '\0';
                mvaddch(inputY, inputX + idx, ' ');
                move(inputY, inputX + idx);
                refresh();
            }
            continue;
        }
        if (idx < maxLen && ch >= 32 && ch <= 126) {
            buffer[idx++] = static_cast<char>(ch);
            buffer[idx] = '\0';
            mvaddch(inputY, inputX + idx - 1, ch);
            refresh();
        }
    }
    buffer[idx] = '\0';
    playerName = std::string(buffer);
    if (playerName.empty()) {
        playerName = "Jugador";
    }
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
}

void Game::promptTwoPlayerNames() {
    // nombre del primer jugador
    echo();
    curs_set(1);
    nodelay(stdscr, FALSE);
    clear();
    mvprintw(LINES/2 - 2, (COLS/2) - 15, "Jugador 1 - Ingresa tu nombre:");
    mvprintw(LINES/2, (COLS/2) - 20, "(ENTER para confirmar, máx 16 chars)");
    refresh();

    char buffer[64];
    int maxLen = 16;
    int idx = 0;
    int ch;
    int inputY = LINES/2 - 1;
    int inputX = (COLS/2) - 12;
    move(inputY, inputX);
    while ((ch = getch()) != '\n' && ch != '\r') {
        if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (idx > 0) {
                idx--;
                buffer[idx] = '\0';
                mvaddch(inputY, inputX + idx, ' ');
                move(inputY, inputX + idx);
                refresh();
            }
            continue;
        }
        if (idx < maxLen && ch >= 32 && ch <= 126) {
            buffer[idx++] = static_cast<char>(ch);
            buffer[idx] = '\0';
            mvaddch(inputY, inputX + idx - 1, ch);
            refresh();
        }
    }
    buffer[idx] = '\0';
    playerName = std::string(buffer);
    if (playerName.empty()) {
        playerName = "Jugador1";
    }

    // nombre del segundo jugador
    clear();
    mvprintw(LINES/2 - 2, (COLS/2) - 15, "Jugador 2 - Ingresa tu nombre:");
    mvprintw(LINES/2, (COLS/2) - 20, "(ENTER para confirmar, máx 16 chars)");
    refresh();

    idx = 0;
    move(inputY, inputX);
    while ((ch = getch()) != '\n' && ch != '\r') {
        if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (idx > 0) {
                idx--;
                buffer[idx] = '\0';
                mvaddch(inputY, inputX + idx, ' ');
                move(inputY, inputX + idx);
                refresh();
            }
            continue;
        }
        if (idx < maxLen && ch >= 32 && ch <= 126) {
            buffer[idx++] = static_cast<char>(ch);
            buffer[idx] = '\0';
            mvaddch(inputY, inputX + idx - 1, ch);
            refresh();
        }
    }
    buffer[idx] = '\0';
    player2Name = std::string(buffer);
    if (player2Name.empty()) {
        player2Name = "Jugador2";
    }

    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
}
