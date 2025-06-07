#include "MenuSystem.h"

MenuSystem::MenuSystem(DisplayManager& display, InputManager& input)
    : displayManager(display), inputManager(input), menuTitle(nullptr), menuItems(nullptr), numMenuItems(0), selectedIdx(0), selectHandler(nullptr) {}

void MenuSystem::begin() {}

void MenuSystem::setMenuItems(const char* title, const char* const* items, int numItems) {
    menuTitle = title;
    menuItems = items;
    numMenuItems = numItems;
    selectedIdx = 0;
}

void MenuSystem::onSelect(void (*handler)(int)) {
    selectHandler = handler;
}

void MenuSystem::showMenu() {
    displayManager.drawMenu(menuTitle, menuItems, numMenuItems, selectedIdx);
}

void MenuSystem::update() {
    inputManager.update();
    if (inputManager.isUpPressed()) {
        if (selectedIdx > 0) selectedIdx--;
        showMenu();
    } else if (inputManager.isDownPressed()) {
        if (selectedIdx < numMenuItems - 1) selectedIdx++;
        showMenu();
    } else if (inputManager.isEnterPressed()) {
        if (selectHandler) selectHandler(selectedIdx);
    }
}

int MenuSystem::getSelectedIndex() const {
    return selectedIdx;
} 