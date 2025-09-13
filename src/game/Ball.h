#ifndef BALL_H
#define BALL_H

class Ball {
private:
    int x, y;
    bool visible;

public:
    Ball(int startX = 40, int startY = 20);
    
    int getX() const { return x; }
    int getY() const { return y; }
    bool isVisible() const { return visible; }
    
    void setPosition(int newX, int newY);
    void setVisible(bool vis) { visible = vis; }
};

#endif
