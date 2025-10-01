#ifndef BALL_H
#define BALL_H

#include <chrono>
#include <mutex>

class Ball {
private:
    int x, y;
    int velocityX, velocityY;
    bool visible;
    
    // Control de timing interno
    std::chrono::steady_clock::time_point lastMoveTime;
    std::chrono::milliseconds moveInterval;
    mutable std::mutex ballMutex;
    
public:
    Ball(int startX = 40, int startY = 20);

    void setPosition(int newX, int newY);
    void setVelocity(int vx, int vy);
    void move();
    void reverseX();
    void reverseY();
    bool shouldMove(); // Nuevo método para verificar si es tiempo de moverse
    void resetMoveTimer(); // Resetear el timer

    int getX() const { 
        std::lock_guard<std::mutex> lock(ballMutex);
        return x; 
    }
    int getY() const { 
        std::lock_guard<std::mutex> lock(ballMutex);
        return y; 
    }
    int getVelocityX() const { 
        std::lock_guard<std::mutex> lock(ballMutex);
        return velocityX; 
    }
    int getVelocityY() const { 
        std::lock_guard<std::mutex> lock(ballMutex);
        return velocityY; 
    }
    bool isVisible() const { 
        std::lock_guard<std::mutex> lock(ballMutex);
        return visible; 
    }
    
    void setVisible(bool vis) { 
        std::lock_guard<std::mutex> lock(ballMutex);
        visible = vis; 
    }
};

#endif
