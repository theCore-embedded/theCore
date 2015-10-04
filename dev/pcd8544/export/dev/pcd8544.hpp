#ifndef DEV_PCD8544_HPP
#define DEV_PCD8544_HPP

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


template< class SPI_dev >
class PCD8544
{
public:
    PCD8544();
    ~PCD8544();

    // Lazy initialization, -1 if error. 0 otherwise
    int init();

    // -1 if error, 0 otherwise
    int open();
    // -1 if error, 0 otherwise
    int close();

    // Graphic primitives
    // -1 if error, 0 otherwise
    int set_point(const point& coord);
    int clear_point(const point& coord);

    // Displays a data
    int flush();
    int clear();

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


    int send(uint8_t byte, DC_state op);

    // TODO: move it somewhere
    void _delay()
    {
        for (volatile int i = 0; i < 10000; ++i) {};
    }

    SPI_dev m_device;
    // TODO: magic numbers
    uint8_t m_array[84][6];
};


template< class SPI_dev >
PCD8544< SPI_dev >::PCD8544()
    :m_device{SPI_direction::TX,
              SPI_mode::master,
              SPI_CPOL::high,
              SPI_CPHA::second_edge,
              SPI_NSS_type::SW,
              SPI_bit_order::MSB,
              0}
    ,m_array{0}
{

    PCD8544_CS::set();
    PCD8544_Reset::set();
    PCD8544_Mode::set();
}

template< class SPI_dev >
PCD8544< SPI_dev >::~PCD8544()
{
    // TODO
}

template< class SPI_dev >
int PCD8544< SPI_dev >::init()
{
    int rc = m_device.init();
    // RESET
    PCD8544_Reset::reset();
    _delay();
    PCD8544_Reset::set();

    return rc;
}


template< class SPI_dev >
int PCD8544< SPI_dev >::open()
{

    int rc = m_device.open();
    // found somewhere in net

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

    return rc;
}


template< class SPI_dev >
int PCD8544< SPI_dev >::close()
{
    // TODO
    return m_device.close();
}


template< class SPI_dev >
int PCD8544< SPI_dev >::set_point(const point& coord)
{
    // TODO: checks
    int x = coord.get_x();
    int y = coord.get_y();

    if (x < 0 || y < 0 || x > 83 || y > 47)
        return -1;

    // Calculate a byte offcet
    int y_byte = y >> 3;

    // Calculate a bit offcet inside a byte
    int y_bit = y & 0x7;

    // Set appropriate bit
    m_array[x][y_byte] |= (1 << y_bit);

    return 0;

}

template< class SPI_dev >
int PCD8544< SPI_dev >::clear_point(const point& coord)
{
    // TODO: checks
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

template< class SPI_dev >
int PCD8544< SPI_dev >::flush()
{
    // TODO: improve error check
    for (unsigned i = 0; i < 84; ++i) {
        for (unsigned j = 0; j < 6; ++j) {
            send(m_array[i][j], DC_state::data);
        }
    }

    _delay();
    return 0;
}

template< class SPI_dev >
int PCD8544< SPI_dev >::clear()
{
    // TODO: improve error check
    // clear
    for (unsigned i = 0; i < 84; ++i) {
        for (unsigned j = 0; j < 6; ++j) {
            m_array[i][j] = 0;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
// Private members

template< class SPI_dev >
int PCD8544< SPI_dev >::send(uint8_t byte, DC_state op)
{

    if (op == DC_state::data)
        PCD8544_Mode::set();
    else
        PCD8544_Mode::reset();

    PCD8544_CS::reset();
    int rc = m_device.write(&byte, 1);
    PCD8544_CS::set();

    return rc < 0 ? rc : 0;
}

#endif
