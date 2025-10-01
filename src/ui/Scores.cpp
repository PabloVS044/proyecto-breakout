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

    int startY = 2;
    int startX = (COLS / 2) - 25;

    mvprintw(1, startX, "PUNTAJES DESTACADOS");

    // Display single player scores
    displaySinglePlayerScores(singleEntries, startY);
    
    // Add some space
    startY += 2;
    
    // Display multiplayer scores
    displayTwoPlayerScores(multiEntries, startY);

    mvprintw(startY + 2, startX, "Presiona cualquier tecla para regresar...");
    refresh();
    getch();
}

void Scores::displaySinglePlayerScores(const std::vector<Entry>& entries, int& startY) {
    int startX = (COLS / 2) - 20;
    
    mvprintw(startY, startX, "UN JUGADOR");
    startY++;
    
    // table header
    mvprintw(startY,     startX,     "+--------------------------+---------+");
    mvprintw(startY + 1, startX,     "| Nombre                   | Puntaje |");
    mvprintw(startY + 2, startX,     "+--------------------------+---------+");

    int rowY = startY + 3;
    int shown = 0;
    for (const auto& e : entries) {
        if (shown >= 3) break;
        char nameBuf[27];
        std::snprintf(nameBuf, sizeof(nameBuf), "%-26.26s", e.name.c_str());
        mvprintw(rowY, startX, "| %s | %7d |", nameBuf, e.score);
        rowY++;
        shown++;
    }
    if (shown == 0) {
        mvprintw(rowY, startX, "| %-26s | %7s |", "(sin registros)", "-");
        rowY++;
    }
    mvprintw(rowY, startX, "+--------------------------+---------+");
    
    startY = rowY + 1;
}

void Scores::displayTwoPlayerScores(const std::vector<Entry>& entries, int& startY) {
    int startX = (COLS / 2) - 25;
    
    mvprintw(startY, startX, "DOS JUGADORES");
    startY++;
    
    // table header
    mvprintw(startY,     startX,     "+------------------+------------------+---------+");
    mvprintw(startY + 1, startX,     "| Jugador 1        | Jugador 2        | Puntaje |");
    mvprintw(startY + 2, startX,     "+------------------+------------------+---------+");

    int rowY = startY + 3;
    int shown = 0;
    for (const auto& e : entries) {
        if (shown >= 3) break;
        char name1Buf[19], name2Buf[19];
        std::snprintf(name1Buf, sizeof(name1Buf), "%-16.16s", e.name.c_str());
        std::snprintf(name2Buf, sizeof(name2Buf), "%-16.16s", e.name2.c_str());
        mvprintw(rowY, startX, "| %s | %s | %7d |", name1Buf, name2Buf, e.score);
        rowY++;
        shown++;
    }
    if (shown == 0) {
        mvprintw(rowY, startX, "| %-16s | %-16s | %7s |", "(sin registros)", "", "-");
        rowY++;
    }
    mvprintw(rowY, startX, "+------------------+------------------+---------+");
    
    startY = rowY + 1;
}



