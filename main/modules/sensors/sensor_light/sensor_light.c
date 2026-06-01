#include "sensor_light.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "LIGHT_SENSOR";

static bool sensor_light_read(void) {
  int level = gpio_get_level(LIGHT_SENSOR_DO_GPIO);
  return level == LIGHT_SENSOR_DAY_LEVEL;
}

static void sensor_light_task(void* arg) {
  (void)arg;

  gpio_config_t config = {
      .pin_bit_mask = 1ULL << LIGHT_SENSOR_DO_GPIO,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&config));

  bool last_detected = sensor_light_read();
  int log_count = 0;
  garden_state_set_ambient_light(last_detected);
  ESP_LOGI(TAG, "ambient light %s", last_detected ? "detected" : "not detected");

  while (true) {
    int raw_level = gpio_get_level(LIGHT_SENSOR_DO_GPIO);
    bool detected = sensor_light_read();
    if (detected != last_detected) {
      garden_state_set_ambient_light(detected);
      ESP_LOGI(TAG, "DO=%d ambient light %s", raw_level,
               detected ? "detected" : "not detected");
      last_detected = detected;
    }

    log_count++;
    if (log_count >= 4) {
      ESP_LOGI(TAG, "DO=%d (%s)", raw_level, detected ? "DAY" : "NIGHT");
      log_count = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(250));
  }
}

void sensor_light_start(void) {
  xTaskCreate(sensor_light_task, "sensor_light", 2048, NULL, 5, NULL);
}
