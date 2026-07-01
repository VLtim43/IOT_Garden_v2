#include "control.h"

#include "automation.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "ir_codes.h"
#include "state.h"
#include "water_pump.h"
#include "w2812b.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static const char* TAG = "CONTROL";

enum {
  CONTROL_ACTION_QUEUE_LEN = 8,
  CONTROL_LOOP_INTERVAL_MS = 250,
};

typedef enum {
  CONTROL_REQUEST_IR_COMMAND,
  CONTROL_REQUEST_AUTOMATION_ACTION,
} control_request_type_t;

typedef struct {
  control_request_type_t type;
  control_source_t source;
  union {
    struct {
      uint32_t raw_code;
      char command_name[GARDEN_IR_COMMAND_MAX_LEN];
    } ir;
    struct {
      size_t rule_index;
      automation_action_t action;
    } automation;
  } payload;
} control_request_t;

// shared queue for IR and automation actions
static QueueHandle_t s_control_queue;

static void control_apply_automation_action(const automation_action_t* action) {
  if (action == NULL) {
    return;
  }

  switch (action->type) {
    case AUTOMATION_ACTION_PUMP_PULSE:
      water_pump_trigger();
      return;
    case AUTOMATION_ACTION_LED_CYCLE_LEFT:
      w2812b_cycle_left();
      return;
    case AUTOMATION_ACTION_LED_CYCLE_RIGHT:
      w2812b_cycle_right();
      return;
    case AUTOMATION_ACTION_LED_TOGGLE:
      w2812b_toggle_enabled();
      return;
    case AUTOMATION_ACTION_NONE:
    default:
      return;
  }
}

static void control_handle_ir_request(const control_request_t* request) {
  uint32_t raw_code = request->payload.ir.raw_code;

  garden_state_set_ir_command(request->payload.ir.command_name);
  garden_state_mark_ir_activity();

  switch (raw_code) {
    case IR_CODE_LEFT:
      w2812b_cycle_left();
      return;
    case IR_CODE_RIGHT:
      w2812b_cycle_right();
      return;
    case IR_CODE_OK:
      w2812b_toggle_enabled();
      return;
    case IR_CODE_STAR:
      water_pump_trigger();
      return;
    default:
      return;
  }
}

static void control_handle_request(const control_request_t* request) {
  if (request == NULL) {
    return;
  }

  switch (request->type) {
    case CONTROL_REQUEST_IR_COMMAND:
      control_handle_ir_request(request);
      return;
    case CONTROL_REQUEST_AUTOMATION_ACTION:
      control_apply_automation_action(&request->payload.automation.action);
      return;
    default:
      return;
  }
}

static void control_queue_automation_actions(const garden_state_t* state) {
  automation_action_request_t actions[AUTOMATION_MAX_RULES];
  size_t action_count = automation_evaluate(state, actions, AUTOMATION_MAX_RULES);

  for (size_t i = 0; i < action_count; i++) {
    control_request_t request = {
        .type = CONTROL_REQUEST_AUTOMATION_ACTION,
        .source = CONTROL_SOURCE_AUTOMATION,
        .payload.automation = {
            .rule_index = actions[i].rule_index,
            .action = actions[i].action,
        },
    };

    if (xQueueSend(s_control_queue, &request, 0) != pdTRUE) {
      ESP_LOGW(TAG, "automation action dropped for rule %u",
               (unsigned)actions[i].rule_index);
    }
  }
}

static void control_task(void* arg) {
  (void)arg;

  while (true) {
    control_request_t request;
    if (xQueueReceive(s_control_queue, &request,
                      pdMS_TO_TICKS(CONTROL_LOOP_INTERVAL_MS)) == pdTRUE) {
      control_handle_request(&request);
      continue;
    }

    // no queued work, poll automation rules
    garden_state_t state = garden_state_get();
    control_queue_automation_actions(&state);
  }
}

void control_start(void) {
  if (s_control_queue != NULL) {
    return;
  }

  automation_init();
  s_control_queue = xQueueCreate(CONTROL_ACTION_QUEUE_LEN, sizeof(control_request_t));
  if (s_control_queue == NULL) {
    ESP_LOGE(TAG, "failed to create control queue");
    return;
  }

  xTaskCreate(control_task, "control", 4096, NULL, 5, NULL);
}

bool control_submit_ir_command(uint32_t raw_code, const char* command_name) {
  control_request_t request = {
      .type = CONTROL_REQUEST_IR_COMMAND,
      .source = CONTROL_SOURCE_IR,
      .payload.ir = {
          .raw_code = raw_code,
      },
  };

  if (s_control_queue == NULL || command_name == NULL) {
    return false;
  }

  strncpy(request.payload.ir.command_name, command_name,
          sizeof(request.payload.ir.command_name));
  request.payload.ir.command_name[sizeof(request.payload.ir.command_name) - 1] =
      '\0';

  if (xQueueSend(s_control_queue, &request, 0) == pdTRUE) {
    return true;
  }

  ESP_LOGW(TAG, "IR command queue full for %s", command_name);
  return false;
}
