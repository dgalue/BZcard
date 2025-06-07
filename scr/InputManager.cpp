#include "InputManager.h"

#define PIN_TOUCH_POWER A0
#define PIN_TOUCH_UP    A2
#define PIN_TOUCH_DOWN  A3
#define PIN_TOUCH_ENTER A1
#define TOUCH_THRESHOLD 500

InputManager::InputManager()
    : touchPower(PIN_TOUCH_POWER, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
      touchUp(PIN_TOUCH_UP, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
      touchDown(PIN_TOUCH_DOWN, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
      touchEnter(PIN_TOUCH_ENTER, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
      powerPressedState(false), upPressedState(false), downPressedState(false), enterPressedState(false) {}

void InputManager::begin() {
    touchPower.begin();
    touchUp.begin();
    touchDown.begin();
    touchEnter.begin();
}

void InputManager::update() {
    powerPressedState = touchPower.measure() > TOUCH_THRESHOLD;
    upPressedState = touchUp.measure() > TOUCH_THRESHOLD;
    downPressedState = touchDown.measure() > TOUCH_THRESHOLD;
    enterPressedState = touchEnter.measure() > TOUCH_THRESHOLD;
}

bool InputManager::isPowerPressed() { return powerPressedState; }
bool InputManager::isUpPressed() { return upPressedState; }
bool InputManager::isDownPressed() { return downPressedState; }
bool InputManager::isEnterPressed() { return enterPressedState; }
bool InputManager::upPressed() { return upPressedState; }
bool InputManager::downPressed() { return downPressedState; }
bool InputManager::enterPressed() { return enterPressedState; }
bool InputManager::powerPressed() { return powerPressedState; }
bool InputManager::anyButtonPressed() { return powerPressedState || upPressedState || downPressedState || enterPressedState; } 