#include "water_pump.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "WATER_PUMP";

enum {
  WATER_PUMP_ON_MS = 1500,
  WATER_PUMP_COOLDOWN_MS = 2000,
};

static bool s_pump_active;
static TickType_t s_cooldown_until_tick;
static portMUX_TYPE s_pump_lock = portMUX_INITIALIZER_UNLOCKED;

void water_pump_init(void) {
  gpio_config_t config = {
      .pin_bit_mask = 1ULL << WATER_PUMP_GPIO,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&config));
  water_pump_set_enabled(false);
  taskENTER_CRITICAL(&s_pump_lock);
  s_pump_active = false;
  s_cooldown_until_tick = 0;
  taskEXIT_CRITICAL(&s_pump_lock);
  garden_state_set_pump_status("OFF");

  ESP_LOGI(TAG, "ready on GPIO%d", WATER_PUMP_GPIO);
}

void water_pump_set_enabled(bool enabled) {
  ESP_ERROR_CHECK(gpio_set_level(WATER_PUMP_GPIO, enabled ? 1 : 0));
}

static void water_pump_pulse_task(void* arg) {
  (void)arg;

  // Keep pulse timing off the control task so queue handling stays responsive.
  vTaskDelay(pdMS_TO_TICKS(WATER_PUMP_ON_MS));
  water_pump_set_enabled(false);
  taskENTER_CRITICAL(&s_pump_lock);
  s_pump_active = false;
  s_cooldown_until_tick = xTaskGetTickCount() + pdMS_TO_TICKS(WATER_PUMP_COOLDOWN_MS);
  taskEXIT_CRITICAL(&s_pump_lock);
  garden_state_set_pump_status("COOLDWN");
  vTaskDelay(pdMS_TO_TICKS(WATER_PUMP_COOLDOWN_MS));
  taskENTER_CRITICAL(&s_pump_lock);
  bool pump_active = s_pump_active;
  taskEXIT_CRITICAL(&s_pump_lock);
  if (!pump_active) {
    garden_state_set_pump_status("OFF");
  }
  vTaskDelete(NULL);
}

void water_pump_trigger(void) {
  TickType_t now = xTaskGetTickCount();
  bool cooling_down = false;

  // Reject overlapping pulses and remember cooldown in one critical section.
  taskENTER_CRITICAL(&s_pump_lock);

  if (s_pump_active) {
    taskEXIT_CRITICAL(&s_pump_lock);
    return;
  }

  if (s_cooldown_until_tick != 0 && now < s_cooldown_until_tick) {
    cooling_down = true;
  } else {
    s_cooldown_until_tick = 0;
    s_pump_active = true;
  }

  taskEXIT_CRITICAL(&s_pump_lock);

  if (cooling_down) {
    garden_state_set_pump_status("COOLDWN");
    return;
  }

  BaseType_t task_created = xTaskCreate(water_pump_pulse_task, "water_pump_pulse",
                                        2048, NULL, 5, NULL);
  if (task_created != pdPASS) {
    taskENTER_CRITICAL(&s_pump_lock);
    s_pump_active = false;
    taskEXIT_CRITICAL(&s_pump_lock);
    garden_state_set_pump_status("OFF");
    ESP_LOGE(TAG, "failed to create pump pulse task");
    return;
  }

  water_pump_set_enabled(true);
  garden_state_set_pump_status("ON");
  ESP_LOGI(TAG, "pump pulse started");
}
