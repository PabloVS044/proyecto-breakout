#include "Paddle.h"

Paddle::Paddle(int startX, int startY, int w) : x(startX), y(startY), width(w), visible(true) {}

void Paddle::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Paddle::moveLeft() {
    if (x > 1) x--;
}

void Paddle::moveRight() {
    if (x + width < 79) x++;
}
