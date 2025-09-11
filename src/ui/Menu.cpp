#include "Menu.h"
#include <ncurses.h>

Menu::Menu() {
    options = {
        "1. Iniciar partida",
        "2. Instrucciones",
        "3. Puntajes destacados",
        "4. Salir"
    };
    selectedIndex = 0;
}

void Menu::display() {
    clear(); // Limpia pantalla ncurses
    mvprintw(0, 0, "====== BREAKOUT ======");
    for (size_t i = 0; i < options.size(); i++) {
        if (i == selectedIndex)
            mvprintw(static_cast<int>(i) + 2, 0, "> %s", options[i].c_str());
        else
            mvprintw(static_cast<int>(i) + 2, 0, "  %s", options[i].c_str());
    }
    refresh();
}

int Menu::navigate() {
    int ch;
    while (true) {
        display();
        ch = getch();
        if ((ch == 'w' || ch == KEY_UP) && selectedIndex > 0) selectedIndex--;
        if ((ch == 's' || ch == KEY_DOWN) && options.size() > 0 && selectedIndex < options.size() - 1) selectedIndex++;
        if (ch == '\n') {
            return static_cast<int>(selectedIndex); // sigue retornando int
        }
    }
}
