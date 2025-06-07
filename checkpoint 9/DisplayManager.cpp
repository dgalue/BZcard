#include "DisplayManager.h"

// Use original working U8g2 software SPI constructor and pin mapping
DisplayManager::DisplayManager()
    : u8g2(U8G2_R0, OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RESET), currentBrightness(128) {}

// Initialize the display (must be called in setup)
void DisplayManager::begin() {
    // Simple initialization that worked in the original code
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void DisplayManager::clear() {
    u8g2.clearBuffer();
}

void DisplayManager::display() {
    u8g2.sendBuffer();
}

void DisplayManager::drawLogo() {
    u8g2.clearBuffer();
    u8g2.drawXBMP(0, 17, 128, 30, bzcard_logo);
    u8g2.sendBuffer();
}

void DisplayManager::drawMenu(const char* title, const char* const* items, int numItems, int selectedIdx) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(0, 12, title);
    for (int i = 0; i < numItems; ++i) {
        if (i == selectedIdx) {
            u8g2.drawBox(0, 14 + i * 12, 128, 12);
            u8g2.setDrawColor(0);
            u8g2.drawStr(2, 24 + i * 12, items[i]);
            u8g2.setDrawColor(1);
        } else {
            u8g2.drawStr(2, 24 + i * 12, items[i]);
        }
    }
    u8g2.sendBuffer();
}

void DisplayManager::drawText(int x, int y, const char* text, uint8_t size) {
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(x, y, text);
}

void DisplayManager::updateMenuDisplay(int currentMenu, int currentOption, bool isMenuActive) {
    // Use the new slot machine style menu
    drawSlotMachineMenu(currentMenu, currentOption, isMenuActive);
}

void DisplayManager::showChargingScreen() {
    u8g2.setPowerSave(0);
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_helvB10_tr);
        u8g2.setCursor(20, 30);
        u8g2.print("Charging...");
        u8g2.drawFrame(45, 40, 30, 15);
        u8g2.drawBox(75, 45, 3, 5);
        unsigned long ms = millis() % 3000;
        int width = map(ms, 0, 3000, 2, 28);
        u8g2.drawBox(46, 41, width, 13);
    } while (u8g2.nextPage());
}

void DisplayManager::showLogo() {
    // Back to original implementation
    u8g2.firstPage();
    do {
        u8g2.drawXBMP(0, ((64-30)/2) + 5, 128, 30, bzcard_logo);
    } while (u8g2.nextPage());
}

void DisplayManager::showLoadingBar(int percentage) {
    // Ensure percentage is within valid range
    percentage = constrain(percentage, 0, 100);
    
    u8g2.setPowerSave(0);
    u8g2.firstPage();
    do {
        // Draw a title - using same font as menu system
        u8g2.setFont(u8g2_font_profont17_tr);
        int titleWidth = u8g2.getStrWidth("Loading...");
        u8g2.drawStr((128 - titleWidth) / 2, 20, "Loading...");
        
        // Draw percentage text - also using menu font for consistency
        char percentText[5];
        sprintf(percentText, "%d%%", percentage);
        int textWidth = u8g2.getStrWidth(percentText);
        u8g2.drawStr((128 - textWidth) / 2, 55, percentText);
        
        // Draw the progress bar frame
        u8g2.drawFrame(14, 25, 100, 15);
        
        // Calculate bar width based on percentage
        int barWidth = map(percentage, 0, 100, 0, 98);
        
        // Draw the progress bar fill
        u8g2.drawBox(15, 26, barWidth, 13);
        
    } while (u8g2.nextPage());
}

// Arrow and icon bitmaps from your design
static const unsigned char image_Layer_4_bits[] U8X8_PROGMEM = {0x18,0x3c,0x7e,0xff};
static const unsigned char image_Layer_6_bits[] U8X8_PROGMEM = {0xff,0x7e,0x3c,0x18};

// Your original icon bitmaps from the .txt files
static const unsigned char image_smartphone_bits[] U8X8_PROGMEM = {0xfe,0x00,0xff,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xff,0x01,0x45,0x01,0xfe,0x00};
static const unsigned char image_crosshairs_bits[] U8X8_PROGMEM = {0x80,0x00,0xc0,0x01,0xb0,0x06,0x88,0x08,0x04,0x10,0x04,0x10,0x82,0x20,0x4f,0x79,0x82,0x20,0x04,0x10,0x04,0x10,0x88,0x08,0xb0,0x06,0xc0,0x01,0x80,0x00,0x00,0x00};
static const unsigned char image_link_bits[] U8X8_PROGMEM = {0x00,0x1e,0x00,0x3f,0x80,0x73,0xc0,0x61,0xc0,0x60,0x60,0x70,0x68,0x39,0x6c,0x1b,0x4e,0x0b,0x07,0x03,0x83,0x01,0xc3,0x01,0xe7,0x00,0x7e,0x00,0x3c,0x00,0x00,0x00};
static const unsigned char image_folder_file_bits[] U8X8_PROGMEM = {0xf8,0x3f,0x00,0x08,0x20,0x00,0xe8,0x2f,0x00,0x0e,0x20,0x00,0xe9,0x2f,0x00,0x09,0x60,0x00,0xe9,0xaf,0x00,0x09,0xa0,0x00,0xe9,0xff,0x03,0x19,0x00,0x02,0x19,0x00,0x02,0x09,0x00,0x01,0x09,0x00,0x01,0x05,0x80,0x00,0x05,0x80,0x00,0xfe,0x7f,0x00};
static const unsigned char image_menu_tools_bits[] U8X8_PROGMEM = {0x01,0x07,0x83,0x06,0x42,0x01,0x44,0x31,0xc8,0x30,0x50,0x2d,0x60,0x12,0xa0,0x0f,0xd0,0x00,0x28,0x07,0x94,0x0d,0x0a,0x1b,0x05,0x36,0x03,0x2c,0x00,0x38,0x00,0x00};

void DisplayManager::drawSlotMachineMenu(int currentMenu, int currentOption, bool isMenuActive) {
    // If in submenu mode, use the submenu display
    if (isMenuActive) {
        drawSlotMachineSubmenu(currentMenu, currentOption);
        return;
    }
    
    // Clear buffer and set modes
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    
    // Menu titles and layout data
    const char* menuTitles[] = {"Contact Inf.", "Social Med.", "Projects", "Games", "Options"};
    const char* fullTitles[] = {"Contact Info", "Social Media", "Projects", "Games", "Options"};
    
    // Calculate previous and next menus
    int prevMenu = (currentMenu - 1 + 5) % 5;
    int nextMenu = (currentMenu + 1) % 5;
    
    // Layer 1: Frame around current menu selection
    u8g2.drawFrame(0, 19, 128, 24);
    
    // Layer 2: Main menu title (current selection) - centered based on your .txt coordinates
    u8g2.setFont(u8g2_font_profont17_tr);
    int mainX = 20; // Default
    switch (currentMenu) {
        case 0: mainX = 20; break; // "Contact Inf."
        case 1: mainX = 24; break; // "Social Med."
        case 2: mainX = 28; break; // "Projects"
        case 3: mainX = 42; break; // "Games"
        case 4: mainX = 32; break; // "Options"
    }
    u8g2.drawStr(mainX, 37, menuTitles[currentMenu]);
    
    // Layer 3: Previous menu option (above) - centered based on your .txt coordinates
    u8g2.setFont(u8g2_font_6x10_tr);
    int prevX = 31; // Default
    switch (prevMenu) {
        case 0: prevX = 28; break; // "Contact Inf."
        case 1: prevX = 31; break; // "Social Med."
        case 2: prevX = 40; break; // "Projects"
        case 3: prevX = 49; break; // "Games"
        case 4: prevX = 43; break; // "Options"
    }
    u8g2.drawStr(prevX, 14, menuTitles[prevMenu]);
    
    // Layer 4: Up arrow
    u8g2.drawXBMP(8, 9, 8, 4, image_Layer_4_bits);
    
    // Layer 5: Next menu option (below) - centered based on your .txt coordinates
    u8g2.setFont(u8g2_font_6x10_tr);
    int nextX = 31; // Default
    switch (nextMenu) {
        case 0: nextX = 28; break; // "Contact Inf."
        case 1: nextX = 31; break; // "Social Med."
        case 2: nextX = 40; break; // "Projects"
        case 3: nextX = 49; break; // "Games"
        case 4: nextX = 43; break; // "Options"
    }
    u8g2.drawStr(nextX, 57, menuTitles[nextMenu]);
    
    // Layer 6: Down arrow
    u8g2.drawXBMP(8, 51, 8, 4, image_Layer_6_bits);
    
    // Layer 7: Icon for current menu
    switch (currentMenu) {
        case 0: // Contact Info
            u8g2.drawXBMP(7, 23, 9, 16, image_smartphone_bits);
            break;
        case 1: // Social Media
            u8g2.drawXBMP(4, 23, 15, 16, image_link_bits);
            break;
        case 2: // Projects
            u8g2.drawXBMP(4, 23, 18, 16, image_folder_file_bits);
            break;
        case 3: // Games
            u8g2.drawXBMP(5, 23, 15, 16, image_crosshairs_bits);
            break;
        case 4: // Options
            u8g2.drawXBMP(6, 23, 16, 16, image_menu_tools_bits);
            break;
    }
    
    // Layer 8: Outer rounded frame
    u8g2.drawRFrame(0, 0, 128, 64, 3);
    
    u8g2.sendBuffer();
}

void DisplayManager::drawSlotMachineSubmenu(int currentMenu, int currentOption) {
    // Skip Options menu - it uses the single-screen format instead
    if (currentMenu == 4) {
        return; // Options menu handled separately
    }
    
    // Clear buffer and set modes
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    
    // Get current menu options
    const char* const* options;
    int optionCount;
    
    // Contact Info submenu
    static const char* contactOptions[] = {"Name", "Title", "Email", "Phone"};
    
    // Social Media submenu  
    static const char* socialOptions[] = {"Website", "GitHub", "LinkedIn"};
    
    // Projects submenu
    static const char* projectOptions[] = {"BZcard", "Project 2", "Project 3", "More..."};
    
    // Games submenu
    static const char* gameOptions[] = {"Brick Breaker", "Snake", "Show Logo"};
    
    switch (currentMenu) {
        case 0: // Contact Info
            options = contactOptions; 
            optionCount = 4;
            break;
        case 1: // Social Media
            options = socialOptions; 
            optionCount = 3;
            break;
        case 2: // Projects
            options = projectOptions; 
            optionCount = 4;
            break;
        case 3: // Games
            options = gameOptions; 
            optionCount = 3;  // Updated count for 3 options
            break;
        default:
            options = contactOptions;
            optionCount = 4;
    }
    
    // Calculate previous and next options (with wrapping)
    int prevOption = (currentOption - 1 + optionCount) % optionCount;
    int nextOption = (currentOption + 1) % optionCount;
    
    // Layer 1: Frame around current option selection
    u8g2.drawFrame(0, 19, 128, 24);
    
    // Layer 2: Current option title (main selection)
    u8g2.setFont(u8g2_font_profont17_tr);
    int titleWidth = u8g2.getStrWidth(options[currentOption]);
    u8g2.drawStr((128 - titleWidth) / 2, 37, options[currentOption]);
    
    // Layer 3: Previous option (above) 
    u8g2.setFont(u8g2_font_6x10_tr);
    int prevWidth = u8g2.getStrWidth(options[prevOption]);
    u8g2.drawStr((128 - prevWidth) / 2, 14, options[prevOption]);
    
    // Layer 4: Up arrow
    u8g2.drawXBMP(8, 9, 8, 4, image_Layer_4_bits);
    
    // Layer 5: Next option (below)
    u8g2.setFont(u8g2_font_6x10_tr);
    int nextWidth = u8g2.getStrWidth(options[nextOption]);
    u8g2.drawStr((128 - nextWidth) / 2, 57, options[nextOption]);
    
    // Layer 6: Down arrow
    u8g2.drawXBMP(8, 51, 8, 4, image_Layer_6_bits);
    
    // Layer 7: Outer rounded frame (no icons in submenu)
    u8g2.drawRFrame(0, 0, 128, 64, 3);
    
    u8g2.sendBuffer();
}

// Brightness control methods
void DisplayManager::setBrightness(int brightness) {
    currentBrightness = constrain(brightness, 0, 255);
    u8g2.setContrast(currentBrightness);
}

int DisplayManager::getBrightness() {
    return currentBrightness;
}

// Options menu display
void DisplayManager::drawOptionsMenu(int currentOption, int brightness, unsigned long displayTimeout, unsigned long sleepTimeout, int rtcWakeInterval) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tr);
    
    // Title
    u8g2.drawStr(35, 12, "OPTIONS");
    
    // Options list
    const char* optionNames[] = {"Brightness", "Display Off", "Sleep Time", "RTC Wake"};
    
    for (int i = 0; i < 4; i++) {
        int y = 24 + i * 10;
        
        // Highlight current option
        if (i == currentOption) {
            u8g2.drawBox(0, y - 8, 128, 10);
            u8g2.setDrawColor(0);
        }
        
        // Draw option name
        u8g2.drawStr(2, y, optionNames[i]);
        
        // Draw current value
        char valueStr[20];
        switch (i) {
            case 0: // Brightness
                sprintf(valueStr, "%d%%", map(brightness, 0, 255, 0, 100));
                break;
            case 1: // Display timeout
                sprintf(valueStr, "%lus", displayTimeout / 1000);
                break;
            case 2: // Sleep timeout
                sprintf(valueStr, "%lus", sleepTimeout / 1000);
                break;
            case 3: // RTC wake interval
                sprintf(valueStr, "%ds", rtcWakeInterval);
                break;
        }
        
        int valueWidth = u8g2.getStrWidth(valueStr);
        u8g2.drawStr(128 - valueWidth - 2, y, valueStr);
        
        if (i == currentOption) {
            u8g2.setDrawColor(1);
        }
    }
    
    u8g2.sendBuffer();
}

// Simplified brightness-only menu
void DisplayManager::drawBrightnessMenu(int brightness) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tr);
    
    // Title
    u8g2.drawStr(40, 15, "BRIGHTNESS");
    
    // Current brightness percentage - now using steps of 10 from 10 to 100
    int brightnessPercent = map(brightness, 26, 255, 10, 100); // Map 26-255 to 10-100%
    char brightnessStr[10];
    sprintf(brightnessStr, "%d%%", brightnessPercent);
    
    // Larger brightness value display with bigger font - moved up for better spacing
    u8g2.setFont(u8g2_font_profont22_tr); // Bigger font for better readability
    int textWidth = u8g2.getStrWidth(brightnessStr);
    u8g2.drawStr((128 - textWidth) / 2, 35, brightnessStr);
    
    // Progress bar - moved up to avoid crowding
    u8g2.drawFrame(10, 42, 108, 8);
    int barWidth = map(brightness, 26, 255, 0, 106); // Map to progress bar width
    u8g2.drawBox(11, 43, barWidth, 6);
    
    // Removed instruction text - controls are intuitive
    
    u8g2.sendBuffer();
}



 