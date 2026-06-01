#include "OLED_display.h"
#include "sensor_dht.h"
#include "sensor_light.h"
#include "state.h"

void app_main(void) {
    garden_state_init();
    sensor_dht_start();
    sensor_light_start();
    oled_display_start();
}
