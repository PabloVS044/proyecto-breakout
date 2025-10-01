#include "Paddle.h"
#include <ncurses.h>

Paddle::Paddle(int startX, int startY, int w) : x(startX), y(startY), width(w), visible(true),
                                                moveInterval(std::chrono::milliseconds(16)) { // ~60 FPS
    lastMoveTime = std::chrono::steady_clock::now();
}

void Paddle::setPosition(int newX, int newY) {
    std::lock_guard<std::mutex> lock(paddleMutex);
    x = newX;
    y = newY;
}

void Paddle::setVisible(bool vis) {
    std::lock_guard<std::mutex> lock(paddleMutex);
    visible = vis;
}

void Paddle::moveLeft() {
    std::lock_guard<std::mutex> lock(paddleMutex);
    if (x > 1) x--;
}

void Paddle::moveRight() {
    std::lock_guard<std::mutex> lock(paddleMutex);
    if (x + width < COLS - 2) x++;
}

bool Paddle::shouldMove() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime);
    return elapsed >= moveInterval;
}

void Paddle::resetMoveTimer() {
    lastMoveTime = std::chrono::steady_clock::now();
}

bool Paddle::tryMoveLeft() {
    if (!shouldMove()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(paddleMutex);
    if (x > 1) {
        x--;
        lastMoveTime = std::chrono::steady_clock::now(); // Actualizar tiempo directamente
        return true;
    }
    return false;
}

bool Paddle::tryMoveRight() {
    if (!shouldMove()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(paddleMutex);
    if (x + width < COLS - 2) {
        x++;
        lastMoveTime = std::chrono::steady_clock::now(); // Actualizar tiempo directamente
        return true;
    }
    return false;
}
