#ifndef PADDLE_H
#define PADDLE_H

#include <chrono>
#include <mutex>

class Paddle {
private:
    int x, y;
    int width;
    bool visible;
    
    // Control de timing interno
    std::chrono::steady_clock::time_point lastMoveTime;
    std::chrono::milliseconds moveInterval;
    mutable std::mutex paddleMutex;
    
public:
    Paddle(int startX = 35, int startY = 22, int w = 5);
    
    int getX() const { 
        std::lock_guard<std::mutex> lock(paddleMutex);
        return x; 
    }
    int getY() const { 
        std::lock_guard<std::mutex> lock(paddleMutex);
        return y; 
    }
    int getWidth() const { 
        std::lock_guard<std::mutex> lock(paddleMutex);
        return width; 
    }
    bool isVisible() const { 
        std::lock_guard<std::mutex> lock(paddleMutex);
        return visible; 
    }
    
    void setPosition(int newX, int newY);
    void setVisible(bool vis);
    void moveLeft();
    void moveRight();
    bool shouldMove();
    void resetMoveTimer();
    bool tryMoveLeft();
    bool tryMoveRight();
};

#endif
