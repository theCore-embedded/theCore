// TODO: consider using better name
#ifndef PLATFORM_pin_DESCR_HPP
#define PLATFORM_pin_DESCR_HPP

#include <common/pin.hpp>
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
static constexpr GPIO_TypeDef *pick_port(pin::port port)
{
    switch (port) {
    case pin::port::port_a:
        return GPIOA;
    case pin::port::port_b:
        return GPIOB;
    case pin::port::port_c:
        return GPIOC;
    case pin::port::port_d:
        return GPIOD;
    case pin::port::port_e:
        return GPIOE;
    case pin::port::port_f:
        return GPIOF;
    case pin::port::port_g:
        return GPIOG;
    case pin::port::port_h:
        return GPIOH;
    case pin::port::port_i:
        return GPIOI;
    case pin::port::port_j:
        return GPIOJ;
    case pin::port::port_k:
        return GPIOK;
    default:
        return nullptr;
    }
}

// TODO: implement, comments
static constexpr uint32_t pick_pin(pin::number pin)
{
    switch (pin) {
    case pin::number::pin_0:
        return GPIO_Pin_0;
    case pin::number::pin_1:
        return GPIO_Pin_1;
    case pin::number::pin_2:
        return GPIO_Pin_2;
    case pin::number::pin_3:
        return GPIO_Pin_3;
    case pin::number::pin_4:
        return GPIO_Pin_4;
    case pin::number::pin_5:
        return GPIO_Pin_5;
    case pin::number::pin_6:
        return GPIO_Pin_6;
    case pin::number::pin_7:
        return GPIO_Pin_7;
    case pin::number::pin_8:
        return GPIO_Pin_8;
    case pin::number::pin_9:
        return GPIO_Pin_9;
    case pin::number::pin_10:
        return GPIO_Pin_10;
    case pin::number::pin_11:
        return GPIO_Pin_11;
    case pin::number::pin_12:
        return GPIO_Pin_12;
    case pin::number::pin_13:
        return GPIO_Pin_13;
    case pin::number::pin_14:
        return GPIO_Pin_14;
    case pin::number::pin_15:
        return GPIO_Pin_15;
    default:
        return (uint32_t) -1;
    }
}

// TODO: implement, comments
static constexpr uint8_t pick_source(pin::number pin)
{
    switch (pin) {
    case pin::number::pin_0:
        return GPIO_PinSource0;
    case pin::number::pin_1:
        return GPIO_PinSource1;
    case pin::number::pin_2:
        return GPIO_PinSource2;
    case pin::number::pin_3:
        return GPIO_PinSource3;
    case pin::number::pin_4:
        return GPIO_PinSource4;
    case pin::number::pin_5:
        return GPIO_PinSource5;
    case pin::number::pin_6:
        return GPIO_PinSource6;
    case pin::number::pin_7:
        return GPIO_PinSource7;
    case pin::number::pin_8:
        return GPIO_PinSource8;
    case pin::number::pin_9:
        return GPIO_PinSource9;
    case pin::number::pin_10:
        return GPIO_PinSource10;
    case pin::number::pin_11:
        return GPIO_PinSource11;
    case pin::number::pin_12:
        return GPIO_PinSource12;
    case pin::number::pin_13:
        return GPIO_PinSource13;
    case pin::number::pin_14:
        return GPIO_PinSource14;
    case pin::number::pin_15:
        return GPIO_PinSource15;
    default:
        return (uint8_t) -1;
    }
}

// TODO: implement, comments
static constexpr GPIOMode_TypeDef pick_mode(pin::function purpose)
{
    switch (purpose) {
    case pin::function::gpio_in:
        return GPIO_Mode_IN;
    case pin::function::gpio_out:
        return GPIO_Mode_OUT;
    case pin::function::analog:
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
static constexpr GPIOOType_TypeDef pick_type(pin::type type)
{
    if (type == pin::type::open_drain) {
        return GPIO_OType_OD;
    } else {
        return GPIO_OType_PP;
    }
}

// TODO: comments
static constexpr GPIOPuPd_TypeDef pick_pp(pin::pp_mode pp)
{
    switch(pp) {
    case pin::pp_mode::no_pull:
        return GPIO_PuPd_NOPULL;
    case pin::pp_mode::pull_down:
        return GPIO_PuPd_DOWN;
    case pin::pp_mode::pull_up:
        return GPIO_PuPd_UP;
    default:
        return (GPIOPuPd_TypeDef) -1;
    }
}

// TODO: implement, comments
static constexpr uint8_t pick_AF(pin::function purpose)
{
    switch (purpose) {
    case pin::function::UART1:
        return GPIO_AF_USART1;
    case pin::function::UART2:
        return GPIO_AF_USART2;
    case pin::function::UART3:
        return GPIO_AF_USART3;
        // TODO: add rest of usarts
    case pin::function::pin_SPI1:
        return GPIO_AF_SPI1;
    case pin::function::pin_SPI2:
        return GPIO_AF_SPI2;
    case pin::function::pin_SPI3:
        return GPIO_AF_SPI3;
    case pin::function::pin_I2C1:
        return GPIO_AF_I2C1;
    case pin::function::pin_I2C2:
        return GPIO_AF_I2C2;
    case pin::function::pin_MCO:
    	return GPIO_AF_MCO;

        // TODO: add rest of SPI
    default:
        return (uint8_t) -1;
    }
}

static constexpr uint32_t pick_periph(pin::port port)
{
    switch (port) {
    case pin::port::port_a:
        return RCC_AHB1Periph_GPIOA;
    case pin::port::port_b:
        return RCC_AHB1Periph_GPIOB;
    case pin::port::port_c:
        return RCC_AHB1Periph_GPIOC;
    case pin::port::port_d:
        return RCC_AHB1Periph_GPIOD;
    case pin::port::port_e:
        return RCC_AHB1Periph_GPIOE;
    default:
        return (uint32_t) -1;
    }
}


// TODO: make it as a constructor
constexpr pin_descr create_pin(pin::port        port,
                               pin::number      pin_num,
                               pin::function    purpose,
                               pin::type        type = pin::type::push_pull,
                               pin::pp_mode     mode = pin::pp_mode::no_pull)
{
    return pin_descr {
        pick_port(port),
                pick_pin(pin_num),
                pick_mode(purpose),
                pick_speed(),
                pick_type(type),
                pick_pp(mode),
                pick_periph(port),
                pick_source(pin_num),
                pick_AF(purpose),

    };
}

#endif // PLATFORM_pin_DESCR_HPP

