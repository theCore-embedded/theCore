// TODO: consider using better name
#ifndef PLATFORM_pin_DESCR_HPP
#define PLATFORM_pin_DESCR_HPP

#include <sys/pin_cfgs.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

// Pin descriptor, used for pinconfig
// TODO: constexpr objects/constructor?
struct pin_descr
{
    // TODO: add APB here
    GPIO_TypeDef             *port;
    const uint32_t           pin;
    const GPIOMode_TypeDef   mode;
    const GPIOSpeed_TypeDef  speed;
    const GPIOOType_TypeDef  type;
    const GPIOPuPd_TypeDef   pupd;
    const uint32_t           ahb;
    const uint8_t            src;
    const uint8_t            af;

    // Inits a pin
    // TODO: move it to the CPP file
    void init() const
    {
        GPIO_InitTypeDef init_struct;

        RCC_AHB1PeriphClockCmd(this->ahb, ENABLE);
        GPIO_StructInit(&init_struct);

        init_struct.GPIO_Pin    = this->pin;
        init_struct.GPIO_Mode   = this->mode;
        init_struct.GPIO_Speed  = this->speed;
        init_struct.GPIO_OType  = this->type;
        init_struct.GPIO_PuPd   = this->pupd;

        GPIO_Init(this->port, &init_struct);

        if (this->mode == GPIO_Mode_AF) {
            GPIO_PinAFConfig(this->port, this->src, this->af);
        }
    }
};

// TODO: implement, comments
static constexpr GPIO_TypeDef *pick_port(const pin_port &port)
{
    switch (port) {
    case pin_port::port_a:
        return GPIOA;
    case pin_port::port_b:
        return GPIOB;
    case pin_port::port_c:
        return GPIOC;
    case pin_port::port_d:
        return GPIOD;
    case pin_port::port_e:
        return GPIOE;
    default:
        return nullptr;
    }
}

// TODO: implement, comments
static constexpr uint32_t pick_pin(const pin_number &pin)
{
    switch (pin) {
    case pin_number::pin_0:
        return GPIO_Pin_0;
    case pin_number::pin_1:
        return GPIO_Pin_1;
    case pin_number::pin_2:
        return GPIO_Pin_2;
    case pin_number::pin_3:
        return GPIO_Pin_3;
    case pin_number::pin_4:
        return GPIO_Pin_4;
    case pin_number::pin_5:
        return GPIO_Pin_5;
    case pin_number::pin_6:
        return GPIO_Pin_6;
    case pin_number::pin_7:
        return GPIO_Pin_7;
    case pin_number::pin_8:
        return GPIO_Pin_8;
    case pin_number::pin_9:
        return GPIO_Pin_9;
    case pin_number::pin_10:
        return GPIO_Pin_10;
    case pin_number::pin_11:
        return GPIO_Pin_11;
    case pin_number::pin_12:
        return GPIO_Pin_12;
    case pin_number::pin_13:
        return GPIO_Pin_13;
    case pin_number::pin_14:
        return GPIO_Pin_14;
    case pin_number::pin_15:
        return GPIO_Pin_15;
    default:
        return (uint32_t) -1;
    }
}

// TODO: implement, comments
static constexpr uint8_t pick_source(const pin_number &pin)
{
    switch (pin) {
    case pin_number::pin_0:
        return GPIO_PinSource0;
    case pin_number::pin_1:
        return GPIO_PinSource1;
    case pin_number::pin_2:
        return GPIO_PinSource2;
    case pin_number::pin_3:
        return GPIO_PinSource3;
    case pin_number::pin_4:
        return GPIO_PinSource4;
    case pin_number::pin_5:
        return GPIO_PinSource5;
    case pin_number::pin_6:
        return GPIO_PinSource6;
    case pin_number::pin_7:
        return GPIO_PinSource7;
    case pin_number::pin_8:
        return GPIO_PinSource8;
    case pin_number::pin_9:
        return GPIO_PinSource9;
    case pin_number::pin_10:
        return GPIO_PinSource10;
    case pin_number::pin_11:
        return GPIO_PinSource11;
    case pin_number::pin_12:
        return GPIO_PinSource12;
    case pin_number::pin_13:
        return GPIO_PinSource13;
    case pin_number::pin_14:
        return GPIO_PinSource14;
    case pin_number::pin_15:
        return GPIO_PinSource15;
    default:
        return (uint8_t) -1;
    }
}

// TODO: implement, comments
static constexpr GPIOMode_TypeDef pick_mode(const pin_function &purpose)
{
    switch (purpose) {
    case pin_function::GPIO_in:
        return GPIO_Mode_IN;
    case pin_function::GPIO_out:
        return GPIO_Mode_OUT;
    case pin_function::analog:
        return GPIO_Mode_AN;
    default:
        return GPIO_Mode_AF;
    }

}

// TODO: implement, comments
static constexpr GPIOSpeed_TypeDef pick_speed()
{
    return GPIO_Medium_Speed;
}

// TODO: implement, comments
static constexpr GPIOOType_TypeDef pick_type(const pin_type &type)
{
    if (type == pin_type::open_drain) {
        return GPIO_OType_OD;
    } else {
        return GPIO_OType_PP;
    }
}

// TODO: implement, comments
static constexpr GPIOPuPd_TypeDef pick_pull_type(const pin_type &type)
{
    switch (type) {
    case pin_type::pull_down:
        return GPIO_PuPd_DOWN;
    case pin_type::pull_up:
        return GPIO_PuPd_UP;
    default:
        return GPIO_PuPd_NOPULL;
    }
}

// TODO: implement, comments
static constexpr uint8_t pick_AF(const pin_function &purpose)
{
    switch (purpose) {
    case pin_function::UART1:
        return GPIO_AF_USART1;
    case pin_function::UART2:
        return GPIO_AF_USART2;
    case pin_function::UART3:
        return GPIO_AF_USART3;
        // TODO: add rest of usarts
    case pin_function::pin_SPI1:
        return GPIO_AF_SPI1;
    case pin_function::pin_SPI2:
        return GPIO_AF_SPI2;
    case pin_function::pin_SPI3:
        return GPIO_AF_SPI3;
        // TODO: add rest of SPI
    default:
        return (uint8_t) -1;
    }
}

static constexpr uint32_t pick_periph(const pin_port &port)
{
    switch (port) {
    case pin_port::port_a:
        return RCC_AHB1Periph_GPIOA;
    case pin_port::port_b:
        return RCC_AHB1Periph_GPIOB;
    case pin_port::port_c:
        return RCC_AHB1Periph_GPIOC;
    case pin_port::port_d:
        return RCC_AHB1Periph_GPIOD;
    case pin_port::port_e:
        return RCC_AHB1Periph_GPIOE;
    default:
        return (uint32_t) -1;
    }
}

// TODO: make it as a constructor
constexpr pin_descr create_pin(const pin_port &port,
                             const pin_number &pin_num,
                             const pin_function &purpose,
                             const pin_type &type
                             )
{
    return pin_descr {
        pick_port(port),
                pick_pin(pin_num),
                pick_mode(purpose),
                pick_speed(),
                pick_type(type),
                pick_pull_type(type),
                pick_periph(port),
                pick_source(pin_num),
                pick_AF(purpose),
    };
}

#endif // PLATFORM_pin_DESCR_HPP

