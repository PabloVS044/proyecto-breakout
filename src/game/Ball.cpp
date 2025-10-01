#include "Ball.h"

Ball::Ball(int startX, int startY) : x(startX), y(startY), velocityX(0), velocityY(-1), visible(true), 
                                    moveInterval(std::chrono::milliseconds(120)) {
    lastMoveTime = std::chrono::steady_clock::now();
}

void Ball::setPosition(int newX, int newY) {
    std::lock_guard<std::mutex> lock(ballMutex);
    x = newX;
    y = newY;
}

void Ball::setVelocity(int vx, int vy) {
    std::lock_guard<std::mutex> lock(ballMutex);
    velocityX = vx;
    velocityY = vy;
}

void Ball::move() {
    std::lock_guard<std::mutex> lock(ballMutex);
    x += velocityX;
    y += velocityY;
}

void Ball::reverseX() {
    std::lock_guard<std::mutex> lock(ballMutex);
    velocityX = -velocityX;
}

void Ball::reverseY() {
    std::lock_guard<std::mutex> lock(ballMutex);
    velocityY = -velocityY;
}

bool Ball::shouldMove() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime);
    return elapsed >= moveInterval;
}

void Ball::resetMoveTimer() {
    lastMoveTime = std::chrono::steady_clock::now();
}