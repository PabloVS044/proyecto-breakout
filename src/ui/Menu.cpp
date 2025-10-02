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
    clear();
    
    // ASCII Art del título centrado
    int centerX = COLS / 2;
    int startY = 2;
    mvprintw(startY++, centerX - 39, "       ____                             __                         __           ");
    mvprintw(startY++, centerX - 39, "      /\\  _`\\                          /\\ \\                       /\\ \\__        ");
    mvprintw(startY++, centerX - 39, "      \\ \\ \\L\\ \\   _ __     __      _   \\ \\ \\/\'\\      ___    __  __\\ \\ ,_\\       ");
    mvprintw(startY++, centerX - 39, "       \\ \\  _ <\' /\\`\'__\\ /\'__`\\  /\'__`\\ \\ \\ , <     / __`\\ /\\ \\/\\ \\\\ \\ \\/       ");
    mvprintw(startY++, centerX - 39, "        \\ \\ \\L\\ \\\\ \\ \\/ /\\  __/ /\\ \\L\\.\\ \\ \\ \\\\`\\  /\\ \\L\\ \\\\ \\ \\_\\ \\\\ \\ \\_      ");
    mvprintw(startY++, centerX - 39, "         \\ \\____/ \\ \\_\\ \\ \\____\\\\ \\__/.\\_\\\\ \\_\\ \\_\\\\ \\____/ \\ \\____/ \\ \\__\\     ");
    mvprintw(startY++, centerX - 39, "          \\/___/   \\/_/  \\/____/ \\/__/\\/_/ \\/_/\\/_/ \\/___/   \\/___/   \\/__/     ");
    
    // Opciones del menú centradas
    int menuStartY = 13;
    int menuStartX = centerX - 10;
    
    for (size_t i = 0; i < options.size(); i++) {
        if (i == selectedIndex) {
            mvprintw(menuStartY + i, menuStartX - 2, "> %s", options[i].c_str());
        } else {
            mvprintw(menuStartY + i, menuStartX, "%s", options[i].c_str());
        }
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
        if (ch == '\n' || ch == '\r') {
            return static_cast<int>(selectedIndex);
        }
    }
}
