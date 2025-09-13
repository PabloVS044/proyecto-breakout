#ifndef RENDERER_H
#define RENDERER_H

#include <ncurses.h>
#include <string>
#include <vector>
#include "../threads/SyncTools.h"

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

class OptimizedRenderer {
private:
    GameSync* gameSync;
    
public:
    OptimizedRenderer(GameSync* sync = nullptr) : gameSync(sync) {}
    
    void setGameSync(GameSync* sync) { gameSync = sync; }
    
    // Rendering de regiones específicas
    void renderPaddle1(int x, int y, int width, int prevX = -1);
    void renderPaddle2(int x, int y, int width, int prevX = -1);
    void renderBall(int x, int y, int prevX = -1, int prevY = -1);
    void renderScore(int score, int highScore);
    void renderBlocks(const std::vector<std::vector<bool>>& blocks, int startX, int startY);
    
    // Utilidades
    void clearRegion(int startX, int startY, int endX, int endY);
    void drawChar(int x, int y, char c);
    void drawString(int x, int y, const std::string& str);
    
    // Setup inicial
    void initGameScreen();
    void drawBorder();
};

// Mantener renderer simple para menú
class Renderer {
private:
    int width, height;
    
public:
    Renderer();
    ~Renderer();
    
    void init();
    void cleanup();
    void clearScreen();
    
    void drawString(int x, int y, const std::string& str);
    void drawTitle();
    void drawMenuOptions(const std::vector<std::string>& options, size_t selected);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif
