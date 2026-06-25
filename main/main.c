#include "OLED_display.h"
#include "clock.h"
#include "ir_remote.h"
#include "sensor_dht.h"
#include "sensor_light.h"
#include "sensor_soil.h"
#include "sensor_water.h"
#include "state.h"
#include "w2812b.h"
#include "water_pump.h"

void app_main(void) {
  garden_state_init();

  sensor_dht_start();
  sensor_light_start();
  sensor_soil_start();
  sensor_water_start();
  clock_start();
  ir_remote_start();
  // water_pump_init();
  // w2812b_start();
  oled_display_start();
}
