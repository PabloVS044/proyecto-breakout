#include "ThreadManager.h"
#include "../game/Game.h"
#include "../ui/Renderer.h"
#include <chrono>
#include <ncurses.h>

ThreadManager::ThreadManager(Game* gameInstance) : gameSync(nullptr), game(gameInstance) {}

ThreadManager::~ThreadManager() {
    stopThreads();
}

void ThreadManager::startThreads() {
    if (!gameSync || !game) return;
    
    // Hilo de entrada de usuario
    threads.emplace_back(&ThreadManager::inputThread, this);
    
    // Hilo de lógica del juego
    threads.emplace_back(&ThreadManager::gameLogicThread, this);
    
    // Hilos de elementos específicos
    threads.emplace_back(&ThreadManager::paddle1UpdateThread, this);
    
    if (game->getGameMode() == TWO_PLAYER) {
        threads.emplace_back(&ThreadManager::paddle2UpdateThread, this);
    }
    
    threads.emplace_back(&ThreadManager::ballUpdateThread, this);
    
    // Hilo maestro de renderizado (coordina todo el renderizado)
    threads.emplace_back(&ThreadManager::masterRenderThread, this);
}

void ThreadManager::stopThreads() {
    if (gameSync) {
        gameSync->shouldExit = true;
        gameSync->gameRunning = false;
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads.clear();
}

void ThreadManager::inputThread() {
    if (!gameSync || !game) return;
    
    nodelay(stdscr, TRUE);
    timeout(0);
    
    auto lastKeyReset = std::chrono::steady_clock::now();
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        // Capturar entrada del usuario
        game->handleInput();
        
        // Procesar movimiento continuo
        game->processKeyStates();
        
        // Resetear teclas cada cierto tiempo para evitar movimiento infinito
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastKeyReset).count() > 150) {
            game->resetKeyStates();
            lastKeyReset = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void ThreadManager::gameLogicThread() {
    if (!gameSync || !game) return;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        // Lógica del juego (colisiones, física, etc.)
        game->updateGameLogic();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void ThreadManager::paddle1UpdateThread() {
    if (!gameSync || !game) return;
    
    int prevX = -1;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        // Solo verificar y renderizar si la región está marcada como sucia
        if (gameSync->paddle1Region.isDirty()) {
            const auto& paddle = game->getPaddle1();
            int currentX = paddle.getX();
            
            OptimizedRenderer renderer(gameSync);
            renderer.renderPaddle1(currentX, paddle.getY(), paddle.getWidth(), prevX);
            prevX = currentX;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void ThreadManager::paddle2UpdateThread() {
    if (!gameSync || !game) return;
    
    int prevX = -1;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        // Solo verificar y renderizar si la región está marcada como sucia
        if (gameSync->paddle2Region.isDirty()) {
            const auto& paddle = game->getPaddle2();
            int currentX = paddle.getX();
            
            OptimizedRenderer renderer(gameSync);
            renderer.renderPaddle2(currentX, paddle.getY(), paddle.getWidth(), prevX);
            prevX = currentX;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void ThreadManager::ballUpdateThread() {
    if (!gameSync || !game) return;
    
    int prevX = -1, prevY = -1;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        // Solo verificar y renderizar si la región está marcada como sucia
        if (gameSync->ballRegion.isDirty()) {
            const auto& ball = game->getBall();
            int currentX = ball.getX();
            int currentY = ball.getY();
            
            OptimizedRenderer renderer(gameSync);
            renderer.renderBall(currentX, currentY, prevX, prevY);
            prevX = currentX;
            prevY = currentY;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void ThreadManager::masterRenderThread() {
    if (!gameSync || !game) return;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        OptimizedRenderer renderer(gameSync);
        
        // Renderizar score si cambió
        if (gameSync->scoreRegion.isDirty()) {
            const auto& scoreboard = game->getScoreboard();
            renderer.renderScore(scoreboard.getScore(), scoreboard.getHighScore());
        }
        
        // Renderizar bloques si cambiaron
        if (gameSync->blocksRegion.isDirty()) {
            const int blockCols = 10;
            const int blockWidth = 3;
            int startXBlocks = (COLS - blockCols * blockWidth) / 2;
            renderer.renderBlocks(game->getBlocks(), startXBlocks, 5);
        }
        
        // Aplicar cambios acumulados a la pantalla
        gameSync->flushChangesToScreen();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}
