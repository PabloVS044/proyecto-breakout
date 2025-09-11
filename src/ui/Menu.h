#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

class Menu {
private:
    std::vector<std::string> options;
    size_t selectedIndex; // <-- cambiado de int a size_t

public:
    Menu();
    void display();
    int navigate();  // Retorna la opción seleccionada
};

#endif
