#ifndef BALL_H
#define BALL_H

class Ball {
private:
    int x, y;
    int velocityX, velocityY;
    bool visible;

public:
    Ball(int startX = 40, int startY = 20);

    void setPosition(int newX, int newY);
    void setVelocity(int vx, int vy);
    void move();
    void reverseX();
    void reverseY();

    int getX() const { return x; }
    int getY() const { return y; }
    int getVelocityX() const { return velocityX; }
    int getVelocityY() const { return velocityY; }
    bool isVisible() const { return visible; }
    
    
    void setVisible(bool vis) { visible = vis; }
};

#endif
