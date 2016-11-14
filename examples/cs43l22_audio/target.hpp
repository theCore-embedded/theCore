// Header contains target-specific interfaces.

#ifndef EXAMPLE_EXTI_TARGET_HPP_
#define EXAMPLE_EXTI_TARGET_HPP_

#include <platform/gpio_device.hpp>
#include <aux/spi_bus.hpp>
#include <aux/i2c_bus.hpp>
#include <dev/cs43l22.hpp>

namespace ecl
{

// PD4 - Reset pin for cs43l22 codec.
using rst_gpio = gpio<gpio_port::d, gpio_num::pin4>;

// I2S3 - I2S for cs43l22 codec.

template<>
struct spi_i2s_cfg<spi_device::bus3>
{
    static constexpr auto bus_type = spi_bus_type::i2s;

    using dma_tx = ecl::dma_wrap<ecl::dma_stream::dma1_5, ecl::dma_channel::ch2>;
    using dma_rx = ecl::dma_wrap<ecl::dma_stream::dma1_2, ecl::dma_channel::ch2>;

    static constexpr I2S_InitTypeDef init_obj = {
        I2S_Mode_MasterTx,
        I2S_Standard_Phillips,
        I2S_DataFormat_16b,
        I2S_MCLKOutput_Enable,
        I2S_AudioFreq_8k,
        I2S_CPOL_High,
    };
};

using i2s3 = spi_i2s_bus<spi_device::bus3>;

// I2C1 - I2C for cs43l22 codec.

using i2c1_cfg = i2c_config<
    i2c_device::bus1,
    i2c_mode::POLL,
    10000, // 10 kHz
    I2C_Mode_I2C,
    I2C_DutyCycle_16_9,
    0x1f,
    I2C_Ack_Enable,
    I2C_AcknowledgedAddress_7bit
    >;

using i2c1 = i2c_bus<i2c1_cfg>;

// Codec itself

using cs43l22_instance = cs43l22<i2c1, i2s3, rst_gpio>;

} // namespace ecl

// Initializes pins
void gpio_init();

#endif // EXAMPLE_EXTI_UART_TARGET_HPP_
