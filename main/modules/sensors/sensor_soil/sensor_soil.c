#include "sensor_soil.h"

#include "adc_shared.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "SOIL_SENSOR";

static void sensor_soil_task(void* arg) {
  (void)arg;

  ESP_ERROR_CHECK(adc_shared_config_channel(SOIL_SENSOR_ADC_CHANNEL));

  while (true) {
    int raw = 0;
    esp_err_t err = adc_shared_read(SOIL_SENSOR_ADC_CHANNEL, &raw);

    if (err == ESP_OK) {
      garden_state_set_soil_raw(raw);
      ESP_LOGI(TAG, "raw=%d", raw);
    } else {
      ESP_LOGW(TAG, "read failed: %s", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sensor_soil_start(void) {
  xTaskCreate(sensor_soil_task, "sensor_soil", 3072, NULL, 5, NULL);
}
