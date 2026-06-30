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
- Queue-based control task for IR and automation actions
- Shared mutex-protected global state used by all runtime tasks

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

- OLED I2C: `SDA=GPIO22`, `SCL=GPIO23`
- DHT11: `GPIO27`
- IR receiver: `GPIO26`
- Water pump control: `GPIO4`
- WS2812B data: `GPIO14`
- DS1302 RTC: `CLK=GPIO18`, `DAT=GPIO19`, `RST=GPIO21`
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

## Runtime Architecture

- `app_main()` initializes the shared state, then starts sensor, clock, control, IR, and OLED tasks.
- Sensor tasks and the RTC task publish readings into `garden_state_t`.
- The IR task decodes NEC remote frames and submits commands to the control queue.
- The control task handles manual IR commands and periodic automation rule evaluation.
- Actuator modules apply LED and pump changes, then write user-visible status back into shared state.
- The OLED task polls shared state and only redraws fields that changed.

```text
Hardware
  -> Sensor / RTC tasks
  -> Shared garden state
  -> Control task + automation rules
  -> Actuators
  -> OLED status display
```

## Garden State Layout

The shared state is a single mutex-protected `garden_state_t` struct. Producers write into it, and control and display logic read from it.

```text
                           ┌──────────────────────────────┐
                           │        garden_state_t        │
                           ├──────────────────────────────┤
                           │ ambient_light_detected       │
                           │ temperature_c                │
                           │ soil_raw                     │
                           │ water_level_percent          │
                           │ ir_activity_count            │
                           │ time_text                    │
                           │ led_color_code               │
                           │ ir_command                   │
                           │ pump_status                  │
                           └──────────────┬───────────────┘
                                          │
              ┌───────────────────────────┼───────────────────────────┐
              │                           │                           │
      ┌───────┴─────────┐          ┌──────┴─────────┐          ┌──────┴───────────┐
      │    Producers    │          │    Readers     │          │ Status Writers   │
      ├─────────────────┤          ├────────────────┤          ├──────────────────┤
      │ DHT task        │          │ Control task   │          │ LED module       │
      │ Soil task       │          │ OLED task      │          │ Pump module      │
      │ Water task      │          │                │          │ IR handler       │
      │ Light task      │          │                │          │ RTC task         │
      │ RTC task        │          │                │          │ Sensor tasks     │
      └─────────────────┘          └────────────────┘          └──────────────────┘
```

## State Data Flow

- Sensor tasks update live measurements such as temperature, soil, water level, and ambient light.
- The RTC task updates `time_text` once per second.
- The IR handler updates `ir_command` and increments `ir_activity_count`.
- The LED module updates `led_color_code`.
- The pump module updates `pump_status`.
- The control task reads a snapshot of state to evaluate automation rules.
- The OLED task reads a snapshot of state and redraws only fields that changed.

## Control Modules

- `state/`: mutex-protected shared `garden_state_t`
- `sensors/`: DHT11, soil ADC, water ADC, light sensor, shared ADC helper
- `clock/`: DS1302 RTC read loop
- `input/`: IR receiver using ESP-IDF RMT and NEC decoding
- `control/`: queue-based command handling and automation rule evaluation
- `actuators/`: WS2812B LED panel and water pump driver logic
- `display/`: SSD1306 OLED rendering

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
