#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include <stdbool.h>

// configure the pump control GPIO and reset pump state
void water_pump_init(void);
// directly drive the pump output pin
void water_pump_set_enabled(bool enabled);
// run one timed pump pulse if not active or cooling down
void water_pump_trigger(void);

#endif
