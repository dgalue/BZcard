#ifndef TETRIS_H
#define TETRIS_H

#include "DisplayManager.h"
#include "InputManager.h"
#include "StorageManager.h"

class Tetris {
public:
    Tetris(DisplayManager& display, InputManager& input, StorageManager& storage);
    void play();
private:
    DisplayManager& displayManager;
    InputManager& inputManager;
    StorageManager& storageManager;
};

#endif // TETRIS_H
