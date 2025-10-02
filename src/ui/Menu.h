#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

class Menu {
private:
    std::vector<std::string> options;
    size_t selectedIndex;

public:
    Menu();
    void display();
    int navigate();
};

#endif
