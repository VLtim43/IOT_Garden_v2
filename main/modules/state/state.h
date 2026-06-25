#ifndef GARDEN_STATE_H
#define GARDEN_STATE_H

#include <stdbool.h>

#define GARDEN_LED_COLOR_CODE_MAX_LEN 8
#define GARDEN_TIME_TEXT_MAX_LEN 6
#define GARDEN_IR_COMMAND_MAX_LEN 8

typedef struct {
  bool ambient_light_detected;
  int temperature_c;
  int soil_raw;
  int water_level_percent;
  unsigned int ir_activity_count;
  char time_text[GARDEN_TIME_TEXT_MAX_LEN];
  char led_color_code[GARDEN_LED_COLOR_CODE_MAX_LEN];
  char ir_command[GARDEN_IR_COMMAND_MAX_LEN];
} garden_state_t;

void garden_state_init(void);
garden_state_t garden_state_get(void);
void garden_state_set_ambient_light(bool detected);
void garden_state_set_temperature(int temperature_c);
void garden_state_set_soil_raw(int raw);
void garden_state_set_water_level(int water_level_percent);
void garden_state_set_time_text(const char* time_text);
void garden_state_set_led_color_code(const char* color_code);
void garden_state_set_ir_command(const char* command);
void garden_state_mark_ir_activity(void);

#endif
