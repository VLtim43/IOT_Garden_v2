#include "state.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <string.h>

static garden_state_t s_state;
static SemaphoreHandle_t s_state_mutex;

// helper to lock state
static void lock_state(void) {
  if (s_state_mutex == NULL) {
    return;
  }
  xSemaphoreTake(s_state_mutex, portMAX_DELAY);
}

// helper to unlock state
static void unlock_state(void) {
  if (s_state_mutex == NULL) {
    return;
  }
  xSemaphoreGive(s_state_mutex);
}

// initialize state
void garden_state_init(void) {
  s_state_mutex = xSemaphoreCreateMutex();
  lock_state();
  s_state.ambient_light_detected = true;
  s_state.temperature_c = 0;
  s_state.soil_raw = 0;
  s_state.water_level_percent = 0;
  s_state.ir_activity_count = 0;
  strncpy(s_state.time_text, "--:--", sizeof(s_state.time_text));
  s_state.time_text[sizeof(s_state.time_text) - 1] = '\0';
  strncpy(s_state.led_color_code, "OFF", sizeof(s_state.led_color_code));
  s_state.led_color_code[sizeof(s_state.led_color_code) - 1] = '\0';
  strncpy(s_state.ir_command, "NONE", sizeof(s_state.ir_command));
  s_state.ir_command[sizeof(s_state.ir_command) - 1] = '\0';
  unlock_state();
}

garden_state_t garden_state_get(void) {
  garden_state_t state;

  lock_state();
  state = s_state;
  unlock_state();

  return state;
}

void garden_state_set_ambient_light(bool detected) {
  lock_state();
  s_state.ambient_light_detected = detected;
  unlock_state();
}

void garden_state_set_temperature(int temperature_c) {
  lock_state();
  s_state.temperature_c = temperature_c;
  unlock_state();
}

void garden_state_set_soil_raw(int raw) {
  lock_state();
  s_state.soil_raw = raw;
  unlock_state();
}

void garden_state_set_water_level(int water_level_percent) {
  lock_state();
  s_state.water_level_percent = water_level_percent;
  unlock_state();
}

void garden_state_set_time_text(const char* time_text) {
  lock_state();
  strncpy(s_state.time_text, time_text, sizeof(s_state.time_text));
  s_state.time_text[sizeof(s_state.time_text) - 1] = '\0';
  unlock_state();
}

void garden_state_set_led_color_code(const char* color_code) {
  lock_state();
  strncpy(s_state.led_color_code, color_code, sizeof(s_state.led_color_code));
  s_state.led_color_code[sizeof(s_state.led_color_code) - 1] = '\0';
  unlock_state();
}

void garden_state_set_ir_command(const char* command) {
  lock_state();
  strncpy(s_state.ir_command, command, sizeof(s_state.ir_command));
  s_state.ir_command[sizeof(s_state.ir_command) - 1] = '\0';
  unlock_state();
}

void garden_state_mark_ir_activity(void) {
  lock_state();
  s_state.ir_activity_count++;
  unlock_state();
}
