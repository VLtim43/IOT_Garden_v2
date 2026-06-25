#include "clock.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "state.h"

static const char* TAG = "DS1302";

enum {
  DS1302_WRITE_SECONDS = 0x80,
  DS1302_READ_SECONDS = 0x81,
  DS1302_WRITE_MINUTES = 0x82,
  DS1302_READ_MINUTES = 0x83,
  DS1302_WRITE_HOURS = 0x84,
  DS1302_READ_HOURS = 0x85,
  DS1302_WRITE_CONTROL = 0x8E,
  DS1302_WRITE_TRICKLE = 0x90,
};

static void ds1302_delay(void) { esp_rom_delay_us(2); }

static void ds1302_begin_session(void) {
  ESP_ERROR_CHECK(gpio_set_level(DS1302_CLK_GPIO, 0));
  ESP_ERROR_CHECK(gpio_set_level(DS1302_RST_GPIO, 1));
  esp_rom_delay_us(4);
}

static void ds1302_end_session(void) {
  ESP_ERROR_CHECK(gpio_set_level(DS1302_RST_GPIO, 0));
  esp_rom_delay_us(4);
}

static void ds1302_set_dat_output(void) {
  ESP_ERROR_CHECK(gpio_set_direction(DS1302_DAT_GPIO, GPIO_MODE_OUTPUT));
  ESP_ERROR_CHECK(gpio_set_pull_mode(DS1302_DAT_GPIO, GPIO_FLOATING));
}

static void ds1302_set_dat_input(void) {
  ESP_ERROR_CHECK(gpio_set_direction(DS1302_DAT_GPIO, GPIO_MODE_INPUT));
  ESP_ERROR_CHECK(gpio_set_pull_mode(DS1302_DAT_GPIO, GPIO_PULLUP_ONLY));
}

static void ds1302_write_byte(uint8_t value, bool read_after) {
  ds1302_set_dat_output();

  for (int bit = 0; bit < 8; bit++) {
    ESP_ERROR_CHECK(gpio_set_level(DS1302_DAT_GPIO, (value >> bit) & 0x01));
    ds1302_delay();
    ESP_ERROR_CHECK(gpio_set_level(DS1302_CLK_GPIO, 1));
    ds1302_delay();

    if (read_after && bit == 7) {
      ds1302_set_dat_input();
    } else {
      ESP_ERROR_CHECK(gpio_set_level(DS1302_CLK_GPIO, 0));
      ds1302_delay();
    }
  }
}

static uint8_t ds1302_read_byte(void) {
  uint8_t value = 0;

  for (int bit = 0; bit < 8; bit++) {
    ESP_ERROR_CHECK(gpio_set_level(DS1302_CLK_GPIO, 1));
    ds1302_delay();
    ESP_ERROR_CHECK(gpio_set_level(DS1302_CLK_GPIO, 0));
    ds1302_delay();

    if (gpio_get_level(DS1302_DAT_GPIO)) {
      value |= 1U << bit;
    }
  }

  return value;
}

static uint8_t ds1302_read_register(uint8_t command) {
  uint8_t value;

  ds1302_begin_session();
  ds1302_write_byte(command, true);
  value = ds1302_read_byte();

  ds1302_end_session();
  ds1302_set_dat_output();
  ESP_ERROR_CHECK(gpio_set_level(DS1302_DAT_GPIO, 0));

  return value;
}

static void ds1302_write_register(uint8_t command, uint8_t value) {
  ds1302_begin_session();
  ds1302_write_byte(command, false);
  ds1302_write_byte(value, false);
  ds1302_end_session();
  ESP_ERROR_CHECK(gpio_set_level(DS1302_DAT_GPIO, 0));
}

static int bcd_to_int(uint8_t value) {
  return ((value >> 4) * 10) + (value & 0x0F);
}

static uint8_t int_to_bcd(int value) {
  return (uint8_t)(((value / 10) << 4) | (value % 10));
}

static bool ds1302_read_time(int* hour, int* minute) {
  uint8_t seconds_raw = ds1302_read_register(DS1302_READ_SECONDS);
  uint8_t minutes_raw = ds1302_read_register(DS1302_READ_MINUTES);
  uint8_t hours_raw = ds1302_read_register(DS1302_READ_HOURS);

  if ((seconds_raw & 0x80) != 0) {
    return false;
  }

  int minute_value = bcd_to_int(minutes_raw & 0x7F);
  int hour_value;

  if ((hours_raw & 0x80) != 0) {
    hour_value = bcd_to_int(hours_raw & 0x1F);
    if ((hours_raw & 0x20) != 0 && hour_value < 12) {
      hour_value += 12;
    } else if ((hours_raw & 0x20) == 0 && hour_value == 12) {
      hour_value = 0;
    }
  } else {
    hour_value = bcd_to_int(hours_raw & 0x3F);
  }

  if (hour_value > 23 || minute_value > 59) {
    return false;
  }

  *hour = hour_value;
  *minute = minute_value;
  return true;
}

static void ds1302_init(void) {
  gpio_config_t output_config = {
      .pin_bit_mask = (1ULL << DS1302_CLK_GPIO) | (1ULL << DS1302_RST_GPIO) |
                      (1ULL << DS1302_DAT_GPIO),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&output_config));
  ESP_ERROR_CHECK(gpio_set_level(DS1302_CLK_GPIO, 0));
  ESP_ERROR_CHECK(gpio_set_level(DS1302_RST_GPIO, 0));
  ESP_ERROR_CHECK(gpio_set_level(DS1302_DAT_GPIO, 0));

  ESP_LOGI(TAG, "ready on CLK=%d DAT=%d RST=%d", DS1302_CLK_GPIO,
           DS1302_DAT_GPIO, DS1302_RST_GPIO);
}

static void ds1302_set_start_time(void) {
  int hour = 0;
  int minute = 0;

  ds1302_write_register(DS1302_WRITE_CONTROL, 0x00);
  ds1302_write_register(DS1302_WRITE_TRICKLE, 0x00);
  ds1302_write_register(DS1302_WRITE_SECONDS, int_to_bcd(0));
  ds1302_write_register(DS1302_WRITE_MINUTES, int_to_bcd(40));
  ds1302_write_register(DS1302_WRITE_HOURS, int_to_bcd(14));

  if (ds1302_read_time(&hour, &minute)) {
    ESP_LOGI(TAG, "start time write read back as %02d:%02d", hour, minute);
  } else {
    ESP_LOGW(TAG, "start time write could not be read back");
  }
}

static void clock_task(void* arg) {
  (void)arg;

  ds1302_init();
  // Run once to seed the RTC, then comment this out to stop overwriting time.
  // ds1302_set_start_time();

  while (true) {
    int hour = 0;
    int minute = 0;
    char time_text[GARDEN_TIME_TEXT_MAX_LEN];

    if (ds1302_read_time(&hour, &minute)) {
      snprintf(time_text, sizeof(time_text), "%02d:%02d", hour, minute);
    } else {
      snprintf(time_text, sizeof(time_text), "--:--");
    }

    garden_state_set_time_text(time_text);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void clock_start(void) {
  xTaskCreate(clock_task, "clock", 3072, NULL, 5, NULL);
}
