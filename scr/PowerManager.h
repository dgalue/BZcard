#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

// Include RTCZero only for SAMD architecture
#ifdef ARDUINO_ARCH_SAMD
  #include <RTCZero.h>
#endif

// Enable USB debugging by default to help troubleshoot
#define USB_DEBUG

// Default power management settings (can be overridden by StorageManager)
#define DEFAULT_RTC_WAKE_SECONDS 5      // Wake up every 5 seconds to check touch
#define DEFAULT_INACTIVITY_TIMEOUT 60   // Go to sleep after 60 seconds of inactivity
#define TOUCH_THRESHOLD 600             // Threshold for touch detection

// Hardware pin definitions
// No dedicated power button in current prototype

class DisplayManager;
class InputManager;
class StorageManager;

class PowerManager {
public:
    PowerManager();
    void begin();
    void update();
    void enterDeepSleep(bool forceDeepSleep = false);
    void exitDeepSleepCycle();
    bool isCharging() const;
    bool isDisplayOn() const;
    bool isShowingChargingScreen() const;
    void resetActivityTimer();
    void resetUsbState();
    bool shouldTurnOffDisplay() const;
    bool shouldEnterDeepSleep() const;
    void setPowerSave(bool enabled);
    void updateActivity();
    bool isUsbConnected();
    void setDisplayOn(bool on);
    
    // Methods for RTC wake approach
    void configureRtcWakeup();
    void handleWakeUp();
    bool isInDeepSleepCycle() const;
    void checkForTouchDuringWake(InputManager& input, DisplayManager& display);
    void setSuppressRtcDisplay(bool suppress);
    
    // Methods for dedicated power button
    bool isPowerButtonPressed();
    void handlePowerButton();
    
    // Configurable timeout methods
    void setDisplayTimeout(unsigned long timeout);
    void setSleepTimeout(unsigned long timeout);
    void setRtcWakeInterval(int seconds);
    unsigned long getDisplayTimeout() const;
    unsigned long getSleepTimeout() const;
    int getRtcWakeInterval() const;
    void updateSettingsFromStorage(StorageManager& storage);

private:
    bool displayOn;
    bool showingChargingScreen;
    bool isChargingState;
    bool lastChargingState;
    unsigned long lastActivityTime;
    unsigned long lastUsbStateChangeTime;
    // No dedicated power button variables needed
    bool isChargingScreen;
    
    // Variables for sleep cycle handling
    bool inDeepSleepCycle;  // Whether we're in the sleep-wake cycle
    unsigned long sleepCycles; // Count of sleep cycles
    bool stayAwake;        // Flag to stay awake if touch detected
    bool suppressRtcDisplay; // Flag to suppress display during RTC wake
    
    // Configurable timeout values
    unsigned long displayTimeout;  // Configurable display off timeout
    unsigned long sleepTimeout;    // Configurable deep sleep timeout
    int rtcWakeInterval;           // Configurable RTC wake interval
    
    // Touch button state handled by InputManager
    
    // Interrupt flag
    static volatile bool wakingFromSleep;
    static volatile bool rtcWakeFlag;
    
    #ifdef ARDUINO_ARCH_SAMD
    RTCZero rtc;
    #endif
    
    static void wakeupInterrupt();
    static void rtcAlarmCallback();
    void setupWakeupSource();
    void disableWakeupSource();
};

#endif // POWER_MANAGER_H 