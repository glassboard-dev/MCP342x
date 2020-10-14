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

#define MCP342x_LSB_VAL (0.0000625)

typedef int8_t(*mcp342x_read_fptr_t)(const uint8_t busAddr, uint8_t *data, const uint32_t len);
typedef int8_t(*mcp342x_write_fptr_t)(const uint8_t busAddr, const uint8_t *data, const uint32_t len);
typedef void(*mcp342x_delay_us_fptr_t)(uint32_t period);

typedef enum {
    MCP342x_RET_OK          = 0,
    MCP342x_RET_ERROR       = -1,
    MCP342x_RET_BUSY        = -2,
    MCP342x_RET_TIMEOUT     = -3,
    MCP342x_RET_INV_PARAM   = -4,
    MCP342x_RET_NULL_PTR    = -5,
} mcp342x_return_code_t;

typedef enum {
    MCP342x_CH_1            = 0b000,
    MCP342x_CH_2            = 0b001,
    MCP342x_CH_3            = 0b010,
    MCP342x_CH_4            = 0b011,
    MCP342x_CH__MAX__       = 0b100
} mcp342x_channel_enum;

typedef enum {
    MCP342x_CM_ONE_SHOT     = 0x00,
    MCP342x_CM_CONT         = 0x01,
    MCP342x_CM__MAX__
} mcp342x_conversion_mode_enum;

typedef enum {
    MCP342x_SR_240SPS       = 0x00,
    MCP342x_SR_60SPS        = 0x01,
    MCP342x_SR_15SPS        = 0x02,
    MCP342x_SR__MAX__
} mcp342x_sample_rate_enum;

typedef enum {
    MCP342x_GAIN_x1         = 0x00,
    MCP342x_GAIN_x2         = 0x01,
    MCP342x_GAIN_x4         = 0x02,
    MCP342x_GAIN_x8         = 0x03,
    MCP342x_GAIN__MAX__
} mcp342x_gain_enum;

typedef union
{
    struct
    {
        uint8_t gain        : 2;
        uint8_t sample_rate : 2;
        uint8_t conv_mode   : 1;
        uint8_t channel     : 2;
        uint8_t ready       : 1;
    } bits;
    uint8_t byte;
} mcp342x_config_t;

typedef struct
{
    uint16_t outputCode;
    float voltage;
} mcp342x_results_t;

typedef union
{
    struct
    {
        uint8_t upper_data;
        uint8_t lower_data;
        mcp342x_config_t config;
    } bits;
    uint8_t bytes[3];
} mcp342x_registers_t;

typedef struct {
    uint8_t i2c_addr;
    mcp342x_read_fptr_t read;
    mcp342x_write_fptr_t write;
    mcp342x_delay_us_fptr_t delay_us;
} mcp342x_dev_intf_t;

typedef struct
{
    mcp342x_dev_intf_t intf;
    mcp342x_registers_t registers;
    mcp342x_results_t results[MCP342x_CH__MAX__];
} mcp342x_dev_t;

mcp342x_return_code_t mcp342x_writeConfig(mcp342x_dev_t *dev);
mcp342x_return_code_t mcp342x_sampleChannel(mcp342x_dev_t *dev, const mcp342x_channel_enum ch);

#endif // _MCP342X_H_

#ifdef __cplusplus
}
#endif