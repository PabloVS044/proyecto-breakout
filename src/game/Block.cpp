#include "Block.h"

Block::Block(int startX, int startY) : x(startX), y(startY), visible(true), destroyed(false) {}

void Block::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}
