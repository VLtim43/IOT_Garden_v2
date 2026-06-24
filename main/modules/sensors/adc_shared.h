#ifndef ADC_SHARED_H
#define ADC_SHARED_H

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

esp_err_t adc_shared_config_channel(adc_channel_t channel);
esp_err_t adc_shared_read(adc_channel_t channel, int* raw);

#endif
