#ifndef GARDEN_STATE_H
#define GARDEN_STATE_H

#include <stdbool.h>

#define GARDEN_LED_COLOR_CODE_MAX_LEN 8

typedef struct {
  bool ambient_light_detected;
  int temperature_c;
  int soil_raw;
  int water_level_percent;
  char led_color_code[GARDEN_LED_COLOR_CODE_MAX_LEN];
} garden_state_t;

void garden_state_init(void);
garden_state_t garden_state_get(void);
void garden_state_set_ambient_light(bool detected);
void garden_state_set_temperature(int temperature_c);
void garden_state_set_soil_raw(int raw);
void garden_state_set_water_level(int water_level_percent);
void garden_state_set_led_color_code(const char* color_code);

#endif
