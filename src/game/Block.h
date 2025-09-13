#ifndef BLOCK_H
#define BLOCK_H

class Block {
private:
    int x, y;
    bool visible;
    bool destroyed;

public:
    Block(int startX, int startY);
    
    int getX() const { return x; }
    int getY() const { return y; }
    bool isVisible() const { return visible && !destroyed; }
    bool isDestroyed() const { return destroyed; }
    
    void setPosition(int newX, int newY);
    void destroy() { destroyed = true; }
    void setVisible(bool vis) { visible = vis; }
};

#endif
