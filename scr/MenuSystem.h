#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "DisplayManager.h"
#include "InputManager.h"

class MenuSystem {
public:
    MenuSystem(DisplayManager& display, InputManager& input);
    void begin();
    void update();
    void showMenu();
    int getSelectedIndex() const;
    void setMenuItems(const char* title, const char* const* items, int numItems);
    void onSelect(void (*handler)(int));

private:
    DisplayManager& displayManager;
    InputManager& inputManager;
    const char* menuTitle;
    const char* const* menuItems;
    int numMenuItems;
    int selectedIdx;
    void (*selectHandler)(int);
};

#endif // MENU_SYSTEM_H 