#include <stdint.h>
#include <stdio.h>
#include "mcp342x.h"

#define MCP342x_ADDR    (0x6E)

int8_t usr_i2c_write(const uint8_t busAddr, const uint8_t *data, const uint32_t len) {
    mcp342x_return_code_t ret = MCP342x_RET_OK;

    // Transmit the data to the specified device from the provided
    // data buffer.

    return ret;
}

int8_t usr_i2c_read(const uint8_t busAddr, uint8_t *data, const uint32_t len) {
    mcp342x_return_code_t ret = MCP342x_RET_OK;

    // Received the specified amount of data from the device and store it
    // in the data buffer

    return ret;
}

void usr_delay_us(uint32_t period) {
    // Delay for the requested period
}

int main(void) {
    mcp342x_return_code_t ret = MCP342x_RET_OK;

    // Create an instance of our mcp342x device
    mcp342x_dev_t dev;

    // Provide the hardware abstraction functions for
    // I2c Read/Write and a micro-second delay function
    dev.intf.i2c_addr = MCP342x_ADDR;
    dev.intf.write = usr_i2c_write;
    dev.intf.read = usr_i2c_read;
    dev.intf.delay_us = usr_delay_us;

    // Init our desired config
    dev.registers.bits.config.bits.conv_mode = MCP342x_CM_CONT;
    dev.registers.bits.config.bits.gain = MCP342x_GAIN_x1;
    dev.registers.bits.config.bits.sample_rate = MCP342x_SR_60SPS;

    ret = mcp342x_writeConfig(&dev);

    if( MCP342x_RET_OK != ret ) {
        return 0;
    }

    while(1) {
        ret = mcp342x_sampleChannel(&dev, MCP342x_CH_1);

        if( MCP342x_RET_OK == ret ) {
            printf("CH1_raw: %d \t CH1_V: %0.2fV", dev.results[MCP342x_CH_1].outputCode, dev.results[MCP342x_CH_1].voltage);
        }
    }

    return 0;
}