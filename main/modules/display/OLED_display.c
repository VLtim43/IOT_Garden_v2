#include "OLED_display.h"

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "pins.h"
#include "ssd1306.h"

// log tag
static const char* TAG = "OLED";

void oled_display_test(void) {
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
  ssd1306_handle_t display_handle = NULL;

  // initialize the screen
  ESP_ERROR_CHECK(ssd1306_init(bus_handle, &display_config, &display_handle));
  ESP_LOGI(TAG, "SSD1306 ready");

  ESP_ERROR_CHECK(ssd1306_clear_display(display_handle, false));
  ESP_ERROR_CHECK(ssd1306_display_text(display_handle, 0, "Hello OLED", false));
}
