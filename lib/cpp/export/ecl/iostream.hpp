#ifndef ECL_IOSTREAM_HPP
#define ECL_IOSTREAM_HPP

#include "istream.hpp"
#include "ostream.hpp"

#include <console_driver.hpp>

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

// ecl == [E]mbedded [C]++ [L]ibrary
namespace ecl {

template< class IO_device >
class iostream
{
public:
    // Provides type information of the underlaying device
    using device_type = IO_device;

    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    iostream(IO_device *device);
    ~iostream();

    // Formatted input functions, similar to ones that defined in STL
    iostream& operator<<(int value);
    iostream& operator<<(unsigned int value);
    iostream& operator<<(char character);
    iostream& operator<<(char *string);
    // For I\O manipulators
    iostream& operator<<(iostream& (*func)(iostream< IO_device >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

    // Puts a single character
    iostream& put(char c);
    iostream& operator>>(int& value);
    iostream& operator>>(unsigned int& value);
    iostream& operator>>(char& character);
    iostream& operator>>(char *string);

    // For I\O manipulators
    iostream& operator>>(iostream& (*func)(iostream< IO_device >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

private:
    // No need to implement methods again,
    // if they are already present
    istream< IO_device > m_in;
    ostream< IO_device > m_out;

    // NOTE: any fields that are essential
    // for this class, must be added to this class
};

//!
//! \brief Static initializer for every translation unit.
//! \details [Nifty counter idiom](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter)
//! is used here.
//!
static struct iostream_initializer
{
    iostream_initializer();
    ~iostream_initializer();
} stream_initializer;

// Standard streams, defined elsewhere
// These streams rely on specific driver, which name should be
// the same accross all targets
extern istream< console_driver > &cin;
extern ostream< console_driver > &cout;
extern ostream< console_driver > &cerr;


//------------------------------------------------------------------------------


template< class IO_device >
iostream< IO_device >::iostream(IO_device *device)
    :m_in{device}
    ,m_out{device}
{
}

template< class IO_device >
iostream< IO_device >::~iostream()
{
}

template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator<< (int value)
{
    m_out << value;
    return *this;
}


template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator<< (unsigned int value)
{
    m_out << value;
    return *this;
}


template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator<<(char character)
{
    m_out << character;
    return *this;
}


template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator<<(char *string)
{
    m_out << string;
    return *this;
}


template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator<<(
        iostream& (*func)(iostream< IO_device >&))
{
    m_out << func;
    return *this;
}

template< class IO_device >
iostream<IO_device>& iostream< IO_device >::put(char c)
{
    m_out.put(c);
    return *this;
}

template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator>>(int& value)
{
    m_in >> value;
    return *this;
}

template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator>>(unsigned int& value)
{
    m_in >> value;
    return *this;
}

template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator>>(char& character)
{
    m_in >> character;
    return *this;
}

template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator>>(char *string)
{
    m_in >> string;
    return *this;
}


template< class IO_device >
iostream<IO_device>& iostream< IO_device >::operator>>(
        iostream& (*func)(iostream< IO_device >&))
{
    m_in >> func;
    return *this;
}




}
#endif // ECL_IOSTREAM_HPP
