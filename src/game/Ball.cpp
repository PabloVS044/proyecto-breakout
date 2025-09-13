#include "Ball.h"

Ball::Ball(int startX, int startY) : x(startX), y(startY), visible(true) {}

void Ball::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}
