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

    while (!inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }
}
