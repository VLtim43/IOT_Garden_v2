#include "default_rules.h"

enum {
  DEFAULT_RULE_TIME_6PM_MINUTES = 18 * 60,
  DEFAULT_RULE_TIME_6AM_MINUTES = 6 * 60,
  DEFAULT_RULE_LED_COLOR_PURPLE = 0,
  DEFAULT_RULE_LED_COLOR_RED = 1,
};

// Central rule list for built-in automation defaults.
static const automation_rule_t DEFAULT_RULES[] = {
    {
        .enabled = true,
        .edge_triggered = true,
        .condition_mode = AUTOMATION_CONDITIONS_ALL,
        .conditions =
            {
                {
                    .enabled = true,
                    .field = AUTOMATION_FIELD_AMBIENT_LIGHT,
                    .op = AUTOMATION_OP_EQ,
                    .value = 0,
                },
                {
                    .enabled = true,
                    .field = AUTOMATION_FIELD_TIME_MINUTES,
                    .op = AUTOMATION_OP_GTE,
                    .value = DEFAULT_RULE_TIME_6PM_MINUTES,
                },
            },
        .action =
            {
                .type = AUTOMATION_ACTION_LED_SET_COLOR,
                .arg0 = DEFAULT_RULE_LED_COLOR_RED,
            },
        .priority = 10,
    },
    {
        .enabled = true,
        .edge_triggered = true,
        .condition_mode = AUTOMATION_CONDITIONS_ALL,
        .conditions =
            {
                {
                    .enabled = true,
                    .field = AUTOMATION_FIELD_AMBIENT_LIGHT,
                    .op = AUTOMATION_OP_EQ,
                    .value = 1,
                },
                {
                    .enabled = true,
                    .field = AUTOMATION_FIELD_TIME_MINUTES,
                    .op = AUTOMATION_OP_GTE,
                    .value = DEFAULT_RULE_TIME_6AM_MINUTES,
                },
            },
        .action =
            {
                .type = AUTOMATION_ACTION_LED_SET_COLOR,
                .arg0 = DEFAULT_RULE_LED_COLOR_PURPLE,
            },
        .priority = 10,
    },
};

const automation_rule_t* default_rules_get(size_t* rule_count) {
  if (rule_count != NULL) {
    *rule_count = sizeof(DEFAULT_RULES) / sizeof(DEFAULT_RULES[0]);
  }

  return DEFAULT_RULES;
}
