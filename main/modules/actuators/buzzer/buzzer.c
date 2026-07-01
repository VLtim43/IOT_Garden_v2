#include "buzzer.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"

#include <stdbool.h>

static const char* TAG = "BUZZER";

enum {
  BUZZER_BEEP_ON_MS = 120,
  BUZZER_BEEP_OFF_MS = 120,
  BUZZER_BEEP_COUNT = 3,
};

static bool s_buzzer_active;

static void buzzer_set_enabled(bool enabled) {
  ESP_ERROR_CHECK(gpio_set_level(BUZZER_GPIO, enabled ? 1 : 0));
}

static void buzzer_triple_task(void* arg) {
  (void)arg;

  for (int i = 0; i < BUZZER_BEEP_COUNT; i++) {
    buzzer_set_enabled(true);
    vTaskDelay(pdMS_TO_TICKS(BUZZER_BEEP_ON_MS));
    buzzer_set_enabled(false);
    if (i + 1 < BUZZER_BEEP_COUNT) {
      vTaskDelay(pdMS_TO_TICKS(BUZZER_BEEP_OFF_MS));
    }
  }

  s_buzzer_active = false;
  vTaskDelete(NULL);
}

void buzzer_init(void) {
  gpio_config_t config = {
      .pin_bit_mask = 1ULL << BUZZER_GPIO,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  ESP_ERROR_CHECK(gpio_config(&config));
  buzzer_set_enabled(false);
  s_buzzer_active = false;
  ESP_LOGI(TAG, "ready on GPIO%d", BUZZER_GPIO);
}

void buzzer_buzz_triple(void) {
  if (s_buzzer_active) {
    return;
  }

  s_buzzer_active = true;
  if (xTaskCreate(buzzer_triple_task, "buzzer_triple", 2048, NULL, 5, NULL) !=
      pdPASS) {
    s_buzzer_active = false;
    buzzer_set_enabled(false);
    ESP_LOGE(TAG, "failed to create buzzer task");
  }
}
