#include "Scores.h"
#include <ncurses.h>
#include <fstream>
#include <algorithm>

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, b - a + 1);
}

std::vector<Scores::Entry> Scores::loadScores(const std::string& path) {
    std::vector<Entry> entries;
    std::ifstream in(path);
    if (!in.is_open()) return entries;
    std::string line;
    while (std::getline(in, line)) {
        // CSV format: "name",score,type or "name1","name2",score,type
        std::string name, name2;
        int score = 0;
        bool isMultiplayer = false;
        
        // Check if it's multiplayer format (has 4 fields)
        size_t comma1 = line.find(',');
        if (comma1 == std::string::npos) continue;
        
        size_t comma2 = line.find(',', comma1 + 1);
        if (comma2 == std::string::npos) continue;
        
        size_t comma3 = line.find(',', comma2 + 1);
        
        if (comma3 != std::string::npos) {
            // Multiplayer format: "name1","name2",score,multi
            std::string type = trim(line.substr(comma3 + 1));
            if (type == "multi") {
                isMultiplayer = true;
                // Parse name1
                size_t firstQuote = line.find('"');
                size_t secondQuote = std::string::npos;
                if (firstQuote != std::string::npos) {
                    secondQuote = line.find('"', firstQuote + 1);
                }
                if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
                    name = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                    
                    // Parse name2
                    size_t thirdQuote = line.find('"', secondQuote + 1);
                    size_t fourthQuote = std::string::npos;
                    if (thirdQuote != std::string::npos) {
                        fourthQuote = line.find('"', thirdQuote + 1);
                    }
                    if (thirdQuote != std::string::npos && fourthQuote != std::string::npos) {
                        name2 = line.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);
                    }
                }
                
                // Parse score
                std::string scoreStr = trim(line.substr(comma2 + 1, comma3 - comma2 - 1));
                try {
                    score = std::stoi(scoreStr);
                } catch (...) {
                    continue;
                }
            }
        } else {
            // Single player format: "name",score,single
            std::string type = trim(line.substr(comma2 + 1));
            if (type == "single") {
                isMultiplayer = false;
                // Parse name
                size_t firstQuote = line.find('"');
                size_t secondQuote = std::string::npos;
                if (firstQuote != std::string::npos) {
                    secondQuote = line.find('"', firstQuote + 1);
                }
                if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
                    name = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                }
                
                // Parse score
                std::string scoreStr = trim(line.substr(comma1 + 1, comma2 - comma1 - 1));
                try {
                    score = std::stoi(scoreStr);
                } catch (...) {
                    continue;
                }
            }
        }
        
        if (!name.empty()) {
            entries.push_back({name, name2, score, isMultiplayer});
        }
    }
    return entries;
}

void Scores::showTop() {
    clear();
    auto entries = loadScores("scores.csv");
    
    // Separate single and multiplayer entries
    std::vector<Entry> singleEntries, multiEntries;
    for (const auto& entry : entries) {
        if (entry.isMultiplayer) {
            multiEntries.push_back(entry);
        } else {
            singleEntries.push_back(entry);
        }
    }
    
    // Sort both lists by score
    std::sort(singleEntries.begin(), singleEntries.end(), [](const Entry& a, const Entry& b){
        return a.score > b.score;
    });
    std::sort(multiEntries.begin(), multiEntries.end(), [](const Entry& a, const Entry& b){
        return a.score > b.score;
    });
    
    // Limit to top 3 for each
    if (singleEntries.size() > 3) singleEntries.resize(3);
    if (multiEntries.size() > 3) multiEntries.resize(3);

    int startY = 1;
    int centerX = COLS / 2;

    // Banner principal
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(startY++, centerX - 38, "                         __              __               ");
    mvprintw(startY++, centerX - 38, "    ______  __ __  _____/  |______      |__| ____   ______");
    mvprintw(startY++, centerX - 38, "    \\____ \\|  |  \\/    \\   __\\__  \\     |  |/ __ \\ /  ___/");
    mvprintw(startY++, centerX - 38, "    |  |_> >  |  /   |  \\  |  / __ \\_   |  \\  ___/ \\___ \\ ");
    mvprintw(startY++, centerX - 38, "    |   __/|____/|___|  /__| (____  /\\__|  |\\___  >____  >");
    mvprintw(startY++, centerX - 38, "    |__|              \\/          \\/______|    \\/     \\/ ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    
    startY += 2;

    // Display single player scores
    displaySinglePlayerScores(singleEntries, startY);
    
    // Add some space
    startY += 2;
    
    // Display multiplayer scores
    displayTwoPlayerScores(multiEntries, startY);

    startY += 2;
    
    // Footer decorativo
    attron(COLOR_PAIR(7) | A_BOLD);
    mvprintw(startY++, centerX - 32, "################################################################");
    attroff(COLOR_PAIR(7) | A_BOLD);
    
    attron(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    mvprintw(startY++, centerX - 25, ">>> PRESIONA CUALQUIER TECLA PARA CONTINUAR <<<");
    attroff(COLOR_PAIR(1) | A_BOLD | A_BLINK);
    
    attron(COLOR_PAIR(7) | A_BOLD);
    mvprintw(startY, centerX - 32, "################################################################");
    attroff(COLOR_PAIR(7) | A_BOLD);
    
    refresh();
    getch();
}

void Scores::displaySinglePlayerScores(const std::vector<Entry>& entries, int& startY) {
    int centerX = COLS / 2;
    
    // Título de sección
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(startY++, centerX - 25, "+================================================+");
    mvprintw(startY++, centerX - 25, "|  ####  UN JUGADOR - TOP 3  ####              |");
    mvprintw(startY++, centerX - 25, "+================================================+");
    attroff(COLOR_PAIR(5) | A_BOLD);
    
    startY++;
    
    // Table header con estilo arcade
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(startY,     centerX - 25, "+---+----------------------------+-----------+");
    mvprintw(startY + 1, centerX - 25, "| # | NOMBRE                     | PUNTAJE   |");
    mvprintw(startY + 2, centerX - 25, "+---+----------------------------+-----------+");
    attroff(COLOR_PAIR(3) | A_BOLD);

    int rowY = startY + 3;
    int shown = 0;
    
    for (const auto& e : entries) {
        if (shown >= 3) break;
        
        // Determinar color según posición
        int colorPair = 3;
        const char* medal = " ";
        if (shown == 0) { 
            colorPair = 2; // Oro
            medal = "*";
        } else if (shown == 1) { 
            colorPair = 7; // Plata
            medal = "*";
        } else if (shown == 2) { 
            colorPair = 6; // Bronce
            medal = "*";
        }
        
        char nameBuf[29];
        std::snprintf(nameBuf, sizeof(nameBuf), "%-28.28s", e.name.c_str());
        
        attron(COLOR_PAIR(colorPair) | A_BOLD);
        mvprintw(rowY, centerX - 25, "| %s |", medal);
        mvprintw(rowY, centerX - 22, " %s | %9d |", nameBuf, e.score);
        attroff(COLOR_PAIR(colorPair) | A_BOLD);
        
        rowY++;
        shown++;
    }
    
    if (shown == 0) {
        attron(COLOR_PAIR(3));
        mvprintw(rowY, centerX - 25, "|   | %-28s | %9s |", "(sin registros)", "---");
        attroff(COLOR_PAIR(3));
        rowY++;
    }
    
    // Completar hasta 3 filas si hay menos
    while (shown < 3) {
        attron(COLOR_PAIR(3));
        mvprintw(rowY, centerX - 25, "|   | %-28s | %9s |", "---", "---");
        attroff(COLOR_PAIR(3));
        rowY++;
        shown++;
    }
    
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(rowY, centerX - 25, "+---+----------------------------+-----------+");
    attroff(COLOR_PAIR(3) | A_BOLD);
    
    startY = rowY + 1;
}

void Scores::displayTwoPlayerScores(const std::vector<Entry>& entries, int& startY) {
    int centerX = COLS / 2;
    
    // Título de sección
    attron(COLOR_PAIR(6) | A_BOLD);
    mvprintw(startY++, centerX - 30, "+========================================================+");
    mvprintw(startY++, centerX - 30, "|  ####  DOS JUGADORES - TOP 3  ####                    |");
    mvprintw(startY++, centerX - 30, "+========================================================+");
    attroff(COLOR_PAIR(6) | A_BOLD);
    
    startY++;
    
    // Table header
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(startY,     centerX - 30, "+---+--------------------+--------------------+-----------+");
    mvprintw(startY + 1, centerX - 30, "| # | JUGADOR 1          | JUGADOR 2          | PUNTAJE   |");
    mvprintw(startY + 2, centerX - 30, "+---+--------------------+--------------------+-----------+");
    attroff(COLOR_PAIR(4) | A_BOLD);

    int rowY = startY + 3;
    int shown = 0;
    
    for (const auto& e : entries) {
        if (shown >= 3) break;
        
        // Determinar color según posición
        int colorPair = 4;
        const char* medal = " ";
        if (shown == 0) { 
            colorPair = 2; // Oro
            medal = "*";
        } else if (shown == 1) { 
            colorPair = 7; // Plata
            medal = "*";
        } else if (shown == 2) { 
            colorPair = 6; // Bronce
            medal = "*";
        }
        
        char name1Buf[21], name2Buf[21];
        std::snprintf(name1Buf, sizeof(name1Buf), "%-20.20s", e.name.c_str());
        std::snprintf(name2Buf, sizeof(name2Buf), "%-20.20s", e.name2.c_str());
        
        attron(COLOR_PAIR(colorPair) | A_BOLD);
        mvprintw(rowY, centerX - 30, "| %s | %s | %s | %9d |", medal, name1Buf, name2Buf, e.score);
        attroff(COLOR_PAIR(colorPair) | A_BOLD);
        
        rowY++;
        shown++;
    }
    
    if (shown == 0) {
        attron(COLOR_PAIR(4));
        mvprintw(rowY, centerX - 30, "|   | %-20s | %-20s | %9s |", "(sin registros)", "", "---");
        attroff(COLOR_PAIR(4));
        rowY++;
    }
    
    // Completar hasta 3 filas si hay menos
    while (shown < 3) {
        attron(COLOR_PAIR(4));
        mvprintw(rowY, centerX - 30, "|   | %-20s | %-20s | %9s |", "---", "---", "---");
        attroff(COLOR_PAIR(4));
        rowY++;
        shown++;
    }
    
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(rowY, centerX - 30, "+---+--------------------+--------------------+-----------+");
    attroff(COLOR_PAIR(4) | A_BOLD);
    
    startY = rowY + 1;
}