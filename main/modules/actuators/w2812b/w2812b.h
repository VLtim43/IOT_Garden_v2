#ifndef W2812B_H
#define W2812B_H

#include <stdint.h>

void w2812b_start(void);
void w2812b_set_all(uint8_t red, uint8_t green, uint8_t blue);
void w2812b_set_color(uint8_t red, uint8_t green, uint8_t blue,
                      const char* color_code);

#endif
