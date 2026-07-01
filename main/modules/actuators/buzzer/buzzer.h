#ifndef BUZZER_H
#define BUZZER_H

// prepare buzzer GPIO for later alarm pulses
void buzzer_init(void);
// play a short three-beep alarm pattern
void buzzer_buzz_triple(void);

#endif
