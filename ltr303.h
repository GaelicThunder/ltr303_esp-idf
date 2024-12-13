#pragma once

#include "driver/i2c.h"

#define LTR303_I2C_ADDR          0x29
#define LTR303_PART_ID           0x86
#define LTR303_MANU_ID           0x87
#define LTR303_ALS_CTRL          0x80
#define LTR303_STATUS            0x8C
#define LTR303_CH1DATA           0x88
#define LTR303_MEAS_RATE         0x85

typedef enum {
    LTR3XX_GAIN_1 = 0,
    LTR3XX_GAIN_2 = 1,
    LTR3XX_GAIN_4 = 2,
    LTR3XX_GAIN_8 = 3,
    LTR3XX_GAIN_48 = 6,
    LTR3XX_GAIN_96 = 7,
} ltr303_gain_t;

typedef enum {
    LTR3XX_INTEGTIME_100,
    LTR3XX_INTEGTIME_50,
    LTR3XX_INTEGTIME_200,
    LTR3XX_INTEGTIME_400,
    LTR3XX_INTEGTIME_150,
    LTR3XX_INTEGTIME_250,
    LTR3XX_INTEGTIME_300,
    LTR3XX_INTEGTIME_350,
} ltr303_integtime_t;

class LTR303 {
private:
    i2c_port_t i2c_port;
    uint8_t i2c_addr;
    
    esp_err_t write_register(uint8_t reg, uint8_t value);
    esp_err_t read_register(uint8_t reg, uint8_t* value);
    esp_err_t read_registers(uint8_t reg, uint8_t* data, size_t len);

public:
    LTR303(i2c_port_t port = I2C_NUM_0, uint8_t addr = LTR303_I2C_ADDR);
    
    esp_err_t begin();
    esp_err_t enable(bool en);
    esp_err_t setGain(ltr303_gain_t gain);
    esp_err_t setIntegrationTime(ltr303_integtime_t time);
    esp_err_t readBothChannels(uint16_t &ch0, uint16_t &ch1);
    uint16_t computeLux(uint16_t ch0, uint16_t ch1);
};
