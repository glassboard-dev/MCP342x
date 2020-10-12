#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "MCP3428.h"

#define MCP3428_ADDRESS_BASE        (0x6E)
#define MCP3428_ADDRESS_WRITE       (MCP3428_ADDRESS_BASE << 1)
#define MCP3428_ADDRESS_READ        ((MCP3428_ADDRESS_BASE << 1) | 0x01)
#define MCP3428_VREF                (2.048)
#define MCP3428_LSB_VAL             (0.0000625)
#define MCP3428_SAMPLE_TIMEOUT_MS   (100)

// Thermistor equation values
#define A_val   (0.0008972439213)
#define B_val   (0.0002500990711)
#define C_val   (0.0000001961752076)
#define VREF    (3.3)
#define RTOP    (30100)


static bool _MCP3428_writeConfig(str_MCP3428_config configByte);
static bool _MCP3428_sampleChannel(enum_MCP3428_channel ch);

static MCP3428_InfoTypeDef *MCP3428_Info;
static str_MCP3428_config MCP3428_DEFAULT_CONFIG =
{
    .bits.GAIN          = 0b00,
    .bits.SAMPLE_RATE   = 0b10,
    .bits.CONV_MODE     = 0b00,
    .bits.CHANNEL       = 0b00,
    .bits.READY         = 0b01
};

static bool _MCP3428_writeConfig(str_MCP3428_config configByte)
{
    bool status = true;

    // Transmit our config byte to the device
    if( HAL_OK != HAL_I2C_Master_Transmit(MCP3428_Info->hi2c, MCP3428_ADDRESS_WRITE, (uint8_t*)&configByte, 0x01, 100) )
    {
        status = false;
    }

    return status;
}

static bool _MCP3428_sampleChannel(enum_MCP3428_channel ch)
{
    bool status = true;
    uint16_t outputCode = 0x0000;
    uint32_t timeoutRef = 0x00;

    // Make sure the RDY bit is set to 1 on the device.. (It will go to a zero when a conversion has been completed)
    MCP3428_Info->registers.bits.CONFIG.bits.READY = 0b1;
    // Set the desired channel in our config struct
    MCP3428_Info->registers.bits.CONFIG.bits.CHANNEL = ch;
    // Clear out the samples from the previous scan
    MCP3428_Info->registers.bits.UPPER_DATA = 0x0000;
    MCP3428_Info->registers.bits.LOWER_DATA = 0x0000;

    // Write the config to the device
    if( HAL_OK == HAL_I2C_Master_Transmit(MCP3428_Info->hi2c, MCP3428_ADDRESS_WRITE, (uint8_t *)&MCP3428_Info->registers.bits.CONFIG.byte, 0x01, 10) )
    {
        // Store the current ticks and use it as a reference when
        // calculating a timeout for reading an ADC sample
        timeoutRef = HAL_GetTick();

        // No problem transmitting. Now read data from the device until the READY bit changes to a zero
        while( MCP3428_Info->registers.bits.CONFIG.bits.READY != 0b0 )
        {
            if( HAL_OK != HAL_I2C_Master_Receive(MCP3428_Info->hi2c, MCP3428_ADDRESS_READ, (uint8_t *)MCP3428_Info->registers.bytes, 0x04, 10) )
            {
                // Error receiving data. Return false
                status = false;
                // Break out of the while loop. We had an error when receiving the data
                break;
            }

            if( (timeoutRef + MCP3428_SAMPLE_TIMEOUT_MS) < HAL_GetTick() )
            {
                // We've waited long enough for a conversion to complete. Return false
                status = false;
                break;
            }
        }

        // We completed a conversion on that channel. Store the voltage, and converted temp values
        if( status )
        {
            // Aggregate the upper and lower data into a single u16 output code
            outputCode = MCP3428_Info->registers.bits.UPPER_DATA << 8 | MCP3428_Info->registers.bits.LOWER_DATA;
            // Compute the voltage value
            MCP3428_Info->results[ch].voltage = outputCode * MCP3428_LSB_VAL;
            // Compute the actual temp value
            MCP3428_Info->results[ch].temp = 1/ (A_val +
                                                (B_val * log( (MCP3428_Info->results[ch].voltage * RTOP) / (VREF - MCP3428_Info->results[ch].voltage) ) ) +
                                                (C_val *
                                                pow(log( (MCP3428_Info->results[ch].voltage * RTOP) / (VREF - MCP3428_Info->results[ch].voltage) ), 3)));
            // Calculated value is in Kelvin - subtract 273
            MCP3428_Info->results[ch].temp -= 273;
            // Convert to fahrenheit
            MCP3428_Info->results[ch].temp = ((MCP3428_Info->results[ch].temp * 9) / 5) + 32;
        }
    }
    else
    {
        status = false;
    }

    return status;
}

bool MCP3428_Init(MCP3428_InfoTypeDef *mcp3428, I2C_HandleTypeDef *hi2c)
{
    bool status = true;

    // Store the reference for our I2C module
    mcp3428->hi2c = hi2c;

    // Give our high level app state struct reference to all of the DRV8703 info
    MCP3428_Info = mcp3428;

    // Copy our default config into our config struct
    memcpy(&MCP3428_Info->registers.bits.CONFIG.byte, &MCP3428_DEFAULT_CONFIG, sizeof(MCP3428_DEFAULT_CONFIG));

    // Write our default config to the device
    status = _MCP3428_writeConfig(MCP3428_Info->registers.bits.CONFIG);

    return status;
}

bool MCP3428_sampleAllChannels(void)
{
    bool status = true;

    // Run through all of the channels on the ADC and sample them
    for(uint8_t i = MCP3428_CH__MAX__; i > 0; i--)
    {
        if( false == _MCP3428_sampleChannel(i - 1) )
        {
            status = false;
            break;
        }
    }

    return status;
}