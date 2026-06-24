#include "sensor_water.h"

#include "adc_shared.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "WATER_SENSOR";

static int clamp_percent(int percent) {
  if (percent < 0) {
    return 0;
  }

  if (percent > 100) {
    return 100;
  }

  return percent;
}

static int raw_to_water_level_percent(int raw) {
  int range = WATER_LEVEL_FULL_RAW - WATER_LEVEL_EMPTY_RAW;

  if (range <= 0) {
    return 0;
  }

  return clamp_percent(((raw - WATER_LEVEL_EMPTY_RAW) * 100) / range);
}

static void sensor_water_task(void* arg) {
  (void)arg;

  ESP_ERROR_CHECK(adc_shared_config_channel(WATER_LEVEL_ADC_CHANNEL));

  while (true) {
    int raw = 0;
    esp_err_t err = adc_shared_read(WATER_LEVEL_ADC_CHANNEL, &raw);

    if (err == ESP_OK) {
      int level_percent = raw_to_water_level_percent(raw);
      garden_state_set_water_level(level_percent);
      ESP_LOGI(TAG, "raw=%d level=%d%%", raw, level_percent);
    } else {
      ESP_LOGW(TAG, "read failed: %s", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sensor_water_start(void) {
  xTaskCreate(sensor_water_task, "sensor_water", 3072, NULL, 5, NULL);
}
