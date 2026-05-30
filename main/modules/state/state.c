#include "state.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static garden_state_t s_state;
static SemaphoreHandle_t s_state_mutex;

static void lock_state(void) {
  if (s_state_mutex == NULL) {
    return;
  }
  xSemaphoreTake(s_state_mutex, portMAX_DELAY);
}

static void unlock_state(void) {
  if (s_state_mutex == NULL) {
    return;
  }
  xSemaphoreGive(s_state_mutex);
}

void garden_state_init(void) {
  s_state_mutex = xSemaphoreCreateMutex();
  lock_state();
  s_state.soil_moisture_percent = 50;
  s_state.air_temperature_c = 24;
  s_state.air_humidity_percent = 60;
  s_state.water_level = true;
  s_state.pump_on = false;
  s_state.light_on = false;
  s_state.auto_mode = true;
  unlock_state();
}

garden_state_t garden_state_get(void) {
  garden_state_t state;

  lock_state();
  state = s_state;
  unlock_state();

  return state;
}

// update state in all sensors
void garden_state_update_sensors(int soil_moisture_percent, int air_temperature_c,
                                 int air_humidity_percent,
                                 bool water_level) {
  lock_state();
  s_state.soil_moisture_percent = soil_moisture_percent;
  s_state.air_temperature_c = air_temperature_c;
  s_state.air_humidity_percent = air_humidity_percent;
  s_state.water_level = water_level;
  unlock_state();
}

// update pump state
void garden_state_set_pump(bool pump_on) {
  lock_state();
  s_state.pump_on = pump_on;
  unlock_state();
}

// update light state
void garden_state_set_light(bool light_on) {
  lock_state();
  s_state.light_on = light_on;
  unlock_state();
}

// update automatic control mode
void garden_state_set_auto_mode(bool auto_mode) {
  lock_state();
  s_state.auto_mode = auto_mode;
  unlock_state();
}
