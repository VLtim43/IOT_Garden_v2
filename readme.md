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
│   └── pins.h
├── main.c
└── modules/
    ├── actuators/
    │   ├── water_pump/
    │   │   ├── water_pump.c
    │   │   └── water_pump.h
    │   └── w2812b/
    │       ├── w2812b.c
    │       └── w2812b.h
    ├── clock/
    │   ├── clock.c
    │   └── clock.h
    ├── display/
    │   ├── OLED_display.c
    │   └── OLED_display.h
    ├── input/
    │   ├── ir_remote.c
    │   └── ir_remote.h
    ├── sensors/
    │   ├── adc_shared.c
    │   ├── adc_shared.h
    │   ├── sensor_dht/
    │   │   ├── sensor_dht.c
    │   │   └── sensor_dht.h
    │   ├── sensor_light/
    │   │   ├── sensor_light.c
    │   │   └── sensor_light.h
    │   ├── sensor_soil/
    │   │   ├── sensor_soil.c
    │   │   └── sensor_soil.h
    │   └── sensor_water/
    │       ├── sensor_water.c
    │       └── sensor_water.h
    └── state/
        ├── state.c
        └── state.h
```
