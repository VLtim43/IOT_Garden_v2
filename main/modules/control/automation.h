#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "state.h"

#define AUTOMATION_MAX_CONDITIONS 8
#define AUTOMATION_MAX_RULES 8

typedef enum {
  AUTOMATION_FIELD_TEMPERATURE_C,
  AUTOMATION_FIELD_SOIL_RAW,
  AUTOMATION_FIELD_WATER_LEVEL_PERCENT,
  AUTOMATION_FIELD_AMBIENT_LIGHT,
  AUTOMATION_FIELD_TIME_MINUTES,
} automation_field_t;

typedef enum {
  AUTOMATION_OP_LT,
  AUTOMATION_OP_LTE,
  AUTOMATION_OP_EQ,
  AUTOMATION_OP_GTE,
  AUTOMATION_OP_GT,
  AUTOMATION_OP_NEQ,
} automation_operator_t;

typedef struct {
  bool enabled;
  automation_field_t field;
  automation_operator_t op;
  int value;
} automation_condition_t;

typedef enum {
  AUTOMATION_CONDITIONS_ALL,
  AUTOMATION_CONDITIONS_ANY,
} automation_condition_mode_t;

typedef enum {
  AUTOMATION_ACTION_NONE,
  AUTOMATION_ACTION_PUMP_PULSE,
  AUTOMATION_ACTION_LED_CYCLE_LEFT,
  AUTOMATION_ACTION_LED_CYCLE_RIGHT,
  AUTOMATION_ACTION_LED_TOGGLE,
} automation_action_type_t;

typedef struct {
  automation_action_type_t type;
  int arg0;
  int arg1;
} automation_action_t;

typedef struct {
  bool enabled;
  bool edge_triggered;
  automation_condition_mode_t condition_mode;
  automation_condition_t conditions[AUTOMATION_MAX_CONDITIONS];
  automation_action_t action;
  uint32_t min_interval_ms;
  uint8_t priority;
} automation_rule_t;

typedef struct {
  size_t rule_index;
  automation_action_t action;
} automation_action_request_t;

void automation_init(void);
bool automation_set_rule(size_t index, const automation_rule_t* rule);
const automation_rule_t* automation_get_rules(size_t* rule_count);
size_t automation_evaluate(const garden_state_t* state,
                           automation_action_request_t* requests,
                           size_t max_requests);

#endif
