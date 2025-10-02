#ifndef GAME_H
#define GAME_H

#include "../ui/Scoreboard.h"
#include "../ui/Renderer.h"
#include "../threads/ThreadManager.h"
#include "../threads/SyncTools.h"
#include "Ball.h"
#include "Paddle.h"
#include "Block.h"
#include <vector>
#include <atomic>
#include <chrono>
#include <string>

enum GameMode {
    SINGLE_PLAYER = 1,
    TWO_PLAYER = 2
};

class Game {
private:
    std::atomic<bool> running;
    GameMode gameMode;
    std::string playerName;
    std::string player2Name;
    Scoreboard scoreboard;
    OptimizedRenderer renderer;
    ThreadManager threadManager;
    GameSync gameSync;
    
    Ball ball;
    Paddle paddle1;
    Paddle paddle2;
    
    std::vector<std::vector<bool>> blockMatrix;
    
    // Sistema de niveles
    int currentLevel;
    static const int MAX_LEVELS = 5;
    
    // Posiciones anteriores para optimización
    int prevPaddle1X, prevPaddle2X;
    int prevBallX, prevBallY;
    bool ballStarted;
    bool gameOver;
    bool gameWon;
    
    void initBlocks();
    void initBlocksForLevel(int level);
    void nextLevel();
    void selectGameMode();
    void promptPlayerName();
    void promptTwoPlayerNames();
    void updateBallPhysics(); 
    void checkCollisions();   
    void resetBall();         
    bool checkPaddleCollision(const Paddle& paddle); 
    void checkBlockCollisions();
    bool checkWinCondition();
    void showGameOver();
    void showGameWon();
    
public:
    Game();
    ~Game();
    
    void run();
    void init();
    void cleanup();
    
    // Métodos para hilos
    void handleInput();
    void processKeyStates();
    void resetKeyStates();
    void updateGameLogic();
    void updatePaddle1Position(int newX);
    void updatePaddle2Position(int newX);
    void updateBallPosition(int newX, int newY);
    
    // Estado de teclas para control simultáneo
    std::atomic<bool> leftPressed;
    std::atomic<bool> rightPressed;
    std::atomic<bool> aPressed;
    std::atomic<bool> dPressed;
    
    // Getters para hilos
    GameMode getGameMode() const { return gameMode; }
    const Paddle& getPaddle1() const { return paddle1; }
    const Paddle& getPaddle2() const { return paddle2; }
    const Ball& getBall() const { return ball; }
    const Scoreboard& getScoreboard() const { return scoreboard; }
    const std::vector<std::vector<bool>>& getBlocks() const { return blockMatrix; }
    
    friend class ThreadManager;
};

#endif
