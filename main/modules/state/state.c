#include "state.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

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
