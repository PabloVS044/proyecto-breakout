#include "Ball.h"

Ball::Ball(int startX, int startY) : x(startX), y(startY), velocityX(1), velocityY(-1), visible(true) {}

void Ball::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Ball::setVelocity(int vx, int vy) {
    velocityX = vx;
    velocityY = vy;
}

void Ball::move() {
    x += velocityX;
    y += velocityY;
}

void Ball::reverseX() {
    velocityX = -velocityX;
}

void Ball::reverseY() {
    velocityY = -velocityY;
}