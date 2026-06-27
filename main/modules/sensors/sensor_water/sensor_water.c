#include "sensor_water.h"

#include "adc_shared.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "WATER_SENSOR";

enum {
  WATER_LEVEL_CONFIRMATION_READS = 2,
};

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
  // This sensor is noisy and not very linear, so use coarse bands instead of a
  // straight percentage formula.
  if (raw >= 1850) {
    return 100;
  }
  if (raw >= 1750) {
    return 90;
  }
  if (raw >= 1650) {
    return 80;
  }
  if (raw >= 1550) {
    return 70;
  }
  if (raw >= 1450) {
    return 55;
  }
  if (raw >= 1350) {
    return 40;
  }
  if (raw >= 1250) {
    return 25;
  }
  if (raw >= 1200) {
    return 20;
  }
  if (raw >= 1000) {
    return 15;
  }
  if (raw >= 100) {
    return 5;
  }

  return 0;
}

static void sensor_water_task(void* arg) {
  (void)arg;

  int last_reported_percent = 0;
  int pending_percent = 0;
  int pending_count = 0;

  ESP_ERROR_CHECK(adc_shared_config_channel(WATER_LEVEL_ADC_CHANNEL));

  while (true) {
    int raw = 0;
    esp_err_t err = adc_shared_read(WATER_LEVEL_ADC_CHANNEL, &raw);

    if (err == ESP_OK) {
      int level_percent = raw_to_water_level_percent(raw);

      if (level_percent == last_reported_percent) {
        pending_percent = level_percent;
        pending_count = 0;
      } else if (level_percent == pending_percent) {
        pending_count++;
      } else {
        pending_percent = level_percent;
        pending_count = 1;
      }

      if (pending_count >= WATER_LEVEL_CONFIRMATION_READS) {
        last_reported_percent = pending_percent;
        garden_state_set_water_level(last_reported_percent);
        pending_count = 0;
      }

      ESP_LOGI(TAG, "raw=%d level=%d%% reported=%d%% confirm=%d/%d", raw,
               level_percent, last_reported_percent, pending_count,
               WATER_LEVEL_CONFIRMATION_READS);
    } else {
      ESP_LOGW(TAG, "read failed: %s", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sensor_water_start(void) {
  xTaskCreate(sensor_water_task, "sensor_water", 3072, NULL, 5, NULL);
}
