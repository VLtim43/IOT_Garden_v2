#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BLINK_GPIO GPIO_NUM_2

void app_main(void) {
  gpio_reset_pin(BLINK_GPIO);

  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

  while (1) {
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gpio_set_level(BLINK_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}