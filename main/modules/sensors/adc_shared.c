#include "adc_shared.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static adc_oneshot_unit_handle_t s_adc_handle;
static SemaphoreHandle_t s_adc_mutex;

// ADC1 is shared by multiple sensor tasks, so reads go through one mutex.
static esp_err_t adc_shared_init(void) {
  if (s_adc_handle != NULL) {
    return ESP_OK;
  }

  adc_oneshot_unit_init_cfg_t unit_config = {
      .unit_id = ADC_UNIT_1,
  };

  esp_err_t err = adc_oneshot_new_unit(&unit_config, &s_adc_handle);
  if (err != ESP_OK) {
    return err;
  }

  s_adc_mutex = xSemaphoreCreateMutex();
  return s_adc_mutex == NULL ? ESP_ERR_NO_MEM : ESP_OK;
}

esp_err_t adc_shared_config_channel(adc_channel_t channel) {
  esp_err_t err = adc_shared_init();
  if (err != ESP_OK) {
    return err;
  }

  adc_oneshot_chan_cfg_t channel_config = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };

  return adc_oneshot_config_channel(s_adc_handle, channel, &channel_config);
}

esp_err_t adc_shared_read(adc_channel_t channel, int* raw) {
  esp_err_t err = adc_shared_init();
  if (err != ESP_OK) {
    return err;
  }

  xSemaphoreTake(s_adc_mutex, portMAX_DELAY);
  err = adc_oneshot_read(s_adc_handle, channel, raw);
  xSemaphoreGive(s_adc_mutex);

  return err;
}
