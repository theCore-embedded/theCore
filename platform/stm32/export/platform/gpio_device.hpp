//! \file
//! \brief GPIO device interface for STM32 platform
//! \todo Detailed explanation.
#ifndef PLATFORM_GPIO_DEVICE_HPP
#define PLATFORM_GPIO_DEVICE_HPP

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

//! Encapsulates pin usage.
//! \tparam Port GPIO port on STM32 device.
//! \tparam Pin  Pin within given port.
template< gpio_port Port, gpio_num Pin >
class gpio
{
public:
    static constexpr auto port = Port;
    static constexpr auto pin = Pin;

    //! Sets output pin level to high.
    static void set();

    //! Sets output pin level to low.
    static void reset();

    //! Toggles pin level.
    static void toggle();

    //! Gets level on a pin.
    //! \return True if level is high, false if level is low.
    static bool get();

private:
    //! \brief Returns STM32 representation of the port.
    //! \todo extract it to the stm32 gpio wrap header.
    static constexpr auto pick_port();

    //! \brief Returns STM32 representation of the pin.
    //! \todo extract it to the stm32 gpio wrap header.
    static constexpr auto pick_pin();
};

//------------------------------------------------------------------------------

template< gpio_port Port, gpio_num Pin >
void gpio< Port, Pin >::set()
{
    constexpr auto hw_port = pick_port();
    constexpr auto hw_pin  = pick_pin();

    GPIO_WriteBit(hw_port, hw_pin, Bit_SET);
}

template< gpio_port Port, gpio_num Pin >
void gpio< Port, Pin >::reset()
{
    constexpr auto hw_port = pick_port();
    constexpr auto hw_pin  = pick_pin();

    GPIO_WriteBit(hw_port, hw_pin, Bit_RESET);
}

template< gpio_port Port, gpio_num Pin >
void gpio< Port, Pin >::toggle()
{
    constexpr auto hw_port = pick_port();
    constexpr auto hw_pin  = pick_pin();

    GPIO_ToggleBits(hw_port, hw_pin);
}

template< gpio_port Port, gpio_num Pin >
bool gpio< Port, Pin >::get()
{
    constexpr auto hw_port = pick_port();
    constexpr auto hw_pin  = pick_pin();

    return GPIO_ReadInputDataBit(hw_port, hw_pin) == Bit_SET;
}

//------------------------------------------------------------------------------

template< gpio_port Port, gpio_num Pin >
constexpr auto gpio< Port, Pin >::pick_port()
{
    switch (Port) {
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
#ifdef GPIOI
        case gpio_port::i:
            return GPIOI;
#endif
#ifdef GPIOJ
        case gpio_port::j:
            return GPIOJ;
#endif
#ifdef GPIOK
        case gpio_port::k:
            return GPIOK;
#endif
    }
}

template< gpio_port Port, gpio_num Pin >
constexpr auto gpio< Port, Pin >::pick_pin()
{
    switch (Pin) {
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
    }
}

} // namespace ecl

#endif
