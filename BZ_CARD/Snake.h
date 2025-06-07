#ifndef SNAKE_H
#define SNAKE_H

#include "DisplayManager.h"
#include "InputManager.h"
#include "StorageManager.h"

class Snake {
public:
    Snake(DisplayManager& display, InputManager& input, StorageManager& storage);
    void play();
private:
    DisplayManager& displayManager;
    InputManager& inputManager;
    StorageManager& storageManager;
};

#endif // SNAKE_H 