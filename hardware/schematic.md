# Schematic Diagram - Treadmill Speed Sensor

## Main Circuit Diagram

```
                                    +--------------------+
                                    |                    |
                                    |      ESP32         |
                                    |   Development      |
                                    |      Board         |
                                    |                    |
                +3.3V               |                    |
                  |                 |  VIN (5V) o--------+------ USB 5V Power
                  |                 |                    |
                  |     +-----------+  3.3V     o--------+
                  |     |           |                    |      +----------+
                  +-----+           |  GND      o--------+------+   GND    |
                  |     |           |                    |      +----------+
                  |   [10kΩ]        |  GPIO4    o--------+--+
                  |     |           |  (Sensor) |        |  |
                  |     |           |                    |  |   Hall Effect
                  |     +------+----+  GPIO2    o--------+--+-- Sensor (A3144)
                  |            |    |  (LED)    |        |  |   or Optical
        +--------(+)-----------|----+           |        |  |   Sensor (TCRT5000)
        |         |            |    |  EN       o        |  |
        |      [100nF]         |    |                    |  +--- OUT (Signal)
        |         |           LED   +--------------------+
        |        GND       [220Ω]                           |
        |                    |                              +--- VCC (3.3V)
        |                   GND                             |
        +------------------------------------------------------+--- GND
                                                           

Magnet on                     Hall Sensor
Treadmill Roller              Mounted Fixed
      |                             |
      |   [N][S]                    |
      |     ↓                       | ← 2-5mm gap
      |  Rotation                   |
      +-----------------------------+
           Pulse generated each
           revolution
```

## Component Details

### Hall Effect Sensor Pinout (A3144 - Facing flat side)
```
    +-----------+
    |  A3144    |
    |  ======   |
    |           |
    |  1  2  3  |
    +--+--+--+--+
       |  |  |
       |  |  +---- OUT (Signal to GPIO4)
       |  +------- GND
       +---------- VCC (3.3V)
```

### Optical Sensor Module (TCRT5000)
```
    +------------------+
    |   TCRT5000       |
    | +----+  +----+   |
    | | IR |  |Photo|  |
    | |LED |  |Trans|  |
    | +----+  +----+   |
    |                  |
    | VCC  GND  DO  AO |
    +--+---+----+---+--+
       |   |    |   |
       |   |    |   +---- AO (Analog, not used)
       |   |    +-------- DO (Digital to GPIO4)
       |   +------------- GND
       +----------------- VCC (3.3V)
```

### ESP32 DevKit Pinout Reference
```
                          +------------+
                          |    USB     |
                          +------------+
    EN     o----+----+----o  ESP32    o----+----+----o  3V3
    VP(36) o    |    |    o  DevKit   o    |    |    o  GND
    VN(39) o    |    |    o  Board    o    |    |    o  IO15
    IO34   o    L    T    o            o    R    T    o  IO2   <-- Status LED
    IO35   o    e    o    o            o    i    o    o  IO0
    IO32   o    f    p    o            o    g    p    o  IO4   <-- Speed Sensor
    IO33   o    t         o            o    h         o  IO16
    IO25   o              o            o    t         o  IO17
    IO26   o    S         o            o              o  IO5
    IO27   o    i         o            o    S         o  IO18
    IO14   o    d         o            o    i         o  IO19
    IO12   o    e         o            o    d         o  IO21
    GND    o              o            o    e         o  RX
    IO13   o              o            o              o  TX
    D2(9)  o              o            o              o  IO22
    D3(10) o              o            o              o  IO23
    CMD    o              o            o              o  GND
           +----+----+----+            +----+----+----+
                |    |                      |    |
           Bottom View                 Bottom View
```

## Wiring Steps

### Step 1: Power Connections
1. Connect ESP32 GND to breadboard GND rail (blue/black line)
2. Connect ESP32 3.3V to breadboard power rail (red line)

### Step 2: Sensor Connections
3. Connect Hall sensor VCC to 3.3V rail
4. Connect Hall sensor GND to GND rail
5. Connect Hall sensor OUT to a new row on breadboard

### Step 3: Pull-up Resistor
6. Connect 10kΩ resistor between sensor OUT row and 3.3V rail

### Step 4: Filtering Capacitor
7. Connect 100nF capacitor between sensor OUT row and GND

### Step 5: ESP32 Connection
8. Connect sensor OUT row to ESP32 GPIO4 with jumper wire

### Step 6: Optional LED (Status Indicator)
9. Connect LED anode (+) to GPIO2
10. Connect LED cathode (-) to 220Ω resistor
11. Connect resistor other end to GND

### Step 7: Power Supply
12. Connect USB cable to ESP32 board

## Physical Installation on Treadmill

```
Side View of Treadmill Roller:

    Treadmill Frame
         |
         |    Hall Sensor
         |    (mounted)
         |        |
         |     +--+--+
         |     |  ▼  |
         +-----+     +-----
               |     |
            ~~~|~~~~~|~~~  ← Treadmill Belt
               |     |
        +------+-----+------+
        |                   |
        |    Roller with    |  ← Magnet attached here
        |  [M] Magnet       |
        |                   |
        +-------------------+
                ↓ Rotation
```

## Testing Setup

### Bench Test (Before Installation)
```
1. Power ESP32 via USB
2. Hold magnet near sensor
3. Move magnet back and forth
4. Check serial monitor for pulse detection

Test Pattern:
   Magnet     Sensor
     [N]        |
      ↓         |
     [S]    →   | ← Pulses detected
      ↓         |
     [N]        |
```

## Alternative Sensor Options

### Option 1: Hall Effect (Recommended for Reliability)
- Pros: No line-of-sight needed, weather resistant
- Cons: Requires magnet installation
- Detection range: 2-10mm

### Option 2: Optical Reflective (TCRT5000)
- Pros: No magnet needed, easy mounting
- Cons: Sensitive to dirt/dust, needs reflective target
- Detection range: 3-10mm

### Option 3: Optical Interrupter (GP1A57HRJ00F)
- Pros: Reliable, no reflector needed
- Cons: Requires physical slot/gap
- Detection range: 3-8mm

## Safety Notes

⚠️ **Important Safety Considerations:**

1. **Insulation**: Use heat shrink tubing on all exposed connections
2. **Strain Relief**: Secure all cables to prevent pulling on connections
3. **Mounting**: Ensure sensor is firmly mounted and won't move during operation
4. **Magnet Security**: Use strong adhesive or mechanical fastening for magnet
5. **Cable Routing**: Keep all cables away from moving parts
6. **Grounding**: Ensure proper grounding to prevent static discharge
7. **Enclosure**: Consider waterproof enclosure if treadmill gets sweaty/wet

## Bill of Materials (BOM)

| Item | Part Number | Quantity | Price (USD) | Link/Source |
|------|-------------|----------|-------------|-------------|
| ESP32 Dev Board | ESP32-DevKitC | 1 | $6-10 | AliExpress/Amazon |
| Hall Sensor | A3144 or OH3144 | 1 | $0.50 | Electronics supplier |
| Neodymium Magnet | 10mm x 3mm | 1 | $0.50 | Hardware store |
| Resistor 10kΩ | 1/4W | 1 | $0.01 | Electronics supplier |
| Capacitor 100nF | Ceramic | 1 | $0.05 | Electronics supplier |
| LED (optional) | 5mm Blue/Green | 1 | $0.10 | Electronics supplier |
| Resistor 220Ω | 1/4W | 1 | $0.01 | Electronics supplier |
| Breadboard | Mini | 1 | $1.50 | Electronics supplier |
| Jumper Wires | Male-Male | 10 | $1.00 | Electronics supplier |
| USB Cable | Micro USB | 1 | $1.00 | - |
| **Total** | | | **$10-15** | |

Optional:
- Enclosure: $2-5
- Perfboard/PCB: $1-3
- Heat shrink tubing: $1
- Cable management: $1

---

Last Updated: 2026-01-20
Version: 1.0
