#ifndef CONTROL_H
#define CONTROL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  CONTROL_SOURCE_IR,
  CONTROL_SOURCE_AUTOMATION,
} control_source_t;

void control_start(void);
bool control_submit_ir_command(uint32_t raw_code, const char* command_name);

#endif
