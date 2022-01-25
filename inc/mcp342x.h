/*! @file mcp342x.h
 * @brief Public header file for the MCP342x 16-Bit, ADC C driver.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MCP342X_H_
#define _MCP342X_H_

#include <stdint.h>
#include <stdbool.h>

#define MCP342x_15_SPS_LSB_VAL  (0.0000625) /*! @brief LSB Value in Volts of a sampled output code at 16bit resolution */
#define MCP342x_60_SPS_LSB_VAL  (0.00025)   /*! @brief LSB Value in Volts of a sampled output code at 14bit resolution */
#define MCP342x_240_SPS_LSB_VAL (0.001)     /*! @brief LSB Value in Volts of a sampled output code at 12bit resolution */

/*!
 * @brief This function pointer API reads I2C data from the specified
 * device on the bus.
 *
 * @param[in] busAddr: Address of the device to be read from
 * @param[out] *data: Pointer to the where we should store the read data
 * @param[in] len: Length of data to be read
 *
 * @return The result of reading I2C data
 */
typedef int8_t(*mcp342x_read_fptr_t)(const uint8_t busAddr, uint8_t *data, const uint32_t len);

/*!
 * @brief This function pointer API writes I2C data to the specified
 * device on the bus.
 *
 * @param[in] busAddr: Address of the device to be written to
 * @param[in] *data: Pointer to the data to be written
 * @param[in] len: Length of data to be written
 *
 * @return The result of writing I2C data
 */
typedef int8_t(*mcp342x_write_fptr_t)(const uint8_t busAddr, const uint8_t *data, const uint32_t len);

/*!
 * @brief This function pointer API delays for the specified time in microseconds.
 *
 * @param[in] period: Duration to be delayed in micro-seconds
 */
typedef void(*mcp342x_delay_us_fptr_t)(uint32_t period);

/*!
 * @brief MCP342x Return codes for the driver API
 */
typedef enum {
    MCP342x_RET_OK          = 0,    /* OK */
    MCP342x_RET_ERROR       = -1,   /* Error */
    MCP342x_RET_BUSY        = -2,   /* Interface Busy */
    MCP342x_RET_TIMEOUT     = -3,   /* Timeout */
    MCP342x_RET_INV_PARAM   = -4,   /* Invalid Parameter */
    MCP342x_RET_NULL_PTR    = -5,   /* NULL Pointer */
} mcp342x_return_code_t;

/*!
 * @brief MCP342x ADC Channels
 */
typedef enum {
    MCP342x_CH_1            = 0x00, /* Channel 1 */
    MCP342x_CH_2            = 0x01, /* Channel 2 */
    MCP342x_CH_3            = 0x02, /* Channel 3 */
    MCP342x_CH_4            = 0x03, /* Channel 4 */
    MCP342x_CH__MAX__       = 0x04  /* Channel MAX */
} mcp342x_channel_enum;

/*!
 * @brief MCP342x Conversion Modes
 */
typedef enum {
    MCP342x_CM_ONE_SHOT     = 0x00, /* One Shot Conversion */
    MCP342x_CM_CONT         = 0x01, /* Continuous Conversion */
    MCP342x_CM__MAX__       = 0x02, /* Conversion Mode Max */
} mcp342x_conversion_mode_enum;

/*!
 * @brief MCP342x Sample Rates
 */
typedef enum {
    MCP342x_SR_240SPS       = 0x00, /* 240 Samples Per Second */
    MCP342x_SR_60SPS        = 0x01, /* 60 Samples Per Second */
    MCP342x_SR_15SPS        = 0x02, /* 15 Samples Per Second */
    MCP342x_SR__MAX__       = 0x04, /* Sample Rate Max */
} mcp342x_sample_rate_enum;

/*!
 * @brief MCP342x Gain Multiplier
 */
typedef enum {
    MCP342x_GAIN_x1         = 0x00, /* 1x Gain */
    MCP342x_GAIN_x2         = 0x01, /* 2x Gain */
    MCP342x_GAIN_x4         = 0x02, /* 4x Gain */
    MCP342x_GAIN_x8         = 0x03, /* 8x Gain */
    MCP342x_GAIN__MAX__     = 0x04, /* Gain Max */
} mcp342x_gain_enum;

/*!
 * @brief MCP342x Configuration Register
 */
typedef union
{
    struct
    {
        uint8_t gain        : 2;    /* Gain */
        uint8_t sample_rate : 2;    /* Sample Rate */
        uint8_t conv_mode   : 1;    /* Conversion Mode */
        uint8_t channel     : 2;    /* Channel */
        uint8_t ready       : 1;    /* Ready Bit */
    } bits;
    uint8_t byte;
} mcp342x_config_t;

/*!
 * @brief MCP342x ADC Results
 */
typedef struct
{
    uint16_t outputCode;            /* 16-bit Output Code */
    float voltage;                  /* Sampled Voltage */
} mcp342x_results_t;

/*!
 * @brief MCP342x Device Registers
 */
typedef union
{
    struct
    {
        uint8_t upper_data;         /* Upper byte of the sampled output code */
        uint8_t lower_data;         /* Lower byte of the sampled output code*/
        mcp342x_config_t config;    /* Device configuration register */
    } bits;
    uint8_t bytes[3];
} mcp342x_registers_t;

/*!
 * @brief MCP342x HW Interface
 */
typedef struct {
    uint8_t i2c_addr;                   /* Device I2C Address */
    mcp342x_read_fptr_t read;           /* User I2C Read Function Pointer */
    mcp342x_write_fptr_t write;         /* User I2C Write Function Pointer */
    mcp342x_delay_us_fptr_t delay_us;   /* User Micro-Second Delay Function Pointer */
} mcp342x_dev_intf_t;

/*!
 * @brief MCP342x Device Instance
 */
typedef struct
{
    mcp342x_dev_intf_t intf;                        /* Device Hardware Interface */
    mcp342x_registers_t registers;                  /* Device Registers */
    mcp342x_results_t results[MCP342x_CH__MAX__];   /* Device Conversion Results */
} mcp342x_dev_t;


/*!
 * @brief This API writes the current config to the device
 *
 * @param[in] *dev: Pointer to the device instance containing the i2c
 * interface to use and config to be written.
 *
 * @return The result of writing the config
 */
mcp342x_return_code_t mcp342x_writeConfig(mcp342x_dev_t *dev);

/*!
 * @brief This API samples the specified channel
 *
 * @param[in] *dev: Pointer to the device instance containing the i2c
 * interface to use for sampling
 * @param[in] ch: The channel to be sampled
 *
 * @return The result of sampling the specified channel
 */
mcp342x_return_code_t mcp342x_sampleChannel(mcp342x_dev_t *dev, const mcp342x_channel_enum ch);

#endif // _MCP342X_H_

#ifdef __cplusplus
}
#endif