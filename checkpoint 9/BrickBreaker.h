#ifndef BRICK_BREAKER_H
#define BRICK_BREAKER_H

#include "DisplayManager.h"
#include "InputManager.h"
#include "StorageManager.h"

class BrickBreaker {
public:
    BrickBreaker(DisplayManager& display, InputManager& input, StorageManager& storage);
    void play();
private:
    DisplayManager& displayManager;
    InputManager& inputManager;
    StorageManager& storageManager;
};

#endif // BRICK_BREAKER_H 