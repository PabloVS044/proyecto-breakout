#include "SyncTools.h"
#include <ncurses.h>

void GameSync::flushChangesToScreen() {
    std::queue<ScreenChange> localChanges;
    
    // Copiar cambios localmente para minimizar tiempo de bloqueo
    {
        std::lock_guard<std::mutex> changeLock(changeBufferMutex);
        localChanges = std::move(pendingChanges);
        pendingChanges = std::queue<ScreenChange>(); // Limpiar la cola original
    }
    
    // Aplicar cambios con bloqueo mínimo de pantalla
    if (!localChanges.empty()) {
        std::lock_guard<std::mutex> screenLock(screenMutex);
        
        while (!localChanges.empty()) {
            const ScreenChange& change = localChanges.front();
            
            if (change.isString) {
                // Renderizar string completo
                mvprintw(change.y, change.x, "%s", change.text.c_str());
            } else {
                // Renderizar carácter individual
                mvaddch(change.y, change.x, change.character);
            }
            
            localChanges.pop();
        }
        
        // Una sola llamada a refresh() para toda la pantalla
        refresh();
    }
}

void GameSync::waitForFrameCompletion() {
    std::unique_lock<std::mutex> lock(screenMutex);
    frameComplete.wait(lock, [this] { return !frameInProgress.load(); });
}

void GameSync::signalFrameStart() {
    frameInProgress.store(true);
    activeRenderThreads.store(0);
}

void GameSync::signalFrameEnd() {
    frameInProgress.store(false);
    frameComplete.notify_all();
}