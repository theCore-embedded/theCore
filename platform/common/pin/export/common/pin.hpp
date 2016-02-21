#ifndef COMMON_PIN_HPP_
#define COMMON_PIN_HPP_

// Pin purpose
enum class pin_function
{
    // Basic purposes
    GPIO_in,
    GPIO_out,
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
};

// Pin numbers
enum class pin_number
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
enum class pin_port
{
    port_a,
    port_b,
    port_c,
    port_d,
    port_e,
    port_f,
};

// Type of pin
enum class pin_type
{
    no_pull,
    floating,
    pull_up,
    pull_down,
    open_drain
};

#endif // COMMON_PIN_HPP_
