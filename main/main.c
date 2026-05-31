#include "OLED_display.h"
#include "state.h"

void app_main(void) {
    garden_state_init();
    oled_display_test();
}
