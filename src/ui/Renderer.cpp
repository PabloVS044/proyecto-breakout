#include "Renderer.h"
#include <cstring>
#include <algorithm>

// OptimizedRenderer para el juego
void OptimizedRenderer::renderPaddle1(int x, int y, int width, int prevX) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->screenMutex);
    
    // Borrar toda la línea del paddle primero para evitar caracteres residuales
    for (int i = 1; i < COLS - 1; i++) {
        if (y >= 0 && y < LINES && i >= 0 && i < COLS) mvaddch(y, i, ' ');
    }
    
    // Dibujar nueva posición usando sprites
    for (int i = 0; i < width; i++) {
        if (y >= 0 && y < LINES && (x + i) >= 0 && (x + i) < COLS) mvaddch(y, x + i, '=');
    }
    
    refresh(); // Forzar actualización inmediata
    gameSync->paddle1Region.clearDirty();
}

void OptimizedRenderer::renderPaddle2(int x, int y, int width, int prevX) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->screenMutex);
    
    // Borrar toda la línea del paddle primero para evitar caracteres residuales
    for (int i = 1; i < COLS - 1; i++) {
        if (y >= 0 && y < LINES && i >= 0 && i < COLS) mvaddch(y, i, ' ');
    }
    
    // Dibujar nueva posición usando sprites (paddle jugador 2)
    for (int i = 0; i < width; i++) {
        if (y >= 0 && y < LINES && (x + i) >= 0 && (x + i) < COLS) mvaddch(y, x + i, '-');
    }
    
    refresh(); // Forzar actualización inmediata
    gameSync->paddle2Region.clearDirty();
}

void OptimizedRenderer::renderBall(int x, int y, int prevX, int prevY) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->screenMutex);
    
    // Borrar posición anterior si es diferente
    if (prevX != -1 && prevY != -1 && (prevX != x || prevY != y)) {
        if (prevY >= 0 && prevY < LINES && prevX >= 0 && prevX < COLS) mvaddch(prevY, prevX, ' ');
    }
    
    // Dibujar nueva posición usando sprite
    if (y >= 0 && y < LINES && x >= 0 && x < COLS) mvaddch(y, x, 'O');
    
    gameSync->ballRegion.clearDirty();
}

void OptimizedRenderer::renderScore(int score, int highScore) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->screenMutex);
    
    mvprintw(0, 2, "Score: %d   High Score: %d", score, highScore);
    
    gameSync->scoreRegion.clearDirty();
}

void OptimizedRenderer::renderBlocks(const std::vector<std::vector<bool>>& blocks, int startX, int startY) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->screenMutex);
    
    const int blockWidth = 3;
    for (size_t row = 0; row < blocks.size(); row++) {
        for (size_t col = 0; col < blocks[row].size(); col++) {
            if (blocks[row][col]) {
                int blockX = startX + static_cast<int>(col) * blockWidth;
                int blockY = startY + static_cast<int>(row);
                
                // Usar sprites de bloques
                if (blockY >= 0 && blockY < LINES && blockX >= 0 && blockX < COLS) mvaddch(blockY, blockX, '[');
                if (blockY >= 0 && blockY < LINES && (blockX + 1) >= 0 && (blockX + 1) < COLS) mvaddch(blockY, blockX + 1, '#');
                if (blockY >= 0 && blockY < LINES && (blockX + 2) >= 0 && (blockX + 2) < COLS) mvaddch(blockY, blockX + 2, ']');
            }
        }
    }
    
    gameSync->blocksRegion.clearDirty();
}
void OptimizedRenderer::clearBlock(int x, int y) {
    if (!gameSync) return;
    
    std::lock_guard<std::mutex> lock(gameSync->screenMutex);
    
    // Borrar el bloque completo (3 caracteres)
    for (int i = 0; i < 3; i++) {
        if (y >= 0 && y < LINES && (x + i) >= 0 && (x + i) < COLS) {
            mvaddch(y, x + i, ' ');
        }
    }
    
    refresh();
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
