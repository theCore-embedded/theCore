#ifndef COMMON_PIN_HPP_
#define COMMON_PIN_HPP_

namespace pin
{

// Pin purpose
enum class function
{
    // Basic purposes
    gpio_in,
    gpio_out,
    analog,

    // Alternate function purposes
    pin_SPI1,
    pin_SPI2,
    pin_SPI3,
    pin_I2C1,
    pin_I2C2,
    UART1,
    UART2,
    UART3,
    pin_MCO,
};

// Pin numbers
enum class number
{
    pin_0,
    pin_1,
    pin_2,
    pin_3,
    pin_4,
    pin_5,
    pin_6,
    pin_7,
    pin_8,
    pin_9,
    pin_10,
    pin_11,
    pin_12,
    pin_13,
    pin_14,
    pin_15,
};

// Pin ports
enum class port
{
    port_a,
    port_b,
    port_c,
    port_d,
    port_e,
    port_f,
};

// Type of pin
enum class type
{
    push_pull,
    open_drain,
};

// Push/pull modes
enum class pp_mode
{
    no_pull,
    pull_down,
    pull_up,
};

}

#endif // COMMON_PIN_HPP_
