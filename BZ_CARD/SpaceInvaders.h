#ifndef SPACE_INVADERS_H
#define SPACE_INVADERS_H

#include "DisplayManager.h"
#include "InputManager.h"
#include "StorageManager.h"

class SpaceInvaders {
public:
    SpaceInvaders(DisplayManager& display, InputManager& input, StorageManager& storage);
    void play();
private:
    DisplayManager& displayManager;
    InputManager& inputManager;
    StorageManager& storageManager;
};

#endif // SPACE_INVADERS_H
