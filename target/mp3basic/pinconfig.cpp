#include <platform/pin_descriptor.hpp>
#include <cstddef>
#include "target/pinconfig.hpp"

// TODO: mopve it to the target.cpp / target.h module
void initialize_pins()
{
    constexpr pin_descr to_init[] = {
        // Console
        // USART3 TX
        create_pin(pin_port::port_d, pin_number::pin_8, pin_function::UART3, pin_type::no_pull),
        // USART3 RX
        create_pin(pin_port::port_d, pin_number::pin_9, pin_function::UART3, pin_type::no_pull),

        // Debug LEDs
        // Orange LD3
        create_pin(pin_port::port_d, pin_number::pin_13, pin_function::GPIO_out, pin_type::no_pull),
        // Green LD3
        create_pin(pin_port::port_d, pin_number::pin_12, pin_function::GPIO_out, pin_type::no_pull),
        // Red LD3
        create_pin(pin_port::port_d, pin_number::pin_14, pin_function::GPIO_out, pin_type::no_pull),
        // Blue LD3
        create_pin(pin_port::port_d, pin_number::pin_15, pin_function::GPIO_out, pin_type::no_pull),

        // SPI2 SCK
        create_pin(pin_port::port_b, pin_number::pin_13, pin_function::pin_SPI2, pin_type::no_pull),
        // SPI2 MISO
        create_pin(pin_port::port_b, pin_number::pin_14, pin_function::pin_SPI2, pin_type::no_pull),
        // SPI2 MOSI
        create_pin(pin_port::port_b, pin_number::pin_15, pin_function::pin_SPI2, pin_type::no_pull),

        // Nokia5110 PCD8544 CS
        // SPI2 NSS TODO: use SPI_NSSInternalSoftwareConfig for slave management?
        //createPin(pin_port::port_b, pin_number::pin_12, pin_function::pin_SPI2, pin_type::no_pull),
        create_pin(pin_port::port_b, pin_number::pin_12, pin_function::GPIO_out, pin_type::no_pull),

        // SDSPI card CS TODO: double check pull\push
        create_pin(pin_port::port_c, pin_number::pin_8, pin_function::GPIO_out, pin_type::no_pull),

        // RESET pin
        create_pin(pin_port::port_c, pin_number::pin_1, pin_function::GPIO_out, pin_type::no_pull),
        // Data/Instruction selection GPIO
        create_pin(pin_port::port_c, pin_number::pin_2, pin_function::GPIO_out, pin_type::no_pull),
        // Light pin
        // NOT USED. Diodes will shine if LIGHT pin connected to the ground
        // createPin(pin_port::port_c, pin_number::pin_4, pin_function::GPIO_out, pin_type::no_pull),
    };

    for (const auto& pin : to_init) {
        pin.init();
    }
}
