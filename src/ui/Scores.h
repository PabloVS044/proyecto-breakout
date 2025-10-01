#ifndef SCORES_H
#define SCORES_H

#include <string>
#include <vector>

class Scores {
public:
    static void showTop();
private:
    struct Entry { 
        std::string name; 
        std::string name2; 
        int score; 
        bool isMultiplayer;
    };
    static std::vector<Entry> loadScores(const std::string& path);
    static void displaySinglePlayerScores(const std::vector<Entry>& entries, int& startY);
    static void displayTwoPlayerScores(const std::vector<Entry>& entries, int& startY);
};

#endif



