#include "water_pump.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "pins.h"

static const char* TAG = "WATER_PUMP";

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

  ESP_LOGI(TAG, "ready on GPIO%d", WATER_PUMP_GPIO);
}

void water_pump_set_enabled(bool enabled) {
  ESP_ERROR_CHECK(gpio_set_level(WATER_PUMP_GPIO, enabled ? 1 : 0));
}
