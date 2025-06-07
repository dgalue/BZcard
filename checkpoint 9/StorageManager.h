#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <FlashStorage.h>

// Settings structure to store in flash
struct UserSettings {
    int brightness;          // Display brightness (0-255)
    unsigned long displayTimeout;  // Display off timeout in milliseconds
    unsigned long sleepTimeout;    // Deep sleep timeout in milliseconds
    int rtcWakeInterval;     // RTC wake interval in seconds
    bool valid;              // Flag to check if settings are initialized
};

class StorageManager {
public:
    StorageManager();
    void begin();
    
    // High score methods
    int getHighScore();
    void saveHighScore(int score);
    
    // Settings methods
    UserSettings getSettings();
    void saveSettings(const UserSettings& settings);
    void resetSettingsToDefault();
    
    // Individual setting accessors
    int getBrightness();
    void setBrightness(int brightness);
    unsigned long getDisplayTimeout();
    void setDisplayTimeout(unsigned long timeout);
    unsigned long getSleepTimeout();
    void setSleepTimeout(unsigned long timeout);
    int getRtcWakeInterval();
    void setRtcWakeInterval(int interval);

private:
    UserSettings currentSettings;
    void loadSettings();
    UserSettings getDefaultSettings();
};

#endif // STORAGE_MANAGER_H 