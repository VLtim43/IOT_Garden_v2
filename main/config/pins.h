#ifndef PINS_H
#define PINS_H

// Board pin labels:
// Top row:    VIN GND D13 D12 D14 D27 D26 D25 D33 D32 D35 D34 VN VP EN
// Bottom row: 3V3 GND D15 D2  D4  RX2 TX2 D5  D18 D19 D21 RX0 TX0 D22 D23

// OLED pins
#define OLED_I2C_SDA_GPIO 21
#define OLED_I2C_SCL_GPIO 22

// Photoresistor module DO pin.
#define LIGHT_SENSOR_DO_GPIO 34
#define LIGHT_SENSOR_DAY_LEVEL 0

// DHT11 sensor data pin.
#define DHT11_DATA_GPIO 27

// IR receiver output pin.
#define IR_RECEIVER_GPIO 26

// IR remote NEC codes.
#define IR_CODE_1 0xBA45FF00UL
#define IR_CODE_2 0xB946FF00UL
#define IR_CODE_3 0xB847FF00UL
#define IR_CODE_4 0xBB44FF00UL
#define IR_CODE_5 0xBF40FF00UL
#define IR_CODE_6 0xBC43FF00UL
#define IR_CODE_7 0xF807FF00UL
#define IR_CODE_8 0xEA15FF00UL
#define IR_CODE_9 0xF609FF00UL
#define IR_CODE_STAR 0xE916FF00UL
#define IR_CODE_0 0xE619FF00UL
#define IR_CODE_HASH 0xF20DFF00UL
#define IR_CODE_UP 0xE718FF00UL
#define IR_CODE_LEFT 0xF708FF00UL
#define IR_CODE_DOWN 0xAD52FF00UL
#define IR_CODE_RIGHT 0xA55AFF00UL
#define IR_CODE_OK 0xE31CFF00UL

// Water pump control pin.
#define WATER_PUMP_GPIO 4

// WS2812B LED strip control pin.
#define W2812B_GPIO 5
#define W2812B_LED_COUNT 25
#define W2812B_BRIGHTNESS 1

// DS1302 RTC pins.
#define DS1302_CLK_GPIO 18
#define DS1302_DAT_GPIO 19
#define DS1302_RST_GPIO 23

// Capacitive soil moisture sensor v2 analog output pin.
#define SOIL_SENSOR_ADC_CHANNEL ADC_CHANNEL_5  //  GPIO 33
#define SOIL_SENSOR_DRY_RAW 2800
#define SOIL_SENSOR_WET_RAW 1200

// Water level sensor signal pin.
#define WATER_LEVEL_ADC_CHANNEL ADC_CHANNEL_7  // GPIO 35
#define WATER_LEVEL_EMPTY_RAW 0
#define WATER_LEVEL_FULL_RAW 3075

#endif
