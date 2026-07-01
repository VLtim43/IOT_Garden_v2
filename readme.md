# IoT Garden v2

ESP32 firmware for a small IoT garden project built with ESP-IDF and C.

It monitors plant and environment data, shows live status on an OLED, accepts IR remote input, and controls a WS2812B LED panel plus a water pump through a shared runtime state.

![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Espressif](https://img.shields.io/badge/espressif-E7352C.svg?style=for-the-badge&logo=espressif&logoColor=white)
![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)

## Firmware Scope

- Soil moisture reading with ADC
- Water reservoir level reading with ADC
- Temperature reading from DHT11
- Ambient light detection
- DS1302 real-time clock display
- SSD1306 OLED status screen
- NEC IR remote input and manual control
- WS2812B LED panel control
- Simple RGB demo LED actuator control
- Buzzer alarm output
- Water pump pulse control with cooldown protection
- Queue-based control task for manual and automation actions
- Default time-and-light-based LED automation rules
- Demo-friendly light-triggered RGB automation rules
- Low-water buzzer alarm rule

## Current Behavior

- `app_main()` initializes shared state, then starts sensor, clock, actuator, control, IR, and OLED modules.
- Sensor tasks publish readings into a mutex-protected `garden_state_t`.
- The IR module decodes commands and sends them to the control queue.
- The control task handles manual commands and polls automation rules.
- Default automation rules switch LEDs to `RED` at night after `18:00`, and back to `PURPL` during day after `06:00`.
- Demo RGB rules switch the discrete RGB LED to `RED` when light is blocked and `GREEN` when light is detected.
- Low-water rule buzzes the buzzer three times when water level drops below `5%`.
- Actuator modules apply LED and pump changes, then write visible status back into shared state.
- The OLED task reads shared state and redraws only changed fields.

## Manual IR Controls

- `LEFT`: cycle LED color left
- `RIGHT`: cycle LED color right
- `OK`: toggle LED panel on or off
- `STAR`: trigger pump pulse
- `1`: RGB LED `RED`
- `2`: RGB LED `GREEN`
- `3`: RGB LED `BLUE`
- `4`: RGB LED `YELLOW`
- `5`: RGB LED `MAGENTA`
- `6`: RGB LED `CYAN`
- `7`: RGB LED `WHITE`
- `8`: RGB LED `OFF`
- `9`: RGB LED `RED` again

## OLED Screen

- RTC time
- Temperature
- Soil reading
- Water level percent
- Current LED color or `OFF`
- Last IR command
- Pump state

## Hardware Used

- ESP32 development board
- DHT11 temperature sensor
- Capacitive soil moisture sensor
- Water level sensor
- Light sensor module
- SSD1306 OLED over I2C
- IR receiver
- DS1302 RTC
- WS2812B 25-LED panel
- Simple RGB LED on three GPIO outputs
- Active buzzer on one GPIO output
- Small water pump

## Pin Map

- OLED I2C: `SDA=GPIO22`, `SCL=GPIO23`
- DHT11: `GPIO27`
- IR receiver: `GPIO26`
- Water pump control: `GPIO4`
- Buzzer: `GPIO5`
- WS2812B data: `GPIO14`
- RGB LED: `BLUE=GPIO13`, `GREEN=GPIO25`, `RED=GPIO32`
- DS1302 RTC: `CLK=GPIO18`, `DAT=GPIO19`, `RST=GPIO21`
- Soil sensor ADC: `GPIO33`
- Water level ADC: `GPIO35`
- Light sensor digital output: `GPIO34`

## Architecture

Core modules:

- `state/`: mutex-protected shared `garden_state_t`
- `sensors/`: DHT11, soil ADC, water ADC, light sensor, ADC helper
- `clock/`: DS1302 RTC update loop
- `input/`: IR receiver and NEC decoding
- `control/`: action queue and automation evaluation
- `control/rules/`: built-in automation rule definitions
- `actuators/`: WS2812B, discrete RGB LED, buzzer, and water pump logic
- `display/`: OLED rendering

## Default Automation Rules

- Night rule: if ambient light is not detected and time is `>= 18:00`, set LED color to `RED`
- Day rule: if ambient light is detected and time is `>= 06:00`, set LED color to `PURPL`
- Low-water rule: if water level is `< 5%`, buzz three times
- Demo RGB night rule: if ambient light is not detected, set RGB LED to `RED`
- Demo RGB day rule: if ambient light is detected, set RGB LED to `GREEN`
- Fixed-color automation uses the same LED palette as manual IR control: `PURPL`, `RED`, `BLUE`
- Built-in rules are defined in `main/modules/control/rules/default_rules.c`

## Control Flow

```text

┌─────────────────┐                     ┌─────────────────┐
│    IR Remote    │-------------------->  Control Queue   │
└─────────────────┘                     └────────┬────────┘
                                                 │
                                                 v
                                        ┌─────────────────┐
┌─────────────────┐                     │  Control Task   │
│  Shared State   │<------------------->│                 │
└────────┬────────┘                     └────────┬────────┘
         │                                       │
         │                                       v
         │                              ┌─────────────────┐
         └----------------------------->│ Automation Eval │
                                        └────────┬────────┘
                                                 │
                                                 v
                                        ┌─────────────────┐
                                        │ LED / Pump /    │
                                        │ RGB / Buzzer    │
                                        └─────────────────┘
```

## Repository Layout

```text
.
├── CMakeLists.txt
├── dependencies.lock
├── main/
│   ├── CMakeLists.txt
│   ├── config/
│   │   ├── ir_codes.h
│   │   └── pins.h
│   ├── idf_component.yml
│   ├── main.c
│   └── modules/
│       ├── actuators/
│       ├── clock/
│       ├── control/
│       ├── display/
│       ├── input/
│       ├── sensors/
│       └── state/
└── readme.md
```

## Build

ESP-IDF environment required.

```bash
idf.py build
```

Flash and monitor when hardware is connected:

```bash
idf.py flash monitor
```

## Electrical Notes

- ESP32 GPIO is `3.3 V` logic.
- WS2812B panel and pump should use a separate `5 V` supply.
- ESP32 and external `5 V` supply must share common ground.
- Do not drive pump directly from an ESP32 GPIO. Use a transistor, MOSFET, relay, or motor driver.
- If the LED panel needs a stronger data signal, use level shifting.
