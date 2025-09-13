#ifndef SYNCTOOLS_H
#define SYNCTOOLS_H

#include <mutex>
#include <condition_variable>
#include <atomic>

struct RenderRegion {
    int startX, startY, endX, endY;
    std::atomic<bool> needsUpdate;
    
    RenderRegion(int sx = 0, int sy = 0, int ex = 0, int ey = 0) 
        : startX(sx), startY(sy), endX(ex), endY(ey), needsUpdate(false) {}
    
    void markDirty() {
        needsUpdate = true;
    }
    
    bool isDirty() {
        return needsUpdate.load();
    }
    
    void clearDirty() {
        needsUpdate = false;
    }
    
    void setRegion(int sx, int sy, int ex, int ey) {
        startX = sx;
        startY = sy;
        endX = ex;
        endY = ey;
    }
};

class GameSync {
public:
    std::mutex screenMutex;
    std::condition_variable renderReady;
    std::atomic<bool> gameRunning;
    std::atomic<bool> shouldExit;
    
    // Regiones de rendering optimizado
    RenderRegion paddle1Region;
    RenderRegion paddle2Region;
    RenderRegion ballRegion;
    RenderRegion scoreRegion;
    RenderRegion blocksRegion;
    
    GameSync() : gameRunning(true), shouldExit(false) {}
};

#endif
