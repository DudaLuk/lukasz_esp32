# Frequently Asked Questions (FAQ)

## General Questions

### Q: Will this work with Zwift?
**A:** Yes! This project implements the standard FTMS (Fitness Machine Service) Bluetooth protocol that Zwift uses for treadmills. It will appear as a treadmill device in Zwift's pairing screen.

### Q: What other apps are compatible?
**A:** Any app that supports FTMS treadmills should work, including:
- Zwift
- TrainerRoad
- Kinomap
- RGT Cycling (for running mode)
- Most fitness tracking apps with BLE treadmill support

### Q: How accurate is the speed measurement?
**A:** With proper calibration, accuracy is typically within ±2-3% of the treadmill's display. Accuracy depends on:
- Correct wheel circumference measurement
- Stable magnet mounting
- Proper sensor positioning
- Good signal filtering

### Q: Do I need programming experience?
**A:** No! You can use the default firmware with just a few configuration changes (wheel circumference). However, basic Arduino/PlatformIO knowledge is helpful for uploading firmware.

## Hardware Questions

### Q: Which ESP32 board should I buy?
**A:** Any ESP32 development board will work. Recommended options:
- ESP32-DevKitC (most common)
- ESP32-WROOM-32
- NodeMCU-32S
- Any board with ESP32 chip and USB programming

### Q: Hall effect sensor or optical sensor?
**A:** Both work well:

**Hall Effect (Recommended):**
- Pros: More reliable, weather-resistant, no line-of-sight needed
- Cons: Requires magnet installation
- Best for: Permanent installations

**Optical (TCRT5000):**
- Pros: No magnet needed, easy to test
- Cons: Can be affected by dust, lighting, or vibrations
- Best for: Quick prototypes or temporary setups

### Q: Where can I buy the components?
**A:** Common sources:
- AliExpress (cheapest, 2-4 week shipping)
- Amazon (faster, slightly more expensive)
- Local electronics stores (immediate, but limited selection)
- DigiKey, Mouser (professional distributors)

### Q: Can I use a different GPIO pin?
**A:** Yes! Any GPIO pin that supports interrupts. Just change `SPEED_SENSOR_PIN` in the code. Avoid pins 6-11 (used for flash).

### Q: How do I power the ESP32?
**A:** Several options:
1. USB cable (easiest - from phone charger or treadmill USB port if available)
2. USB power bank (portable)
3. 3.7V LiPo battery with charging circuit
4. 5V wall adapter

### Q: Do I need the LED?
**A:** No, it's optional. The built-in LED on GPIO2 will indicate connection status even without an external LED.

## Installation Questions

### Q: How do I attach the magnet to the treadmill roller?
**A:** Options:
1. Strong adhesive (epoxy or super glue)
2. Double-sided foam tape (3M VHB)
3. Small bracket with screw (if accessible)
4. Ensure it's balanced and secure!

### Q: What's the optimal distance between sensor and magnet?
**A:** 
- Hall effect sensor: 2-5mm (closer is better)
- Optical sensor: 3-10mm (test for best distance)
- Too close: May not trigger properly
- Too far: Weak or no signal

### Q: Will this void my treadmill warranty?
**A:** Possibly. This is a non-destructive modification if done carefully, but check your warranty terms. Consider:
- Using removable mounting methods
- Not drilling holes in the treadmill
- Consulting manufacturer before modification

### Q: Can I install this without taking the treadmill apart?
**A:** Usually yes! Most treadmills have accessible rollers. You may need to:
1. Remove motor cover
2. Access front or rear roller
3. Attach magnet and sensor
4. Route cable safely

## Software Questions

### Q: Do I need to install PlatformIO?
**A:** PlatformIO is recommended but not required. Alternatives:
- Arduino IDE (simpler but less features)
- VSCode with PlatformIO extension (recommended)
- Command-line PlatformIO (for advanced users)

### Q: How do I upload firmware to ESP32?
**A:**
```bash
1. Install PlatformIO: pip install platformio
2. Connect ESP32 via USB
3. Run: pio run --target upload
```

### Q: How do I calibrate the wheel circumference?
**A:**
1. Manually rotate belt one complete revolution
2. Measure distance traveled (in cm)
3. Update `WHEEL_CIRCUMFERENCE_CM` in `src/main.cpp`
4. Re-upload firmware

Alternative method:
1. Run at known speed (e.g., 5 km/h)
2. Check serial output
3. Calculate: new_value = old_value × (measured / target)

### Q: Can I change the BLE device name?
**A:** Yes! In `src/main.cpp`, find:
```cpp
NimBLEDevice::init("Treadmill Sensor");
```
Change to your preferred name (max ~20 characters).

### Q: How do I check if it's working?
**A:**
1. Open serial monitor: `pio device monitor`
2. Manually rotate treadmill belt
3. Look for pulse detection and speed calculation
4. Verify BLE advertising messages

## Zwift-Specific Questions

### Q: How do I pair with Zwift?
**A:**
1. Start Zwift
2. Go to "Paired Devices"
3. Look under "Controllable Trainer" or "Treadmill"
4. Select "Treadmill Sensor" (or your custom name)
5. Follow on-screen instructions

### Q: Zwift doesn't see my device?
**A:** Troubleshooting:
1. Ensure ESP32 is powered and BLE is advertising (check serial monitor)
2. Restart Zwift
3. Ensure no other device is connected to the sensor
4. Move ESP32 closer to Zwift device
5. Check Bluetooth is enabled on Zwift device
6. Try pairing from Zwift Companion app

### Q: Speed in Zwift is incorrect?
**A:** Recalibrate:
1. Run at known speed on treadmill
2. Compare with Zwift display
3. Adjust `WHEEL_CIRCUMFERENCE_CM`
4. Formula: new = old × (zwift_speed / treadmill_speed)

### Q: Can I use this with Zwift's incline feature?
**A:** Not yet. Current version only transmits speed. Future versions may include:
- Accelerometer for incline detection
- Manual incline input
- Integration with treadmill's incline motor (if electronic)

## Troubleshooting

### Q: No pulses detected?
**A:** Check:
1. Sensor power (should have 3.3V)
2. Magnet distance (2-5mm for hall effect)
3. Sensor orientation (correct face toward magnet)
4. Wiring connections (especially pull-up resistor)
5. Test sensor with multimeter (voltage should change)

### Q: Erratic or noisy readings?
**A:** Solutions:
1. Add larger capacitor (try 220nF or 470nF)
2. Increase software filtering (see CONFIGURATION.md)
3. Check for loose connections
4. Ensure magnet is balanced and secure
5. Use shielded cable for sensor
6. Route sensor cable away from power lines

### Q: BLE connection drops frequently?
**A:** Try:
1. Move ESP32 closer to Zwift device
2. Remove obstacles between devices
3. Use higher BLE power setting
4. Ensure stable power supply to ESP32
5. Check for WiFi interference (BLE uses 2.4GHz)
6. Update ESP32 firmware

### Q: Speed is 2x or 0.5x what it should be?
**A:** 
- If 2x too high: Double `WHEEL_CIRCUMFERENCE_CM` OR set `PULSES_PER_REVOLUTION` to 2
- If 2x too low: Halve `WHEEL_CIRCUMFERENCE_CM` OR check if multiple magnets
- Verify number of magnets matches `PULSES_PER_REVOLUTION`

### Q: Sensor works but Zwift shows 0 speed?
**A:** Check:
1. BLE connection is established (LED should be on)
2. Serial monitor shows speed being calculated
3. FTMS service is properly initialized
4. Try unpairing and re-pairing in Zwift
5. Restart both ESP32 and Zwift

## Advanced Questions

### Q: Can I add heart rate monitoring?
**A:** Yes! You can:
1. Add a heart rate sensor (MAX30102 or similar)
2. Extend the BLE services to include Heart Rate Service (0x180D)
3. Transmit heart rate data alongside speed
(This requires firmware modifications)

### Q: Can I log data to SD card?
**A:** Yes, you can add:
1. SD card module
2. Logging code in the main loop
3. Record: timestamp, speed, pulses, connection status
(Requires additional hardware and code)

### Q: Can I display speed on OLED screen?
**A:** Yes! Add:
1. I2C OLED display (SSD1306)
2. Display library (Adafruit_SSD1306)
3. Code to update display with current speed
(Example code can be added in future version)

### Q: Can I control treadmill speed from Zwift?
**A:** This requires:
1. Understanding your treadmill's control protocol
2. Adding FTMS control point handling
3. Hardware to interface with treadmill motor controller
4. **WARNING**: This is complex and potentially dangerous!

### Q: Can I use multiple sensors?
**A:** Yes, for example:
1. Speed sensor on front roller
2. Cadence sensor on user's leg
3. Each with separate ESP32 or shared board
4. Zwift can pair multiple sensors simultaneously

### Q: What about battery life?
**A:** Typical consumption:
- Active (BLE connected): 80-120 mA
- Idle (advertising): 40-60 mA
- With 2000mAh battery: ~16-20 hours continuous use
- Add deep sleep mode for longer battery life (requires code changes)

### Q: Can I 3D print an enclosure?
**A:** Yes! Design considerations:
1. Ventilation holes for heat dissipation
2. Cable routing for USB and sensor wires
3. Mounting holes or clips
4. LED visibility
5. Access to USB port and buttons
(3D model files to be added in future)

## Safety Questions

### Q: Is this safe to use?
**A:** When properly installed, yes. However:
- Ensure all electrical connections are insulated
- Keep electronics away from moisture
- Secure all cables away from moving parts
- Ensure magnet won't come loose at high speed
- Don't modify treadmill's electrical systems
- Use at your own risk

### Q: What if magnet flies off?
**A:** Prevention:
- Use strong adhesive or mechanical fastening
- Ensure magnet is balanced
- Test at low speeds first
- Check regularly for loosening
- Consider using multiple smaller magnets

### Q: Can I use this in a commercial gym?
**A:** Consider:
- May violate gym policies
- Warranty concerns
- Liability issues
- Get permission first
- Consider portable/removable design

## Support Questions

### Q: Where can I get help?
**A:**
- GitHub Issues: Bug reports and feature requests
- GitHub Discussions: Questions and community support
- Serial monitor: Debug information
- Documentation: This FAQ and other docs

### Q: How can I contribute?
**A:**
- Report bugs or suggest features (GitHub Issues)
- Submit improvements (Pull Requests)
- Share your build photos
- Help others in Discussions
- Improve documentation

### Q: Can I sell devices based on this project?
**A:** Yes, it's MIT licensed! You can:
- Build and sell commercial products
- Modify and redistribute
- Use in commercial settings
- No need to share your changes (but appreciated!)
- Must include original license

---

## Still have questions?

Check:
1. [README.md](../README.md) - Main project documentation
2. [hardware/README.md](../hardware/README.md) - Hardware guide
3. [CONFIGURATION.md](CONFIGURATION.md) - Configuration examples
4. [GitHub Issues](https://github.com/DudaLuk/lukasz_esp32/issues) - Existing questions

Or open a new [GitHub Discussion](https://github.com/DudaLuk/lukasz_esp32/discussions) for community help!
