#include "OLED_display.h"

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "ssd1306.h"
#include "state.h"

#include <stdio.h>

// log tag
static const char* TAG = "OLED";
static ssd1306_handle_t s_display_handle;

enum {
  OLED_FULL_REFRESH_MS = 60000,
  OLED_STATE_POLL_MS = 250,
  OLED_IR_ICON_X = 104,
  OLED_LIGHT_ICON_X = 120,
};

static const uint8_t BLANK_ICON_8X8[] = {
    0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

static const uint8_t WIFI_ICON_8X8[] = {
    0b00111100, 0b01000010, 0b10011001, 0b00100100,
    0b01000010, 0b00011000, 0b00011000, 0b00000000,
};

static const uint8_t SUN_ICON_8X8[] = {
    0b10010001, 0b01011010, 0b00111100, 0b11111111,
    0b00111100, 0b01011010, 0b10010001, 0b00000000,
};

static const uint8_t MOON_ICON_8X8[] = {
    0b00011100, 0b00111110, 0b01111000, 0b01110000,
    0b01111000, 0b00111110, 0b00011100, 0b00000000,
};

static void oled_display_init(void) {
  // bus config for the I2C bus
  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_NUM_0,
      .sda_io_num = OLED_I2C_SDA_GPIO,  // 21
      .scl_io_num = OLED_I2C_SCL_GPIO,  // 22
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };

  // initialize the I2C
  i2c_master_bus_handle_t bus_handle = NULL;
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
  ESP_LOGI(TAG, "I2C bus ready on SDA=%d SCL=%d", OLED_I2C_SDA_GPIO,
           OLED_I2C_SCL_GPIO);

  // 128x64 OLED default config
  ssd1306_config_t display_config = I2C_SSD1306_128x64_CONFIG_DEFAULT;

  // initialize the screen
  ESP_ERROR_CHECK(ssd1306_init(bus_handle, &display_config, &s_display_handle));
  ESP_LOGI(TAG, "SSD1306 ready");

  ESP_ERROR_CHECK(ssd1306_clear_display(s_display_handle, false));
}

// update if light is ON or OFF
static void oled_display_update_light_icon(bool daylight) {
  const uint8_t* icon = daylight ? SUN_ICON_8X8 : MOON_ICON_8X8;

  ESP_ERROR_CHECK(ssd1306_display_image(s_display_handle, 0, OLED_LIGHT_ICON_X,
                                        BLANK_ICON_8X8,
                                        sizeof(BLANK_ICON_8X8)));
  ESP_ERROR_CHECK(ssd1306_display_bitmap(s_display_handle, OLED_LIGHT_ICON_X, 0,
                                         icon, 8, 8, false));
}

// update if recieving IR commands
static void oled_display_update_ir_icon(bool active) {
  const uint8_t* icon = active ? WIFI_ICON_8X8 : BLANK_ICON_8X8;

  ESP_ERROR_CHECK(ssd1306_display_image(s_display_handle, 0, OLED_IR_ICON_X,
                                        BLANK_ICON_8X8,
                                        sizeof(BLANK_ICON_8X8)));
  ESP_ERROR_CHECK(ssd1306_display_bitmap(s_display_handle, OLED_IR_ICON_X, 0,
                                         icon, 8, 8, false));
}

static void oled_display_update_temperature(int temperature_c) {
  char line[16];

  snprintf(line, sizeof(line), "Temp: %d C", temperature_c);
  ESP_ERROR_CHECK(ssd1306_clear_display_page(s_display_handle, 1, false));
  ESP_ERROR_CHECK(ssd1306_display_text(s_display_handle, 1, line, false));
}

static void oled_display_update_soil_moisture(int moisture_percent) {
  char line[16];

  snprintf(line, sizeof(line), "Soil: %d %%", moisture_percent);
  ESP_ERROR_CHECK(ssd1306_clear_display_page(s_display_handle, 2, false));
  ESP_ERROR_CHECK(ssd1306_display_text(s_display_handle, 2, line, false));
}

// render all elements on the screen
static void oled_display_render_full(const garden_state_t* state) {
  ESP_ERROR_CHECK(ssd1306_clear_display(s_display_handle, false));
  ESP_ERROR_CHECK(ssd1306_display_text(s_display_handle, 0, "12:45", false));
  oled_display_update_ir_icon(true);
  oled_display_update_light_icon(state->ambient_light_detected);
  oled_display_update_temperature(state->temperature_c);
  oled_display_update_soil_moisture(state->soil_moisture_percent);
}

static void oled_display_task(void* arg) {
  (void)arg;

  garden_state_t state = garden_state_get();
  bool last_ambient_light = state.ambient_light_detected;
  int last_temperature_c = state.temperature_c;
  int last_soil_moisture_percent = state.soil_moisture_percent;
  bool ir_active = true;
  int full_refresh_count = 0;

  oled_display_init();
  oled_display_render_full(&state);

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(OLED_STATE_POLL_MS));

    state = garden_state_get();
    if (state.ambient_light_detected != last_ambient_light) {
      oled_display_update_light_icon(state.ambient_light_detected);
      last_ambient_light = state.ambient_light_detected;
    }

    if (state.temperature_c != last_temperature_c) {
      oled_display_update_temperature(state.temperature_c);
      last_temperature_c = state.temperature_c;
    }

    if (state.soil_moisture_percent != last_soil_moisture_percent) {
      oled_display_update_soil_moisture(state.soil_moisture_percent);
      last_soil_moisture_percent = state.soil_moisture_percent;
    }

    if (ir_active) {
      ir_active = false;
      oled_display_update_ir_icon(ir_active);
    }

    full_refresh_count++;
    if ((full_refresh_count * OLED_STATE_POLL_MS) >= OLED_FULL_REFRESH_MS) {
      oled_display_render_full(&state);
      full_refresh_count = 0;
    }
  }
}

void oled_display_start(void) {
  xTaskCreate(oled_display_task, "oled_display", 4096, NULL, 5, NULL);
}
