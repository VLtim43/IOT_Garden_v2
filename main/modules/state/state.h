#ifndef GARDEN_STATE_H
#define GARDEN_STATE_H

#include <stdbool.h>

typedef struct {
  int soil_moisture_percent;
  int air_temperature;
  int air_humidity_percent;
  int light_bool;
  bool water_bool;
  bool pump_bool;
  bool grow_light_bool;
} garden_state_t;
void garden_state_init(void);
garden_state_t garden_state_get(void);
void garden_state_update_sensors(int soil_moisture_percent, int air_temperature,
                                 int air_humidity_percent, int light_bool,
                                 bool water_bool);
void garden_state_set_pump(bool pump_bool);
void garden_state_set_grow_light(bool grow_light_bool);

#endif