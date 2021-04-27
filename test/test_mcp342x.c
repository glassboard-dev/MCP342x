#include <string.h>
#include "unity.h"
#include "mcp342x.h"

mcp342x_dev_t mcp342x_device;

int8_t usr_i2c_write(const uint8_t busAddr, const uint8_t *data, const uint32_t len);
int8_t usr_i2c_read(const uint8_t busAddr, uint8_t *data, const uint32_t len);
void usr_delay_us(uint32_t period);

mcp342x_return_code_t desired_read_ret = MCP342x_RET_OK;

void setUp(void)
{
    // Set our desired read return result
    desired_read_ret = MCP342x_RET_OK;

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
    mcp342x_registers_t tmpReg = {0};

    switch( desired_read_ret ) {
        case MCP342x_RET_OK:
            // A valid i2c read would see the ready bit change to zero.. Go ahead and mask the incoming data
            // as our register bit map and set the ready bit
            memcpy(&tmpReg, data, sizeof(tmpReg));
            tmpReg.bits.config.bits.ready = 0x00;
            memcpy(data, &tmpReg, sizeof(tmpReg));
            ret = MCP342x_RET_OK;
            break;

        case MCP342x_RET_TIMEOUT:
            // A timeout would see a RET OK on the read,
            // but the READY bit would just never get set..
            // So just return OK but don't set ready bit.
            ret = MCP342x_RET_OK;
            break;

        case MCP342x_RET_ERROR:
            ret = MCP342x_RET_ERROR;
            break;

        default:
            break;
    }

    return ret;
}

void usr_delay_us(uint32_t period) {
    // Delay for the requested period
}

/**************** WRITE CONFIG *************************************/
void test_mcp342x_writeConfig_AllValid(void)
{
    mcp342x_return_code_t ret = mcp342x_writeConfig(&mcp342x_device);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_OK, ret);
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

/**************** SAMPLE CHANNEL *************************************/
void test_mcp342x_sampleChannel_AllValid_SingleChannel(void)
{
    mcp342x_return_code_t ret = MCP342x_RET_OK;

    ret = mcp342x_sampleChannel(&mcp342x_device, 0);

    TEST_ASSERT_EQUAL_INT(MCP342x_RET_OK, ret);
}

void test_mcp342x_sampleChannel_AllValid_MultiChannel(void)
{
    mcp342x_return_code_t ret = MCP342x_RET_OK;

    for(uint8_t i = 0; i < MCP342x_CH__MAX__; i++) {
        ret = mcp342x_sampleChannel(&mcp342x_device, i);
        TEST_ASSERT_EQUAL_INT(MCP342x_RET_OK, ret);
    }
}

void test_mcp342x_sampleChannel_ReadTimeout(void)
{
    mcp342x_return_code_t ret = MCP342x_RET_OK;
    desired_read_ret = MCP342x_RET_TIMEOUT;

    ret = mcp342x_sampleChannel(&mcp342x_device, 0);
    TEST_ASSERT_EQUAL_INT(MCP342x_RET_TIMEOUT, ret);
}

void test_mcp342x_sampleChannel_GenericReadError(void)
{
    mcp342x_return_code_t ret = MCP342x_RET_OK;
    desired_read_ret = MCP342x_RET_ERROR;

    ret = mcp342x_sampleChannel(&mcp342x_device, 0);
    TEST_ASSERT_EQUAL_INT(MCP342x_RET_ERROR, ret);
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

