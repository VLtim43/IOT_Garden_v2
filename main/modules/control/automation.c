#include "automation.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  bool matched_last_tick;
  TickType_t last_fire_tick;
} automation_rule_runtime_t;

// configured rules and per-rule runtime state
static automation_rule_t s_rules[AUTOMATION_MAX_RULES];
static automation_rule_runtime_t s_rule_runtime[AUTOMATION_MAX_RULES];

static bool automation_parse_time_minutes(const char* time_text,
                                          int* minutes_out) {
  if (time_text == NULL || minutes_out == NULL) {
    return false;
  }

  if (time_text[0] < '0' || time_text[0] > '9' || time_text[1] < '0' ||
      time_text[1] > '9' || time_text[2] != ':' || time_text[3] < '0' ||
      time_text[3] > '9' || time_text[4] < '0' || time_text[4] > '9') {
    return false;
  }

  int hour = ((time_text[0] - '0') * 10) + (time_text[1] - '0');
  int minute = ((time_text[3] - '0') * 10) + (time_text[4] - '0');
  if (hour > 23 || minute > 59) {
    return false;
  }

  *minutes_out = (hour * 60) + minute;
  return true;
}

static bool automation_get_field_value(const garden_state_t* state,
                                       automation_field_t field,
                                       int* value_out) {
  if (state == NULL || value_out == NULL) {
    return false;
  }

  switch (field) {
    case AUTOMATION_FIELD_TEMPERATURE_C:
      *value_out = state->temperature_c;
      return true;
    case AUTOMATION_FIELD_SOIL_RAW:
      *value_out = state->soil_raw;
      return true;
    case AUTOMATION_FIELD_WATER_LEVEL_PERCENT:
      *value_out = state->water_level_percent;
      return true;
    case AUTOMATION_FIELD_AMBIENT_LIGHT:
      *value_out = state->ambient_light_detected ? 1 : 0;
      return true;
    case AUTOMATION_FIELD_TIME_MINUTES:
      return automation_parse_time_minutes(state->time_text, value_out);
    default:
      return false;
  }
}

static bool automation_compare(int left, automation_operator_t op, int right) {
  switch (op) {
    case AUTOMATION_OP_LT:
      return left < right;
    case AUTOMATION_OP_LTE:
      return left <= right;
    case AUTOMATION_OP_EQ:
      return left == right;
    case AUTOMATION_OP_GTE:
      return left >= right;
    case AUTOMATION_OP_GT:
      return left > right;
    case AUTOMATION_OP_NEQ:
      return left != right;
    default:
      return false;
  }
}

static bool automation_condition_matches(const garden_state_t* state,
                                         const automation_condition_t* condition) {
  int field_value = 0;

  if (condition == NULL || !condition->enabled) {
    return false;
  }

  if (!automation_get_field_value(state, condition->field, &field_value)) {
    return false;
  }

  return automation_compare(field_value, condition->op, condition->value);
}

static bool automation_rule_matches(const garden_state_t* state,
                                    const automation_rule_t* rule) {
  bool saw_condition = false;
  bool matched_any = false;

  if (rule == NULL || !rule->enabled) {
    return false;
  }

  for (size_t i = 0; i < AUTOMATION_MAX_CONDITIONS; i++) {
    const automation_condition_t* condition = &rule->conditions[i];
    if (!condition->enabled) {
      continue;
    }

    saw_condition = true;
    bool matched = automation_condition_matches(state, condition);
    if (rule->condition_mode == AUTOMATION_CONDITIONS_ALL && !matched) {
      return false;
    }

    if (matched) {
      matched_any = true;
    }
  }

  if (!saw_condition) {
    return false;
  }

  if (rule->condition_mode == AUTOMATION_CONDITIONS_ALL) {
    return true;
  }

  return matched_any;
}

void automation_init(void) {
  memset(s_rules, 0, sizeof(s_rules));
  memset(s_rule_runtime, 0, sizeof(s_rule_runtime));
}

bool automation_set_rule(size_t index, const automation_rule_t* rule) {
  if (rule == NULL || index >= AUTOMATION_MAX_RULES) {
    return false;
  }

  s_rules[index] = *rule;
  s_rule_runtime[index].matched_last_tick = false;
  s_rule_runtime[index].last_fire_tick = 0;
  return true;
}

const automation_rule_t* automation_get_rules(size_t* rule_count) {
  if (rule_count != NULL) {
    *rule_count = AUTOMATION_MAX_RULES;
  }

  return s_rules;
}

static void automation_insert_request(automation_action_request_t* requests,
                                      size_t* request_count,
                                      size_t max_requests,
                                      const automation_action_request_t* request) {
  if (requests == NULL || request_count == NULL || request == NULL ||
      *request_count >= max_requests) {
    return;
  }

  size_t insert_index = *request_count;
  const automation_rule_t* new_rule = &s_rules[request->rule_index];
  // keep higher priority actions first
  for (size_t i = 0; i < *request_count; i++) {
    const automation_rule_t* existing_rule = &s_rules[requests[i].rule_index];
    if (new_rule->priority > existing_rule->priority) {
      insert_index = i;
      break;
    }
  }

  for (size_t i = *request_count; i > insert_index; i--) {
    requests[i] = requests[i - 1];
  }

  requests[insert_index] = *request;
  (*request_count)++;
}

size_t automation_evaluate(const garden_state_t* state,
                           automation_action_request_t* requests,
                           size_t max_requests) {
  size_t request_count = 0;
  TickType_t now = xTaskGetTickCount();

  if (state == NULL || requests == NULL || max_requests == 0) {
    return 0;
  }

  for (size_t i = 0; i < AUTOMATION_MAX_RULES; i++) {
    automation_rule_t* rule = &s_rules[i];
    automation_rule_runtime_t* runtime = &s_rule_runtime[i];
    bool matched = automation_rule_matches(state, rule);
    bool should_fire = matched;

    if (!matched) {
      runtime->matched_last_tick = false;
      continue;
    }

    if (rule->edge_triggered && runtime->matched_last_tick) {
      should_fire = false;
    }

    // enforce minimum gap between repeated actions
    if (should_fire && rule->min_interval_ms > 0 && runtime->last_fire_tick != 0 &&
        (now - runtime->last_fire_tick) < pdMS_TO_TICKS(rule->min_interval_ms)) {
      should_fire = false;
    }

    runtime->matched_last_tick = true;

    if (!should_fire || request_count >= max_requests ||
        rule->action.type == AUTOMATION_ACTION_NONE) {
      continue;
    }

    automation_action_request_t request = {
        .rule_index = i,
        .action = rule->action,
    };

    automation_insert_request(requests, &request_count, max_requests, &request);
    runtime->last_fire_tick = now;
  }

  return request_count;
}
