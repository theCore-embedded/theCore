//! \file
//! \brief Pin configuraiton helpers.

#ifndef PLATFORM_AUX_TM4C_PIN_CFG_HPP_
#define PLATFORM_AUX_TM4C_PIN_CFG_HPP_

// Missing in TiwaWare headers
#include <cstdint>

#include <gpio.h>
#include <sysctl.h>
#include <inc/hw_memmap.h>

#include <type_traits>

namespace ecl
{

namespace gpio_hw
{

//! GPIO ports.
enum class port
{
    a = GPIO_PORTA_BASE,
    b = GPIO_PORTB_BASE,
    c = GPIO_PORTC_BASE,
    d = GPIO_PORTD_BASE,
    e = GPIO_PORTE_BASE,
    f = GPIO_PORTF_BASE,
};

//! GPIO numbers.
enum class num
{
    pin0 = GPIO_PIN_0,
    pin1 = GPIO_PIN_1,
    pin2 = GPIO_PIN_2,
    pin3 = GPIO_PIN_3,
    pin4 = GPIO_PIN_4,
    pin5 = GPIO_PIN_5,
    pin6 = GPIO_PIN_6,
    pin7 = GPIO_PIN_7,
};

//! GPIO directions.
enum class dir
{
    in  = GPIO_DIR_MODE_IN,
    out = GPIO_DIR_MODE_OUT,
    hw  = GPIO_DIR_MODE_HW,
};

//! GPIO types.
enum class pad_type
{
    push_pull = GPIO_PIN_TYPE_STD,
    pull      = GPIO_PIN_TYPE_STD_WPU,
    push      = GPIO_PIN_TYPE_STD_WPD,
    od        = GPIO_PIN_TYPE_OD,
    analog    = GPIO_PIN_TYPE_ANALOG,
    wake_high = GPIO_PIN_TYPE_WAKE_HIGH,
    wake_low  = GPIO_PIN_TYPE_WAKE_LOW,
};

//! GPIO maximum current rating, in milliamperes.
enum class strength
{
    i2ma    = GPIO_STRENGTH_2MA,
    i4ma    = GPIO_STRENGTH_4MA,
    i6ma    = GPIO_STRENGTH_6MA,
    i8ma    = GPIO_STRENGTH_8MA,
    i8ma_sc = GPIO_STRENGTH_8MA_SC, // 8mA drive with slew rate control
    i10ma   = GPIO_STRENGTH_10MA,
    i12ma   = GPIO_STRENGTH_12MA,
};

//! GPIO Interrupt sources.
enum class int_source
{
    falling     = GPIO_FALLING_EDGE,
    rising      = GPIO_RISING_EDGE,
    both        = GPIO_BOTH_EDGES,
    low         = GPIO_LOW_LEVEL,
    high        = GPIO_HIGH_LEVEL,
    discrete    = GPIO_DISCRETE_INT,
};

//! Gather given pins into one bin, suitable for passing to the TivaWare functions.
template<num Gn, num ...Gns>
struct pin_bin
{
    static constexpr uint8_t
        pins = pin_bin<Gns...>::pins | static_cast<std::underlying_type_t<num>>(Gn);
};

template<num Gn>
struct pin_bin<Gn>
{
    static constexpr uint8_t pins = static_cast<std::underlying_type_t<num>>(Gn);
};

//! Gets peripheral associated with given port.
template<port Pt>
static constexpr auto get_periph()
{
    switch (Pt) {
        case port::a:
            return SYSCTL_PERIPH_GPIOF;
        case port::b:
            return SYSCTL_PERIPH_GPIOB;
        case port::c:
            return SYSCTL_PERIPH_GPIOC;
        case port::d:
            return SYSCTL_PERIPH_GPIOD;
        case port::e:
            return SYSCTL_PERIPH_GPIOE;
        case port::f:
            return SYSCTL_PERIPH_GPIOF;
    }
}

//! Enables peripheral for given port.
template<port Pt>
static inline void enable_periph()
{
    constexpr auto periph = get_periph<Pt>();

    // Enable peripheral
    SysCtlPeripheralEnable(periph);

    // Wait few cycles before peripheral is ready
    while (SysCtlPeripheralReady(periph) == false) { }
}

//! Applies given configuration for given GPIOs.
//! \tparam Pt GPIO port.
//! \tparam P  GPIO pin.
//! \tparam Pn Rest of GPIO pins (optional)
//! \param[in] d Direction of pins.
//! \param[in] t Types of pins.
//! \param[in] s Maximum current of pins.
template<port Pt, num P, num ...Pn>
static inline void apply_cfg(dir d      = dir::out,
                             pad_type t = pad_type::push_pull,
                             strength s = strength::i2ma)
{
    constexpr auto pins = pin_bin<P, Pn...>::pins;
    constexpr auto pt = static_cast<std::underlying_type_t<port>>(Pt);

    GPIODirModeSet(pt, pins, static_cast<std::underlying_type_t<dir>>(d));
    GPIOPadConfigSet(pt, pins,
                     static_cast<std::underlying_type_t<strength>>(s),
                     static_cast<std::underlying_type_t<pad_type>>(t));
}

} // namespace gpio_hw

} // namespace ecl

#endif // PLATFORM_AUX_TM4C_PIN_CFG_HPP_
