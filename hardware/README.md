# Hardware Documentation - Treadmill Speed Sensor

## Overview
This document describes the hardware design for a treadmill speed sensor that connects to Zwift via BLE.

## Components List

### Main Components
1. **ESP32 Development Board** (e.g., ESP32-DevKitC, ESP32-WROOM-32)
   - Quantity: 1
   - Purpose: Main microcontroller with BLE support
   - Cost: ~$5-10

2. **Hall Effect Sensor** (e.g., A3144 or OH3144)
   - Quantity: 1
   - Purpose: Detect magnet passing (speed measurement)
   - Alternative: Optical sensor (TCRT5000)
   - Cost: ~$0.50

3. **Magnet** (Neodymium)
   - Quantity: 1
   - Size: 10mm x 3mm (or similar)
   - Purpose: Trigger hall effect sensor
   - Cost: ~$0.50

### Supporting Components
4. **Resistor** 10kΩ
   - Quantity: 1
   - Purpose: Pull-up resistor for sensor signal
   - Cost: ~$0.01

5. **Capacitor** 100nF (0.1µF)
   - Quantity: 1
   - Purpose: Debouncing/filtering
   - Cost: ~$0.05

6. **LED** (optional, for status indication)
   - Quantity: 1
   - Color: Blue or Green
   - Cost: ~$0.10

7. **Resistor** 220Ω (for LED)
   - Quantity: 1
   - Purpose: Current limiting for LED
   - Cost: ~$0.01

8. **Power Supply**
   - Option 1: USB cable (5V)
   - Option 2: Battery (3.7V LiPo with charging circuit)
   - Option 3: Wall adapter (5V, 1A)

9. **Enclosure** (optional)
   - 3D printed case or plastic project box
   - Dimensions: ~80mm x 60mm x 25mm

### Optional Components
10. **Voltage Regulator** (if using battery)
    - LDO regulator (e.g., AMS1117-3.3)
    - Purpose: Step down voltage to 3.3V

11. **Push Button** (optional)
    - Purpose: Reset or calibration
    - Cost: ~$0.20

## Circuit Schematic

```
                                    ESP32
                                   +-------+
                                   |       |
                   +3.3V --------->| 3.3V  |
                                   |       |
    Hall Sensor    +-------------->| GPIO4 |  (Speed Sensor Input)
    (OUT)          |               |       |
                   |               | GPIO2 |-----> LED (Status) --[220Ω]-- GND
                   |               |       |
                   +--[10kΩ]--+3.3V| GND   |<----- GND
                   |               |       |
                  [100nF]          | EN    |
                   |               |       |
                  GND              | VIN   |<----- 5V (USB)
                                   +-------+

Hall Sensor Connection:
    VCC  ---> 3.3V
    GND  ---> GND
    OUT  ---> GPIO4 (with 10kΩ pull-up and 100nF capacitor to GND)

Alternative - Optical Sensor (TCRT5000):
    VCC  ---> 3.3V
    GND  ---> GND
    DO   ---> GPIO4 (digital output)
```

## Detailed Pin Connections

### ESP32 Pins:
- **GPIO4**: Speed sensor input (interrupt-capable pin)
- **GPIO2**: Built-in LED (status indicator)
- **3.3V**: Power supply for sensor
- **GND**: Ground
- **VIN/USB**: 5V power input

### Hall Effect Sensor (A3144):
- **Pin 1 (VCC)**: Connect to ESP32 3.3V
- **Pin 2 (GND)**: Connect to ESP32 GND
- **Pin 3 (OUT)**: Connect to ESP32 GPIO4 (with pull-up resistor)

### Optical Sensor Alternative (TCRT5000):
- **VCC**: Connect to ESP32 3.3V
- **GND**: Connect to ESP32 GND
- **DO**: Connect to ESP32 GPIO4
- **AO**: Not used (or can be used for analog reading)

## Assembly Instructions

### 1. Sensor Installation on Treadmill

#### For Hall Effect Sensor:
1. **Identify the rotating part**: Find the treadmill roller or belt drive wheel
2. **Mount the magnet**: 
   - Clean the surface of the rotating part
   - Attach the neodymium magnet securely using strong adhesive or double-sided tape
   - Ensure the magnet is balanced and won't come loose during operation
3. **Mount the sensor**:
   - Position the hall effect sensor 2-5mm away from the magnet's path
   - The sensor should detect the magnet once per revolution
   - Secure with mounting bracket or adhesive
   - Ensure the sensor is stable and won't shift during treadmill operation

#### For Optical Sensor:
1. **Prepare the wheel**: 
   - Add a reflective tape strip or paint a white mark on the rotating part
   - Alternatively, create a slot or hole for optical interruption
2. **Mount the sensor**:
   - Position TCRT5000 3-10mm from the reflective surface
   - Adjust the potentiometer on the module for proper sensitivity
   - Secure the sensor to prevent movement

### 2. Electronics Assembly

1. **Solder components** (if using perfboard/PCB):
   - Connect 10kΩ pull-up resistor between GPIO4 and 3.3V
   - Connect 100nF capacitor between GPIO4 and GND (close to ESP32)
   - Optional: Add LED with 220Ω resistor to GPIO2

2. **Alternative - Breadboard prototyping**:
   - Use jumper wires for initial testing
   - Verify connections before soldering

3. **Wire the sensor**:
   - Use shielded cable if distance > 30cm
   - Keep sensor wires away from high-voltage lines
   - Add ferrite beads if experiencing interference

### 3. Power Supply

#### USB Power:
- Simple USB cable connected to ESP32
- Can be powered from treadmill's USB port (if available)
- Or use external USB power bank/adapter

#### Battery Power:
- 3.7V LiPo battery with protection circuit
- Add TP4056 charging module for USB charging
- Use voltage regulator if needed
- Battery capacity: 1000-2000mAh for several hours of use

### 4. Enclosure (Optional)

1. **3D Printed Case**:
   - Design available in `hardware/case-design.stl` (to be created)
   - Includes mounting holes for ESP32 and cable routing

2. **Project Box**:
   - Drill holes for USB cable and sensor wire
   - Add ventilation holes for heat dissipation
   - Secure ESP32 with standoffs or adhesive

## Calibration

### Measuring Wheel Circumference:
1. Mark a point on the treadmill belt
2. Rotate manually and measure one complete revolution
3. Update `WHEEL_CIRCUMFERENCE_CM` in the firmware

### Adjusting Pulses per Revolution:
- Default: 1 magnet = 1 pulse per revolution
- If multiple magnets: Update `PULSES_PER_REVOLUTION`

### Speed Verification:
1. Upload firmware with serial monitoring enabled
2. Run treadmill at known speed (e.g., 5 km/h)
3. Compare displayed speed with treadmill's speed
4. Adjust calibration values if needed

## Testing

### 1. Sensor Test:
```
- Manually rotate wheel/belt
- Check serial monitor for pulse detection
- Verify pulse count increases
```

### 2. BLE Connection Test:
```
- Power on ESP32
- Check for "Treadmill Sensor" in Zwift device list
- Verify connection and data transmission
```

### 3. Speed Accuracy Test:
```
- Run at multiple speeds: 3, 5, 8, 10 km/h
- Compare ESP32 reading with treadmill display
- Verify Zwift receives correct data
```

## Troubleshooting

### No pulses detected:
- Check sensor power (3.3V present)
- Verify magnet proximity (2-5mm for hall sensor)
- Check pull-up resistor connection
- Test sensor with multimeter (output should toggle)

### Erratic readings:
- Add/increase capacitor value (try 220nF or 470nF)
- Check for loose connections
- Ensure magnet is secure
- Shield sensor cable from noise

### BLE not connecting:
- Check serial monitor for BLE initialization messages
- Verify Zwift is looking for FTMS devices
- Restart ESP32 and Zwift app
- Check ESP32 antenna (some boards have external antenna option)

### Speed inaccurate:
- Re-measure wheel circumference
- Adjust WHEEL_CIRCUMFERENCE_CM in firmware
- Check for multiple triggers per revolution
- Verify PULSES_PER_REVOLUTION setting

## Safety Considerations

1. **Electrical Safety**:
   - Use proper insulation for all connections
   - Keep electronics away from moisture
   - Ensure secure mounting to prevent entanglement

2. **Mechanical Safety**:
   - Ensure magnet is securely attached (won't fly off at high speed)
   - Route cables safely away from moving parts
   - Use cable ties to secure all wiring

3. **Treadmill Warranty**:
   - This modification may void warranty
   - Consult treadmill manual before installation
   - Consider non-permanent mounting methods

## Power Consumption

- Active (BLE connected): ~80-120 mA
- Idle (advertising): ~40-60 mA
- Deep sleep (if implemented): ~10 µA

Estimated battery life with 2000mAh battery:
- Continuous use: ~16-20 hours
- With sleep mode: Several days

## Future Improvements

1. Add incline sensor (accelerometer/gyroscope)
2. Implement battery monitoring
3. Add OLED display for local speed readout
4. Add cadence/step rate measurement
5. Support for multiple treadmill profiles
6. Web configuration interface
7. Data logging to SD card

## References

- FTMS Specification: Bluetooth GATT Fitness Machine Service
- ESP32 Documentation: https://docs.espressif.com/
- Hall Effect Sensor Datasheet: A3144/OH3144
- Zwift BLE Protocol Documentation
