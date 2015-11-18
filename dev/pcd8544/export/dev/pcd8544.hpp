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


// TODO: add GPIO type here, instead of using special names for SPI CS
// and LCD D/C
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

    // Convinient aliases
    using SPI_f_t                  = typename SPI_dev::s_t;
    using DMA_f_t                  = typename SPI_dev::DMA_TX_t::s_t;
    using DMA_t                    = typename SPI_dev::DMA_TX_t;
    static constexpr auto SPI_mode = SPI_dev::com_type;

    // DMA mode init
    template< SPI_com_type mode = SPI_mode,
              typename std::enable_if< mode == SPI_com_type::DMA, int >::type = 0 >
    int internal_mode_init();

    // IRQ mode init
    template< SPI_com_type mode = SPI_mode,
              typename std::enable_if< mode == SPI_com_type::IRQ, int >::type = 0 >
    int internal_mode_init();

    // Blocking send in DMA mode
    template< SPI_com_type mode = SPI_mode,
              typename std::enable_if< mode == SPI_com_type::DMA, int >::type = 0 >
    int send(uint8_t byte, DC_state op);

    // Blocking send in IRQ mode
    template< SPI_com_type mode = SPI_mode,
              typename std::enable_if< mode == SPI_com_type::IRQ, int >::type = 0 >
    int send(uint8_t byte, DC_state op);

    // Bulk data write in DMA mode
    template< SPI_com_type mode = SPI_mode,
              typename std::enable_if< mode == SPI_com_type::DMA, int >::type = 0 >
    int internal_flush();

    // Bulk data write in IRQ mode
    template< SPI_com_type mode = SPI_mode,
              typename std::enable_if< mode == SPI_com_type::IRQ, int >::type = 0 >
    int internal_flush();

    // IRQ hander TODO: enable if
    void IRQ_handler(SPI_f_t status);

    // DMA hander TODO: enable if
    void DMA_handler(DMA_f_t status);

    // TODO: move it somewhere
    void _delay()
    {
        for (volatile int i = 0; i < 10000; ++i) {};
    }

    SPI_dev             m_device;
    volatile SPI_f_t    m_status;
    volatile DMA_f_t    m_DMA_status;
    DMA_t               m_DMA;      // ?
    uint8_t             m_array[84][6];    // TODO: magic numbers
};


template< class SPI_dev >
PCD8544< SPI_dev >::PCD8544()
    :m_device{}
    ,m_status{0}
    ,m_DMA_status{0}
    ,m_DMA{}
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
    if (rc < 0)
        return rc;

    m_device.lock();
    // RESET
    PCD8544_CS::set();
    PCD8544_Reset::reset();
    _delay();
    PCD8544_Reset::set();
    m_device.unlock();

    return internal_mode_init();
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
    if ((m_device.get_status() & SPI_dev::flags::BSY)) {
        // Device not ready, buffer under processing
        return -2;
    }

    // TODO: checks
    // TODO: common parts (see clear_point())
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
    if ((m_device.get_status() & SPI_dev::flags::BSY)) {
        // Device not ready, buffer under processing
        return -2;
    }

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

template< class SPI_dev >
int PCD8544< SPI_dev >::flush()
{
    auto status = m_device.get_status();

    if (!(status & SPI_dev::flags::TX_RDY) || (status & SPI_dev::flags::BSY)) {
        // Device not ready
        return -2;
    }

    return internal_flush();
}

template< class SPI_dev >
int PCD8544< SPI_dev >::clear()
{
    if (m_device.get_status() & SPI_dev::flags::BSY) {
        // Device not ready
        return -2;
    }

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
template< SPI_com_type mode,
          typename std::enable_if < mode == SPI_com_type::IRQ, int >::type >
int PCD8544< SPI_dev >::internal_mode_init()
{
    // Catch all IRQs

    // Avoid using std::bind since it usses dynamic memory
    auto handler = [this]() {
        this->IRQ_handler(this->m_device.get_status());
    };

    m_device.mask_IRQ();
    m_device.register_IRQ(handler);

    return 0;
}

template< class SPI_dev >
template< SPI_com_type mode,
          typename std::enable_if < mode == SPI_com_type::DMA, int >::type >
int PCD8544< SPI_dev >::internal_mode_init()
{
    return 0;
}

template< class SPI_dev >
template< SPI_com_type mode,
          typename std::enable_if < mode == SPI_com_type::IRQ, int >::type >
int PCD8544< SPI_dev >::send(uint8_t byte, DC_state op)
{
    m_device.lock();

    if (op == DC_state::data)
        PCD8544_Mode::set();
    else
        PCD8544_Mode::reset();

    PCD8544_CS::reset();

    m_device.mask_IRQ();
    m_status = 0;
    m_device.unmask_IRQ();

    // In order to complete send, we need to wait until device will be ready
    while (!(m_status & SPI_dev::flags::TX_RDY)) {}

    int rc = m_device.write(&byte, 1);

    PCD8544_CS::set();

    m_device.unlock();

    return rc < 0 ? rc : 0;
}

template< class SPI_dev >
template< SPI_com_type mode,
          typename std::enable_if < mode == SPI_com_type::DMA, int >::type >
int PCD8544< SPI_dev >::send(uint8_t byte, DC_state op)
{
    m_device.lock();

    if (op == DC_state::data)
        PCD8544_Mode::set();
    else
        PCD8544_Mode::reset();

    PCD8544_CS::reset();

    auto handler = [this]() {
        this->DMA_handler(this->m_DMA.get_status());
    };

    m_DMA.enable_IRQ(handler, DMA_t::flags::TC);

    m_DMA.set_origin(DMA_t::role::memory,
                     &byte,
                     sizeof(byte));

    int rc = m_device.write(m_DMA);

    // Wait for transaction to be over
    while (!(m_DMA_status & DMA_t::flags::TC)) {}

    // Request completed
    m_DMA.complete_IRQ(m_DMA_status);
    m_DMA.complete();

    m_DMA_status = 0;

    PCD8544_CS::set();

    m_device.unlock();

    return rc < 0 ? rc : 0;
}

template< class SPI_dev >
template< SPI_com_type mode,
          typename std::enable_if < mode == SPI_com_type::IRQ, int >::type >
int PCD8544< SPI_dev >::internal_flush()
{
    // TODO: improve error check
    // TODO: improve remainder calculation. What if buffer
    // was not consumed entirely? May be, catch interrupt and then
    // send rest of the data.

    int ret = 0;

    for (unsigned i = 0; i < 84; ++i) {
        for (unsigned j = 0; j < 6; ++j) {
            ret = send(m_array[i][j], DC_state::data);
            if (ret < 0)
                return ret;
        }
    }

    _delay();
    return ret;
}

template< class SPI_dev >
template< SPI_com_type mode,
          typename std::enable_if < mode == SPI_com_type::DMA, int >::type >
int PCD8544< SPI_dev >::internal_flush()
{
    m_device.lock();

    PCD8544_Mode::set();
    PCD8544_CS::reset();

    auto handler = [this]() {
        this->DMA_handler(this->m_DMA.get_status());
    };

    m_DMA.enable_IRQ(handler, DMA_t::flags::TC);

    m_DMA.set_origin(DMA_t::role::memory,
                     reinterpret_cast< uint8_t* > (m_array),
                     sizeof(m_array));


    m_device.write(m_DMA);

    // Wait for transaction to be over
    while (!(m_DMA_status & DMA_t::flags::TC)) {}
    while ((m_device.get_status() & SPI_dev::flags::BSY)) { }

    m_DMA.complete();

    // Release the device
    PCD8544_CS::set();
    PCD8544_Mode::reset();

    m_DMA_status = 0;
    m_device.unlock();

    return 0;
}

template< class SPI_dev >
void PCD8544< SPI_dev >::IRQ_handler(SPI_f_t status)
{
    // Do nothing special for now
    m_status = status;
    return;
}

template< class SPI_dev >
void PCD8544< SPI_dev >::DMA_handler(DMA_f_t status)
{
    // Do nothing special for now
    m_DMA_status |= status;
    m_DMA.complete_IRQ(status);
    return;
}

#endif
