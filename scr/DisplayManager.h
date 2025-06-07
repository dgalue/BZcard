#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <U8g2lib.h>
#include "Logo.h"

// Pin definitions for software SPI (match original working code)
#define OLED_CLK 8     // Restored to original value
#define OLED_MOSI 7
#define OLED_CS 12
#define OLED_DC 10    // Restored to original value
#define OLED_RESET 11

class DisplayManager {
public:
    DisplayManager();
    void begin();
    void clear();
    void display();
    void drawLogo();
    void drawMenu(const char* title, const char* const* items, int numItems, int selectedIdx);
    void drawText(int x, int y, const char* text, uint8_t size = 1);
    void updateMenuDisplay(int currentMenu, int currentOption, bool isMenuActive);
    void showChargingScreen();
    void showLogo();
    void showLoadingBar(int percentage); // Show a loading bar with percentage
    
    // Enhanced slot machine-style menu methods
    void drawSlotMachineMenu(int currentMenu, int currentOption, bool isMenuActive);
    void drawSlotMachineSubmenu(int currentMenu, int currentOption);
    
    // Brightness control methods
    void setBrightness(int brightness);  // Set brightness (0-255)
    int getBrightness();                // Get current brightness
    void drawOptionsMenu(int currentOption, int brightness, unsigned long displayTimeout, unsigned long sleepTimeout, int rtcWakeInterval);
    void drawBrightnessMenu(int brightness); // Simplified brightness-only menu

    U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2;

private:
    int currentBrightness;
};

#endif // DISPLAY_MANAGER_H 