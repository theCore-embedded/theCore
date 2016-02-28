#ifndef DEV_BUS_BUS_HPP_
#define DEV_BUS_BUS_HPP_

#include <ecl/err.hpp>
#include <functional>

namespace ecl
{

template< typename Bus >
class generic_bus
{
public:
    enum class event
    {
        tx_complete  = Bus::tx,
        rx_complete  = Bus::rx,
        tx_error     = Bus::tx_error,
        rx_error     = Bus::rx_error,
    };

    using handler = std::function< void(void *ctx, event type) >;

    generic_bus();
    ~generic_bus();

    void lock();
    void unlock();

    err set_buffers(const uint8_t *tx, uint8_t *rx, size_t size);
    err set_buffers(size_t count, uint8_t fill_byte = 0xff);

    err xfer();
    err xfer(const handler &event_handler);

private:
    static void bus_handler(void *ctx, typename Bus::event type);
};

}

#endif
