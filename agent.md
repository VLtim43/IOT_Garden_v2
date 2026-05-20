# Agent Instructions

## Project Context

This is `IOT_Garden_v2`, an ESP32 IoT Garden firmware project built with ESP-IDF and C. The project targets garden monitoring and automation hardware, including soil humidity, temperature/humidity, water level, light sensing, a buzzer, an OLED screen, IR controls, LEDs, and a water pump.

## Repository Layout

- `CMakeLists.txt` defines the ESP-IDF project.
- `sdkconfig` contains ESP-IDF configuration.
- `main/main.c` is the application entry point.
- `main/config/pins.h` stores pin definitions, IR patterns, and LED configuration.
- `main/modules/` contains feature modules for actuators, sensors, display, input, clock, and tasks.
- `build/` contains generated build artifacts and should not be edited manually.

## Development Guidelines

- Keep firmware changes small, explicit, and hardware-aware.
- Prefer ESP-IDF APIs and FreeRTOS primitives already used in the project.
- Keep pin mappings and hardware constants centralized in `main/config/pins.h`.
- Add or update headers when module functions need to be shared across files.
- Do not modify generated files under `build/`.
- Preserve the existing C style unless a file clearly uses a different local style.

## Build And Verification

- Use `idf.py build` to compile the firmware.
- Use `idf.py flash monitor` only when hardware is connected and the serial port is configured.
- If ESP-IDF environment variables are missing, source the local ESP-IDF environment before building.

## Safety Notes

- Be careful with GPIO changes because incorrect pin assignments can damage connected hardware.
- Treat pump, LED, sensor, and display timing as hardware-sensitive behavior.
- Avoid blocking delays in code paths that should remain responsive; prefer FreeRTOS timing patterns where appropriate.
