#ifndef ECL_OSTREAM_HPP
#define ECL_OSTREAM_HPP

namespace ecl {

template< typename stream >
stream& endl(stream &ios)
{
    ios.put('\n');
    ios.put('\r');

    // TODO: flush stream when appropriate functionality
    // will be ready
    return ios;
}


template< class IO_device >
class ostream
{
public:
    // Provides type information of the underlaying device
    using device_type = IO_device;

    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    ostream(IO_device *device);
    ~ostream();

    ostream &operator<<(int value);
    ostream &operator<<(unsigned int value);
    ostream &operator<<(char character);
    ostream &operator<<(const char *string);

    // For I\O manipulators
    ostream &operator<<(ostream& (*func)(ostream< IO_device >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

    // Puts a single character
    ostream &put(char c);

private:
    // Simply, a device driver object
    IO_device *m_device;
};


//------------------------------------------------------------------------------


template< class IO_device >
ostream< IO_device >::ostream(IO_device *device)
    :m_device{device}
{
}

template< class IO_device >
ostream< IO_device >::~ostream()
{
}

template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<< (int value)
{
    unsigned int higher_multiplicand = 1;
    unsigned int out_digit = 0;
    int val2 = value;
    char out_character;

    while (val2 / 10 != 0) {
        higher_multiplicand *= 10;
        val2 /= 10;
    }

    int delim = higher_multiplicand;

    if (value < 0) {
        m_device->write((uint8_t *) "-", 1);
        value *= -1;
        delim *= -1;
    }

    for (unsigned int i = 1; i <= higher_multiplicand; i *= 10) {
        out_digit = value / delim;
        out_character = out_digit + 48;

        if (m_device->write((uint8_t *) &out_character, 1) < 0) {
            break; //FIXME: add error handling
        }

        value = value - out_digit * delim;
        delim /= 10;
    }

    return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<< (unsigned int value)
{
    unsigned int higher_multiplicand = 1;
    unsigned int out_digit = 0;
    unsigned int val2 = value;
    char out_character;

    while (val2 / 10 != 0) {
        higher_multiplicand *= 10;
        val2 /= 10;
    }

    int delim = higher_multiplicand;

    for (unsigned int i = 1; i <= higher_multiplicand; i *= 10) {
        out_digit = value / delim;
        out_character = out_digit + 48;

        if (m_device->write((uint8_t *) &out_character, 1) < 0) {
            break; //FIXME: add error handling
        }

        value = value - out_digit * delim;
        delim /= 10;
    }

    return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<<(char character)
{
    if (m_device->write((uint8_t *) &character, 1) < 0) {
        return *this; //FIXME: add error handling
    }

    return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<<(const char *string)
{
    // TODO: improve this function in a way it can write a string
    // to a device with chunks that are splitted by '\n' symbol
    // What about somehow move it to the platform layer?

    size_t i = 0;
    while (string[i] != '\0') {
        if (string[i] == '\n') {
            uint8_t carret_ret = '\r';
            m_device->write(&carret_ret, 1);
        }

        if (m_device->write((const uint8_t *) &string[i], 1) < 0) {
            break; //FIXME: add error handling
        }

        i++;
    }
    return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<<(
        ostream& (*func)(ostream< IO_device >&))
{
    return func(*this);
}

template< class IO_device >
ostream<IO_device> &ostream< IO_device >::put(char c)
{
    m_device->write((uint8_t *)&c, 1);
    return *this;
}

}

#endif // ECL_OSTREAM_HPP
