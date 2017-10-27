/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ECL_OSTREAM_HPP
#define ECL_OSTREAM_HPP

#include <stdio.h>
#include <ecl/assert.h>

namespace ecl 
{  
   
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
    // Provides type information of the underlying device
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

    // Disabled for now.
    ostream &operator=(ostream &) = delete;
    ostream(const ostream &) = delete;

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
    char buff[32];
    
    int n =  snprintf(buff, sizeof(buff), "%d", value);
    
    ecl_assert(n > 0 && n < static_cast<int>(sizeof(buff)));
    
    m_device->write(reinterpret_cast<uint8_t*>(buff), n);
    
    return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<< (unsigned int value)
{
    
   char buff[32];
   
   int n = snprintf(buff, sizeof(buff), "%u", value);
   
   ecl_assert(n > 0 && n < static_cast<int>(sizeof(buff)));

   m_device->write(reinterpret_cast<uint8_t*>(buff), n);
       
   return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<<(char character)
{
    this->put(character);
    
    return *this;
}


template< class IO_device >
ostream<IO_device> &ostream< IO_device >::operator<<(const char *string)
{
    // TODO: improve this function in a way it can write a string
    // to a device with chunks that are split by '\n' symbol
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

    m_device->write((uint8_t*)&c, 1);
    
    return *this;
}

}

#endif // ECL_OSTREAM_HPP
