#include "ltr303.h"
#include "esp_log.h"

#define TAG "LTR303"

LTR303::LTR303(i2c_port_t port, uint8_t addr) : i2c_port(port), i2c_addr(addr) {}

esp_err_t LTR303::write_register(uint8_t reg, uint8_t value) {
    uint8_t write_buf[2] = {reg, value};
    return i2c_master_write_to_device(i2c_port, i2c_addr, write_buf, sizeof(write_buf), pdMS_TO_TICKS(100));
}

esp_err_t LTR303::read_register(uint8_t reg, uint8_t* value) {
    return i2c_master_write_read_device(i2c_port, i2c_addr, &reg, 1, value, 1, pdMS_TO_TICKS(100));
}

esp_err_t LTR303::read_registers(uint8_t reg, uint8_t* data, size_t len) {
    return i2c_master_write_read_device(i2c_port, i2c_addr, &reg, 1, data, len, pdMS_TO_TICKS(100));
}

esp_err_t LTR303::begin() {
    uint8_t id;
    ESP_ERROR_CHECK(read_register(LTR303_PART_ID, &id));
    if (id != 0xA0) {
        ESP_LOGE(TAG, "Wrong device ID: 0x%02x", id);
        return ESP_FAIL;
    }
    
    ESP_ERROR_CHECK(read_register(LTR303_MANU_ID, &id));
    if (id != 0x05) {
        ESP_LOGE(TAG, "Wrong manufacturer ID: 0x%02x", id);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t LTR303::enable(bool en) {
    uint8_t ctrl;
    ESP_ERROR_CHECK(read_register(LTR303_ALS_CTRL, &ctrl));
    if (en) {
        ctrl |= 0x01;
    } else {
        ctrl &= ~0x01;
    }
    return write_register(LTR303_ALS_CTRL, ctrl);
}

esp_err_t LTR303::setGain(ltr303_gain_t gain) {
    uint8_t ctrl;
    ESP_ERROR_CHECK(read_register(LTR303_ALS_CTRL, &ctrl));
    ctrl &= ~(0x07 << 2);
    ctrl |= (gain << 2);
    return write_register(LTR303_ALS_CTRL, ctrl);
}

esp_err_t LTR303::readBothChannels(uint16_t &ch0, uint16_t &ch1) {
    uint8_t data[4];
    ESP_ERROR_CHECK(read_registers(LTR303_CH1DATA, data, 4));
    
    ch1 = (data[1] << 8) | data[0];
    ch0 = (data[3] << 8) | data[2];
    
    return ESP_OK;
}

uint16_t LTR303::computeLux(uint16_t ch0, uint16_t ch1) {
    float ratio = ch1 / (float)(ch0 + ch1);
    float lux = 0.0f;
    
    if (ratio < 0.45f) {
        lux = (1.7743f * ch0 + 1.1059f * ch1);
    } else if (ratio < 0.64f) {
        lux = (4.2785f * ch0 - 1.9548f * ch1);
    } else if (ratio < 0.85f) {
        lux = (0.5926f * ch0 + 0.1185f * ch1);
    }
    
    return (uint16_t)(lux + 0.5f);
}
