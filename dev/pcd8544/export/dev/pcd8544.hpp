#ifndef DEV_PCD8544_HPP
#define DEV_PCD8544_HPP

#include <ecl/thread/semaphore.hpp>
#include <os/utils.hpp>

#include <utility>

namespace ecl
{

// TODO: move it somewhere
class point
{
public:
    point() :point{0, 0} { }
    point(int x, int y) :m_coords(x, y) { }

    int get_x() const { return m_coords.first; }
    int get_y() const { return m_coords.second; }

    void set_x(int x) { m_coords.first = x; }
    void set_y(int y) { m_coords.second = y; }

private:
    std::pair< int, int > m_coords;
};


// TODO: add GPIO type here, instead of using special names for SPI CS
// and LCD D/C
template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
class pcd8544
{
public:
    pcd8544();
    ~pcd8544();

    // Lazy initialization
    // err::ok if succeed
    err init();

    // err::ok if succeed.
    err open();
    // err::ok if succeed.
    err close();

    // Graphic primitive operations.
    // err::ok if succeed
    err set_point(const point& coord);
    err clear_point(const point& coord);

    // Sends VRAM data to a display
    // err::ok if succeed
    err flush();
    err clear();

private:
    // Rows as they are represented in a device
    static constexpr uint8_t cols   = 84;
    static constexpr uint8_t rows   = 6;

    // Logical boundaries
    static constexpr uint8_t max_X  = 84;
    static constexpr uint8_t max_Y  = 48;

    // D/C values
    enum class DC_state
    {
        command = 0,
        data    = 1
    };

    // Instruction set, H = 0, D/C = 0

    // No operation
    static constexpr uint8_t NOP                = 0b00000000;

    // Function set opcodes:

    // Function set prefix
    static constexpr uint8_t FS_prefix          = 0b00100000;
    // Function set power down bit
    static constexpr uint8_t FS_PD_on           = 0b00000100;
    // Function set vertical addressing mode enable
    // Clearing this bit will result in horisontal mode
    static constexpr uint8_t FS_V_on            = 0b00000010;
    // Function set extended instruction set mode enable
    // Clearing this bit will result in basic instruction set
    static constexpr uint8_t FS_H_on            = 0b00000001;

    // "Write data" instruction does not require a mask,
    // all 8 bits are used. D/C = 1 is required to send data

    // Instruction set, H = 0, D/C = 0

    // Display control opcodes:

    // Display blank
    static constexpr uint8_t display_blank      = 0b00001000;
    // Normal mode
    static constexpr uint8_t normal_mode        = 0b00001100;
    // All segments 'on'
    static constexpr uint8_t all_on             = 0b00001001;
    // Inverse mode
    static constexpr uint8_t inverse_mode       = 0b00001101;

    // Instruction set, H = 1, D/C = 0

    // Temperature control opcodes:

    // Temperature control prefix
    static constexpr uint8_t TC_prefix          = 0b00000100;
    // Temperature coefficient mask
    static constexpr uint8_t TC_mask            = 0b00000011;

    // Bias system opcodes:

    // Bias system prefix
    static constexpr uint8_t BS_prefix          = 0b00010000;
    // Bias system mask
    static constexpr uint8_t BS_mask            = 0b00000111;

    // VOP opcodes:

    // VOP prefix
    static constexpr uint8_t VOP_prefix         = 0b10000000;
    // VOP mask
    static constexpr uint8_t VOP_mask           = 0b01111111;

    // Blocking send
    err send(uint8_t byte, DC_state op);

    // Bulk data write
    err internal_flush();

    uint8_t                 m_array[84][6];    // TODO: magic numbers
    ecl::binary_semaphore   m_sem;             // Handle bus events
};


template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::pcd8544()
    :m_array{0}
    ,m_sem{}
{

    Cs_gpio::set();
    Rst_gpio::set();
    Mode_gpio::set();
}

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::~pcd8544()
{
    // TODO
}

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::init()
{
    Spi::lock();

    auto rc = Spi::init();
    if (is_error(rc)) {
        Spi::unlock();
        return rc;
    }

    // RESET
    Cs_gpio::set();
    Rst_gpio::reset();

    ecl::os::this_thread::sleep_for(1000);

    Rst_gpio::set();
    Spi::unlock();

    return err::ok;
}


template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::open()
{
    // Found somewhere in the net

    // Extended set on
    send(FS_prefix | FS_H_on, DC_state::command);

    // Setup VOP
    send(VOP_prefix | (VOP_mask & 0x40), DC_state::command);

    // Setup temperature control
    send(TC_prefix | 0x2, DC_state::command);

    //  Set bias voltage
    send(BS_prefix | (BS_mask & 0x3), DC_state::command);

    // Vertical addressing on, back to the basic instruction set
    send(FS_prefix | FS_V_on, DC_state::command);

    // Set normal display mode
    send(normal_mode, DC_state::command);

    return err::ok;
}


template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::close()
{
    // TODO
    return err::ok;
}


template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::set_point(const point& coord)
{
    // TODO: checks
    // TODO: common parts (see clear_point())
    int x = coord.get_x();
    int y = coord.get_y();

    if (x < 0 || y < 0 || x > 83 || y > 47)
        return err::inval;

    // Calculate a byte offcet
    int y_byte = y >> 3;

    // Calculate a bit offcet inside a byte
    int y_bit = y & 0x7;

    // Set appropriate bit
    m_array[x][y_byte] |= (1 << y_bit);

    return err::ok;
}

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::clear_point(const point& coord)
{
    // TODO: checks
    // TODO: common parts (see set_point())
    int x = coord.get_x();
    int y = coord.get_y();

    if (x < 0 || y < 0 || x > 83 || y > 47)
        return -1;

    // Calculate a byte offcet
    int y_byte = y >> 3;

    // Calculate a bit offcet inside a byte
    int y_bit = y & 0x7;

    // Clear appropriate bit
    m_array[x][y_byte] &= ~(1 << y_bit);

    return 0;
}

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::flush()
{
    return internal_flush();
}

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::clear()
{
    // TODO: improve error check
    // TODO: memset
    for (unsigned i = 0; i < 84; ++i) {
        for (unsigned j = 0; j < 6; ++j) {
            m_array[i][j] = 0;
        }
    }

    return err::ok;
}

//------------------------------------------------------------------------------
// Private members

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::send(uint8_t byte, DC_state op)
{
    Spi::lock();

    if (op == DC_state::data)
        Mode_gpio::set();
    else
        Mode_gpio::reset();

    Cs_gpio::reset();

    Spi::set_buffers(&byte, nullptr, 1);
    Spi::xfer();

    Cs_gpio::set();

    Spi::unlock();

    return err::ok;
}

template< class Spi, class Cs_gpio, class Mode_gpio, class Rst_gpio >
err pcd8544< Spi, Cs_gpio, Mode_gpio, Rst_gpio >::internal_flush()
{
    Spi::lock();

    Mode_gpio::set();
    Cs_gpio::reset();

    auto tx = reinterpret_cast< uint8_t* > (m_array);
    Spi::set_buffers(tx, nullptr, sizeof(m_array));
    Spi::xfer();

    Spi::unlock();

    // Release the device
    Cs_gpio::set();
    Mode_gpio::reset();

    return err::ok;
}

} // namespace ecl

#endif
