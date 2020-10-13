/*! @file mcp342x.c
 * @brief Driver source for the MCP342x 16-Bit, ADC C driver.
 */

#include <stdint.h>
#include <stdio.h>
#include "mcp342x.h"

#define MAX_READ_RETRY  10 /*! @brief Max number of retries before giving up on sampling the specificed channel */

/*!
 * @brief This API writes the current config to the device
 */
mcp342x_return_code_t mcp342x_writeConfig(mcp342x_dev_t *dev) {
    mcp342x_return_code_t ret = MCP342x_RET_OK;

    if( dev == NULL ) {
        // The pointer to the mcp342x dev is NULL
        ret = MCP342x_RET_NULL_PTR;
    }

    if( (dev->registers.bits.config.bits.conv_mode >= MCP342x_CM__MAX__) ||
        (dev->registers.bits.config.bits.gain >= MCP342x_GAIN__MAX__) ||
        (dev->registers.bits.config.bits.sample_rate >= MCP342x_SR__MAX__) ) {
        // One of the config values provideed to use is not valid.
        ret = MCP342x_RET_INV_PARAM;
    }

    if( ret = MCP342x_RET_OK ) {
        // Write the config to our device
        ret = dev->intf.write((dev->intf.i2c_addr << 1), &dev->registers.bits.config.byte, 0x01);
    }

    return ret;
}

/*!
 * @brief This API samples the specified channel
 */
mcp342x_return_code_t mcp342x_sampleChannel(mcp342x_dev_t *dev, mcp342x_channel_enum ch) {
    mcp342x_return_code_t ret = MCP342x_RET_OK;
    uint8_t retry = 0;

    if( NULL == dev ) {
        ret = MCP342x_RET_NULL_PTR;
    }
    else if( ch >= MCP342x_CH__MAX__ )
    {
        ret = MCP342x_RET_INV_PARAM;
    }

    if( MCP342x_RET_OK == ret ) {
        // Make sure the RDY bit is set to 1 on the device.. (It will go to a zero when a conversion has been completed)
        dev->registers.bits.config.bits.ready = 0b1;
        // Set the desired channel in our config struct
        dev->registers.bits.config.bits.channel = ch;
        // Clear out the samples from the previous scan
        dev->registers.bits.upper_data = 0x00;
        dev->registers.bits.lower_data = 0x00;
        // Clear out the previous results
        dev->results->outputCode = 0x0000;
        dev->results->voltage = 0.0;

        // Write the channel config to the device
        ret = mcp342x_writeConfig(dev);
    }

    if( MCP342x_RET_OK == ret ) {
        // Keep reading and checking for the ready bit or until we max out
        // on retries.
        while( dev->registers.bits.config.bits.ready != 0b0 ) {
            // Read the results
            ret = dev->intf.read((dev->intf.i2c_addr << 1) | 0x01, dev->registers.bytes, 0x04);

            if( ret != MCP342x_RET_OK ) {
                // Something went wrong with our READ, break out and return.
                break;
            }
            else if( MAX_READ_RETRY >= retry ) {
                // We exceeded our max number of retries.
                // Return with a timeout errorcode
                ret = MCP342x_RET_TIMEOUT;
                break;
            }
            else
            {
                // The ready bit isn't set.
                // Increment the retry count.
                retry++;
            }

            // Wait 10ms before checking for the ready bit
            dev->intf.delay_us(10000);
        }
    }

    if( MCP342x_RET_OK == ret ) {
        // Create the 16-bit output code from the lower and upper results registers
        dev->results->outputCode = (uint16_t)(dev->registers.bits.upper_data << 8) | dev->registers.bits.lower_data;
        // Determine the sampled voltage from the output code
        dev->results->voltage = dev->results->outputCode * MCP342x_LSB_VAL;
    }

    // Return our return code from retrieving a channel sample
    return ret;
}