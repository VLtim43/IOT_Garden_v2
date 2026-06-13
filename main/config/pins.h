#ifndef PINS_H
#define PINS_H

// OLED pins
#define OLED_I2C_SDA_GPIO 21
#define OLED_I2C_SCL_GPIO 22

// Photoresistor module DO pin.
#define LIGHT_SENSOR_DO_GPIO 34
#define LIGHT_SENSOR_DAY_LEVEL 0

// DHT11 sensor data pin.
#define DHT11_DATA_GPIO 27

// Capacitive soil moisture sensor v2 analog output pin.
#define SOIL_SENSOR_ADC_CHANNEL ADC_CHANNEL_7
#define SOIL_SENSOR_DRY_RAW 2800
#define SOIL_SENSOR_WET_RAW 1200

#endif
