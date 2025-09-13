#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "SyncTools.h"
#include <thread>
#include <vector>
#include <memory>

class Game; // Forward declaration

class ThreadManager {
private:
    GameSync* gameSync;
    std::vector<std::thread> threads;
    Game* game;
    
    void inputThread();
    void renderThread();
    void paddle1RenderThread();
    void paddle2RenderThread();
    
public:
    ThreadManager(Game* gameInstance);
    ~ThreadManager();
    
    void startThreads();
    void stopThreads();
    void setGameSync(GameSync* sync) { gameSync = sync; }
};

#endif
