# Bio Master

**Smart greenhouse automation system with crop-specific climate profiles**

[![Arduino](https://img.shields.io/badge/Arduino-UNO-00979D?style=flat-square&logo=arduino)](https://www.arduino.cc/)
[![ESP8266](https://img.shields.io/badge/ESP8266-WiFi-blue?style=flat-square&logo=espressif)](https://www.espressif.com/)
[![Blynk](https://img.shields.io/badge/Blynk-IoT-23C48E?style=flat-square)](https://blynk.io/)
[![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)](LICENSE)

---

## Overview

Bio Master is a greenhouse automation controller that monitors temperature, humidity, soil moisture, and light — then automatically controls irrigation, heating, ventilation, and grow lights based on the selected crop profile.

Select a crop from the LCD menu (tomato, potato, cucumber, etc.) and the system auto-configures optimal temperature range, humidity range, and soil moisture threshold. Or set custom values manually.

**What it does:**

- 10 built-in crop profiles with optimal climate parameters
- Custom parameter mode for any crop
- Automatic irrigation based on soil moisture threshold
- Automatic heating/ventilation based on temperature range
- Automatic grow light control based on ambient light (LDR)
- WiFi remote control via Blynk app (manual override for pump and light)
- Captive portal for WiFi setup (no hardcoded credentials)
- LCD menu navigation with rotary encoder

---

## Architecture

```
┌──────────────────┐      SoftwareSerial       ┌──────────────────┐
│   Arduino UNO    │◄──────────────────────────►│    ESP8266       │
│                  │     <T23.4> <H65.0>        │                  │
│  DHT11 (temp/hum)│     <S45> <I72>            │  WiFi + Blynk    │
│  Soil sensor (A0)│     ────────────►          │  Captive Portal  │
│  LDR (A1)        │                            │  LittleFS config │
│  Rotary encoder  │     <R1> <L0>              │                  │
│  LCD 16x2 I2C    │     ◄────────────          │  Web UI (glass   │
│  4 relays        │     (pump/light cmds)      │  design CSS)     │
│  Buzzer          │                            │                  │
└──────────────────┘                            └──────────────────┘
         │                                               │
         ▼                                               ▼
  ┌─────────────┐                                ┌──────────────┐
  │  Greenhouse  │                                │  Blynk App   │
  │  Pump        │                                │  V1: Temp    │
  │  Heater      │                                │  V2: Humidity│
  │  Ventilation │                                │  V3: Soil    │
  │  Grow Light  │                                │  V4: Pump    │
  └─────────────┘                                │  V5: Light   │
                                                  │  V6: LDR     │
                                                  └──────────────┘
```

---

## Crop Profiles

Select a crop from the LCD menu and the system auto-configures all parameters:

| Crop | Temp Min | Temp Max | Humidity Min | Humidity Max | Soil Moisture |
|------|----------|----------|-------------|-------------|---------------|
| Tomato | 25°C | 30°C | 60% | 80% | 60% |
| Carrot | 15°C | 25°C | 40% | 60% | 60% |
| Potato | 15°C | 22°C | 70% | 80% | 65% |
| Onion | 15°C | 25°C | 40% | 60% | 60% |
| Eggplant | 21°C | 30°C | 70% | 80% | 65% |
| Cabbage | 15°C | 20°C | 70% | 80% | 70% |
| Pumpkin | 21°C | 30°C | 50% | 70% | 70% |
| Watermelon | 21°C | 30°C | 50% | 70% | 80% |
| Melon | 21°C | 30°C | 50% | 70% | 80% |
| Cucumber | 25°C | 30°C | 60% | 80% | 65% |
| Custom | User-defined | User-defined | User-defined | User-defined | User-defined |

---

## Hardware

| Component | Qty | Purpose |
|-----------|-----|---------|
| Arduino UNO | 1 | Main controller — sensors, relays, LCD, encoder |
| ESP8266 (NodeMCU) | 1 | WiFi, Blynk cloud, captive portal |
| DHT11 | 1 | Temperature + humidity sensor |
| Soil moisture sensor | 1 | Analog soil moisture (A0) |
| LDR (photoresistor) | 1 | Ambient light level (A1) |
| Rotary encoder (KY-040) | 1 | Menu navigation + parameter adjustment |
| LCD 16x2 I2C | 1 | Display — sensor data + menu |
| 4-channel relay module | 1 | Pump, heater, ventilation, grow light |
| Buzzer | 1 | Audio feedback on menu selection |

### Pin Map

```
Arduino UNO
────────────────────────────────────
D2        → Relay 2 (grow light)
D3        → ESP8266 SoftwareSerial
D4        → ESP8266 SoftwareSerial
D5        → Encoder SW (button)
D6        → Encoder CLK
D7        → Encoder DT
D8        → DHT11 data
D9        → Buzzer
D11       → Relay 4 (ventilation)
D12       → Relay 3 (heater)
D13       → Relay 1 (water pump)
A0        → Soil moisture sensor
A1        → LDR (photoresistor)

ESP8266 (NodeMCU)
────────────────────────────────────
D2 (GPIO4) → Arduino SoftwareSerial RX
D3 (GPIO0) → Arduino SoftwareSerial TX
```

---

## Communication Protocol

Arduino and ESP8266 exchange data via framed serial protocol at 9600 baud:

```
Frame format:  <TYPE VALUE>\n

Arduino → ESP8266 (sensor data):
  <T23.4>   Temperature (°C)
  <H65.0>   Humidity (%)
  <S45>     Soil moisture (%)
  <I72>     Light level (%)

ESP8266 → Arduino (commands):
  <R1>      Pump ON (manual override)
  <R0>      Pump OFF (auto mode)
  <L1>      Light ON (manual override)
  <L0>      Light OFF (auto mode)
```

---

## Control Logic

**Irrigation:** Soil moisture below threshold → pump ON. Moisture reaches threshold → pump OFF (unless manual override from Blynk).

**Temperature:** Below min → heater + ventilation ON. Above max → ventilation ON. In range → both OFF.

**Lighting:** LDR below 30% → grow light ON. Above 30% → light OFF (unless manual override from Blynk).

**LCD backlight:** Auto-off after 200 seconds of inactivity. Encoder input wakes it.

---

## WiFi Setup

On first boot, ESP8266 creates an access point:

```
SSID:     Bio Master
Password: samurai2023
```

1. Connect to "Bio Master" WiFi from your phone
2. Captive portal opens automatically (iOS, Android, Windows)
3. Enter your home WiFi SSID and password
4. Credentials saved to LittleFS — survives reboots

---

## Blynk Virtual Pins

| Pin | Type | Direction | Description |
|-----|------|-----------|-------------|
| V1 | Float | ESP → App | Temperature (°C) |
| V2 | Float | ESP → App | Humidity (%) |
| V3 | Int | ESP → App | Soil moisture (%) |
| V4 | Int | App → ESP | Pump control (0=auto, 1=ON) |
| V5 | Int | App → ESP | Light control (0=auto, 1=ON) |
| V6 | Int | ESP → App | Ambient light (%) |

---

## Project Structure

```
Bio-Master/
├── firmware/
│   ├── Bio_Master/
│   │   ├── Bio_Master.ino       # Main: setup, loop, sensors, serial protocol
│   │   ├── Encoder.ino          # Rotary encoder + LCD menu navigation
│   │   ├── Ekinlar.ino          # 10 crop profiles with preset parameters
│   │   ├── Boshqalar.ino        # Custom parameter input menu
│   │   ├── Harorat.ino          # Temperature control (heater + ventilation)
│   │   ├── Rele.ino             # Relay control helpers
│   │   └── Chiqish.ino          # Menu exit handler
│   └── Bio_Master_Wifi/
│       └── Bio_Master_Wifi.ino  # ESP8266: WiFi, Blynk, captive portal
├── README.md
└── LICENSE
```

---

## Quick Start

### 1. Flash Arduino UNO

```
1. Open Bio_Master/Bio_Master.ino in Arduino IDE
2. Install libraries: GyverEncoder, DHT, LiquidCrystal_I2C
3. Select board: Arduino UNO
4. Upload
```

### 2. Flash ESP8266

```
1. Open Bio_Master_Wifi/Bio_Master_Wifi.ino in Arduino IDE
2. Install libraries: ESP8266WiFi, BlynkSimpleEsp8266, ArduinoJson, LittleFS
3. Select board: NodeMCU 1.0 (ESP-12E Module)
4. Upload
```

### 3. Connect WiFi

Connect to "Bio Master" AP → enter WiFi credentials → done.

---

## Dependencies

| Library | Platform | Purpose |
|---------|----------|---------|
| GyverEncoder | Arduino | Rotary encoder with button |
| DHT | Arduino | DHT11 temperature/humidity |
| LiquidCrystal_I2C | Arduino | 16x2 LCD via I2C |
| ESP8266WiFi | ESP8266 | WiFi connectivity |
| BlynkSimpleEsp8266 | ESP8266 | Blynk IoT cloud |
| ArduinoJson | ESP8266 | Config file parsing |
| LittleFS | ESP8266 | Flash filesystem for config |
| DNSServer | ESP8266 | Captive portal DNS redirect |

---

## Author

**Temur Eshmurodov** — [@myseringan](https://github.com/myseringan)

## License

MIT License — free to use and modify.
