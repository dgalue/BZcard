# BZ_CARD: Modular Interactive Arduino Business Card

## 🌟 Project Overview
This project implements an interactive business card on an Arduino-compatible board with an OLED display and capacitive touch buttons. It features:
- OLED menu system with multiple sections (Contact, Social, Projects, Games)
- Two built-in games: Brick Breaker and Snake
- Animated logo display
- Power management (sleep, wake, charging detection)
- Persistent high score storage

---

## 🛠️ Hardware Requirements
- Arduino-compatible board (SAMD21 recommended, e.g., MKR series)
- SSD1306 128x64 OLED display (I2C or SPI)
- 4x capacitive touch buttons (using Adafruit_FreeTouch)
- USB for power/charging detection

---

## 📚 Library Dependencies
Install these libraries via the Arduino Library Manager:
- [U8g2](https://github.com/olikraus/u8g2) (for OLED graphics)
- [Adafruit_FreeTouch](https://github.com/adafruit/Adafruit_FreeTouch) (for capacitive touch)
- [RTCZero](https://github.com/arduino-libraries/RTCZero) (for sleep/wake)
- [FlashStorage](https://github.com/cmaglie/FlashStorage) (for high score storage)

---

## 📁 File/Module Structure
All files must be in the **same folder** as `BZ_CARD.ino`:

- `BZ_CARD.ino` — Main entry point, wires up all modules
- `Logo.h` — Contains the logo bitmap
- `DisplayManager.h/cpp` — OLED display logic (drawing, logo, menu)
- `InputManager.h/cpp` — Touch button handling
- `MenuSystem.h/cpp` — Menu navigation and selection
- `PowerManager.h/cpp` — Power management, sleep, charging
- `StorageManager.h/cpp` — Persistent storage (high score)
- `BrickBreaker.h/cpp` — Full Brick Breaker game logic
- `Snake.h/cpp` — Full Snake game logic

---

## 🚦 How to Compile & Upload
1. Place **all files** in the same folder.
2. Open `BZ_CARD.ino` in the Arduino IDE.
3. Install all required libraries (see above).
4. Select your board and port.
5. Click **Upload**.

---

## 🧩 What Each Module/Class Does
- **Logo.h**: Stores the logo bitmap in PROGMEM for display.
- **DisplayManager**: Handles all OLED drawing (logo, menus, text, game graphics).
- **InputManager**: Reads capacitive touch buttons and debounces input.
- **MenuSystem**: Manages menu navigation, selection, and menu rendering.
- **PowerManager**: Handles sleep/wake, charging detection, and display power state.
- **StorageManager**: Reads/writes persistent high score using FlashStorage.
- **BrickBreaker**: Implements the full Brick Breaker game (paddle, ball, bricks, scoring, high score).
- **Snake**: Implements the full Snake game (movement, food, scoring, high score).
- **BZ_CARD.ino**: Sets up all modules, defines menu structure, and launches games/logo from the menu.

---

## 📝 How to Extend or Customize
- **Add new menu items**: Edit the arrays in `BZ_CARD.ino` and update the menu logic.
- **Add new games**: Create a new game module, add to the menu, and wire up in `onMenuSelect()`.
- **Change logo**: Replace the bitmap in `Logo.h`.
- **Change pin mappings**: Update pin defines in `InputManager.cpp`.
- **Change display type**: Update the U8g2 constructor in `DisplayManager.h`/`.cpp`.

---

## 🛠️ Troubleshooting & Tips
- **All files must be in the same folder as `BZ_CARD.ino`** (no subfolders).
- **Missing library errors**: Install the required libraries via Library Manager.
- **Cannot open source file errors**: Ignore in VSCode, but Arduino IDE must have all files in the same folder.
- **Touch not working**: Check wiring and pin assignments in `InputManager.cpp`.
- **Display blank**: Check display wiring, power, and that the correct constructor is used in `DisplayManager`.
- **Game not launching**: Ensure `onMenuSelect()` in `BZ_CARD.ino` matches your menu structure.

---

## 📖 Code Walkthrough (Key Pieces)
- **Menu Navigation**: `MenuSystem` handles up/down/enter, and calls `onMenuSelect()` when an item is selected.
- **Game Launching**: `onMenuSelect()` in `BZ_CARD.ino` launches the correct game or logo.
- **Persistent High Score**: `StorageManager` uses FlashStorage to save/load scores.
- **Power Management**: `PowerManager` can be extended to handle deep sleep and charging screens.
- **Display Drawing**: `DisplayManager` provides all drawing primitives and menu rendering.

---

## 🧠 Further Help
If you want to:
- Add new features
- Integrate with IoT/cloud
- Add alerts or notifications
- Improve graphics or UX
Just ask for specific code or guidance! 