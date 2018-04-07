/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ECL_ISTREAM_HPP
#define ECL_ISTREAM_HPP

#include <cstddef>
#include <cstdint>
#include <ctype.h>
#include <limits.h>

namespace ecl
{

template<typename stream>
stream& noskipws(stream &ios)
{
    // TODO: flush stream when appropriate functionality
    // will be ready
    ios.skipws(false);
    return ios;
}

template<typename stream>
stream& skipws(stream &ios)
{
    // TODO: flush stream when appropriate functionality
    // will be ready
    ios.skipws(true);
    return ios;
}

template<class IO_device>
class istream
{
public:
    // Provides type information of the underlying device
    using device_type = IO_device;

    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    istream(IO_device *device);
    ~istream();

    istream &operator>>(int &value);
    istream &operator>>(unsigned int &value);
    istream &operator>>(char &character);
    istream &operator>>(char *string);
    // For I\O manipulators
    istream &operator>>(istream& (*func)(istream< IO_device >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

    // unformatted input
    int get();
    istream &get(char &c);

    // Set whitespace skipping status.
    void skipws(bool state = true);

    // Disabled for now.
    istream &operator=(istream &) = delete;
    istream(const istream &) = delete;

private:
    // Simply, a device driver object
    IO_device *m_device;
    // Skip whitespaces flag
    bool      m_skipws = true;

    /**
     * Removes all leading space characters from the stream. Returns first
     * non-space character via @p next.
     *
     * @retval -1 in case of error
     * @retval 0 all spaces were successfully skipped
     */
    int skip_leading_spaces(char &next);
    int mul_with_overflow(int a, int b);
    int add_with_overflow(int a, int b);
};


//------------------------------------------------------------------------------


template<class IO_device>
istream< IO_device >::istream(IO_device *device)
    :m_device{device}
{
}

template<class IO_device>
istream< IO_device >::~istream()
{
}


template<class IO_device>
istream<IO_device> &istream< IO_device >::operator>>(int &value)
{
    int new_value = 0;
    char in_character;
    int symbol_code;
    bool sign = false; //TRUE is used to indicate negative number
    bool first = true; //TRUE indicates the first round of cycle
    char first_char;

    if (skip_leading_spaces(first_char) != 0) {
        return *this;
    }

    in_character = first_char;

    do {
        symbol_code = in_character;

        if (symbol_code >= '0' && symbol_code <= '9') {
            new_value = add_with_overflow(mul_with_overflow(new_value, 10), symbol_code - '0');
        } else if (symbol_code == '-' && first) {
            sign = true;
        } else if (in_character != '\n') {
            break; //FIXME: add error handling
        }

        if (m_device->read(reinterpret_cast<uint8_t*>(&in_character), 1) <= 0) {
            break;
        }

        first = false;
    } while (!isspace(in_character));

    if (sign) {
        new_value *= -1;
    }

    value = new_value;
    return *this;
}

template<class IO_device>
istream<IO_device> &istream< IO_device >::operator>>(unsigned int &value)
{
    int new_value = 0;
    char in_character;
    int symbol_code;
    char first_char;

    if (skip_leading_spaces(first_char) != 0) {
        return *this;
    }

    in_character = first_char;

    do {
        symbol_code = in_character;
        if (symbol_code >= '0' && symbol_code <= '9') {
            new_value = new_value * 10 + (symbol_code - '0');
        } else if (in_character != '\n') {
            break; //FIXME: add error handling
        }

        if (m_device->read(reinterpret_cast<uint8_t*>(&in_character), 1) <= 0) {
            break; //FIXME: add error handling
        }

    } while (!isspace(in_character));

    value = new_value;
    return *this;
}

template<class IO_device>
istream<IO_device> &istream< IO_device >::operator>>(char &character)
{
    char first_char;

    if (skip_leading_spaces(first_char) != 0) {
        return *this;
    }

    character = first_char;

    return *this;
}

template<class IO_device>
istream<IO_device> &istream< IO_device >::operator>>(char *string)
{
    size_t i = 0;
    char first_char;

    if (skip_leading_spaces(first_char) != 0) {
        string[0] = '\0';
        return *this;
    }

    string[0] = first_char;
    do {
        i++;
        if (m_device->read((uint8_t *) &string[i], 1) <= 0) {
            break; //FIXME: add error handling
        }
    } while (!isspace(string[i]));
    string[i] = '\0';
    return *this;
}


template<class IO_device>
istream<IO_device> &istream< IO_device >::operator>>(
        istream& (*func)(istream< IO_device >&))
{
    return func(*this);
}

template<class IO_device>
int istream< IO_device >::get()
{
    int c = 0;
    if (m_device->read((uint8_t *) &c, 1) <= 0) {
        return -1;
    }

    return c;
}

template<class IO_device>
istream<IO_device> &istream< IO_device >::get(char &character)
{
    m_device->read((uint8_t *) &character, 1);

    return *this;
}

template<class IO_device>
int istream< IO_device >::skip_leading_spaces(char &next)
{
    char c = 0;

    do {
        if (m_device->read((uint8_t *) &c, 1) <= 0) {
            return -1;
        }
    } while (isspace(c) && m_skipws);

    next = c;
    return 0;
}

template<class IO_device>
int istream< IO_device >::mul_with_overflow(int a, int b)
{
    if (a > 0) {
        if (b > 0) {  /* a and b are positive */
            if (a > (INT_MAX / b)) {
                return INT_MAX;
            }
        } else { /* a positive, b non-positive */
            if (b < (INT_MIN / a)) {
                return INT_MIN;
            }
        }
    } else {
        if (b > 0) { /* a is non-positive, b is positive */
            if (a < (INT_MIN / b)) {
                return INT_MIN;
            }
        } else { /* a and b are non-positive */
            if ( (a != 0) && (b < (INT_MAX / a))) {
                return INT_MAX;
            }
        }
    }

    return a * b;
}

template<class IO_device>
int istream< IO_device >::add_with_overflow(int a, int b)
{
    if (b > 0 && a > (INT_MAX - b)) {
        return INT_MAX;
    } else if (b < 0 && a < INT_MIN - b) {
        return INT_MIN;
    }
    return a + b;
}

template<class IO_device>
void istream< IO_device >::skipws(bool state)
{
    m_skipws = state;
}


} // namespace ecl

#endif // ECL_istream_HPP
