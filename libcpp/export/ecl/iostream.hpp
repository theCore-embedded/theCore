#ifndef IOSTREAM_HPP
#define IOSTREAM_HPP

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>


// ecl == [E]mbedded [C]++ [L]ibrary
namespace ecl {

template< typename stream >
stream& endl(stream &ios)
{
    ios.put('\n');
    ios.put('\r');

    // TODO: flush stream when appropriate functionality
    // will be ready
}

// TODO: concepts must be used when they will be ready
// TODO: think about inheritance of IO devices
// TODO: think about inheritance of IO streams


// NOTE: this are
template< class IODevice >
class istream
{
public:
    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    istream(IODevice *device){};
    // TODO: when device driver will support isOpen()
    // and isReady() opearations  then the constructor must
    // check if device is opened\initialized already.
    // If it is not opened or initialized then class
    // must prepare device driver for use

    // Formatted input functions, similar to ones that defined in STL

    istream& operator>>(int& value)
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

    istream& operator>>(unsigned int& value)
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

    istream& operator>>(char& character)
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

    istream& operator>>(char *string) // char * != uint8_t *
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


    // For I\O manipulators
    istream& operator>>(istream& (*func)(istream< IODevice >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

private:
    // Simply, a device driver object
    IODevice *m_device;

    // NOTE: any fields that are essential
    // for this class, must be added to this class
};

template< class IODevice >
class ostream
{
public:
    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    ostream(IODevice *device){};
    // TODO: when device driver will support isOpen()
    // and isReady() opearations  then the constructor must
    // check if device is opened\initialized already.
    // If it is not opened or initialized then class
    // must prepare device driver for use


    ostream& operator<<(int value)
    {
        int higher_multiplicand = 1;
        int out_digit = 0;
        int val2 = value;
        char out_character;

        while(val2 / 10 != 0) {
            higher_multiplicand *= 10;
            val2 /= 10;
        }

        if (value < 0) {
            m_device->write((uint8_t *) "-", 1);
            value *= -1;
        }

        for (int i = 1; i <= higher_multiplicand; i *= 10) {
            out_digit = value * i / higher_multiplicand;
            out_character = out_digit + 48;

            if (m_device->write((uint8_t *) &out_character, 1) < 0) {
                break; //FIXME: add error handling
            }

            value = value - out_digit * higher_multiplicand / i;
        }

        return *this;
    }

    ostream& operator<<(unsigned int value)
    {
        int higher_multiplicand = 1;
        int out_digit = 0;
        int val2 = value;
        char out_character;

        while(val2 / 10 != 0) {
            higher_multiplicand *= 10;
            val2 /= 10;
        }

        for (int i = 1; i <= higher_multiplicand; i *= 10) {
            out_digit = value * i / higher_multiplicand;
            out_character = out_digit + 48;

            if (m_device->write((uint8_t *) &out_character, 1) < 0) {
                break; //FIXME: add error handling
            }

            value = value - out_digit * higher_multiplicand / i;
        }

        return *this;
    }

    ostream& operator<<(char &character)
    {
        if(m_device->write((uint8_t *) &character,1) < 0) {
            *this; //FIXME: add error handling
        }

        return *this;
    }

    ostream& operator<<(char *string)
    {
        size_t i = 0;
        while(string[i] != 0) {
            if(m_device->write((uint8_t *) &string[i],1) < 0) {
                break; //FIXME: add error handling
            }

            i++;
        }
        return *this;
    }

    // For I\O manipulators
    ostream& operator<<(ostream& (*func)(ostream< IODevice >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

    // Puts a single character
    ostream& put(char c)
    {
        m_device->write(&c, 1);
        return *this;
    }


private:
    // Simply, a device driver object
    IODevice *m_device;

    // NOTE: any fields that are essential
    // for this class, must be added to this class
};


template< class IODevice >
class iostream
{
public:
    // Initializes a stream with given device
    // NOTE: device must be initialized and opened already
    iostream(IODevice *device)
        :m_in(device)
        ,m_out(device)
    {};
    // TODO: when device driver will support isOpen()
    // and isReady() opearations  then the constructor must
    // check if device is opened\initialized already.
    // If it is not opened or initialized then class
    // must prepare device driver for use


    // Formatted input functions, similar to ones that defined in STL
    iostream& operator<<(int value)
    {
        m_out << value;
        return *this;
    }

    iostream& operator<<(unsigned int value)
    {
        m_out << value;
        return *this;
    }

    iostream& operator<<(char character)
    {
        m_out << character;
        return *this;
    }

    iostream& operator<<(char *string)
    {
        m_out << string;
        return *this;
    }

    // For I\O manipulators
    iostream& operator<<(iostream& (*func)(iostream< IODevice >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

    // Puts a single character
    iostream& put(char c);


    iostream& operator>>(int& value)
    {
        m_in >> value;
        return *this;
    }

    iostream& operator>>(unsigned int& value)
    {
        m_in >> value;
        return *this;
    }

    iostream& operator>>(char& character)
    {
        m_in >> character;
        return *this;
    }

    iostream& operator>>(char *string)
    {
        m_in >> string;
        return *this;
    }

    // For I\O manipulators
    iostream& operator>>(iostream& (*func)(iostream< IODevice >&));
    // NOTE: this will be used later with different manipulators,
    // such that used for hex or octal output of integers

private:
    // No need to implement methods again,
    // if they are already present
    istream< IODevice > m_in;
    ostream< IODevice > m_out;

    // NOTE: any fields that are essential
    // for this class, must be added to this class
};

}
#endif // IOSTREAM_HPP
