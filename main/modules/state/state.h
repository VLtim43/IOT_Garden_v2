#ifndef GARDEN_STATE_H
#define GARDEN_STATE_H

#include <stdbool.h>

typedef struct {
  bool ambient_light_detected;
  int temperature_c;
  int soil_raw;
  int water_level_percent;
} garden_state_t;

void garden_state_init(void);
garden_state_t garden_state_get(void);
void garden_state_set_ambient_light(bool detected);
void garden_state_set_temperature(int temperature_c);
void garden_state_set_soil_raw(int raw);
void garden_state_set_water_level(int water_level_percent);

#endif
