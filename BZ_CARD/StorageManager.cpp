#include "StorageManager.h"

FlashStorage(highScoreStorage, int);
FlashStorage(settingsStorage, UserSettings);

StorageManager::StorageManager() {}

void StorageManager::begin() {
    loadSettings();
}

int StorageManager::getHighScore() {
    int score = highScoreStorage.read();
    if (score < 0 || score > 9999) return 0;
    return score;
}

void StorageManager::saveHighScore(int score) {
    highScoreStorage.write(score);
}

// Settings methods implementation
UserSettings StorageManager::getSettings() {
    return currentSettings;
}

void StorageManager::saveSettings(const UserSettings& settings) {
    currentSettings = settings;
    settingsStorage.write(settings);
}

void StorageManager::resetSettingsToDefault() {
    currentSettings = getDefaultSettings();
    saveSettings(currentSettings);
}

// Individual setting accessors
int StorageManager::getBrightness() {
    return currentSettings.brightness;
}

void StorageManager::setBrightness(int brightness) {
    currentSettings.brightness = constrain(brightness, 0, 255);
    saveSettings(currentSettings);
}

unsigned long StorageManager::getDisplayTimeout() {
    return currentSettings.displayTimeout;
}

void StorageManager::setDisplayTimeout(unsigned long timeout) {
    currentSettings.displayTimeout = timeout;
    saveSettings(currentSettings);
}

unsigned long StorageManager::getSleepTimeout() {
    return currentSettings.sleepTimeout;
}

void StorageManager::setSleepTimeout(unsigned long timeout) {
    currentSettings.sleepTimeout = timeout;
    saveSettings(currentSettings);
}

int StorageManager::getRtcWakeInterval() {
    return currentSettings.rtcWakeInterval;
}

void StorageManager::setRtcWakeInterval(int interval) {
    currentSettings.rtcWakeInterval = constrain(interval, 1, 300); // 1 second to 5 minutes
    saveSettings(currentSettings);
}

// Private methods
void StorageManager::loadSettings() {
    currentSettings = settingsStorage.read();
    
    // Check if settings are valid/initialized
    if (!currentSettings.valid) {
        // First time or corrupted settings, use defaults
        currentSettings = getDefaultSettings();
        saveSettings(currentSettings);
    }
}

UserSettings StorageManager::getDefaultSettings() {
    UserSettings defaults;
    defaults.brightness = 128;           // Medium brightness
    defaults.displayTimeout = 60000;    // 1 minute
    defaults.sleepTimeout = 120000;     // 2 minutes
    defaults.rtcWakeInterval = 5;       // 5 seconds
    defaults.valid = true;
    return defaults;
} 