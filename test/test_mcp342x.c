#include "unity.h"

#include "mcp342x.h"

mcp342x_dev_t mcp342x_device;

int8_t usr_i2c_write(const uint8_t busAddr, const uint8_t *data, const uint32_t len);
int8_t usr_i2c_read(const uint8_t busAddr, uint8_t *data, const uint32_t len);
void usr_delay_us(uint32_t period);

void setUp(void)
{
    // Provide the hardware abstraction functions for
    // I2c Read/Write and a micro-second delay function
    mcp342x_device.intf.i2c_addr = 0x6E;
    mcp342x_device.intf.write = usr_i2c_write;
    mcp342x_device.intf.read = usr_i2c_read;
    mcp342x_device.intf.delay_us = usr_delay_us;

    // Init our desired config
    mcp342x_device.registers.bits.config.bits.gain = MCP342x_GAIN_x1;
    mcp342x_device.registers.bits.config.bits.sample_rate = MCP342x_SR_60SPS;
}

void tearDown(void)
{
}

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

void test_mcp342x_writeConfig_NullDevice(void)
{
    mcp342x_return_code_t ret =  mcp342x_writeConfig(NULL);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_NULL_PTR, ret);
}

void test_mcp342x_writeConfig_NullWriteIntf(void)
{
    mcp342x_return_code_t ret;
    mcp342x_device.intf.write = NULL;

    ret = mcp342x_writeConfig(&mcp342x_device);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_NULL_PTR, ret);
}

void test_mcp342x_sampleChannel_NullDevice(void)
{
    mcp342x_return_code_t ret = mcp342x_sampleChannel(NULL, 0);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_NULL_PTR, ret);
}

void test_mcp342x_sampleChannel_NullWriteIntf(void)
{
    mcp342x_return_code_t ret;
    mcp342x_device.intf.write = NULL;

    ret = mcp342x_sampleChannel(&mcp342x_device, 0);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_NULL_PTR, ret);
}

void test_mcp342x_sampleChannel_NullReadIntf(void)
{
    mcp342x_return_code_t ret;
    mcp342x_device.intf.read = NULL;

    ret = mcp342x_sampleChannel(&mcp342x_device, 0);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_NULL_PTR, ret);
}

void test_mcp342x_sampleChannel_NullDelayIntf(void)
{
    mcp342x_return_code_t ret;
    mcp342x_device.intf.delay_us = NULL;

    ret =  mcp342x_sampleChannel(&mcp342x_device, 0);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_NULL_PTR, ret);
}

void test_mcp342x_sampleChannel_InvalidChannel(void)
{
    mcp342x_return_code_t ret = mcp342x_sampleChannel(&mcp342x_device, MCP342x_CH__MAX__);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_INV_PARAM, ret);
}

