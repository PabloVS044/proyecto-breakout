#ifndef PADDLE_H
#define PADDLE_H

class Paddle {
private:
    int x, y;
    int width;
    bool visible;

public:
    Paddle(int startX = 35, int startY = 22, int w = 5);
    
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    bool isVisible() const { return visible; }
    
    void setPosition(int newX, int newY);
    void setVisible(bool vis) { visible = vis; }
    void moveLeft();
    void moveRight();
};

#endif
