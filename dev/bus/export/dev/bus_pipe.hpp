#ifndef DEV_BUS_DEV_BUS_PIPE_HPP_
#define DEV_BUS_DEV_BUS_PIPE_HPP_

//!
//! \file
//! \brief      Bus pipe module.
//! \copyright
//! \todo Description and examples
//!

#include <ecl/err.hpp>

namespace ecl
{

//!
//! \brief Bus pipe (blocking) adapter.
//! This adapter provides functionality of a blocking pipe
//! built over generic bus interface.
//! \tparam GBus Generic bus driver.
//! \sa generic_bus
//!
template< class GBus >
class bus_pipe
{
public:
    //!
    //! \brief Constructs a pipe.
    //!
    bus_pipe();
    //!
    //! \brief Destructs a pipe.
    //!
    ~bus_pipe();

    //!
    //! \brief Inits a pipe.
    //! Lazy initialization. Inis a bus.
    //! \return Status of operation.
    //!
    ecl::err init();

    //!
    //! \brief Writes a data to a pipe.
    //! If error is occurred before transfer has started then this method
    //! will immediately exit and return negative integer value to notify
    //! about error.
    //! If error occur during transaction then return value will be less than
    //! bytes requested or zero, indicating amount of bytes that were
    //! successfully written to a pipe.
    //! Last error can be obtained via \sa last_error() call
    //! Caller will be blocked until all bytes will be transfered
    //! or error will occur.
    //! \pre Bus is initialized.
    //! \param[in]  data     Data to write. Must not be null.
    //! \param[in]  count    Count of bytes that must be written to a pipe.
    //!                      Can be zero.
    //! \return     Value indicating either error if negative or
    //!             amount of bytes transferred if positive.
    ssize_t write(const uint8_t *data, size_t count);

    //!
    //! \brief Reads a data from a pipe.
    //! If error is occurred before transfer has started then this method
    //! will immediately exit and return negative integer value to notify
    //! about error.
    //! If error occur during transaction or there is less data than request
    //! then return value will be less than size of provided buffer,
    //! indicating amount of bytes that were successfully read from a pipe.
    //! Last error can be obtained via \sa last_error() call.
    //! Caller will be blocked until all bytes will be written to a buffer
    //! or error will occur.
    //! \pre Bus is initialized and buffer is valid.
    //! \param[out] buffer     Data buffer to read to. Must not be null.
    //! \param[in]  count      Size of a buffer. Can be zero.
    //! \return     Value indicating an error if negative or bytes stored
    //!             to a buffer if positive.
    //!
    ssize_t read(uint8_t *buffer, size_t size);

    //!
    //! \brief Gets last error occurred.
    //! \return Error defined in ecl::err
    //!
    ecl::err last_error() const;

private:
    err  m_last;    //!< Last bus error.

    //!
    //! \brief Checks if error occur before xfer has started
    //! \retval true Error occurs on start
    //! \retval false Error hasn't occur at all, or occurred during xfer
    //!
    bool err_on_start() const;
};


template< class GBus >
bus_pipe< GBus >::bus_pipe()
{

}

template< class GBus >
bus_pipe< GBus >::~bus_pipe()
{

}

template< class GBus >
ecl::err bus_pipe< GBus >::init()
{
    return (m_last = GBus::init());
}

template< class GBus >
ssize_t bus_pipe< GBus >::write(const uint8_t *data, size_t count)
{
    ecl_assert(data);

    if (!count)
        return count;

    size_t sent = 0;
    ssize_t ret;

    GBus::lock();
    GBus::set_buffers(data, nullptr, count);

    m_last = GBus::xfer(&sent);

    GBus::unlock();

    ret = sent;

    if (err_on_start()) {
        // Error occur, right at the start
        ret = -1;
    }

    return ret;
}

template< class GBus >
ssize_t bus_pipe< GBus >::read(uint8_t *buffer, size_t size)
{
    ecl_assert(buffer);

    if (!size)
        return size;

    size_t read = 0;
    ssize_t ret;

    GBus::lock();
    GBus::set_buffers(nullptr, buffer, size);

    m_last = GBus::xfer(nullptr, &read);

    GBus::unlock();

    ret = read;

    if (err_on_start()) {
        // Error occur, right at the start
        ret = -1;
    }

    return ret;
}

template< class GBus >
err bus_pipe< GBus >::last_error() const
{
    return m_last;
}

//------------------------------------------------------------------------------


template< class GBus >
bool bus_pipe< GBus >::err_on_start() const
{
    return is_error(m_last) && !(m_last == err::io);
}


}

#endif // DEV_BUS_DEV_BUS_PIPE_HPP_
