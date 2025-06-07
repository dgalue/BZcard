#include "PowerManager.h"
#include "InputManager.h"
#include "DisplayManager.h"
#include "StorageManager.h"

// Include hardware-specific headers for SAMD
#ifdef ARDUINO_ARCH_SAMD
  #include <sam.h>
#endif

// Define external variables
extern InputManager inputManager;
extern DisplayManager displayManager;

// Static member variables initialization
volatile bool PowerManager::wakingFromSleep = false;
volatile bool PowerManager::rtcWakeFlag = false;

// Static variable to track forced sleep state
static bool forcingSleepCycle = false;

PowerManager::PowerManager()
    : displayOn(true), showingChargingScreen(false), isChargingState(false), lastChargingState(false),
      lastActivityTime(0), lastUsbStateChangeTime(0), isChargingScreen(false), 
      inDeepSleepCycle(false), sleepCycles(0), stayAwake(false), suppressRtcDisplay(false),
      displayTimeout(60000), sleepTimeout(120000), rtcWakeInterval(DEFAULT_RTC_WAKE_SECONDS) {
    wakingFromSleep = false;
    rtcWakeFlag = false;
    forcingSleepCycle = false;
}

void PowerManager::begin() {
#ifdef ARDUINO_ARCH_SAMD
    rtc.begin();
    
    // Force display on at startup
    displayOn = true;
    
    Serial.println("Power management initialized with deep sleep support (SAMD architecture)");
    
    // Initialize deep sleep cycle state
    inDeepSleepCycle = false;
    stayAwake = true; // Start awake
    sleepCycles = 0;
    forcingSleepCycle = false;
    isChargingState = false; // Always in one mode
    suppressRtcDisplay = false; // Allow display by default
    
    // Set initial activity time
    resetActivityTimer();
#else
    Serial.println("Power management initialized (deep sleep not available)");
#endif
}

void PowerManager::update() {
    // Advanced message state tracking
    static unsigned long lastWakeEventTime = 0;
    static bool wakeMessageShown = false;
    static bool rtcMessageShown = false;
    
    if (wakingFromSleep) {
        wakingFromSleep = false;
        
        // Only print wake message once per wake event
        if (!wakeMessageShown) {
            Serial.println("Waking from sleep");
            wakeMessageShown = true;
            lastWakeEventTime = millis();
        }
    }
    
    // Check for RTC wake flag
    if (rtcWakeFlag) {
        rtcWakeFlag = false;
        
        // Only call handleWakeUp if not suppressed
        if (!suppressRtcDisplay) {
            handleWakeUp();
        } else {
            Serial.println("RTC wake suppressed - keeping display off");
        }
        
        // Only print RTC message once per RTC wake event
        if (!rtcMessageShown) {
            Serial.println("Processed RTC wake event");
            rtcMessageShown = true;
        }
    }
    
    // Reset message flags after 30 seconds
    if (millis() - lastWakeEventTime > 30000) {
        wakeMessageShown = false;
        rtcMessageShown = false;
    }
    
    // Only check for deep sleep timeout in the power manager
    if (inDeepSleepCycle && stayAwake) {
        // Check for timeout
        if (millis() - lastActivityTime > sleepTimeout) {
            // No activity for timeout period, go back to sleep
            Serial.println("Activity timeout, returning to sleep...");
            stayAwake = false;
            
            // Configure next RTC wake
            configureRtcWakeup();
            
            // Enter sleep again, preserving forced status
            enterDeepSleep(forcingSleepCycle);
                }
            }
        }

// Using capacitive touch buttons, not dedicated power button
bool PowerManager::isPowerButtonPressed() {
    // Use the external touch system in the main code
    return false;
}

// Empty function as we're using the original touch system
void PowerManager::handlePowerButton() {
    // Do nothing - handled through InputManager
}

void PowerManager::enterDeepSleep(bool forceDeepSleep) {
#ifdef ARDUINO_ARCH_SAMD
    // Update force flag if called with force=true
    if (forceDeepSleep) {
        forcingSleepCycle = true;
    }
    
    Serial.println("DEEP SLEEP REQUEST: Entering deep sleep mode");
    
    // Always assume battery mode
    isChargingState = false;
    showingChargingScreen = false;

    // Entering sleep cycle
    inDeepSleepCycle = true;
    sleepCycles++;
    
    Serial.print("Deep sleep cycle #");
    Serial.println(sleepCycles);
    
    // Turn off display
    displayManager.u8g2.setPowerSave(1);
    displayOn = false;
    
    // Configure RTC for next wake
    configureRtcWakeup();
    
    // Disable USB to save power
    USBDevice.detach();
    
    // Flush serial before sleep
    Serial.flush();
    delay(10); // Small delay to ensure serial buffer is flushed
    
    // Configure for deep sleep
    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // Enter deep sleep
    __WFI();
    
    // Code resumes here after wakeup
    
    // Only re-enable USB if the device will stay awake
    // USB reconnection will be handled in exitDeepSleepCycle() when actually waking up
    Serial.println("Woke up from deep sleep - checking if staying awake...");
#else
    Serial.println("Deep sleep not supported on this board");
    Serial.println("SAMD architecture required for deep sleep");
#endif
}

void PowerManager::configureRtcWakeup() {
#ifdef ARDUINO_ARCH_SAMD
    // Configure RTC for periodic wakeup
    rtc.setTime(0, 0, 0);
    rtc.setDate(1, 1, 20); // Use 2-digit year to avoid warnings
    
    // Set alarm to wake up after rtcWakeInterval
    rtc.setAlarmTime(0, 0, rtcWakeInterval);
    rtc.enableAlarm(rtc.MATCH_SS);  // Match on seconds
    rtc.attachInterrupt(rtcAlarmCallback);
#endif
}

void PowerManager::handleWakeUp() {
    // Don't turn on display if suppressed
    if (suppressRtcDisplay) {
        Serial.println("Wake up display suppressed");
        return;
    }
    
    // Always operate in one mode
    Serial.println("Waking up, showing logo");
    
    // Ensure display is on
    displayManager.u8g2.setPowerSave(0);
    displayOn = true;
    
    // Show logo when waking up
    displayManager.showLogo();
    delay(2000); // Show logo for 2 seconds
    
    // Then show menu
    displayManager.updateMenuDisplay(0, 0, false); // Reset to main menu
}

void PowerManager::checkForTouchDuringWake(InputManager& input, DisplayManager& display) {
    // Check all touch buttons
    bool powerTouched = input.isPowerPressed();
    bool upTouched = input.isUpPressed();
    bool downTouched = input.isDownPressed();
    bool enterTouched = input.isEnterPressed();
    
    // Check dedicated power button
    bool powerButtonTouched = isPowerButtonPressed();
    
    // Check if any button is touched, including dedicated power button
    if (powerTouched || upTouched || downTouched || enterTouched || powerButtonTouched) {
        // Touch detected, exit deep sleep cycle
        Serial.println("Touch detected during wake, exiting deep sleep cycle");
        stayAwake = true;
        inDeepSleepCycle = false;
        
        // Reset forced sleep on user interaction
        forcingSleepCycle = false;
        
        // Re-enable display
        display.u8g2.setPowerSave(0);
        displayOn = true;

        // Always operate in menu mode
        showingChargingScreen = false;
        
        // Show logo first when waking up
        display.showLogo();
        delay(2000); // Show logo for 2 seconds
        
        // Then show menu
        display.updateMenuDisplay(0, 0, false); // Reset to main menu
        
        // Reset activity timer
        resetActivityTimer();
    } else {
        // No touch detected, go back to sleep with same force setting
        enterDeepSleep(forcingSleepCycle);
    }
}

bool PowerManager::isInDeepSleepCycle() const {
    return inDeepSleepCycle;
}

void PowerManager::setupWakeupSource() {
#ifdef ARDUINO_ARCH_SAMD
    // Nothing needed here as we're using RTC for wakeup
#endif
}

void PowerManager::disableWakeupSource() {
#ifdef ARDUINO_ARCH_SAMD
    // Disable RTC alarm
    rtc.disableAlarm();
#endif
}

// Static callback for RTC alarm
void PowerManager::rtcAlarmCallback() {
    rtcWakeFlag = true;
}

// Static callback for other wake sources
void PowerManager::wakeupInterrupt() {
    wakingFromSleep = true;
}

// Standard accessor methods
bool PowerManager::isCharging() const {
    // Always return false - operating in one mode only
    return false;
}

bool PowerManager::isDisplayOn() const {
    return displayOn;
}

bool PowerManager::isShowingChargingScreen() const {
    return false; // Never show charging screen
}

void PowerManager::resetActivityTimer() {
    lastActivityTime = millis();
}

bool PowerManager::shouldTurnOffDisplay() const {
    return false; 
}

bool PowerManager::shouldEnterDeepSleep() const {
    return (millis() - lastActivityTime > sleepTimeout);
}

void PowerManager::setPowerSave(bool enabled) { 
    // Set display power save mode 
}

void PowerManager::updateActivity() { 
    lastActivityTime = millis(); 
    stayAwake = true; // Keep device awake on activity
}

bool PowerManager::isUsbConnected() {
    // Always return false - no USB detection
    return false;
}

// New method to explicitly exit deep sleep cycle
void PowerManager::exitDeepSleepCycle() {
    if (inDeepSleepCycle) {
        Serial.println("Explicitly exiting deep sleep cycle");
        inDeepSleepCycle = false;
        stayAwake = true;
        forcingSleepCycle = false;
        
        // Re-enable USB and Serial when actually staying awake
        USBDevice.attach();
        Serial.end();
        Serial.begin(115200);
        displayManager.u8g2.begin();
        
        delay(100); // Brief delay for USB and serial to reconnect
        Serial.println("USB and Serial reconnected after wake up");
        
        // Make sure RTC alarm is disabled
        rtc.disableAlarm();
        rtcWakeFlag = false;
        
        // Reset timers
        resetActivityTimer();
        
        // Re-enable display handling
        suppressRtcDisplay = false;
        
        // Ensure display is on
        displayManager.u8g2.setPowerSave(0);
        displayOn = true;
    }
}

// New method to fully reset USB state after disconnect
void PowerManager::resetUsbState() {
    // Do nothing - no USB detection anymore
}

// New method to suppress RTC display
void PowerManager::setSuppressRtcDisplay(bool suppress) {
    suppressRtcDisplay = suppress;
}

// New method to manually set display state
void PowerManager::setDisplayOn(bool on) {
    displayOn = on;
}

// Configurable timeout methods
void PowerManager::setDisplayTimeout(unsigned long timeout) {
    displayTimeout = timeout;
}

void PowerManager::setSleepTimeout(unsigned long timeout) {
    sleepTimeout = timeout;
}

void PowerManager::setRtcWakeInterval(int seconds) {
    rtcWakeInterval = constrain(seconds, 1, 300); // 1 second to 5 minutes
}

unsigned long PowerManager::getDisplayTimeout() const {
    return displayTimeout;
}

unsigned long PowerManager::getSleepTimeout() const {
    return sleepTimeout;
}

int PowerManager::getRtcWakeInterval() const {
    return rtcWakeInterval;
}

void PowerManager::updateSettingsFromStorage(StorageManager& storage) {
    displayTimeout = storage.getDisplayTimeout();
    sleepTimeout = storage.getSleepTimeout();
    rtcWakeInterval = storage.getRtcWakeInterval();
    
    Serial.print("PowerManager updated with settings - Display: ");
    Serial.print(displayTimeout / 1000);
    Serial.print("s, Sleep: ");
    Serial.print(sleepTimeout / 1000);
    Serial.print("s, RTC: ");
    Serial.print(rtcWakeInterval);
    Serial.println("s");
} 