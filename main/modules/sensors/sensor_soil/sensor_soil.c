#include "sensor_soil.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "SOIL_SENSOR";

static int clamp_percent(int percent) {
  if (percent < 0) {
    return 0;
  }

  if (percent > 100) {
    return 100;
  }

  return percent;
}

static int raw_to_moisture_percent(int raw) {
  int range = SOIL_SENSOR_DRY_RAW - SOIL_SENSOR_WET_RAW;

  if (range <= 0) {
    return 0;
  }

  return clamp_percent(((SOIL_SENSOR_DRY_RAW - raw) * 100) / range);
}

static void sensor_soil_task(void* arg) {
  (void)arg;

  adc_oneshot_unit_handle_t adc_handle;
  adc_oneshot_unit_init_cfg_t unit_config = {
      .unit_id = ADC_UNIT_1,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &adc_handle));

  adc_oneshot_chan_cfg_t channel_config = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc_handle, SOIL_SENSOR_ADC_CHANNEL,
                                 &channel_config));

  while (true) {
    int raw = 0;
    esp_err_t err = adc_oneshot_read(adc_handle, SOIL_SENSOR_ADC_CHANNEL, &raw);

    if (err == ESP_OK) {
      int moisture_percent = raw_to_moisture_percent(raw);
      garden_state_set_soil_moisture(moisture_percent);
      ESP_LOGI(TAG, "raw=%d moisture=%d%%", raw, moisture_percent);
    } else {
      ESP_LOGW(TAG, "read failed: %s", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sensor_soil_start(void) {
  xTaskCreate(sensor_soil_task, "sensor_soil", 3072, NULL, 5, NULL);
}
