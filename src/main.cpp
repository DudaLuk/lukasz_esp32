/**
 * Treadmill Speed Sensor with BLE for Zwift
 * 
 * This project reads speed data from a hall effect or optical sensor
 * and transmits it to Zwift via BLE using FTMS (Fitness Machine Service)
 * 
 * Hardware connections:
 * - Speed sensor: GPIO 4 (interrupt pin)
 * - LED indicator: GPIO 2 (built-in LED)
 */

#include <Arduino.h>
#include <NimBLEDevice.h>

// BLE Service and Characteristic UUIDs for FTMS
#define FTMS_SERVICE_UUID           "00001826-0000-1000-8000-00805f9b34fb"
#define TREADMILL_DATA_CHAR_UUID    "00002acd-0000-1000-8000-00805f9b34fb"
#define FEATURE_CHAR_UUID           "00002acc-0000-1000-8000-00805f9b34fb"
#define CONTROL_POINT_CHAR_UUID     "00002ad9-0000-1000-8000-00805f9b34fb"

// Device Information Service
#define DEVICE_INFO_SERVICE_UUID    "0000180a-0000-1000-8000-00805f9b34fb"
#define MANUFACTURER_NAME_UUID      "00002a29-0000-1000-8000-00805f9b34fb"
#define MODEL_NUMBER_UUID           "00002a24-0000-1000-8000-00805f9b34fb"

// Pin definitions
#define SPEED_SENSOR_PIN 4
#define LED_PIN 2

// Treadmill parameters
#define WHEEL_CIRCUMFERENCE_CM 200.0  // Circumference in cm, adjust based on your setup
#define PULSES_PER_REVOLUTION 1       // Number of pulses per wheel revolution

// Speed calculation constants
#define CM_TO_KM_FACTOR 100000.0      // Convert cm to km (100,000 cm = 1 km)
#define MS_TO_HOUR_FACTOR 3600000.0   // Convert ms to hours (3,600,000 ms = 1 hour)
#define SPEED_FILTER_ALPHA 0.7        // Weight for new speed reading (0.0-1.0)
#define SPEED_FILTER_BETA 0.3         // Weight for previous speed reading (0.0-1.0)

// Speed calculation variables
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
volatile unsigned int pulseCount = 0;
float currentSpeed = 0.0;  // Speed in km/h
unsigned long lastSpeedUpdate = 0;
const unsigned long SPEED_TIMEOUT = 2000;  // Reset speed if no pulse for 2 seconds

// BLE objects
NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pTreadmillDataChar = nullptr;
NimBLECharacteristic* pFeatureChar = nullptr;
NimBLECharacteristic* pControlPointChar = nullptr;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Server callbacks
class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Client connected");
        digitalWrite(LED_PIN, HIGH);
    }

    void onDisconnect(NimBLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Client disconnected");
        digitalWrite(LED_PIN, LOW);
    }
};

// Control Point callback handler
class ControlPointCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.print("Control Point received: ");
            for (int i = 0; i < value.length(); i++) {
                Serial.print(value[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
            
            // Handle control commands (e.g., reset, start, stop)
            if (value[0] == 0x00) {  // Request Control
                uint8_t response[] = {0x80, 0x00, 0x01};  // Success response
                pCharacteristic->setValue(response, 3);
                pCharacteristic->notify();
            }
        }
    }
};

// Interrupt service routine for speed sensor
// Note: Keep this ISR minimal - just capture timing data
void IRAM_ATTR speedSensorISR() {
    // Using micros() for more precise timing (1us resolution vs 1ms for millis)
    // This is safe in ISR as micros() uses hardware timer
    unsigned long currentTime = micros();
    pulseInterval = currentTime - lastPulseTime;
    lastPulseTime = currentTime;
    pulseCount++;
}

// Calculate speed from pulse interval
void updateSpeed() {
    unsigned long currentTime = micros();
    
    // Convert timeout to microseconds for comparison
    unsigned long timeoutMicros = SPEED_TIMEOUT * 1000UL;
    
    // Check for timeout (treadmill stopped)
    if (currentTime - lastPulseTime > timeoutMicros) {
        currentSpeed = 0.0;
        pulseInterval = 0;
        return;
    }
    
    // Calculate speed if we have valid pulse data
    if (pulseInterval > 0 && pulseInterval < timeoutMicros) {
        // Speed = (distance per pulse / time per pulse) converted to km/h
        // distance per pulse = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REVOLUTION (in cm)
        // time per pulse = pulseInterval (in microseconds)
        // speed (km/h) = (distance_cm / CM_TO_KM_FACTOR) / (time_us / (MS_TO_HOUR_FACTOR * 1000))
        float distancePerPulse = WHEEL_CIRCUMFERENCE_CM / PULSES_PER_REVOLUTION;
        currentSpeed = (distancePerPulse / CM_TO_KM_FACTOR) / (pulseInterval / (MS_TO_HOUR_FACTOR * 1000.0));
        
        // Apply exponential moving average filter to smooth speed readings
        static float lastSpeed = 0.0;
        currentSpeed = SPEED_FILTER_ALPHA * currentSpeed + SPEED_FILTER_BETA * lastSpeed;
        lastSpeed = currentSpeed;
    }
}

// Create FTMS Treadmill Data packet
void sendTreadmillData() {
    if (!deviceConnected || pTreadmillDataChar == nullptr) {
        return;
    }
    
    updateSpeed();
    
    // FTMS Treadmill Data format
    // Flags: 2 bytes
    // Instantaneous Speed: 2 bytes (0.01 km/h resolution)
    uint8_t data[4];
    
    // Flags - bit 0: Instantaneous Speed present
    uint16_t flags = 0x0001;
    data[0] = flags & 0xFF;
    data[1] = (flags >> 8) & 0xFF;
    
    // Speed in 0.01 km/h units
    uint16_t speedValue = (uint16_t)(currentSpeed * 100);
    data[2] = speedValue & 0xFF;
    data[3] = (speedValue >> 8) & 0xFF;
    
    pTreadmillDataChar->setValue(data, 4);
    pTreadmillDataChar->notify();
}

void initBLE() {
    Serial.println("Initializing BLE...");
    
    // Initialize NimBLE
    NimBLEDevice::init("Treadmill Sensor");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);  // Maximum power
    
    // Create BLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    
    // Create FTMS Service
    NimBLEService* pFTMSService = pServer->createService(FTMS_SERVICE_UUID);
    
    // Treadmill Data Characteristic (Notify)
    pTreadmillDataChar = pFTMSService->createCharacteristic(
        TREADMILL_DATA_CHAR_UUID,
        NIMBLE_PROPERTY::NOTIFY
    );
    
    // Feature Characteristic (Read)
    pFeatureChar = pFTMSService->createCharacteristic(
        FEATURE_CHAR_UUID,
        NIMBLE_PROPERTY::READ
    );
    
    // Set Treadmill Features
    // Bit 0: Instantaneous Speed supported
    uint8_t features[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    pFeatureChar->setValue(features, 8);
    
    // Control Point Characteristic (Write, Indicate)
    pControlPointChar = pFTMSService->createCharacteristic(
        CONTROL_POINT_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE
    );
    pControlPointChar->setCallbacks(new ControlPointCallbacks());
    
    // Start FTMS Service
    pFTMSService->start();
    
    // Create Device Information Service
    NimBLEService* pDeviceInfoService = pServer->createService(DEVICE_INFO_SERVICE_UUID);
    
    NimBLECharacteristic* pManufacturerChar = pDeviceInfoService->createCharacteristic(
        MANUFACTURER_NAME_UUID,
        NIMBLE_PROPERTY::READ
    );
    pManufacturerChar->setValue("Lukasz DIY");
    
    NimBLECharacteristic* pModelChar = pDeviceInfoService->createCharacteristic(
        MODEL_NUMBER_UUID,
        NIMBLE_PROPERTY::READ
    );
    pModelChar->setValue("Treadmill Sensor v1.0");
    
    pDeviceInfoService->start();
    
    // Start advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(FTMS_SERVICE_UUID);
    pAdvertising->addServiceUUID(DEVICE_INFO_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    
    pAdvertising->start();
    Serial.println("BLE advertising started");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Treadmill Speed Sensor Starting...");
    
    // Initialize LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize speed sensor pin with interrupt
    pinMode(SPEED_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), speedSensorISR, FALLING);
    
    // Initialize BLE
    initBLE();
    
    Serial.println("System ready. Waiting for connection...");
}

void loop() {
    // Handle BLE connection changes
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);  // Give the bluetooth stack time to get ready
        pServer->startAdvertising();
        Serial.println("Start advertising again");
        oldDeviceConnected = deviceConnected;
    }
    
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
    
    // Send treadmill data periodically when connected
    static unsigned long lastDataSend = 0;
    unsigned long currentTime = millis();
    
    if (deviceConnected && (currentTime - lastDataSend >= 500)) {
        sendTreadmillData();
        lastDataSend = currentTime;
        
        // Debug output
        Serial.print("Speed: ");
        Serial.print(currentSpeed, 2);
        Serial.print(" km/h, Pulses: ");
        Serial.print(pulseCount);
        Serial.print(", Interval: ");
        Serial.println(pulseInterval);
    }
    
    delay(10);
}
