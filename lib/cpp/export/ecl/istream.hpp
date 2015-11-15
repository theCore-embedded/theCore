#ifndef ECL_ISTREAM_HPP
#define ECL_ISTREAM_HPP

namespace ecl {
template< class IO_device >
class istream
{
public:
    // Provides type information of the underlaying device
    using device_type = IO_device;

    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    istream(IO_device *device);
    ~istream();

    istream& operator>>(int& value);
    istream& operator>>(unsigned int& value);
    istream& operator>>(char& character);
    istream& operator>>(char *string);
    // For I\O manipulators
    istream& operator>>(istream& (*func)(istream< IO_device >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

private:
    // Simply, a device driver object
    IO_device *m_device;
};


//------------------------------------------------------------------------------


template< class IO_device >
istream< IO_device >::istream(IO_device *device)
    :m_device{device}
{
}

template< class IO_device >
istream< IO_device >::~istream()
{
}


template< class IO_device >
istream<IO_device>& istream< IO_device >::operator>>(int& value)
{
    value = 0;
    char in_character;
    char in_char_for_spaces;
    int symbol_code;
    bool sign = false; //TRUE is used to indicate negative number
    bool first = true; //TRUE indicates the first round of cycle

    do{
        if(m_device->read((uint8_t*) &in_char_for_spaces, 1) < 0) {
            break;
        }
    } while(in_char_for_spaces == ' ');

    in_character = in_char_for_spaces;

    do{
        symbol_code = in_character;
        if(symbol_code >= 48 && symbol_code <= 58) {

            value = value * 10 + (symbol_code - 48);
        }

        else if(symbol_code == 45 && first == true) {
            sign = true;
            first = false;
        }

        else if (in_character != '\n') {
            break; //FIXME: add error handling
        }

        if(m_device->read((uint8_t*) &in_character, 1) < 0) {
            break;
        }
    } while(in_character != '\n' && in_character != ' ');

    if(sign == true) {
        value *= -1;
    }

    return *this;
}

template< class IO_device >
istream<IO_device>& istream< IO_device >::operator>>(unsigned int& value)
{
    value = 0;
    char in_character;
    char in_char_for_spaces;
    int symbol_code;

    do{
        if(m_device->read((uint8_t*) &in_char_for_spaces, 1) < 0) {
            break;
        }
    } while(in_char_for_spaces == ' ');

    in_character = in_char_for_spaces;

    do{
        symbol_code = in_character;
        if(symbol_code >= 48 && symbol_code <= 58) {

            value = value * 10 + (symbol_code - 48);
        }

        else if (in_character != '\n') {
            break; //FIXME: add error handling
        }

        if(m_device->read((uint8_t*) &in_character, 1) < 0) {
            break; //FIXME: add error handling
        }

    } while(in_character != '\n' && in_character != ' ');

    return *this;
}

template< class IO_device >
istream<IO_device>& istream< IO_device >::operator>>(char& character)
{
    char in_char_for_spaces;

    do{
        if(m_device->read((uint8_t*) &in_char_for_spaces, 1) < 0) {
            break;
        }
    } while(in_char_for_spaces == ' ');

    character = in_char_for_spaces;

    return *this;
}

template< class IO_device >
istream<IO_device>& istream< IO_device >::operator>>(char *string)
{
    size_t i = 1;
    char in_char_for_spaces;

    do{
        if(m_device->read((uint8_t*) &in_char_for_spaces, 1) < 0) {
            break;
        }
    } while(in_char_for_spaces == ' ');

    string[0] = in_char_for_spaces;

    do {
        if(m_device->read((uint8_t *) &string[i], 1) < 0) {
            break; //FIXME: add error handling
        }
    }
    while(string[i] != '\n' && string[i++] != ' ');
    return *this;
}


template< class IO_device >
istream<IO_device>& istream< IO_device >::operator>>(
        istream& (*func)(istream< IO_device >&))
{
    return func(*this);
}



}

#endif // ECL_istream_HPP
