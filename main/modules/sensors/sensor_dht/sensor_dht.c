#include "sensor_dht.h"

#include "dht.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "DHT11";

static void sensor_dht_task(void* arg) {
  (void)arg;

  // DHT11 needs a short settle time before the first read.
  gpio_set_pull_mode(DHT11_DATA_GPIO, GPIO_PULLUP_ONLY);
  vTaskDelay(pdMS_TO_TICKS(1500));

  while (true) {
    int16_t temperature = 0;
    esp_err_t err =
        dht_read_data(DHT_TYPE_DHT11, DHT11_DATA_GPIO, NULL, &temperature);

    if (err == ESP_OK) {
      int temperature_c = temperature / 10;
      garden_state_set_temperature(temperature_c);
      ESP_LOGI(TAG, "temperature %dC", temperature_c);
    } else {
      ESP_LOGW(TAG, "read failed: %s", esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sensor_dht_start(void) {
  xTaskCreate(sensor_dht_task, "sensor_dht", 3072, NULL, 5, NULL);
}
