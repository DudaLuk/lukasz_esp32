# Czujnik Prędkości Bieżni dla Zwift - Instrukcja w języku polskim

## 📱 Przegląd projektu

Ten projekt to DIY czujnik prędkości dla bieżni oparty na ESP32, który łączy się z aplikacją Zwift przez Bluetooth Low Energy (BLE). Umożliwia śledzenie treningów biegowych na bieżni w aplikacji Zwift przy użyciu standardowego protokołu FTMS (Fitness Machine Service).

## 🎯 Funkcje

- ✅ **Kompatybilny ze Zwift**: Używa standardowego protokołu Bluetooth FTMS
- ✅ **Pomiar prędkości w czasie rzeczywistym**: Dokładny pomiar prędkości z rolki bieżni
- ✅ **Łatwa instalacja**: Prosty montaż czujnika Halla lub optycznego
- ✅ **Niski koszt**: Całkowity koszt poniżej 60 PLN
- ✅ **Bezprzewodowy**: Połączenie BLE z urządzeniem z Zwift
- ✅ **Open Source**: W pełni konfigurowalny firmware i hardware

## 📋 Wymagania

### Hardware
- Płytka rozwojowa ESP32 (ESP32-DevKitC lub podobna)
- Czujnik Halla (A3144) lub czujnik optyczny (TCRT5000)
- Magnes neodymowy (10mm x 3mm)
- Podstawowe komponenty (rezystory, kondensator)
- Pełna lista materiałów w [hardware/README.md](../hardware/README.md)

### Software
- PlatformIO (zalecane) lub Arduino IDE
- Aplikacja Zwift (iOS, Android, Windows lub macOS)

## 🚀 Szybki start

### 1. Montaż hardware

1. **Zamocuj magnes** do rolki bieżni lub napędu pasa
2. **Zamontuj czujnik** 2-5mm od ścieżki magnesu
3. **Podłącz czujnik** do ESP32 GPIO4
4. **Zasil ESP32** przez USB (5V)

Szczegółowa instrukcja w [hardware/README.md](../hardware/README.md)

### 2. Wgranie firmware

#### Używając PlatformIO (zalecane)

```bash
# Sklonuj repozytorium
git clone https://github.com/DudaLuk/lukasz_esp32.git
cd lukasz_esp32

# Zainstaluj PlatformIO Core
pip install platformio

# Zbuduj i wgraj
pio run --target upload

# Monitoruj port szeregowy
pio device monitor
```

#### Używając Arduino IDE

1. Zainstaluj wsparcie dla płytek ESP32
2. Zainstaluj bibliotekę NimBLE-Arduino (v1.4.1+)
3. Otwórz `src/main.cpp`
4. Wybierz płytkę: "ESP32 Dev Module"
5. Wgraj

### 3. Kalibracja

1. **Zmierz obwód koła**:
   - Ręcznie obróć pas bieżni o jeden pełny obrót
   - Zmierz przebytą odległość
   - Zaktualizuj wartość `WHEEL_CIRCUMFERENCE_CM` w `src/main.cpp`

2. **Ustaw impulsy na obrót**:
   - Domyślnie: 1 (jeden magnes)
   - Jeśli masz wiele magnesów, zaktualizuj `PULSES_PER_REVOLUTION`

3. **Test**:
   - Uruchom bieżnię ze znaną prędkością (np. 5 km/h)
   - Sprawdź wyjście monitora szeregowego
   - Zweryfikuj dokładność prędkości

### 4. Połącz ze Zwift

1. Włącz ESP32
2. Otwórz Zwift
3. Przejdź do parowania urządzeń
4. Poszukaj "Treadmill Sensor" na liście
5. Sparuj jako Treadmill (Bieżnia)
6. Zacznij biegać!

## 📐 Schemat połączeń

```
Połączenia ESP32:
┌─────────────────┐
│     ESP32       │
│                 │
│  GPIO4 ◄────────┼─── OUT czujnika Halla (z pull-up 10kΩ)
│  GPIO2 ─────────┼─── LED (opcjonalny, wskaźnik statusu)
│  3.3V  ─────────┼─── VCC czujnika
│  GND   ─────────┼─── GND czujnika
│  VIN   ◄────────┼─── Zasilanie USB 5V
└─────────────────┘

Okablowanie czujnika:
Czujnik Halla (A3144):
  Pin 1 (VCC) → 3.3V
  Pin 2 (GND) → GND  
  Pin 3 (OUT) → GPIO4 + 10kΩ do 3.3V + 100nF do GND
```

Pełny schemat w katalogu [hardware/](../hardware/)

## 🔧 Konfiguracja

Edytuj te wartości w `src/main.cpp` aby dopasować do Twojej bieżni:

```cpp
// Konfiguracja pinów
#define SPEED_SENSOR_PIN 4    // Zmień jeśli używasz innego GPIO
#define LED_PIN 2             // Wbudowana dioda LED

// Kalibracja
#define WHEEL_CIRCUMFERENCE_CM 200.0  // Zmierz swoją bieżnię
#define PULSES_PER_REVOLUTION 1       // Liczba magnesów

// Obliczanie prędkości
const unsigned long SPEED_TIMEOUT = 2000;  // ms do uznania zatrzymania
```

## 📊 Jak to działa

1. **Detekcja czujnika**: Czujnik Halla wykrywa przejście magnesu raz na obrót rolki
2. **Obliczanie prędkości**: Firmware oblicza prędkość na podstawie czasu między impulsami
3. **Transmisja BLE**: Dane o prędkości wysyłane przez protokół FTMS (co 500ms)
4. **Integracja ze Zwift**: Zwift odbiera dane jak z komercyjnej bieżni

Wzór:
```
Prędkość (km/h) = (Obwód_cm / Czas_między_impulsami_ms) * 3.6
```

## 🔍 Rozwiązywanie problemów

| Problem | Rozwiązanie |
|---------|-------------|
| Brak wykrywanych impulsów | Sprawdź zasilanie czujnika, odległość magnesu (2-5mm), okablowanie |
| Chaotyczne odczyty | Dodaj większy kondensator (220nF), sprawdź luźne połączenia |
| BLE się nie łączy | Zrestartuj ESP32, sprawdź wyjście szeregowe, zweryfikuj ustawienia Zwift |
| Prędkość zbyt wysoka/niska | Przekalibruj WHEEL_CIRCUMFERENCE_CM, sprawdź PULSES_PER_REVOLUTION |
| Połączenie się przerywa | Zmniejsz odległość do urządzenia, sprawdź stabilność zasilania |

Szczegółowe rozwiązywanie problemów w [hardware/README.md](../hardware/README.md)

## 📱 Przetestowane z

- ✅ Zwift (Windows, macOS, iOS)
- ✅ TrainerRoad
- ✅ Inne aplikacje kompatybilne z FTMS

## 🛠️ Struktura projektu

```
lukasz_esp32/
├── src/
│   └── main.cpp           # Główny kod firmware
├── hardware/
│   ├── README.md          # Dokumentacja hardware
│   └── schematic.md       # Schemat obwodu
├── docs/
│   └── README_PL.md       # Ten plik (instrukcja po polsku)
├── platformio.ini         # Konfiguracja PlatformIO
└── README.md             # Główny README (po angielsku)
```

## 💰 Szacunkowe koszty (PLN)

| Element | Cena |
|---------|------|
| ESP32 Development Board | 25-40 PLN |
| Czujnik Halla A3144 | 2-3 PLN |
| Magnes neodymowy | 2-3 PLN |
| Rezystory, kondensatory | 1-2 PLN |
| Przewody, płytka stykowa | 5-10 PLN |
| Obudowa (opcjonalna) | 5-15 PLN |
| **Łącznie** | **35-75 PLN** |

## 🔐 Bezpieczeństwo i prywatność

- Żadne dane nie są przechowywane ani przesyłane na zewnętrzne serwery
- Połączenie BLE jest tylko lokalne
- Open source - możesz sam sprawdzić kod

## 📝 Licencja

Ten projekt jest open source i dostępny na licencji MIT.

## 🙏 Podziękowania

- Zwift za wsparcie protokołu FTMS
- Opiekunom biblioteki NimBLE-Arduino
- Społeczność ESP32

## 📧 Wsparcie

- Problemy: [GitHub Issues](https://github.com/DudaLuk/lukasz_esp32/issues)
- Dyskusje: [GitHub Discussions](https://github.com/DudaLuk/lukasz_esp32/discussions)

## ⚠️ Zastrzeżenie

Ten projekt wymaga modyfikacji sprzętu do ćwiczeń. Upewnij się, że wszystkie modyfikacje są bezpieczne i nie unieważniają gwarancji. Używasz na własne ryzyko.

---

**Powodzenia z projektem!** 🏃‍♂️💨
