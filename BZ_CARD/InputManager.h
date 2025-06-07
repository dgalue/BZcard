#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Adafruit_FreeTouch.h>

class InputManager {
public:
    InputManager();
    void begin();
    void update();
    bool isPowerPressed();
    bool isUpPressed();
    bool isDownPressed();
    bool isEnterPressed();
    bool upPressed();
    bool downPressed();
    bool enterPressed();
    bool powerPressed();
    bool anyButtonPressed();

private:
    Adafruit_FreeTouch touchPower;
    Adafruit_FreeTouch touchUp;
    Adafruit_FreeTouch touchDown;
    Adafruit_FreeTouch touchEnter;
    bool powerPressedState, upPressedState, downPressedState, enterPressedState;
};

#endif // INPUT_MANAGER_H 