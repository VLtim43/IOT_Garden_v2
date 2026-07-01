#include "default_rules.h"

#include "rgb_led.h"

enum {
  DEFAULT_RULE_TIME_6PM_MINUTES = 18 * 60,
  DEFAULT_RULE_TIME_6AM_MINUTES = 6 * 60,
  DEFAULT_RULE_LED_COLOR_PURPLE = 0,
  DEFAULT_RULE_LED_COLOR_RED = 1,
};

/*
Example rule template with field meanings:

{
    .enabled = true, // Master on/off for this rule.
    .edge_triggered = true, // Fire once when conditions become true; false repeats.
    .condition_mode = AUTOMATION_CONDITIONS_ALL, // ALL = every enabled condition must match. ANY = at least one must match.
    .conditions =
        {
            {
                .enabled = true, // Ignore this condition when false.
                .field = AUTOMATION_FIELD_AMBIENT_LIGHT, // 1 = day/light detected, 0 = night/no light.
                .op = AUTOMATION_OP_EQ, // Comparison operator: LT, LTE, EQ, GTE, GT, NEQ.
                .value = 0, // Value compared against selected field.
            },
            {
                .enabled = true,
                .field = AUTOMATION_FIELD_TIME_MINUTES, // Minutes since 00:00. Example: 18:00 = 1080.
                .op = AUTOMATION_OP_GTE,
                .value = 18 * 60,
            },
            {
                .enabled = false,
                .field = AUTOMATION_FIELD_TEMPERATURE_C, // Integer temperature in Celsius from DHT sensor.
                .op = AUTOMATION_OP_GT,
                .value = 30,
            },
            {
                .enabled = false,
                .field = AUTOMATION_FIELD_SOIL_RAW, // Raw soil ADC reading. Lower/wet vs higher/dry depends on sensor calibration.
                .op = AUTOMATION_OP_LT,
                .value = 1500,
            },
            {
                .enabled = false,
                .field = AUTOMATION_FIELD_WATER_LEVEL_PERCENT, // Reservoir level as percent from 0 to 100.
                .op = AUTOMATION_OP_LTE,
                .value = 20,
            },
        },
    .action =
        {
            .type = AUTOMATION_ACTION_LED_SET_COLOR, // Supported actions: NONE, PUMP_PULSE, LED_CYCLE_LEFT, LED_CYCLE_RIGHT, LED_TOGGLE, LED_SET_COLOR, RGB_LED_SET_COLOR.
            .arg0 = 1, // Meaning depends on action type. For LED_SET_COLOR: palette index 0=PURPL, 1=RED, 2=BLUE.
            .arg1 = 0, // Spare argument for future action-specific data.
        },
    .min_interval_ms = 0, // Minimum delay before this rule can fire again. 0 = no extra delay.
    .priority = 10, // Higher priority actions are queued first when multiple rules match.
}
*/

// Central rule list for built-in automation defaults.
static const automation_rule_t DEFAULT_RULES[] = {
    {
        // Demo rule: cover the light sensor and the RGB LED turns red.
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
        // Demo rule: uncover the light sensor and the RGB LED turns green.
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
            },
        .action =
            {
                .type = AUTOMATION_ACTION_RGB_LED_SET_COLOR,
                .arg0 = RGB_LED_COLOR_RED,
            },
        .priority = 20,
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
            },
        .action =
            {
                .type = AUTOMATION_ACTION_RGB_LED_SET_COLOR,
                .arg0 = RGB_LED_COLOR_GREEN,
            },
        .priority = 20,
    },
};

const automation_rule_t* default_rules_get(size_t* rule_count) {
  if (rule_count != NULL) {
    *rule_count = sizeof(DEFAULT_RULES) / sizeof(DEFAULT_RULES[0]);
  }

  return DEFAULT_RULES;
}
