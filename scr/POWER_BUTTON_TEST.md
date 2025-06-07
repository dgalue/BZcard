# 🔋 Power Button Wake Test Guide

## ✅ **Testing the Power Button Wake Feature**

### **What Was Fixed:**
- **Issue**: PowerManager was overriding the local `displayOn` variable every loop
- **Solution**: Added `setDisplayOn()` method to keep PowerManager state synchronized
- **Added**: Debug output to help troubleshoot button detection

### **Test Procedure:**

#### **1. Upload and Monitor** 📤
```bash
arduino-cli upload --fqbn arduino:samd:mkrzero -p [YOUR_PORT] .
arduino-cli monitor -p [YOUR_PORT] -c baudrate=115200
```

#### **2. Test Sequence** ⏱️
1. **Start fresh** - Device should show startup sequence and menu
2. **Wait 60+ seconds** - Let device timeout, watch for:
   ```
   Display turning off due to inactivity
   ```
3. **Press power button (A0 touch sensor)** - Should see:
   ```
   DEBUG: Display off, checking power button. Power pressed: 1, Time since last wake: [time], Deep sleep: 0
   Power button pressed - turning display back on
   ```
4. **Verify display turns on** and shows menu

#### **3. Debug Output** 🔍
When display is off (60-120 second window), you should see debug messages every 2 seconds:
```
DEBUG: Display off, checking power button. Power pressed: 0, Time since last wake: [time], Deep sleep: 0
```

When you touch the power button:
```
DEBUG: Display off, checking power button. Power pressed: 1, Time since last wake: [time], Deep sleep: 0
Power button pressed - turning display back on
```

#### **4. If Still Not Working** 🔧

Check these debug values:
- **Power pressed**: Should be `1` when touching A0 sensor
- **Deep sleep**: Should be `0` during 60-120 second window  
- **Time since last wake**: Should be > 1000ms for debounce

**Possible Issues:**
1. **Power pressed always 0**: Touch sensor A0 not working
2. **Deep sleep = 1**: Device entering sleep too early
3. **No debug messages**: Logic condition not met

#### **5. Touch Sensor Test** 👆
To test A0 touch sensor separately, add this to `handleNormalOperation()`:
```cpp
static unsigned long lastTouchDebug = 0;
if (millis() - lastTouchDebug > 1000) {
    Serial.print("Touch A0 value: ");
    Serial.println(inputManager.isPowerPressed());
    lastTouchDebug = millis();
}
```

### **Expected Behavior Timeline:**
- **0-60s**: Normal operation, display on
- **60-120s**: 🎯 **Display off, power button should wake it**
- **120s+**: Deep sleep mode

---
**✨ The power button wake feature should now work in the critical 60-120 second window!** 