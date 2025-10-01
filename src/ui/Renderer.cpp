#include "Renderer.h"
#include <cstring>
#include <algorithm>
#include <cstdio>

// OptimizedRenderer para el juego
void OptimizedRenderer::renderPaddle1(int x, int y, int width, int prevX) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->paddle1Region.regionMutex);
    
    // Borrar posición anterior si es diferente y válida
    if (prevX != -1 && prevX != x) {
        for (int i = 0; i < width; i++) {
            if (y >= 0 && y < LINES && (prevX + i) >= 0 && (prevX + i) < COLS) {
                gameSync->addScreenChange(ScreenChange(prevX + i, y, ' '));
            }
        }
    }
    
    // Dibujar nueva posición usando sprites
    for (int i = 0; i < width; i++) {
        if (y >= 0 && y < LINES && (x + i) >= 0 && (x + i) < COLS) {
            gameSync->addScreenChange(ScreenChange(x + i, y, '='));
        }
    }
    
    gameSync->paddle1Region.clearDirty();
}

void OptimizedRenderer::renderPaddle2(int x, int y, int width, int prevX) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->paddle2Region.regionMutex);
    
    // Borrar posición anterior si es diferente y válida
    if (prevX != -1 && prevX != x) {
        for (int i = 0; i < width; i++) {
            if (y >= 0 && y < LINES && (prevX + i) >= 0 && (prevX + i) < COLS) {
                gameSync->addScreenChange(ScreenChange(prevX + i, y, ' '));
            }
        }
    }
    
    // Dibujar nueva posición usando sprites (paddle jugador 2)
    for (int i = 0; i < width; i++) {
        if (y >= 0 && y < LINES && (x + i) >= 0 && (x + i) < COLS) {
            gameSync->addScreenChange(ScreenChange(x + i, y, '-'));
        }
    }
    
    gameSync->paddle2Region.clearDirty();
}

void OptimizedRenderer::renderBall(int x, int y, int prevX, int prevY) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->ballRegion.regionMutex);
    
    // Borrar posición anterior si es diferente
    if (prevX != -1 && prevY != -1 && (prevX != x || prevY != y)) {
        if (prevY >= 0 && prevY < LINES && prevX >= 0 && prevX < COLS) {
            gameSync->addScreenChange(ScreenChange(prevX, prevY, ' '));
        }
    }
    
    // Dibujar nueva posición usando sprite
    if (y >= 0 && y < LINES && x >= 0 && x < COLS) {
        gameSync->addScreenChange(ScreenChange(x, y, 'O'));
    }
    
    gameSync->ballRegion.clearDirty();
}

void OptimizedRenderer::renderScore(int score, int highScore) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->scoreRegion.regionMutex);
    
    // Limpiar solo la región necesaria para el score
    for (int i = 0; i < 50; i++) {  // Longitud máxima esperada del score
        gameSync->addScreenChange(ScreenChange(i, 0, ' '));
    }
    
    // Formatear y añadir el texto del score
    char scoreText[100];
    snprintf(scoreText, sizeof(scoreText), "Score: %d   High Score: %d", score, highScore);
    gameSync->addScreenChange(ScreenChange(2, 0, std::string(scoreText)));
    
    gameSync->scoreRegion.clearDirty();
}

void OptimizedRenderer::renderBlocks(const std::vector<std::vector<bool>>& blocks, int startX, int startY) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->blocksRegion.regionMutex);
    
    const int blockWidth = 3;
    for (size_t row = 0; row < blocks.size(); row++) {
        for (size_t col = 0; col < blocks[row].size(); col++) {
            int blockX = startX + static_cast<int>(col) * blockWidth;
            int blockY = startY + static_cast<int>(row);
            
            if (blocks[row][col]) {
                // Usar sprites de bloques
                if (blockY >= 0 && blockY < LINES && blockX >= 0 && blockX < COLS) {
                    gameSync->addScreenChange(ScreenChange(blockX, blockY, '['));
                }
                if (blockY >= 0 && blockY < LINES && (blockX + 1) >= 0 && (blockX + 1) < COLS) {
                    gameSync->addScreenChange(ScreenChange(blockX + 1, blockY, '#'));
                }
                if (blockY >= 0 && blockY < LINES && (blockX + 2) >= 0 && (blockX + 2) < COLS) {
                    gameSync->addScreenChange(ScreenChange(blockX + 2, blockY, ']'));
                }
            } else {
                // Limpiar bloques destruidos
                if (blockY >= 0 && blockY < LINES) {
                    for (int i = 0; i < blockWidth; i++) {
                        if ((blockX + i) >= 0 && (blockX + i) < COLS) {
                            gameSync->addScreenChange(ScreenChange(blockX + i, blockY, ' '));
                        }
                    }
                }
            }
        }
    }
    
    gameSync->blocksRegion.clearDirty();
}
void OptimizedRenderer::clearBlock(int x, int y) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->blocksRegion.regionMutex);
    
    // Borrar el bloque completo (3 caracteres)
    for (int i = 0; i < 3; i++) {
        if (y >= 0 && y < LINES && (x + i) >= 0 && (x + i) < COLS) {
            gameSync->addScreenChange(ScreenChange(x + i, y, ' '));
        }
    }
}

void OptimizedRenderer::initGameScreen() {
    clear();
    drawBorder();
    refresh();
}

void OptimizedRenderer::drawBorder() {
    box(stdscr, '|', '-'); // Usar sprites de pared
}

void OptimizedRenderer::clearRegion(int startX, int startY, int endX, int endY) {
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            mvaddch(y, x, ' ');
        }
    }
}

void OptimizedRenderer::drawChar(int x, int y, char c) {
    if (y >= 0 && y < LINES && x >= 0 && x < COLS) mvaddch(y, x, c);
}

void OptimizedRenderer::drawString(int x, int y, const std::string& str) {
    mvprintw(y, x, "%s", str.c_str());
}

// Renderer simple para menú
Renderer::Renderer() : width(80), height(24) {}

Renderer::~Renderer() {
    cleanup();
}

void Renderer::init() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(16);
    
    getmaxyx(stdscr, height, width);
    width = std::min(width, 80);
    height = std::min(height, 24);
    
    clear();
    refresh();
}

void Renderer::cleanup() {
    endwin();
}

void Renderer::clearScreen() {
    clear();
    refresh();
}

void Renderer::drawString(int x, int y, const std::string& str) {
    mvprintw(y, x, "%s", str.c_str());
}

void Renderer::drawTitle() {
    clear();
    
    // ASCII Art del título desde sprites
    int startY = 2;
    drawString(10, startY++, "##################################################################################");
    drawString(10, startY++, "#       ____                             __                         __           #");
    drawString(10, startY++, "#      /\\  _`\\                          /\\ \\                       /\\ \\__        #");
    drawString(10, startY++, "#      \\ \\ \\L\\ \\   _ __     __      _   \\ \\ \\/\'\\      ___    __  __\\ \\ ,_\\       #");
    drawString(10, startY++, "#       \\ \\  _ <\' /\\`\'__\\ /\'__`\\  /\'__`\\ \\ \\ , <     / __`\\ /\\ \\/\\ \\\\ \\ \\/       #");
    drawString(10, startY++, "#        \\ \\ \\L\\ \\\\ \\ \\/ /\\  __/ /\\ \\L\\.\\ \\ \\ \\\\`\\  /\\ \\L\\ \\\\ \\ \\_\\ \\\\ \\ \\_      #");
    drawString(10, startY++, "#         \\ \\____/ \\ \\_\\ \\ \\____\\\\ \\__/.\\_\\\\ \\_\\ \\_\\\\ \\____/ \\ \\____/ \\ \\__\\     #");
    drawString(10, startY++, "#          \\/___/   \\/_/  \\/____/ \\/__/\\/_/ \\/_/\\/_/ \\/___/   \\/___/   \\/__/     #");
    drawString(10, startY++, "##################################################################################");
    
    refresh();
}

void Renderer::drawMenuOptions(const std::vector<std::string>& options, size_t selected) {
    int startY = 13;
    int startX = 35;
    
    for (size_t i = 0; i < options.size(); i++) {
        if (i == selected) {
            drawString(startX - 2, startY + static_cast<int>(i), "> " + options[i]);
        } else {
            drawString(startX, startY + static_cast<int>(i), options[i]);
        }
    }
}
