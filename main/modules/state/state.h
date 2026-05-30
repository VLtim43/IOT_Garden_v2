#ifndef GARDEN_STATE_H
#define GARDEN_STATE_H

#include <stdbool.h>

typedef struct {
  int soil_moisture_percent;
  int air_temperature_c;
  int air_humidity_percent;
  bool water_level;
  bool pump_on;
  bool light_on;
  bool auto_mode;
} garden_state_t;

void garden_state_init(void);
garden_state_t garden_state_get(void);
void garden_state_update_sensors(int soil_moisture_percent, int air_temperature_c,
                                 int air_humidity_percent,
                                 bool water_level);
void garden_state_set_pump(bool pump_on);
void garden_state_set_light(bool light_on);
void garden_state_set_auto_mode(bool auto_mode);

#endif
