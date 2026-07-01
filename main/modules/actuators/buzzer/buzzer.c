#include "buzzer.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
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
  BUZZER_TONE_HZ = 2000,
  BUZZER_DUTY = 512,
};

static bool s_buzzer_active;

static void buzzer_set_enabled(bool enabled) {
  if (enabled) {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                                  BUZZER_DUTY));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
  } else {
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
  }
}

static void buzzer_triple_task(void* arg) {
  (void)arg;

  // Active buzzer only needs short on/off pulses, not PWM tone generation.
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

  ledc_timer_config_t timer_config = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_10_BIT,
      .timer_num = LEDC_TIMER_0,
      .freq_hz = BUZZER_TONE_HZ,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

  ledc_channel_config_t channel_config = {
      .gpio_num = BUZZER_GPIO,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL_0,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

  buzzer_set_enabled(false);
  s_buzzer_active = false;
  ESP_LOGI(TAG, "ready on GPIO%d", BUZZER_GPIO);
}

void buzzer_buzz_triple(void) {
  if (s_buzzer_active) {
    return;
  }

  // Ignore retriggers while one alarm pattern is already playing.
  s_buzzer_active = true;
  if (xTaskCreate(buzzer_triple_task, "buzzer_triple", 2048, NULL, 5, NULL) !=
      pdPASS) {
    s_buzzer_active = false;
    buzzer_set_enabled(false);
    ESP_LOGE(TAG, "failed to create buzzer task");
  }
}
