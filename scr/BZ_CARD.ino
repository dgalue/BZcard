#include <Arduino.h>
#include "DisplayManager.h"
#include "InputManager.h"
#include "MenuSystem.h"
#include "PowerManager.h"
#include "StorageManager.h"
#include "BrickBreaker.h"
#include "Snake.h"

// --- State and Menu Variables (from original) ---
int currentMenu = 0;
int currentOption = 0;
const int totalMenus = 5;  // Added Options menu
bool isMenuActive = false;
bool inOptionsMenu = false;  // Track if we're in options submenu
bool showingSocialLink = false;  // Track if we're displaying a social media link
bool showingContactInfo = false;  // Track if we're displaying contact information
bool displayOn = true;
unsigned long lastActivityTime = 0;
unsigned long wakeupTime = 0;           // Tracks when we woke up from sleep
bool graceActive = false;               // Whether we're in the grace period after wakeup
bool startupSequenceShown = false;     // Flag to prevent multiple startup sequences

// Timeout values
const unsigned long displayOffTimeout = 60000;         // 1 minute to turn off display
const unsigned long deepSleepTimeout = 120000;         // 2 minutes to enter deep sleep
const unsigned long wakeupGracePeriod = 100;          // 100ms grace period after wakeup

// Key repeat settings
const unsigned long keyRepeatDelay = 500;    // Initial delay before repeating (ms)
const unsigned long keyRepeatInterval = 300;  // Interval between repeats (ms)

// Button state tracking
bool prevUpState = false;
bool prevDownState = false;
bool prevEnterState = false;
bool prevPowerState = false;
unsigned long lastUpPressTime = 0;
unsigned long lastDownPressTime = 0;
unsigned long lastEnterPressTime = 0;
unsigned long lastPowerWakeTime = 0;

// Options menu button state tracking (separate from main menu)
bool optionsPrevUpState = false;
bool optionsPrevDownState = false;
bool optionsPrevEnterState = false;
bool optionsPrevPowerState = false;

// --- Menu Data ---
const char* menuTitles[] = {"Contact Info", "Social Media", "Projects", "Games", "Options"};
const char* contactOptions[] = {"Name", "Title", "Email", "Phone"};
const char* socialOptions[] = {"Website", "GitHub", "LinkedIn"};
const char* projectOptions[] = {"BZcard", "Project 2", "Project 3", "More..."};
const char* gameOptions[] = {"Brick Breaker", "Snake", "Show Logo"};
const char* optionsOptions[] = {"Brightness", "Display Off", "Sleep Time", "RTC Wake"};
const int optionCounts[] = {4, 3, 4, 3, 4};  // Updated games count to 3

// --- Module Instances ---
DisplayManager displayManager;
InputManager inputManager;
StorageManager storageManager;
MenuSystem menuSystem(displayManager, inputManager);
PowerManager powerManager;
BrickBreaker brickBreaker(displayManager, inputManager, storageManager);
Snake snake(displayManager, inputManager, storageManager);

// --- Function Prototypes ---
void updateMenuDisplay();
void handleNormalOperation();
void handleOptionsMenu();
void showStartupSequence();
void resetAllStates();

void onMenuSelect(int idx) {
    if (currentMenu == 3) { // Games menu
        switch (idx) {
            case 0: brickBreaker.play(); break;
            case 1: snake.play(); break;
            case 2: displayManager.showLogo(); delay(2000); break;
        }
        menuSystem.showMenu();
    } else if (currentMenu == 4) { // Options menu
        inOptionsMenu = true;
        // Options will be handled in the main loop
    }
    // Add other menu actions as needed
}

void setup() {
    Serial.begin(115200);
    
    // Initialize all modules
    displayManager.begin();
    inputManager.begin();
    storageManager.begin();
    powerManager.begin();
    
    // Load settings and configure modules
    UserSettings settings = storageManager.getSettings();
    displayManager.setBrightness(settings.brightness);
    powerManager.updateSettingsFromStorage(storageManager);
    
    // Initialize state variables
    displayOn = true;
    graceActive = false;
    startupSequenceShown = false;
    inOptionsMenu = false;
    
    // Show startup sequence with logo and loading bar
    Serial.println("Starting - showing startup sequence");
    showStartupSequence();
    startupSequenceShown = true;
    
    // Initialize activity timers
    lastActivityTime = millis();
    powerManager.resetActivityTimer();
}

void loop() {
    // Update input states
    inputManager.update();
    
    // Suppress RTC display when in deep sleep cycle
    if (powerManager.isInDeepSleepCycle()) {
        powerManager.setSuppressRtcDisplay(true);
        } else {
        powerManager.setSuppressRtcDisplay(false);
    }
    
    // Update power manager
    powerManager.update();
    
    // Get current display state from power manager
    displayOn = powerManager.isDisplayOn();
    
    // Check if we're in a deep sleep cycle
    if (powerManager.isInDeepSleepCycle()) {
        // Force display off during deep sleep
        displayManager.u8g2.setPowerSave(1);
      displayOn = false;
        
        // Check if power button is pressed (only use power button to wake up)
        if (inputManager.powerPressed()) {
            // Power button pressed, exit deep sleep cycle
            Serial.println("Power button pressed, exiting deep sleep cycle");
            powerManager.exitDeepSleepCycle();
            
            // Reset all states after wake-up
            resetAllStates();
            
            // Turn on display and show startup sequence (only once)
            displayOn = true;
            displayManager.u8g2.setPowerSave(0);
            
            if (!startupSequenceShown) {
                showStartupSequence();
                startupSequenceShown = true;
      } else {
                // Just show the menu directly to avoid multiple sequences
                displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            }
            
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
    return;
  }
  
        // Check if we're in the grace period after wakeup
        if (graceActive) {
            // Keep display off during grace period
            displayManager.u8g2.setPowerSave(1);
            
            // If grace period has expired, check for power button
            if (millis() - wakeupTime > wakeupGracePeriod) {
                graceActive = false;
                
                // Only check for power button, not any button
                if (inputManager.powerPressed()) {
                    // Power button pressed, exit deep sleep cycle
                    Serial.println("Power button pressed during grace period, exiting deep sleep");
                    powerManager.exitDeepSleepCycle();
                    
                    // Reset all states after wake-up
                    resetAllStates();
                    
                    // Turn on display and show startup sequence (only once)
  displayOn = true;
                    displayManager.u8g2.setPowerSave(0);
                    
                    if (!startupSequenceShown) {
                        showStartupSequence();
                        startupSequenceShown = true;
  } else {
                        // Just show the menu directly to avoid multiple sequences
                        displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
  }
  
  lastActivityTime = millis();
                    powerManager.resetActivityTimer();
  } else {
                    // No power button press, go back to sleep
                    // Just enter deep sleep directly without displaying anything
                    displayManager.u8g2.setPowerSave(1);
                    powerManager.enterDeepSleep();
                }
            }
        } else {
            // We just woke from sleep - start grace period
            wakeupTime = millis();
            graceActive = true;
            
            // Keep display off during grace period
            displayOn = false;
            displayManager.u8g2.setPowerSave(1);
            
            Serial.println("Starting grace period after wakeup");
        }
        
        return; // Exit loop early
    } else {
        // Reset grace period flag when not in deep sleep cycle
        graceActive = false;
        // Reset startup sequence flag when awake and active
        startupSequenceShown = false;
    }
    
    // Normal operation when awake
    if (displayOn) {
        handleNormalOperation();
    }
    
    // CHECK FOR POWER BUTTON TO WAKE DISPLAY (between 1-2 minutes)
    // This check happens when the display is already off but the device
    // hasn't yet entered deep sleep (after the display timeout, before the
    // sleep timeout). It allows the user to wake the screen back up.
    if (!displayOn && !powerManager.isInDeepSleepCycle()) {
        if (inputManager.powerPressed() && millis() - lastPowerWakeTime > 1000) { // 1 second debounce
            Serial.println("Power button pressed - turning display back on");
            displayOn = true;
            powerManager.setDisplayOn(true); // Update PowerManager state
            displayManager.u8g2.setPowerSave(0);
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
            lastPowerWakeTime = millis(); // Update debounce timer
        }
    }
    
    // AUTOMATIC POWER MANAGEMENT
    // Track display state to only show message once when it changes
    static bool wasDisplayOn = true;
    static bool messageShown = false;
    
    // Turn display off after displayOffTimeout (use PowerManager setting)
    if (displayOn && millis() - lastActivityTime > powerManager.getDisplayTimeout()) {
        // Only print message if we haven't already shown it for this off cycle
        if (!messageShown) {
            Serial.println("Display turning off due to inactivity");
            messageShown = true;
        }
        
        displayOn = false;
        powerManager.setDisplayOn(false); // Update PowerManager state
        displayManager.u8g2.setPowerSave(1);
    }
    
    // Reset the message flag when display turns back on
    if (displayOn && !wasDisplayOn) {
        messageShown = false;
    }
    
    // Track previous state
    wasDisplayOn = displayOn;
    
    // Enter deep sleep after deepSleepTimeout - only check occasionally
    static unsigned long lastSleepCheck = 0;
    static bool sleepMessageShown = false;
    
    if (millis() - lastActivityTime > powerManager.getSleepTimeout()) {
        // Don't check too frequently
        if (millis() - lastSleepCheck > 5000) {
            lastSleepCheck = millis();
            
            // Only show message once per sleep cycle
            if (!sleepMessageShown) {
                Serial.println("DEEP SLEEP: Entering deep sleep due to inactivity");
                sleepMessageShown = true;
            }
            
            displayOn = false;
            displayManager.u8g2.setPowerSave(1);
            powerManager.enterDeepSleep(); // Auto sleep (not forced)
        }
    } else {
        // Reset the message flag when activity happens
        sleepMessageShown = false;
    }
    
    // Small delay to reduce CPU usage
    delay(10);
  }
  
void handleNormalOperation() {
    // Check if we're in options menu mode
    if (inOptionsMenu) {
        handleOptionsMenu();
        return;
    }
    
    // Check if we're showing a social media link
    if (showingSocialLink) {
        // Only handle ENTER to go back, but wait for button to be released first
        bool currentEnterState = inputManager.enterPressed();
        if (currentEnterState && !prevEnterState) {
            Serial.println("ENTER pressed - returning to menu");
            showingSocialLink = false;
            isMenuActive = true; // Return to submenu, not main menu
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
        }
        prevEnterState = currentEnterState;  // Always update the state
        return;
    }
    
    // Check if we're showing contact information
    if (showingContactInfo) {
        // Only handle ENTER to go back, but wait for button to be released first
        bool currentEnterState = inputManager.enterPressed();
        if (currentEnterState && !prevEnterState) {
            Serial.println("ENTER pressed - returning to menu");
            showingContactInfo = false;
            isMenuActive = true; // Return to submenu, not main menu
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
        }
        prevEnterState = currentEnterState;  // Always update the state
        return;
    }
    
    // MENU NAVIGATION
    // UP button with key repeat
    bool currentUpState = inputManager.upPressed();
    if (currentUpState) {
        if (!prevUpState || (millis() - lastUpPressTime > keyRepeatDelay && 
                            millis() - lastUpPressTime - keyRepeatDelay > keyRepeatInterval)) {
            // Initial press or repeat interval reached
            lastUpPressTime = millis();
  
            if (isMenuActive) {
                if (currentOption > 0) currentOption--;
                else currentOption = optionCounts[currentMenu] - 1;
            } else {
                if (currentMenu > 0) currentMenu--;
                else currentMenu = totalMenus - 1;
                // Ensure currentOption is within bounds for the new menu
                if (currentOption >= optionCounts[currentMenu]) {
                    currentOption = 0; // Reset to first option if out of bounds
                }
            }
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
        }
    } else {
        // Button released, reset time
        if (prevUpState) lastUpPressTime = 0;
    }
    prevUpState = currentUpState;
    
    // DOWN button with key repeat
    bool currentDownState = inputManager.downPressed();
    if (currentDownState) {
        if (!prevDownState || (millis() - lastDownPressTime > keyRepeatDelay && 
                              millis() - lastDownPressTime - keyRepeatDelay > keyRepeatInterval)) {
            // Initial press or repeat interval reached
            lastDownPressTime = millis();
  
            if (isMenuActive) {
                if (currentOption < optionCounts[currentMenu] - 1) currentOption++;
                else currentOption = 0;
            } else {
                if (currentMenu < totalMenus - 1) currentMenu++;
                else currentMenu = 0;
                // Ensure currentOption is within bounds for the new menu
                if (currentOption >= optionCounts[currentMenu]) {
                    currentOption = 0; // Reset to first option if out of bounds
                }
            }
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
        }
    } else {
        // Button released, reset time
        if (prevDownState) lastDownPressTime = 0;
    }
    prevDownState = currentDownState;
    
    // ENTER button with key repeat (for menu navigation only)
    bool currentEnterState = inputManager.enterPressed();
    if (currentEnterState) {
        if (!prevEnterState) {
            // Process only on initial press, not on repeat
            lastEnterPressTime = millis();
  
            if (isMenuActive) {
                if (currentMenu == 3) { // Games menu
                    switch (currentOption) {
                        case 0: brickBreaker.play(); break;
                        case 1: snake.play(); break;
                        case 2: displayManager.showLogo(); delay(2000); break;
                    }
                    isMenuActive = false;
                } else if (currentMenu == 0) { // Contact Info menu
                    // Display the actual contact information
                    displayManager.u8g2.clearBuffer();
                    displayManager.u8g2.setFont(u8g2_font_6x12_tr);
                    
                    switch (currentOption) {
                        case 0: // Name
                            displayManager.u8g2.drawStr(10, 20, "NAME");
                            displayManager.u8g2.drawStr(10, 35, "Diego Galue");
                            break;
                        case 1: // Title
                            displayManager.u8g2.drawStr(10, 15, "TITLE");
                            displayManager.u8g2.drawStr(10, 30, "M.Sc. Electronics");
                            displayManager.u8g2.drawStr(10, 45, "Tinkerer & Maker");
                            break;
                        case 2: // Email
                            displayManager.u8g2.drawStr(10, 20, "EMAIL");
                            displayManager.u8g2.drawStr(10, 35, "diego.galue@");
                            displayManager.u8g2.drawStr(10, 50, "diegogalue.tech");
                            break;
                        case 3: // Phone
                            displayManager.u8g2.drawStr(10, 20, "PHONE");
                            displayManager.u8g2.drawStr(10, 35, "+49-179-930-5391");
                            break;
                    }
                    
                    displayManager.u8g2.sendBuffer();
                    showingContactInfo = true;  // Set flag to handle return
                    isMenuActive = false;
                    // Set prevEnterState to current state to wait for button release then press
                    prevEnterState = inputManager.enterPressed();
                    
                    lastActivityTime = millis();
                    powerManager.resetActivityTimer();
                    return; // Exit immediately to prevent further processing
                } else if (currentMenu == 1) { // Social Media menu
                    // Display the actual links
                    displayManager.u8g2.clearBuffer();
                    displayManager.u8g2.setFont(u8g2_font_6x12_tr);
                    
                    switch (currentOption) {
                        case 0: // Website
                            displayManager.u8g2.drawStr(10, 20, "WEBSITE");
                            displayManager.u8g2.drawStr(10, 35, "diegogalue.tech");
                            break;
                        case 1: // GitHub
                            displayManager.u8g2.drawStr(10, 20, "GITHUB");
                            displayManager.u8g2.drawStr(10, 35, "github.com/dgalue");
                            break;
                        case 2: // LinkedIn
                            displayManager.u8g2.drawStr(10, 20, "LINKEDIN");
                            displayManager.u8g2.drawStr(10, 35, "linkedin.com/in/");
                            displayManager.u8g2.drawStr(10, 50, "diegogalue");
                            break;
                    }
                    
                    displayManager.u8g2.sendBuffer();
                    showingSocialLink = true;  // Set flag to handle return
                    isMenuActive = false;
                    // Set prevEnterState to current state to wait for button release then press
                    prevEnterState = inputManager.enterPressed();
                    
                    lastActivityTime = millis();
                    powerManager.resetActivityTimer();
                    return; // Exit immediately to prevent further processing
                } else {
                    isMenuActive = !isMenuActive;
                }
            } else {
                // Handle Options menu directly without intermediate isMenuActive state
                if (currentMenu == 4) { // Options menu
                    inOptionsMenu = true;
                    currentOption = 0; // Reset to first option in Options
                } else {
                    isMenuActive = !isMenuActive;
                    // Ensure currentOption is within bounds for the selected menu
                    if (currentOption >= optionCounts[currentMenu]) {
                        currentOption = 0; // Reset to first option if out of bounds
                    }
                }
            }
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
        }
    }
    prevEnterState = currentEnterState;
    
    // POWER button handling - exit submenus back to main menu
    bool currentPowerState = inputManager.powerPressed();
    if (currentPowerState && !prevPowerState) {
        if (isMenuActive) {
            // Exit submenu back to main menu
            isMenuActive = false;
            displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
            lastActivityTime = millis();
            powerManager.resetActivityTimer();
        }
    }
    prevPowerState = currentPowerState;
}

// Handle options menu navigation and value changes
void handleOptionsMenu() {
    // Get current settings
    UserSettings settings = storageManager.getSettings();
    
    // Display the brightness control (single option)
    displayManager.drawBrightnessMenu(settings.brightness);
    
    // Handle UP button to increase brightness
    bool currentUpState = inputManager.upPressed();
    if (currentUpState && !optionsPrevUpState) {
        // Increase brightness by 10% (steps of 10 from 10% to 100%)
        // Current range: 26-255 maps to 10%-100%
        int currentPercent = map(settings.brightness, 26, 255, 10, 100);
        currentPercent += 10;
        if (currentPercent > 100) currentPercent = 10; // Wrap around to 10%
        
        settings.brightness = map(currentPercent, 10, 100, 26, 255);
        displayManager.setBrightness(settings.brightness);
        storageManager.saveSettings(settings);
        
        lastActivityTime = millis();
        powerManager.resetActivityTimer();
    }
    optionsPrevUpState = currentUpState;
    
    // Handle DOWN button to decrease brightness  
    bool currentDownState = inputManager.downPressed();
    if (currentDownState && !optionsPrevDownState) {
        // Decrease brightness by 10% (steps of 10 from 10% to 100%)
        // Current range: 26-255 maps to 10%-100%
        int currentPercent = map(settings.brightness, 26, 255, 10, 100);
        currentPercent -= 10;
        if (currentPercent < 10) currentPercent = 100; // Wrap around to 100%
        
        settings.brightness = map(currentPercent, 10, 100, 26, 255);
        displayManager.setBrightness(settings.brightness);
        storageManager.saveSettings(settings);
        
        lastActivityTime = millis();
        powerManager.resetActivityTimer();
    }
    optionsPrevDownState = currentDownState;
    
    // Handle ENTER button to exit options menu
    bool currentEnterState = inputManager.enterPressed();
    if (currentEnterState && !optionsPrevEnterState) {
        inOptionsMenu = false;
        currentOption = 0; // Reset to first option
        displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
        lastActivityTime = millis();
        powerManager.resetActivityTimer();
    }
    optionsPrevEnterState = currentEnterState;
    
    // Handle POWER button to exit options menu (backup exit method)
    bool currentPowerState = inputManager.powerPressed();
    if (currentPowerState && !optionsPrevPowerState) {
        inOptionsMenu = false;
        currentOption = 0; // Reset to first option
        displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
        lastActivityTime = millis();
        powerManager.resetActivityTimer();
    }
    optionsPrevPowerState = currentPowerState;
}

// Show startup sequence with logo and loading bar
void showStartupSequence() {
    // Show logo for 2 seconds
    displayManager.showLogo();
            delay(2000);
    
    // Show loading bar for 3 seconds with increasing percentage
    unsigned long startTime = millis();
    while (millis() - startTime < 3000) {
        int elapsed = millis() - startTime;
        int percentage = map(elapsed, 0, 3000, 0, 100);
        displayManager.showLoadingBar(percentage);
        delay(30); // Small delay to avoid updating too frequently
    }
    
    // Finally show the menu
    displayManager.updateMenuDisplay(currentMenu, currentOption, isMenuActive);
}

// Reset all state variables after wake-up to ensure proper operation
void resetAllStates() {
    Serial.println("Resetting all states after wake-up");
    
    // Reset button states
    prevUpState = false;
    prevDownState = false;
    prevEnterState = false;
    prevPowerState = false;
    
    // Reset button timing
    lastUpPressTime = 0;
    lastDownPressTime = 0;
    lastEnterPressTime = 0;
    lastPowerWakeTime = 0;
    
    // Reset menu states
    currentMenu = 0;
    currentOption = 0;
    isMenuActive = false;
    inOptionsMenu = false;
    showingSocialLink = false;
    showingContactInfo = false;
    
    // Reset flags
    graceActive = false;
    
    // Reinitialize input manager to ensure touch sensors work
    inputManager.begin();
    
    // Reset options menu states
    optionsPrevUpState = false;
    optionsPrevDownState = false;
    optionsPrevEnterState = false;
    optionsPrevPowerState = false;
    
    Serial.println("All states reset - menu should be responsive");
}
