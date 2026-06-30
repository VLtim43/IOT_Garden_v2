#include "w2812b.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "W2812B";
static led_strip_handle_t s_strip;

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  const char* color_code;
} w2812b_color_t;

static const w2812b_color_t W2812B_COLORS[] = {
    {.red = 180, .green = 0, .blue = 255, .color_code = "PURPL"},
    {.red = 255, .green = 0, .blue = 0, .color_code = "RED"},
    {.red = 0, .green = 0, .blue = 255, .color_code = "BLUE"},
};
enum {
  W2812B_CHANGE_COOLDOWN_MS = 250,
};

static int s_color_index = 0;
static bool s_enabled = true;
static TickType_t s_last_change_tick;

static bool w2812b_can_change_now(void) {
  TickType_t now = xTaskGetTickCount();

  if (s_last_change_tick != 0 &&
      (now - s_last_change_tick) < pdMS_TO_TICKS(W2812B_CHANGE_COOLDOWN_MS)) {
    return false;
  }

  s_last_change_tick = now;
  return true;
}

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

static void w2812b_apply_color_index(void) {
  const w2812b_color_t* color = &W2812B_COLORS[s_color_index];
  if (!s_enabled) {
    w2812b_set_all(0, 0, 0);
    garden_state_set_led_color_code("OFF");
    return;
  }

  w2812b_set_color(color->red, color->green, color->blue, color->color_code);
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

void w2812b_cycle_left(void) {
  if (!w2812b_can_change_now()) {
    return;
  }

  s_color_index--;
  if (s_color_index < 0) {
    s_color_index = (int)(sizeof(W2812B_COLORS) / sizeof(W2812B_COLORS[0])) - 1;
  }

  w2812b_apply_color_index();
}

void w2812b_cycle_right(void) {
  if (!w2812b_can_change_now()) {
    return;
  }

  s_color_index++;
  if (s_color_index >=
      (int)(sizeof(W2812B_COLORS) / sizeof(W2812B_COLORS[0]))) {
    s_color_index = 0;
  }

  w2812b_apply_color_index();
}

void w2812b_toggle_enabled(void) {
  if (!w2812b_can_change_now()) {
    return;
  }

  s_enabled = !s_enabled;
  w2812b_apply_color_index();
}

void w2812b_start(void) {
  w2812b_init();
  s_color_index = 0;
  s_enabled = true;
  s_last_change_tick = 0;
  w2812b_apply_color_index();
}
