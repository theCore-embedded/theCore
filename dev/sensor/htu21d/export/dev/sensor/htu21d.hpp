//!
//! \file
//! \brief Driver for HTU21D digital humidity sensor with temperature output
//!  See, http://www.meas-spec.com/downloads/HTU21D.pdf
//!

#ifndef __DEV_SENSOR_HTU21D_HPP__
#define __DEV_SENSOR_HTU21D_HPP__

#include <ecl/err.hpp>

namespace ecl
{

namespace sensor
{

//! \brief Defines resolution modes for HTU21D sensor.
//! \details Possible modes are (RM here - relative humidity):
//!  rm12_t14 - 12 bits for RM, 14 bits for temperature
//!  rm8_t12 - 8 bits for RM, 12 bits for temperature
//!  rm10_t13 - 10 bits for RM, 13 bits for temperature
//!  rm11_t11 - 11 bits for RM, 11 bits for temperature
//!
enum class htu21d_resolution
{
    rm12_t14,
    rm8_t12,
    rm10_t13,
    rm11_t11
};

//! \brief HTU21D sensor driver implementation.
//! \tparam I2C generic bus driver
//!
template <class i2c_dev>
class htu21d {
public:
    htu21d() = delete;
    ~htu21d() = delete;

    //! \brief Inits sensor and underlying I2C platform bus.
    //! \retval Status of operation.
    //!
    static err init();

    //! \brief Performs soft reset of the sensor.
    //! \note The soft reset takes around 15ms.
    //!  User code should wait at least 15ms to finish reset procedure.
    //! \retval Status of the operation.
    //!
    static err soft_reset();

    //! \brief Reads raw temperature sample from sensor.
    //! \details Sample should be processed to receive physical value.
    //! \param[out] sample Variable in which raw temp. sample will be written.
    //! \retval Status of the operation.
    //!
    static err get_sample_temperature(uint16_t &sample);

    //! \brief Reads raw relative humidity sample from sensor.
    //! \details Sample should be processed to receive physical value.
    //! \param[out] sample Variable in which raw RM sample will be written.
    //! \retval Status of the operation.
    //!
    static err get_sample_humidity(uint16_t &sample);

    //! \brief Reads sample from sensor and converts to a physical value.
    //! \param[out] temperature Temperature in (1000 * (temperature in C degree))
    //!  will be written in this parameter.
    //! \retval Status of the operation.
    //!
    static err get_temperature(int &temperature);

    //! \brief Reads sample from sensor and converts to a physical value.
    //! \param[out] humidity Relative humidity in (1000 * (humidity in %))
    //!  will be written in this parameter.
    //! \retval Status of the operation.
    //!
    static err get_humidity(int &humidity);

    //! \brief Checks end of battery status (see RM for details)
    //! \retval True if battery power > 2.5V, False otherwise.
    //!
    static bool is_battery_low();

    //! \brief Checks if on-chip heater is enabled
    //! \retval True if enabled, False otherwise.
    //!
    static bool is_heater_enabled();

    //! \brief Enables on-chip heater (see RM for details)
    //! \details The heater is intended to be used for functionality diagnosis:
    //!  relative humidity drops upon rising temperature. The heater consumes
    //!  about 5.5mW and provides a temperature increase of about 0.5-1.5°C
    //! \retval Status of the operation.
    //!
    static err enable_heater();

    //! \brief Disables on-chip heater (see RM for details)
    //! \retval Status of the operation.
    //!
    static err disable_heater();

    //! \brief Sets resolution mode for measuring
    //!  Default mode (htu21d_resolution::rm12_t14) is entered after power on
    //! \param[in] mode Can be value of type htu21d_resolution
    //! \retval Status of the operation.
    //!
    static err set_resolution_mode(htu21d_resolution mode);

    //! \brief Returns current resolution mode
    //! \param[out] mode The value in which result will be written.
    //!  Can be value of type htu21d_resolution
    //! \retval Status of the operation.
    //!
    static err get_resolution_mode(htu21d_resolution &mode);

private:
    //! Sensor I2C commands
    enum {
        TRIGGER_TEMPERATURE_HM = 0xE3,
        TRIGGER_HUMIDITY_HM = 0xE5,
        TRIGGER_TEMPERATURE = 0xF3,
        TRIGGER_HUMIDITY = 0xF5,
        WRITE_USER_REG = 0xE6,
        READ_USER_REG = 0xE7,
        SOFT_RESET = 0xFE
    };

    //! Sensor I2C address. Cannot be changed.
    static constexpr uint8_t i2_addr = 0x80;

    //! Used to get battery power status
    static constexpr uint8_t battery_low_bit = 0x40;

    //! Used to control on-chip heater
    static constexpr uint8_t on_chip_heater_bit = 0x4;

    //! Should be user to clear resolution bits only
    static constexpr uint8_t resolution_clear_mask = 0x7E;

    //! Resolution modes masks
    //! Used to enable rm12_t14 mode
    static constexpr uint8_t rm12_t14_mask = 0x0;
    //! Used to enable rm8_t12 mode
    static constexpr uint8_t rm8_t12_mask = 0x1;
    //! Used to enable rm10_t13 mode
    static constexpr uint8_t rm10_t13_mask = 0x80;
    //! Used to enable rm11_t11 mode
    static constexpr uint8_t rm11_t11_mask = 0x81;

    //! Reads sample from sensor in I2C hold master mode.
    //! In this mode sensor holds SCL until measurements is finished
    static err i2c_get_sample_hold_master(uint8_t cmd, uint16_t &sample);

    //! Reads sensor user register.
    //! Allows to get useful information about sensor state
    //!
    static err read_user_register(uint8_t &value);

    //! Writes to the sensor user register.
    //! Allows to configure sensor resolution.
    //!
    static err write_user_register(uint8_t value);
};

template <class i2c_dev>
err htu21d<i2c_dev>::init()
{
    return i2c_dev::init();
}

template <class i2c_dev>
err htu21d<i2c_dev>::soft_reset()
{
    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    uint8_t cmd = SOFT_RESET;

    i2c_dev::lock();
    err rc = i2c_dev::set_buffers(&cmd, nullptr, 1);
    if (rc == err::ok) {
        rc = i2c_dev::xfer();
    }
    i2c_dev::unlock();

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::read_user_register(uint8_t &value)
{
    uint8_t cmd = READ_USER_REG;

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    i2c_dev::lock();
    err rc = i2c_dev::set_buffers(&cmd, &value, 1);
    if (rc == err::ok) {
        i2c_dev::xfer();
    }
    i2c_dev::unlock();

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::write_user_register(uint8_t value)
{
    uint8_t tx_buff[] = {WRITE_USER_REG, value};

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    i2c_dev::lock();
    err rc = i2c_dev::set_buffers(tx_buff, nullptr, sizeof(tx_buff));
    if (rc == err::ok) {
        i2c_dev::xfer();
    }
    i2c_dev::unlock();

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::i2c_get_sample_hold_master(uint8_t cmd, uint16_t &sample)
{
    // MSB, LSB, CRC
    uint8_t data[3] = {};
    uint16_t value = 0;

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    // send command to start measuring
    i2c_dev::lock();
    err rc = i2c_dev::set_buffers(&cmd, nullptr, 1);
    if (rc == err::ok) {
        rc = i2c_dev::xfer();
    }
    i2c_dev::unlock();

    if (rc != err::ok) {
        return rc;
    }

    // read data, last byte is CRC
    i2c_dev::lock();
    rc = i2c_dev::set_buffers(nullptr, data, sizeof(data));
    if (rc == err::ok) {
        rc = i2c_dev::xfer();
    }
    i2c_dev::unlock();

    sample = ((data[0] << 8) | data[1]);

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::get_sample_temperature(uint16_t &value)
{
    return i2c_get_sample_hold_master(TRIGGER_TEMPERATURE_HM, value);
}

template <class i2c_dev>
err htu21d<i2c_dev>::get_sample_humidity(uint16_t &value)
{
    return i2c_get_sample_hold_master(TRIGGER_HUMIDITY_HM, value);
}

template <class i2c_dev>
err htu21d<i2c_dev>::get_temperature(int &value)
{
    uint16_t sample = 0;

    err rc = get_sample_temperature(sample);
    if (rc != err::ok) {
        return rc;
    }

    // clear last 2 bits according to RM,
    // since they contain status information
    sample &= ~3;

    value = -46850 + ((175000 * static_cast<uint64_t>(sample)) / (1 << 16));

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::get_humidity(int &value)
{
    uint16_t sample = 0;

    err rc = get_sample_humidity(sample);
    if (rc != err::ok) {
        return rc;
    }

    // clear last 2 bits according to RM,
    // since they contain status information
    sample &= ~3;

    value = -6000 + ((125000 * static_cast<uint64_t>(sample)) / (1 << 16));

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::set_resolution_mode(htu21d_resolution mode)
{
    uint8_t user_reg = 0;

    err rc = read_user_register(user_reg);
    if (rc != err::ok) {
        return rc;
    }

    // clear resolution bits first
    user_reg &= resolution_clear_mask;

    switch (mode) {
    case htu21d_resolution::rm12_t14:
        user_reg |= rm12_t14_mask;
        break;
    case htu21d_resolution::rm8_t12:
        user_reg |= rm8_t12_mask;
        break;
    case htu21d_resolution::rm10_t13:
        user_reg |= rm10_t13_mask;
        break;
    case htu21d_resolution::rm11_t11:
        user_reg |= rm11_t11_mask;
        break;
    default:
        //default mode
        user_reg |= rm12_t14_mask;
        break;
    }

    rc = write_user_register(user_reg);

    return err::ok;
}

template <class i2c_dev>
err htu21d<i2c_dev>::get_resolution_mode(htu21d_resolution &mode)
{
    uint8_t user_reg = 0;
    err rc = read_user_register(user_reg);
    if (rc != err::ok) {
        return rc;
    }

    user_reg &= ~resolution_clear_mask;

    switch (user_reg) {
    case rm12_t14_mask:
        mode = htu21d_resolution::rm12_t14;
        break;
    case rm8_t12_mask:
        mode = htu21d_resolution::rm8_t12;
        break;
    case rm10_t13_mask:
        mode = htu21d_resolution::rm10_t13;
        break;
    case rm11_t11_mask:
        mode = htu21d_resolution::rm11_t11;
        break;
    default:
        // default mode
        mode = htu21d_resolution::rm12_t14;
        break;
    }

    return rc;
}

template <class i2c_dev>
bool htu21d<i2c_dev>::is_battery_low()
{
    uint8_t user_reg = 0;

    read_user_register(user_reg);

    return (user_reg & battery_low_bit);
}

template <class i2c_dev>
bool htu21d<i2c_dev>::is_heater_enabled()
{
    uint8_t user_reg = 0;

    read_user_register(user_reg);

    return (user_reg & on_chip_heater_bit);
}

template <class i2c_dev>
err htu21d<i2c_dev>:: enable_heater()
{
    uint8_t user_reg = 0;
    err rc = read_user_register(user_reg);
    if (rc != err::ok) {
        return rc;
    }

    user_reg |= on_chip_heater_bit;

    rc = write_user_register(user_reg);

    return rc;
}

template <class i2c_dev>
err htu21d<i2c_dev>::disable_heater()
{
    uint8_t user_reg = 0;
    err rc = read_user_register(user_reg);
    if (rc != err::ok) {
        return rc;
    }

    user_reg &= ~on_chip_heater_bit;

    rc = write_user_register(user_reg);

    return rc;
}

} // namespace sensor

} // namespace ecl

#endif //__DEV_SENSOR_HTU21D_HPP__
