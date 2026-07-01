#include "clock.h"
#include "control.h"
#include "OLED_display.h"
#include "ir_remote.h"
#include "rgb_led.h"
#include "sensor_dht.h"
#include "sensor_light.h"
#include "sensor_soil.h"
#include "sensor_water.h"
#include "state.h"
#include "w2812b.h"
#include "water_pump.h"

void app_main(void) {
  garden_state_init();

  // sensors start
  sensor_dht_start();
  sensor_light_start();
  sensor_soil_start();
  sensor_water_start();

  // clock start
  clock_start();

  // actuators start
  w2812b_start();
  water_pump_init();
  rgb_led_init();

  // control start
  control_start();

  // IR reciever start
  ir_remote_start();

  // OLED screen start
  oled_display_start();
}
