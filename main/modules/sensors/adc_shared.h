#ifndef ADC_SHARED_H
#define ADC_SHARED_H

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

// configure one ADC1 channel for shared reads
esp_err_t adc_shared_config_channel(adc_channel_t channel);
// read one raw sample through the shared ADC mutex
esp_err_t adc_shared_read(adc_channel_t channel, int* raw);

#endif
