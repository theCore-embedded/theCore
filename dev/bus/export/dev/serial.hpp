//! \file
//! \brief Serial interface for platform drivers with listen mode support.
//! \todo Find better place (and probably name?) for current header and class.
#ifndef DEV_BUS_SERIAL_HPP_
#define DEV_BUS_SERIAL_HPP_

#include <ecl/err.hpp>

namespace ecl
{

//! Serial driver interface.
//! \details The serial allows to abstract async,
//! interrupt-driven nature of platform-level drivers and
//! provide synchronus, buffered data management interface.
//! \tparam PBus Exclusevely owned platform bus.
template<class PBus>
class serial
{
public:
    //! Initialize serial driver and underlying platform bus.
    //! \pre Driver is not initialized.
    //! \return Operation status.
    err init();

    //! Obtains a byte from serial device.
    //! \pre Driver is initialized.
    //! \details Call will block if there is no data to return.
    //! Otherwise data will be stored in the given argument and
    //! method will return.
    //! \param[out] byte Place for incoming byte. Will remain
    //!                  unchanged if error occur.
    //! \return Operation status.
    //!
    err recv_byte(uint8_t &byte);

    //! Gets buffer from serial device.
    //! \pre Driver is initialized.
    //! \details Call will block if there is no data to return.
    //! Otherwise, buffer will be populated with avaliable data
    //! and method will return.
    //! \param[out]     buf Buffer to fill with data.
    //! \param[in,out]  sz  Size of buffer. Will be updated with
    //!                     amount of bytes written to the buffer.
    //!                     In case of error, size will be updated
    //!                     with bytes successfully stored in the buffer
    //!                     before error occur.
    //! \return Operation status.
    err recv_buf(uint8_t *buf, size_t &sz);

    //! Sends byte to a serial device.
    //! \pre Driver is initialized.
    //! \details It may not block if internal buffering is applied.
    //! It may also block if platform device is not yet ready to
    //! send data.
    //! \param[in] byte Byte to send.
    //! \return Operation status.
    err send_byte(uint8_t byte);


    //! Sends buffer to a serial stream.
    //! \pre Driver is initialized.
    //! \details It may not block if internal buffering is applied.
    //! It may also block if platform device is not yet ready to
    //! send data.
    //! \param[in]      buf Buffer to send.
    //! \param[in,out]  sz  Size of buffer. Will be updated with
    //!                     amount of bytes sent In case of error,
    //!                     size will be updated with amount of bytes
    //!                     successfully sent before error occur.
    //! \return Operation status.
    err send_buf(const uint8_t *buf, size_t &sz)
};

} // namespace ecl

#endif // DEV_BUS_SERIAL_HPP_
