#include "w2812b.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "W2812B";
static led_strip_handle_t s_strip;

static uint8_t apply_brightness(uint8_t value) {
  if (value == 0 || W2812B_BRIGHTNESS == 0) {
    return 0;
  }

  uint8_t scaled = (uint8_t)((uint16_t)value * W2812B_BRIGHTNESS / 255);
  return scaled == 0 ? 1 : scaled;
}

static void w2812b_init(void) {
  led_strip_config_t strip_config = {
      .strip_gpio_num = W2812B_GPIO,
      .max_leds = W2812B_LED_COUNT,
      .led_model = LED_MODEL_WS2812,
      .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
      .flags = {.invert_out = false},
  };
  led_strip_rmt_config_t rmt_config = {
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = 10 * 1000 * 1000,
      .mem_block_symbols = 0,
      .flags = {.with_dma = false},
  };

  ESP_ERROR_CHECK(
      led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip));
  ESP_ERROR_CHECK(led_strip_clear(s_strip));

  ESP_LOGI(TAG, "ready on GPIO%d with %d LEDs", W2812B_GPIO, W2812B_LED_COUNT);
}

void w2812b_set_all(uint8_t red, uint8_t green, uint8_t blue) {
  if (s_strip == NULL) {
    return;
  }

  uint8_t scaled_red = apply_brightness(red);
  uint8_t scaled_green = apply_brightness(green);
  uint8_t scaled_blue = apply_brightness(blue);

  for (int i = 0; i < W2812B_LED_COUNT; i++) {
    ESP_ERROR_CHECK(
        led_strip_set_pixel(s_strip, i, scaled_red, scaled_green, scaled_blue));
  }

  ESP_ERROR_CHECK(led_strip_refresh(s_strip));
}

void w2812b_set_color(uint8_t red, uint8_t green, uint8_t blue,
                      const char* color_code) {
  w2812b_set_all(red, green, blue);
  garden_state_set_led_color_code(color_code);
}

static void w2812b_task(void* arg) {
  (void)arg;

  w2812b_init();
  w2812b_set_color(180, 0, 255, "PURPL");

  vTaskDelete(NULL);
}

void w2812b_start(void) {
  xTaskCreate(w2812b_task, "w2812b", 3072, NULL, 5, NULL);
}
