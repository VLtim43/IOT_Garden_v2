# Agent Instructions

## Project Context

This is `IOT_Garden_v2`, a college ESP32 IoT Garden firmware project built with ESP-IDF and C. The project simulates monitoring and caring for a small plant using sensors, scheduled watering, scheduled plant lighting, local status display, and manual IR control.

The ESP32 runs at 3.3 V logic. Some peripherals need a separate 5 V supply, especially the WS281x LED panel and the small water pump. Code and documentation should keep this mixed-voltage setup clear: the ESP32 controls these devices, but it must not power high-current 5 V loads directly from GPIO pins.

## Hardware Scope

- ESP32 development board using ESP-IDF.
- Soil humidity sensor for plant moisture monitoring.
- Temperature/humidity sensor for environment readings.
- Water level sensor for reservoir state.
- Light sensor for ambient light information.
- OLED screen for local status and menu/state display.
- IR receiver for manual control commands.
- Clock/time source for schedules and periodic actions.
- WS281x RGB LED panel/strip for scheduled colored plant lighting.
- Small water pump for scheduled or manual watering.
- Optional buzzer if needed for alerts or feedback.

## Architecture Expectations

- Use FreeRTOS tasks for independent responsibilities instead of putting all logic in `app_main`.
- Keep sensor reading, display refresh, IR input handling, scheduling, LED control, and pump control as separate responsibilities.
- Use queues, task notifications, event groups, or shared state with synchronization when tasks need to communicate.
- Avoid long blocking delays in responsive paths; prefer `vTaskDelay`, timers, or scheduled task loops.
- Keep college-project simulation constraints in mind: implementation may use simplified behavior, but it should still be structured like real firmware.

## Repository Layout

- `CMakeLists.txt` defines the ESP-IDF project.
- `sdkconfig` contains ESP-IDF configuration.
- `main/main.c` is the application entry point and should mainly initialize modules and create tasks.
- `main/CMakeLists.txt` registers source files and include paths for the ESP-IDF component.
- `main/config/pins.h` stores pin definitions, IR patterns, LED configuration, timing constants, and hardware limits.
- `main/modules/actuators/` contains output device modules such as the water pump and WS281x LEDs.
- `main/modules/sensors/` contains sensor modules for soil, DHT, light, and water level readings.
- `main/modules/display/` contains OLED display code.
- `main/modules/input/` contains IR receiver/manual control code.
- `main/modules/clock/` contains clock or schedule-time logic.
- `main/modules/tasks/` contains FreeRTOS task creation and task loop coordination.
- `build/` contains generated build artifacts and should not be edited manually.

## Development Guidelines

- Keep firmware changes small, explicit, and hardware-aware.
- Prefer ESP-IDF APIs and FreeRTOS primitives.
- Keep pin mappings and hardware constants centralized in `main/config/pins.h`.
- Add or update headers when module functions need to be shared across files.
- Keep module boundaries clear: drivers should expose simple functions, while task files should coordinate behavior.
- Do not modify generated files under `build/`.
- Preserve the existing C style unless a file clearly uses a different local style.
- Prefer readable, simple code over production-level abstraction because this is a college project.

## Electrical Notes

- ESP32 GPIO pins are 3.3 V logic and must not be connected directly to 5 V signal outputs.
- The WS281x LED panel and pump should use the separate 5 V USB adapter supply.
- The ESP32 and external 5 V supply should share a common ground when the ESP32 controls 5 V devices.
- The pump must be switched with a proper driver stage such as a transistor, MOSFET, relay module, or motor driver, not directly from an ESP32 GPIO.
- High-current LED and pump wiring should be treated separately from low-power sensor wiring.
- If a 5 V peripheral requires a 5 V data signal, document the need for level shifting instead of assuming direct ESP32 drive is always safe.

## Build And Verification

- Use `idf.py build` to compile the firmware.
- Use `idf.py flash monitor` only when hardware is connected and the serial port is configured.
- If ESP-IDF environment variables are missing, source the local ESP-IDF environment before building.
- When hardware is unavailable, keep behavior testable through logs, simulated readings, or clearly isolated module functions.

## Safety Notes

- Be careful with GPIO changes because incorrect pin assignments can damage connected hardware.
- Treat pump, LED, sensor, and display timing as hardware-sensitive behavior.
- Watering logic should include a maximum pump-on duration to avoid leaving the pump active indefinitely.
- Water level should be checked before automatic watering when that module is available.
- Manual IR controls should not bypass basic pump safety limits.
