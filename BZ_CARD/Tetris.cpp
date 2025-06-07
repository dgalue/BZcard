#include "Tetris.h"
#include <Arduino.h>

Tetris::Tetris(DisplayManager& display, InputManager& input, StorageManager& storage)
    : displayManager(display), inputManager(input), storageManager(storage) {}

void Tetris::play() {
    displayManager.u8g2.firstPage();
    do {
        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
        int w = displayManager.u8g2.getStrWidth("TETRIS");
        displayManager.u8g2.drawStr((128 - w) / 2, 30, "TETRIS");
        displayManager.u8g2.setFont(u8g2_font_6x10_tr);
        displayManager.u8g2.drawStr(22, 50, "Not implemented");
    } while (displayManager.u8g2.nextPage());

    while (!inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }
}
