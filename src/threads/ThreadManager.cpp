#include "ThreadManager.h"
#include "../game/Game.h"
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
    
    // Hilos de rendering específicos por región
    threads.emplace_back(&ThreadManager::paddle1RenderThread, this);
    
    if (game->getGameMode() == TWO_PLAYER) {
        threads.emplace_back(&ThreadManager::paddle2RenderThread, this);
    }
    
    // Hilo de rendering general
    threads.emplace_back(&ThreadManager::renderThread, this);
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
    timeout(0); // No esperar por entrada
    
    auto lastKeyReset = std::chrono::steady_clock::now();
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        // Capturar entrada
        game->handleInput();
        
        // Procesar movimiento continuo
        game->processKeyStates();
        
        // Resetear teclas cada cierto tiempo para evitar movimiento infinito
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastKeyReset).count() > 100) {
            game->resetKeyStates();
            lastKeyReset = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}

void ThreadManager::paddle1RenderThread() {
    if (!gameSync || !game) return;
    
    int prevX = -1;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        if (gameSync->paddle1Region.isDirty()) {
            const auto& paddle = game->getPaddle1();
            OptimizedRenderer renderer(gameSync);
            renderer.renderPaddle1(paddle.getX(), paddle.getY(), paddle.getWidth(), prevX);
            prevX = paddle.getX();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Reducir frecuencia
    }
}

void ThreadManager::paddle2RenderThread() {
    if (!gameSync || !game) return;
    
    int prevX = -1;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        if (gameSync->paddle2Region.isDirty()) {
            const auto& paddle = game->getPaddle2();
            OptimizedRenderer renderer(gameSync);
            renderer.renderPaddle2(paddle.getX(), paddle.getY(), paddle.getWidth(), prevX);
            prevX = paddle.getX();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Reducir frecuencia
    }
}

void ThreadManager::renderThread() {
    if (!gameSync || !game) return;
    
    while (gameSync->gameRunning && !gameSync->shouldExit) {
        OptimizedRenderer renderer(gameSync);
        
        // Renderizar score si cambió
        if (gameSync->scoreRegion.isDirty()) {
            const auto& scoreboard = game->getScoreboard();
            renderer.renderScore(scoreboard.getScore(), scoreboard.getHighScore());
        }
        
        // Renderizar pelota si cambió
        if (gameSync->ballRegion.isDirty()) {
            // Aquí se renderizará la pelota cuando se implemente la física
        }
        
        // Renderizar bloques si cambiaron
        if (gameSync->blocksRegion.isDirty()) {
            renderer.renderBlocks(game->getBlocks(), 10, 3);
        }
        
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS para elementos menos críticos
    }
}
