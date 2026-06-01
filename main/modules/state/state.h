#ifndef GARDEN_STATE_H
#define GARDEN_STATE_H

#include <stdbool.h>

typedef struct {
  bool ambient_light_detected;
  int temperature_c;
} garden_state_t;

void garden_state_init(void);
garden_state_t garden_state_get(void);
void garden_state_set_ambient_light(bool detected);
void garden_state_set_temperature(int temperature_c);

#endif
