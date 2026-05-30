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
  s_state.air_temperature = 24;
  s_state.air_humidity_percent = 60;
  s_state.light_bool = 50;
  s_state.water_bool = true;
  s_state.pump_bool = false;
  s_state.grow_light_bool = false;
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
void garden_state_update_sensors(int soil_moisture_percent, int air_temperature,
                                 int air_humidity_percent, int light_bool,
                                 bool water_bool) {
  lock_state();
  s_state.soil_moisture_percent = soil_moisture_percent;
  s_state.air_temperature = air_temperature;
  s_state.air_humidity_percent = air_humidity_percent;
  s_state.light_bool = light_bool;
  s_state.water_bool = water_bool;
  unlock_state();
}

// update pump state
void garden_state_set_pump(bool pump_bool) {
  lock_state();
  s_state.pump_bool = pump_bool;
  unlock_state();
}

// upate grow lights state
void garden_state_set_grow_light(bool grow_light_bool) {
  lock_state();
  s_state.grow_light_bool = grow_light_bool;
  unlock_state();
}
