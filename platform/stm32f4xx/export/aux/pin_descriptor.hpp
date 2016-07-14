// TODO: consider using better name
#ifndef PLATFORM_pin_DESCR_HPP
#define PLATFORM_pin_DESCR_HPP

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

namespace ecl
{

// GPIO numbers
enum class gpio_num
{
    pin0,
    pin1,
    pin2,
    pin3,
    pin4,
    pin5,
    pin6,
    pin7,
    pin8,
    pin9,
    pin10,
    pin11,
    pin12,
    pin13,
    pin14,
    pin15,
};

// GPIO ports
enum class gpio_port
{
    a,
    b,
    c,
    d,
    e,
    f,
    g,
    h,
    i,
    j,
    k
};

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
    spi1,
    spi2,
    spi3,
    spi4,
    spi5,
    spi6,
    i2c1,
    i2c2,
    i2c3,
    uart1,
    uart2,
    uart3,
    mc0,
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

} // namespace pin

// Pin descriptor, used for pinconfig
// TODO: constexpr objects/constructor?
struct pin_descr
{
    // TODO: add APB here
    GPIO_TypeDef *port;
    const uint32_t pin;
    const GPIOMode_TypeDef mode;
    const GPIOSpeed_TypeDef speed;
    const GPIOOType_TypeDef type;
    const GPIOPuPd_TypeDef pupd;
    const uint32_t ahb;
    const uint8_t src;
    const uint8_t af;

    // Inits a pin
    // TODO: move it to the CPP file
    void init() const
    {
        GPIO_InitTypeDef init_struct;

        RCC_AHB1PeriphClockCmd(this->ahb, ENABLE);
        GPIO_StructInit(&init_struct);

        init_struct.GPIO_Pin = this->pin;
        init_struct.GPIO_Mode = this->mode;
        init_struct.GPIO_Speed = this->speed;
        init_struct.GPIO_OType = this->type;
        init_struct.GPIO_PuPd = this->pupd;

        GPIO_Init(this->port, &init_struct);

        if (this->mode == GPIO_Mode_AF) {
            GPIO_PinAFConfig(this->port, this->src, this->af);
        }
    }
};

// TODO: implement, comments
static constexpr GPIO_TypeDef *pick_port(gpio_port port)
{
    switch (port) {
        case gpio_port::a:
            return GPIOA;
        case gpio_port::b:
            return GPIOB;
        case gpio_port::c:
            return GPIOC;
        case gpio_port::d:
            return GPIOD;
        case gpio_port::e:
            return GPIOE;
        case gpio_port::f:
            return GPIOF;
        case gpio_port::g:
            return GPIOG;
        case gpio_port::h:
            return GPIOH;
        case gpio_port::i:
            return GPIOI;
        case gpio_port::j:
            return GPIOJ;
        case gpio_port::k:
            return GPIOK;
        default:
            return nullptr;
    }
}

// TODO: implement, comments
static constexpr uint32_t pick_pin(gpio_num pin)
{
    switch (pin) {
        case gpio_num::pin0:
            return GPIO_Pin_0;
        case gpio_num::pin1:
            return GPIO_Pin_1;
        case gpio_num::pin2:
            return GPIO_Pin_2;
        case gpio_num::pin3:
            return GPIO_Pin_3;
        case gpio_num::pin4:
            return GPIO_Pin_4;
        case gpio_num::pin5:
            return GPIO_Pin_5;
        case gpio_num::pin6:
            return GPIO_Pin_6;
        case gpio_num::pin7:
            return GPIO_Pin_7;
        case gpio_num::pin8:
            return GPIO_Pin_8;
        case gpio_num::pin9:
            return GPIO_Pin_9;
        case gpio_num::pin10:
            return GPIO_Pin_10;
        case gpio_num::pin11:
            return GPIO_Pin_11;
        case gpio_num::pin12:
            return GPIO_Pin_12;
        case gpio_num::pin13:
            return GPIO_Pin_13;
        case gpio_num::pin14:
            return GPIO_Pin_14;
        case gpio_num::pin15:
            return GPIO_Pin_15;
        default:
            return (uint32_t) - 1;
    }
}

// TODO: implement, comments
static constexpr uint8_t pick_source(gpio_num pin)
{
    switch (pin) {
        case gpio_num::pin0:
            return GPIO_PinSource0;
        case gpio_num::pin1:
            return GPIO_PinSource1;
        case gpio_num::pin2:
            return GPIO_PinSource2;
        case gpio_num::pin3:
            return GPIO_PinSource3;
        case gpio_num::pin4:
            return GPIO_PinSource4;
        case gpio_num::pin5:
            return GPIO_PinSource5;
        case gpio_num::pin6:
            return GPIO_PinSource6;
        case gpio_num::pin7:
            return GPIO_PinSource7;
        case gpio_num::pin8:
            return GPIO_PinSource8;
        case gpio_num::pin9:
            return GPIO_PinSource9;
        case gpio_num::pin10:
            return GPIO_PinSource10;
        case gpio_num::pin11:
            return GPIO_PinSource11;
        case gpio_num::pin12:
            return GPIO_PinSource12;
        case gpio_num::pin13:
            return GPIO_PinSource13;
        case gpio_num::pin14:
            return GPIO_PinSource14;
        case gpio_num::pin15:
            return GPIO_PinSource15;
        default:
            return (uint8_t) - 1;
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
    switch (pp) {
        case pin::pp_mode::no_pull:
            return GPIO_PuPd_NOPULL;
        case pin::pp_mode::pull_down:
            return GPIO_PuPd_DOWN;
        case pin::pp_mode::pull_up:
            return GPIO_PuPd_UP;
        default:
            return (GPIOPuPd_TypeDef) - 1;
    }
}

// TODO: implement, comments
static constexpr uint8_t pick_AF(pin::function purpose)
{
    switch (purpose) {
        case pin::function::uart1:
            return GPIO_AF_USART1;
        case pin::function::uart2:
            return GPIO_AF_USART2;
        case pin::function::uart3:
            return GPIO_AF_USART3;
            // TODO: add rest of usarts
        case pin::function::spi1:
            return GPIO_AF_SPI1;
        case pin::function::spi2:
            return GPIO_AF_SPI2;
        case pin::function::spi3:
            return GPIO_AF_SPI3;
        case pin::function::i2c1:
            return GPIO_AF_I2C1;
        case pin::function::i2c2:
            return GPIO_AF_I2C2;
        case pin::function::i2c3:
            return GPIO_AF_I2C3;
        case pin::function::mc0:
            return GPIO_AF_MCO;

            // TODO: add rest of SPI
        default:
            return (uint8_t) - 1;
    }
}

static constexpr uint32_t pick_periph(gpio_port port)
{
    switch (port) {
        case gpio_port::a:
            return RCC_AHB1Periph_GPIOA;
        case gpio_port::b:
            return RCC_AHB1Periph_GPIOB;
        case gpio_port::c:
            return RCC_AHB1Periph_GPIOC;
        case gpio_port::d:
            return RCC_AHB1Periph_GPIOD;
        case gpio_port::e:
            return RCC_AHB1Periph_GPIOE;
        default:
            return (uint32_t) - 1;
    }
}


// TODO: make it as a constructor
constexpr pin_descr create_pin(gpio_port port,
                               gpio_num pin_num,
                               pin::function purpose,
                               pin::type type = pin::type::push_pull,
                               pin::pp_mode mode = pin::pp_mode::no_pull)
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

} // namespace ecl

#endif // PLATFORM_pin_DESCR_HPP

