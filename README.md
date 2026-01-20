# Treadmill Speed Sensor for Zwift

A DIY ESP32-based speed sensor for treadmills that connects to Zwift via Bluetooth Low Energy (BLE), allowing you to track your treadmill runs in Zwift using the standard FTMS (Fitness Machine Service) protocol.

## 🎯 Features

- ✅ **Zwift Compatible**: Uses standard FTMS Bluetooth protocol
- ✅ **Real-time Speed**: Accurate speed measurement from treadmill roller
- ✅ **Easy Installation**: Simple hall effect or optical sensor setup
- ✅ **Low Cost**: Total cost under $15
- ✅ **Wireless**: BLE connection to your device running Zwift
- ✅ **Open Source**: Fully customizable firmware and hardware

## 📋 Requirements

### Hardware
- ESP32 development board (ESP32-DevKitC or similar)
- Hall effect sensor (A3144) or optical sensor (TCRT5000)
- Neodymium magnet (10mm x 3mm)
- Basic components (resistors, capacitor)
- See [hardware/README.md](hardware/README.md) for complete bill of materials

### Software
- PlatformIO (recommended) or Arduino IDE
- Zwift app (iOS, Android, Windows, or macOS)

## 🚀 Quick Start

### 1. Hardware Setup

1. **Attach magnet** to treadmill roller or belt drive
2. **Mount sensor** 2-5mm from magnet path
3. **Connect sensor** to ESP32 GPIO4
4. **Power ESP32** via USB (5V)

See detailed instructions in [hardware/README.md](hardware/README.md)

### 2. Firmware Upload

#### Using PlatformIO (Recommended)

```bash
# Clone repository
git clone https://github.com/DudaLuk/lukasz_esp32.git
cd lukasz_esp32

# Install PlatformIO Core
pip install platformio

# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor
```

#### Using Arduino IDE

1. Install ESP32 board support
2. Install NimBLE-Arduino library (v1.4.1+)
3. Open `src/main.cpp`
4. Select board: "ESP32 Dev Module"
5. Upload

### 3. Calibration

1. **Measure wheel circumference**:
   - Manually rotate treadmill belt one full revolution
   - Measure distance traveled
   - Update `WHEEL_CIRCUMFERENCE_CM` in `src/main.cpp`

2. **Set pulses per revolution**:
   - Default: 1 (one magnet)
   - If multiple magnets, update `PULSES_PER_REVOLUTION`

3. **Test**:
   - Run treadmill at known speed (e.g., 5 km/h)
   - Check serial monitor output
   - Verify speed accuracy

### 4. Connect to Zwift

1. Power on ESP32
2. Open Zwift
3. Go to device pairing
4. Look for "Treadmill Sensor" in the list
5. Pair as Treadmill
6. Start running!

## 📐 Circuit Diagram

```
ESP32 Connections:
┌─────────────────┐
│     ESP32       │
│                 │
│  GPIO4 ◄────────┼─── Hall Sensor OUT (with 10kΩ pull-up)
│  GPIO2 ─────────┼─── LED (optional, status indicator)
│  3.3V  ─────────┼─── Sensor VCC
│  GND   ─────────┼─── Sensor GND
│  VIN   ◄────────┼─── 5V USB Power
└─────────────────┘

Sensor Wiring:
Hall Sensor (A3144):
  Pin 1 (VCC) → 3.3V
  Pin 2 (GND) → GND  
  Pin 3 (OUT) → GPIO4 + 10kΩ to 3.3V + 100nF to GND
```

Full schematic and PCB layouts in [hardware/](hardware/) directory.

## 🔧 Configuration

Edit these values in `src/main.cpp` to match your setup:

```cpp
// Pin Configuration
#define SPEED_SENSOR_PIN 4    // Change if using different GPIO
#define LED_PIN 2             // Built-in LED

// Calibration
#define WHEEL_CIRCUMFERENCE_CM 200.0  // Measure your treadmill
#define PULSES_PER_REVOLUTION 1       // Number of magnets

// Speed Calculation
const unsigned long SPEED_TIMEOUT = 2000;  // ms to consider stopped
```

## 📊 How It Works

1. **Sensor Detection**: Hall effect sensor detects magnet passing once per roller revolution
2. **Speed Calculation**: Firmware calculates speed based on time between pulses
3. **BLE Transmission**: Speed data sent via FTMS protocol (every 500ms)
4. **Zwift Integration**: Zwift receives data as if from a commercial treadmill

Formula:
```
Speed (km/h) = (Circumference_cm / Pulse_Interval_ms) * 3.6
```

## 🔍 Troubleshooting

| Problem | Solution |
|---------|----------|
| No pulses detected | Check sensor power, magnet distance (2-5mm), wiring |
| Erratic readings | Add larger capacitor (220nF), check for loose connections |
| BLE not connecting | Restart ESP32, check serial output, verify Zwift settings |
| Speed too high/low | Recalibrate WHEEL_CIRCUMFERENCE_CM, check PULSES_PER_REVOLUTION |
| Connection drops | Reduce distance to device, check power supply stability |

See detailed troubleshooting in [hardware/README.md](hardware/README.md)

## 📱 Tested With

- ✅ Zwift (Windows, macOS, iOS)
- ✅ TrainerRoad
- ✅ Other FTMS-compatible apps

## 🛠️ Development

### Project Structure

```
lukasz_esp32/
├── src/
│   └── main.cpp           # Main firmware code
├── hardware/
│   ├── README.md          # Hardware documentation
│   └── schematic.png      # Circuit diagram
├── platformio.ini         # PlatformIO configuration
└── README.md             # This file
```

### Building

```bash
# Build only
pio run

# Upload
pio run --target upload

# Clean
pio run --target clean

# Monitor serial
pio device monitor -b 115200
```

### Dependencies

- NimBLE-Arduino: Efficient BLE library for ESP32
- Arduino framework for ESP32

## 🔐 Security & Privacy

- No data is stored or transmitted to external servers
- BLE connection is local only
- Open source - audit the code yourself

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📝 License

This project is open source and available under the MIT License.

## 🙏 Acknowledgments

- Zwift for the FTMS protocol support
- NimBLE-Arduino library maintainers
- ESP32 community

## 📧 Support

- Issues: [GitHub Issues](https://github.com/DudaLuk/lukasz_esp32/issues)
- Discussions: [GitHub Discussions](https://github.com/DudaLuk/lukasz_esp32/discussions)

## 🎯 Roadmap

- [x] Basic speed sensing
- [x] FTMS BLE implementation
- [x] Zwift compatibility
- [ ] Incline sensing (accelerometer)
- [ ] Heart rate monitor integration
- [ ] Web-based configuration
- [ ] Mobile app for calibration
- [ ] 3D printable enclosure design
- [ ] PCB design

## 📸 Gallery

(Add photos of your build here!)

---

**Disclaimer**: This project involves modifying exercise equipment. Ensure all modifications are safe and do not void warranties. Use at your own risk.