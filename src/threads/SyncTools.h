#ifndef SYNCTOOLS_H
#define SYNCTOOLS_H

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <memory>
#include <string>

// Estructura para cambios pendientes en pantalla
struct ScreenChange {
    int x, y;
    char character;
    bool isString;
    std::string text;
    
    ScreenChange(int x, int y, char c) : x(x), y(y), character(c), isString(false) {}
    ScreenChange(int x, int y, const std::string& str) : x(x), y(y), character(' '), isString(true), text(str) {}
};

struct RenderRegion {
    int startX, startY, endX, endY;
    std::atomic<bool> needsUpdate;
    std::mutex regionMutex;
    
    RenderRegion(int sx = 0, int sy = 0, int ex = 0, int ey = 0) 
        : startX(sx), startY(sy), endX(ex), endY(ey), needsUpdate(false) {}
    
    void markDirty() {
        needsUpdate.store(true);
    }
    
    bool isDirty() {
        return needsUpdate.load();
    }
    
    void clearDirty() {
        needsUpdate.store(false);
    }
    
    void setRegion(int sx, int sy, int ex, int ey) {
        std::lock_guard<std::mutex> lock(regionMutex);
        startX = sx;
        startY = sy;
        endX = ex;
        endY = ey;
    }
};

class GameSync {
public:
    // Mutex principal para la pantalla completa
    std::mutex screenMutex;
    
    // Mutex para el buffer de cambios
    std::mutex changeBufferMutex;
    
    // Condition variables para sincronización
    std::condition_variable renderReady;
    std::condition_variable frameComplete;
    
    // Estados atómicos
    std::atomic<bool> gameRunning;
    std::atomic<bool> shouldExit;
    std::atomic<bool> frameInProgress;
    std::atomic<int> activeRenderThreads;
    
    // Buffer de cambios pendientes
    std::queue<ScreenChange> pendingChanges;
    
    // Regiones de rendering optimizado
    RenderRegion paddle1Region;
    RenderRegion paddle2Region;
    RenderRegion ballRegion;
    RenderRegion scoreRegion;
    RenderRegion blocksRegion;
    
    GameSync() : gameRunning(true), shouldExit(false), frameInProgress(false), activeRenderThreads(0) {}
    
    // Métodos para manejo seguro del buffer
    void addScreenChange(const ScreenChange& change) {
        std::lock_guard<std::mutex> lock(changeBufferMutex);
        pendingChanges.push(change);
    }
    
    void flushChangesToScreen();
    void waitForFrameCompletion();
    void signalFrameStart();
    void signalFrameEnd();
};

#endif
