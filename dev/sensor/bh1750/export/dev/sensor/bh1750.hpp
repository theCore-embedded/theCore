//!
//! \file
//! \brief BH1750 Digital 16bit Serial Output Type Ambient Light Sensor IC
//!  See, https://e-radionica.com/productdata/BH1750FVI.pdf
//!

#ifndef DEV_SENSOR_BH1750_HPP_
#define DEV_SENSOR_BH1750_HPP_

#include <ecl/err.hpp>
#include <ecl/thread/thread.hpp>

namespace ecl
{

namespace sensor
{

//! \brief This namespace contains sensor specific parameters
//!
namespace bh1750_cfg
{

//! \brief Represents I2C slave address for BH1750 sensor.
//!  According to RM, it is possible to select 2 type of I2C slave-address.
//!  It depends on ADDR pin state: pulled-down to GND - low, pulled-up to VCC - high
//!
enum class i2c_address
{
    low,
    high,
};

//! \brief Represents resolution mode BH1750 sensor.
//!  According to RM, there are 3 resolution modes:
//!  L-mode (4lx), H-mode (1lx) and H-mode2 (0.5lx)
//!
enum class resolution
{
    low,
    high,
    high2
};

}; // bh1750_cfg


//! \brief BH1750 Ambient Light sensor driver implementation.
//! \tparam i2c_dev I2C generic bus driver
//! \tparam address I2C slave address.
//!  This parameter can be value of type bh1750_cfg::i2c_address
//!
template <class i2c_dev, bh1750_cfg::i2c_address address>
class bh1750
{
public:
    bh1750() = delete;
    ~bh1750() = delete;

    //! \brief Inits sensor and underlying I2C platform bus.
    //! \retval Status of operation.
    //!
    static ecl::err init();

    //! \brief Sets sensor resolution mode. Default mode is bh1750_cfg::high.
    //! \param[in] resolution Resolution mode.
    //!  This parameter can be value of type bh1750_cfg::resolution.
    //!
    static void set_resolution(bh1750_cfg::resolution resolution);

    //! \brief Returns current sensor resolution mode.
    //! \retval Current resolution mode.
    //!  The return value can be value of type bh1750_cfg::resolution.
    //!
    static bh1750_cfg::resolution get_resolution();

    //! \brief Reads raw illuminance sample from sensor.
    //! \details This method blocks until measurement is not finished.
    //!  Measurement time depends on resolution mode: at least 25 ms for low mode
    //!  and 200 ms for high and high2 modes.
    //! \param[out] sample  Raw sample value will be written in this parameter.
    //! \retval Status of the operation.
    //!
    static ecl::err get_illuminance_sample(uint16_t &sample);

    //! \brief Reads sample from sensor and converts to a physical value.
    //! \details This method blocks until measurement is not finished.
    //!  Measurement time depends on resolution mode: at least 25 ms for low mode
    //!  and 200 ms for high and high2 modes.
    //! \param[out] value Illuminance in (1000 * (illuminance in lx))
    //!  will be written in this parameter.
    //! \retval Status of the operation.
    //!
    static ecl::err get_illuminance(uint32_t &value);

private:
    //! Returns I2C slave address based on class template parameter
    static constexpr uint8_t pick_i2c_slave_address();

    //! I2C slave address when ADDR pin is pulled down to GND
    static constexpr uint8_t i2s_address_low = 0x46;
    //! I2C slave address when ADDR pin is pulled up to VCC
    static constexpr uint8_t i2s_address_high = 0xB8;

    //! Maximum measurement time for low resolution mode according to RM
    static constexpr uint16_t measurement_time_ms_low = 25;
    //! Maximum measurement time for high and high2 resolution modes according to RM
    static constexpr uint16_t measurement_time_ms_high = 200;

    //! Current resolution mode
    static bh1750_cfg::resolution m_resolution;

    //! List of commands, supported by sensor
    enum {
        CMD_POWER_DOWN = 0x0,
        CMD_POWER_UP = 0x1,
        CMD_RESET = 0x7,
        CMD_CONTINUOUSLY_HR_MODE = 0x10,
        CMD_CONTINUOUSLY_HR_MODE2 = 0x11,
        CMD_CONTINUOUSLY_LR_MODE = 0x13,
        CMD_ONE_TIME_HR_MODE = 0x20,
        CMD_ONE_TIME_HR_MODE2 = 0x21,
        CMD_ONE_TIME_LR_MODE = 0x23,
    };
};

template <class i2c_dev, bh1750_cfg::i2c_address address>
bh1750_cfg::resolution bh1750<i2c_dev, address>::m_resolution = bh1750_cfg::resolution::high;

template <class i2c_dev, bh1750_cfg::i2c_address address>
ecl::err bh1750<i2c_dev, address>::init()
{
    return i2c_dev::init();
}

template <class i2c_dev, bh1750_cfg::i2c_address address>
constexpr uint8_t bh1750<i2c_dev, address>::pick_i2c_slave_address()
{
    if (address == bh1750_cfg::i2c_address::low) {
        return i2s_address_low;
    } else {
        return i2s_address_high;
    }
}

template <class i2c_dev, bh1750_cfg::i2c_address address>
void bh1750<i2c_dev, address>::set_resolution(bh1750_cfg::resolution resolution)
{
    m_resolution = resolution;
}

template <class i2c_dev, bh1750_cfg::i2c_address address>
bh1750_cfg::resolution bh1750<i2c_dev, address>::get_resolution()
{
    return m_resolution;
}

template <class i2c_dev, bh1750_cfg::i2c_address address>
ecl::err bh1750<i2c_dev, address>::get_illuminance_sample(uint16_t &sample)
{
    uint8_t i2c_addr = pick_i2c_slave_address();

    uint8_t cmd = CMD_ONE_TIME_HR_MODE;
    uint16_t delay = measurement_time_ms_high;

    switch (m_resolution) {
    case bh1750_cfg::resolution::high:
        cmd = CMD_ONE_TIME_HR_MODE;
        delay = measurement_time_ms_high;
        break;
    case bh1750_cfg::resolution::high2:
        cmd = CMD_ONE_TIME_HR_MODE2;
        delay = measurement_time_ms_high;
        break;
    case bh1750_cfg::resolution::low:
        cmd = CMD_ONE_TIME_LR_MODE;
        delay = measurement_time_ms_low;
        break;
    default:
        break;
    }

    i2c_dev::lock();
    i2c_dev::platform_handle().set_slave_addr(i2c_addr);

    err rc = i2c_dev::set_buffers(&cmd, nullptr, 1);
    if (rc == err::ok) {
        rc = i2c_dev::xfer();
    }
    i2c_dev::unlock();

    if (rc != err::ok) {
        return rc;
    }

    ecl::this_thread::sleep_for(delay);

    uint8_t data[2] = {};

    i2c_dev::lock();
    i2c_dev::platform_handle().set_slave_addr(i2c_addr);

    rc = i2c_dev::set_buffers(nullptr, data, sizeof(data));
    if (rc == err::ok) {
        rc = i2c_dev::xfer();
    }
    i2c_dev::unlock();

    if (rc != err::ok) {
        return rc;
    }

    sample = ((data[0] << 8) | data[1]);

    return err::ok;

}

template <class i2c_dev, bh1750_cfg::i2c_address address>
ecl::err bh1750<i2c_dev, address>::get_illuminance(uint32_t &value)
{
    uint16_t sample = 0;

    err rc = get_illuminance_sample(sample);
    if (rc != err::ok) {
        return rc;
    }

    value = ((1000000 * static_cast<uint64_t>(sample)) / 1200);

    //! low resolution provides 4lx per count and high2 0.5lx per count
    if (m_resolution == bh1750_cfg::resolution::low) {
        value *= 4;
    } else if (m_resolution == bh1750_cfg::resolution::high2) {
        value /= 2;
    }

    return err::ok;
}


}; // namespace sensor

}; // namespace ecl

#endif // DEV_SENSOR_BH1750_HPP_
