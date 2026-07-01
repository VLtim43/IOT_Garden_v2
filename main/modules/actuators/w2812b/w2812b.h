#ifndef W2812B_H
#define W2812B_H

#include <stdint.h>

// initialize the LED strip and apply the default color
void w2812b_start(void);
// write one RGB value to the whole strip
void w2812b_set_all(uint8_t red, uint8_t green, uint8_t blue);
// write one RGB value and publish its short label to shared state
void w2812b_set_color(uint8_t red, uint8_t green, uint8_t blue,
                      const char* color_code);
// select one color from the built-in strip palette
void w2812b_set_color_index(int index);
// move to the previous palette color
void w2812b_cycle_left(void);
// move to the next palette color
void w2812b_cycle_right(void);
// turn the strip output on or off without losing palette position
void w2812b_toggle_enabled(void);

#endif
