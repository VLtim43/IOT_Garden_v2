# IOT Garden V.2

![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Espressif](https://img.shields.io/badge/espressif-E7352C.svg?style=for-the-badge&logo=espressif&logoColor=white)
![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)

---

- Soil Humidity Sensor
- Temp/Humidity Sensor
- Water Level Sensor
- Light Sensor
- Buzzer
- OLED Screen
- IR controls

## Current Firmware

- OLED status screen with boot splash
- DS1302 RTC clock shown on the OLED
- Soil, water level, temperature, and ambient light sensor tasks
- WS2812B LED panel control on a 25 LED panel
- Water pump pulse control with cooldown protection
- IR remote logging, button decoding, and manual controls

## Current Manual Controls

- `LEFT` and `RIGHT`: cycle LED panel colors `PURPL -> RED -> BLUE`
- `OK`: toggle LED panel on and off
- `STAR`: run the water pump for `3 s`, then force `2 s` cooldown

## Current OLED Info

- Time from the DS1302 RTC
- Temperature
- Soil reading
- Water level percent
- Current LED color or `OFF`
- Last IR command received
- Pump state: `ON`, `COOLDWN`, or `OFF`

## Pin Map

- OLED I2C: `SDA=GPIO21`, `SCL=GPIO22`
- DHT11: `GPIO27`
- IR receiver: `GPIO26`
- Water pump control: `GPIO4`
- WS2812B data: `GPIO5`
- DS1302 RTC: `CLK=GPIO18`, `DAT=GPIO19`, `RST=GPIO23`
- Soil sensor ADC: `GPIO33`
- Water level ADC: `GPIO35`
- Light sensor DO: `GPIO34`

## State / Task Setup

All modules share data through the global garden state. Sensor tasks write new readings into state, actuator tasks update outputs from state, and the display task periodically reads state to draw the OLED UI.

```text
                                       ┌────────────────┐
                                       │     State      │
                                       └───────┬────────┘
                                               │
                    ┌──────────────────────────┼──────────────────────────┐
                    │                          │                          │
             ┌──────┴───────┐           ┌──────┴───────┐           ┌──────┴───────┐
             │    Sensor    │           │   Actuator   │           │   Display    │
             │    Tasks     │           │     Task     │           │    Tasks     │
             └──────────────┘           └──────────────┘           └──────────────┘
```

## Main Directory Tree

```text
main/
├── CMakeLists.txt
├── idf_component.yml
├── config/
│   ├── ir_codes.h
│   └── pins.h
├── main.c
└── modules/
    ├── actuators/
    │   ├── water_pump/
    │   │   └── water_pump.c
    │   └── w2812b/
    │       └── w2812b.c
    ├── clock/
    │   └── clock.c
    ├── display/
    │   └── OLED_display.c
    ├── input/
    │   └── ir_remote.c
    ├── sensors/
    │   ├── adc_shared.c
    │   ├── sensor_dht/
    │   │   └── sensor_dht.c
    │   ├── sensor_light/
    │   │   └── sensor_light.c
    │   ├── sensor_soil/
    │   │   └── sensor_soil.c
    │   └── sensor_water/
    │       └── sensor_water.c
    └── state/
        └── state.c
```
