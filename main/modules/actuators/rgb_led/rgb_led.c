#include "rgb_led.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "pins.h"
#include "state.h"

#include <stdbool.h>

static const char* TAG = "RGB_LED";

static void rgb_led_set_channel(gpio_num_t gpio, bool enabled) {
  ESP_ERROR_CHECK(gpio_set_level(gpio, enabled ? 1 : 0));
}

static const char* rgb_led_color_code(rgb_led_color_t color) {
  switch (color) {
    case RGB_LED_COLOR_RED:
      return "RED";
    case RGB_LED_COLOR_GREEN:
      return "GREEN";
    case RGB_LED_COLOR_BLUE:
      return "BLUE";
    case RGB_LED_COLOR_YELLOW:
      return "YELLOW";
    case RGB_LED_COLOR_MAGENTA:
      return "MAGNTA";
    case RGB_LED_COLOR_CYAN:
      return "CYAN";
    case RGB_LED_COLOR_WHITE:
      return "WHITE";
    case RGB_LED_COLOR_OFF:
    default:
      return "OFF";
  }
}

void rgb_led_init(void) {
  gpio_config_t config = {
      .pin_bit_mask = (1ULL << RGB_LED_BLUE_GPIO) | (1ULL << RGB_LED_GREEN_GPIO) |
                      (1ULL << RGB_LED_RED_GPIO),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  ESP_ERROR_CHECK(gpio_config(&config));
  rgb_led_set_color(RGB_LED_COLOR_OFF);
  ESP_LOGI(TAG, "ready on B=%d G=%d R=%d", RGB_LED_BLUE_GPIO,
           RGB_LED_GREEN_GPIO, RGB_LED_RED_GPIO);
}

void rgb_led_set_color(rgb_led_color_t color) {
  bool red = false;
  bool green = false;
  bool blue = false;

  // This discrete RGB LED is driven by simple on/off channel combinations.
  switch (color) {
    case RGB_LED_COLOR_RED:
      red = true;
      break;
    case RGB_LED_COLOR_GREEN:
      green = true;
      break;
    case RGB_LED_COLOR_BLUE:
      blue = true;
      break;
    case RGB_LED_COLOR_YELLOW:
      red = true;
      green = true;
      break;
    case RGB_LED_COLOR_MAGENTA:
      red = true;
      blue = true;
      break;
    case RGB_LED_COLOR_CYAN:
      green = true;
      blue = true;
      break;
    case RGB_LED_COLOR_WHITE:
      red = true;
      green = true;
      blue = true;
      break;
    case RGB_LED_COLOR_OFF:
    default:
      break;
  }

  rgb_led_set_channel(RGB_LED_RED_GPIO, red);
  rgb_led_set_channel(RGB_LED_GREEN_GPIO, green);
  rgb_led_set_channel(RGB_LED_BLUE_GPIO, blue);
  garden_state_set_rgb_led_color_code(rgb_led_color_code(color));
}
