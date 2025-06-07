#include "SpaceInvaders.h"
#include <Arduino.h>

SpaceInvaders::SpaceInvaders(DisplayManager& display, InputManager& input, StorageManager& storage)
    : displayManager(display), inputManager(input), storageManager(storage) {}

void SpaceInvaders::play() {
    displayManager.u8g2.firstPage();
    do {
        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
        int w = displayManager.u8g2.getStrWidth("SPACE INVADERS");
        displayManager.u8g2.drawStr((128 - w) / 2, 30, "SPACE INVADERS");
        displayManager.u8g2.setFont(u8g2_font_6x10_tr);
        displayManager.u8g2.drawStr(22, 50, "Not implemented");
    } while (displayManager.u8g2.nextPage());

    // Wait for the launch button to be released
    while (inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }

    // Wait for a new button press before exiting
    while (!inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }

    // Wait for release so the menu doesn't immediately re-trigger
    while (inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }
}
