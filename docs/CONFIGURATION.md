# Configuration Examples for Different Treadmill Models

This file contains example configuration values for different treadmill models. Adjust the values in `src/main.cpp` based on your treadmill specifications.

## Generic Treadmill (Default)
```cpp
#define WHEEL_CIRCUMFERENCE_CM 200.0
#define PULSES_PER_REVOLUTION 1
#define SPEED_TIMEOUT 2000
```

## Small Compact Treadmill
- Belt length: ~120cm
- Smaller rollers
```cpp
#define WHEEL_CIRCUMFERENCE_CM 120.0
#define PULSES_PER_REVOLUTION 1
#define SPEED_TIMEOUT 2000
```

## Large Commercial Treadmill
- Belt length: ~400cm
- Large rollers
```cpp
#define WHEEL_CIRCUMFERENCE_CM 400.0
#define PULSES_PER_REVOLUTION 1
#define SPEED_TIMEOUT 2000
```

## Treadmill with Multiple Magnets
- If you install 2 magnets on opposite sides for better accuracy
```cpp
#define WHEEL_CIRCUMFERENCE_CM 200.0
#define PULSES_PER_REVOLUTION 2
#define SPEED_TIMEOUT 2000
```

## How to Measure Your Treadmill

### Method 1: Direct Measurement (Most Accurate)
1. Mark a starting point on the belt with tape
2. Manually rotate the belt exactly one revolution
3. Measure the distance the mark traveled
4. This is your WHEEL_CIRCUMFERENCE_CM

### Method 2: Roller Diameter Measurement
1. Measure the diameter of the front roller (D in cm)
2. Calculate: WHEEL_CIRCUMFERENCE_CM = π × D
3. Example: If diameter is 64cm: 3.14159 × 64 = 201cm

### Method 3: Speed Comparison
1. Use default value (200.0)
2. Run at known speed (e.g., 5.0 km/h)
3. Check serial monitor output
4. Calculate correction: New_value = Current_value × (Displayed_speed / Target_speed)
5. Example: If showing 6.0 when should be 5.0:
   - New_value = 200.0 × (6.0 / 5.0) = 240.0

## Pin Configuration for Different Boards

### ESP32-DevKitC (Default)
```cpp
#define SPEED_SENSOR_PIN 4
#define LED_PIN 2
```

### ESP32-WROOM-32
```cpp
#define SPEED_SENSOR_PIN 4
#define LED_PIN 2
```

### ESP32-S2
```cpp
#define SPEED_SENSOR_PIN 4
#define LED_PIN 15  // Built-in LED on different pin
```

### ESP32-C3
```cpp
#define SPEED_SENSOR_PIN 4
#define LED_PIN 8  // Built-in LED
```

## Sensor Configuration

### Hall Effect Sensor (A3144)
- Trigger: FALLING edge
- Pull-up: 10kΩ required
- Range: 2-5mm from magnet
```cpp
pinMode(SPEED_SENSOR_PIN, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), speedSensorISR, FALLING);
```

### Optical Sensor (TCRT5000)
- Trigger: Can be FALLING or RISING depending on module
- Pull-up: Usually included on module
- Range: 3-10mm from reflective surface
```cpp
pinMode(SPEED_SENSOR_PIN, INPUT);
attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), speedSensorISR, FALLING);
```

## Advanced Configuration

### Faster Update Rate (for competitive gaming)
```cpp
// In loop() function, change:
if (deviceConnected && (currentTime - lastDataSend >= 500)) {
// To:
if (deviceConnected && (currentTime - lastDataSend >= 250)) {
```

### Longer Timeout (for very slow walking)
```cpp
const unsigned long SPEED_TIMEOUT = 5000;  // 5 seconds
```

### More Aggressive Filtering (for noisy sensors)
```cpp
// In updateSpeed() function, change:
currentSpeed = 0.7 * currentSpeed + 0.3 * lastSpeed;
// To:
currentSpeed = 0.5 * currentSpeed + 0.5 * lastSpeed;
```

### Less Filtering (for faster response)
```cpp
// In updateSpeed() function, change:
currentSpeed = 0.7 * currentSpeed + 0.3 * lastSpeed;
// To:
currentSpeed = 0.9 * currentSpeed + 0.1 * lastSpeed;
```

## BLE Configuration

### Change Device Name
```cpp
// In initBLE() function:
NimBLEDevice::init("Treadmill Sensor");
// Change to:
NimBLEDevice::init("My Treadmill");
```

### Reduce BLE Power (for battery operation)
```cpp
// In initBLE() function, change:
NimBLEDevice::setPower(ESP_PWR_LVL_P9);  // Maximum power
// To:
NimBLEDevice::setPower(ESP_PWR_LVL_N0);  // Minimum power
```

## Troubleshooting Values

### If speed is consistently 2x too high:
```cpp
// Either double the circumference:
#define WHEEL_CIRCUMFERENCE_CM 400.0  // if was 200.0
// Or double the pulses:
#define PULSES_PER_REVOLUTION 2  // if was 1
```

### If speed is consistently 2x too low:
```cpp
// Either halve the circumference:
#define WHEEL_CIRCUMFERENCE_CM 100.0  // if was 200.0
// Or halve the pulses:
#define PULSES_PER_REVOLUTION 1  // if was 2 (can't go below 1)
```

### If speed jumps around erratically:
1. Increase capacitor value (try 220nF or 470nF)
2. Increase filtering in code (see above)
3. Check for loose connections
4. Ensure magnet is secure and balanced

## Example Real-World Values

These are actual measured values from various treadmills:

### Kettler Track 3
- Wheel circumference: 195cm
- Pulses: 1

### NordicTrack T6.5
- Wheel circumference: 240cm
- Pulses: 1

### ProForm 505 CST
- Wheel circumference: 210cm
- Pulses: 1

### Horizon T101
- Wheel circumference: 185cm
- Pulses: 1

---

**Note**: Always verify your configuration by comparing the speed shown in the serial monitor with the speed displayed on your treadmill's console at multiple speeds (e.g., 3, 5, 8, 10 km/h).
