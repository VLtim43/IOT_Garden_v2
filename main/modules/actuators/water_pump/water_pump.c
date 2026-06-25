#include "water_pump.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "WATER_PUMP";

enum {
  WATER_PUMP_ON_MS = 3000,
  WATER_PUMP_COOLDOWN_MS = 2000,
};

static bool s_pump_active;
static TickType_t s_cooldown_until_tick;

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
  s_pump_active = false;
  s_cooldown_until_tick = 0;
  garden_state_set_pump_status("OFF");

  ESP_LOGI(TAG, "ready on GPIO%d", WATER_PUMP_GPIO);
}

void water_pump_set_enabled(bool enabled) {
  ESP_ERROR_CHECK(gpio_set_level(WATER_PUMP_GPIO, enabled ? 1 : 0));
}

static void water_pump_pulse_task(void* arg) {
  (void)arg;

  vTaskDelay(pdMS_TO_TICKS(WATER_PUMP_ON_MS));
  water_pump_set_enabled(false);
  s_pump_active = false;
  s_cooldown_until_tick = xTaskGetTickCount() + pdMS_TO_TICKS(WATER_PUMP_COOLDOWN_MS);
  garden_state_set_pump_status("COOLDWN");
  vTaskDelay(pdMS_TO_TICKS(WATER_PUMP_COOLDOWN_MS));
  if (!s_pump_active) {
    garden_state_set_pump_status("OFF");
  }
  vTaskDelete(NULL);
}

void water_pump_trigger(void) {
  TickType_t now = xTaskGetTickCount();

  if (s_pump_active) {
    return;
  }

  if (s_cooldown_until_tick != 0 && now < s_cooldown_until_tick) {
    garden_state_set_pump_status("COOLDWN");
    return;
  }

  s_cooldown_until_tick = 0;
  s_pump_active = true;
  water_pump_set_enabled(true);
  garden_state_set_pump_status("ON");
  ESP_LOGI(TAG, "pump pulse started");
  xTaskCreate(water_pump_pulse_task, "water_pump_pulse", 2048, NULL, 5, NULL);
}
