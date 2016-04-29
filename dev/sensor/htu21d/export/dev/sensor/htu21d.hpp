//!
//! \file
//! \brief Driver for HTU21D digital humidity sensor with temperature output
//! See, http://www.meas-spec.com/downloads/HTU21D.pdf
//!

#ifndef __DEV_SENSOR_HTU21D_HPP__
#define __DEV_SENSOR_HTU21D_HPP__

namespace sensor {

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
    static ecl::err init();

    //! \brief Performs soft reset of the sensor.
    //! \note The soft reset takes around 15ms.
    //! User code should wait at least 15ms to finish reset procedure.
    //!
    static void soft_reset();

    //! \brief Reads raw temperature sample from sensor.
    //! \details Sample should be processed to receive physical value.
    //! \retval Raw temperature sample.
    //!
    static uint16_t get_sample_temperature();

    //! \brief Reads raw relative humidity sample from sensor.
    //! \details Sample should be processed to receive physical value.
    //! \retval Raw relative humidity sample.
    //!
    static uint16_t get_sample_humidity();

    //! \brief Reads sample from sensor and converts to a physical value.
    //! \retval Temperature in (1000 * (temperature in C degree)).
    //!
    static int get_temperature();

    //! \brief Reads sample from sensor and converts to a physical value.
    //! \retval Relative humidity in (1000 * (humidity in %)).
    //!
    static int get_humidity();

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
    //! relative humidity drops upon rising temperature. The heater consumes
    //! about 5.5mW and provides a temperature increase of about 0.5-1.5°C
    //!
    static void enable_heater();

    //! \brief Disables on-chip heater (see RM for details)
    //!
    static void disable_heater();

    //! \brief Describes possible resolution modes for sensor.
    //!
    typedef enum {
        RESOLUTION_12_14BIT = 0x0,
        RESOLUTION_8_12BIT = 0x1,
        RESOLUTION_10_13BIT = 0x80,
        RESOLUTION_11_11BIT = 0x81,
    } resolution_t;

    //! \brief Sets resolution mode for measuring
    //! \details Possible modes are (RM here - relative humidity):
    //!  RESOLUTION_12_14BIT - 12 bits for RM, 14 bits for temperature
    //!  RESOLUTION_8_12BIT - 8 bits for RM, 12 bits for temperature
    //!  RESOLUTION_10_13BIT - 10 bits for RM, 13 bits for temperature
    //!  RESOLUTION_11_11BIT - 11 bits for RM, 11 bits for temperature
    //!  Default mode (RESOLUTION_12_14BIT) is entered after power on
    //! \tparams Can be value of type resolution_t
    //!
    static void set_resolution_mode(resolution_t mode);

    //! \brief Returns current resolution mode
    //! \retval Can be value of type resolution_t
    //!
    static resolution_t get_resolution_mode();

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

    //! Should be user to clear resolution bits only
    static constexpr uint8_t resolution_clear_mask = 0x7E;

    //! Used to get battery power status
    static constexpr uint8_t battery_low_bit = 0x40;

    //! Used to control on-chip heater
    static constexpr uint8_t on_chip_heater_bit = 0x4;

    //! Reads sample from sensor in I2C hold master mode.
    //! In this mode sensor holds SCL until measurements is finished
    static uint16_t i2c_get_sample_hold_master(uint8_t cmd);

    //! Reads sensor user register.
    //! Allows to get useful information about sensor state
    //!
    static uint8_t read_user_register();

    //! Writes to the sensor user register.
    //! Allows to configure sensor resolution.
    //!
    static void write_user_register(uint8_t value);
};

template <class i2c_dev>
ecl::err htu21d<i2c_dev>::init()
{
    return i2c_dev::init();
}

template <class i2c_dev>
void htu21d<i2c_dev>::soft_reset()
{
    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    uint8_t cmd = SOFT_RESET;

    i2c_dev::lock();
    i2c_dev::set_buffers(&cmd, nullptr, 1);
    i2c_dev::xfer();
    i2c_dev::unlock();
}

template <class i2c_dev>
uint8_t htu21d<i2c_dev>::read_user_register()
{
    uint8_t value = 0;
    uint8_t cmd = READ_USER_REG;

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    i2c_dev::lock();
    i2c_dev::set_buffers(&cmd, &value, 1);
    i2c_dev::xfer();
    i2c_dev::unlock();

    return value;
}

template <class i2c_dev>
void htu21d<i2c_dev>::write_user_register(uint8_t value)
{
    uint8_t tx_buff[] = {WRITE_USER_REG, value};

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    i2c_dev::lock();
    i2c_dev::set_buffers(tx_buff, nullptr, sizeof(tx_buff));
    i2c_dev::xfer();
    i2c_dev::unlock();
}

template <class i2c_dev>
uint16_t htu21d<i2c_dev>::i2c_get_sample_hold_master(uint8_t cmd)
{
    // MSB, LSB, CRC
    uint8_t data[3] = {};
    uint16_t value = 0;

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    // send command to start measuring
    i2c_dev::lock();
    i2c_dev::set_buffers(&cmd, nullptr, 1);
    i2c_dev::xfer();
    i2c_dev::unlock();

    // read data, last byte is CRC
    i2c_dev::lock();
    i2c_dev::set_buffers(nullptr, data, sizeof(data));
    i2c_dev::xfer();
    i2c_dev::unlock();

    return ((data[0] << 8) | data[1]);
}

template <class i2c_dev>
uint16_t htu21d<i2c_dev>::get_sample_temperature()
{
    return i2c_get_sample_hold_master(TRIGGER_TEMPERATURE_HM);
}

template <class i2c_dev>
uint16_t htu21d<i2c_dev>::get_sample_humidity()
{
    return i2c_get_sample_hold_master(TRIGGER_HUMIDITY_HM);
}

template <class i2c_dev>
int htu21d<i2c_dev>::get_temperature()
{
    uint16_t sample = get_sample_temperature();

    // clear last 2 bits according to RM,
    // since they contain status information
    sample &= ~3;

    return -46850 + ((175000 * (long long)sample) / (1 << 16));
}

template <class i2c_dev>
int htu21d<i2c_dev>::get_humidity()
{
    uint16_t sample = get_sample_humidity();

    // clear last 2 bits according to RM,
    // since they contain status information
    sample &= ~3;

    return -6000 + ((125000 * (long long)sample) / (1 << 16));
}

template <class i2c_dev>
void htu21d<i2c_dev>::set_resolution_mode(resolution_t mode)
{
    uint8_t user_reg = read_user_register();

    // switch here explicitly to avoid issues in case of
    // casting to htu21d_resolution_t in caller function
    switch (mode) {
    case RESOLUTION_12_14BIT:
    case RESOLUTION_8_12BIT:
    case RESOLUTION_10_13BIT:
    case RESOLUTION_11_11BIT:
        // clear resolution bits first
        user_reg &= resolution_clear_mask;
        // apply mode mask
        user_reg |= mode;
        break;
    default:
        break;
    }

    write_user_register(user_reg);
}

template <class i2c_dev>
typename htu21d<i2c_dev>::resolution_t htu21d<i2c_dev>::get_resolution_mode()
{
    uint8_t user_reg = read_user_register();

    user_reg &= ~resolution_clear_mask;

    if (user_reg == RESOLUTION_11_11BIT) {
        return RESOLUTION_11_11BIT;
    } else if (user_reg == RESOLUTION_8_12BIT) {
        return RESOLUTION_8_12BIT;
    } else if (user_reg == RESOLUTION_10_13BIT) {
        return RESOLUTION_10_13BIT;
    } else {
        return RESOLUTION_12_14BIT;
    }
}

template <class i2c_dev>
bool htu21d<i2c_dev>::is_battery_low()
{
    uint8_t user_reg = read_user_register();

    return (user_reg & battery_low_bit);
}

template <class i2c_dev>
bool htu21d<i2c_dev>::is_heater_enabled()
{
    uint8_t user_reg = read_user_register();

    return (user_reg & on_chip_heater_bit);
}

template <class i2c_dev>
void htu21d<i2c_dev>:: enable_heater()
{
    uint8_t user_reg = read_user_register();

    user_reg |= on_chip_heater_bit;

    write_user_register(user_reg);
}

template <class i2c_dev>
void htu21d<i2c_dev>::disable_heater()
{
    uint8_t user_reg = read_user_register();

    user_reg &= ~on_chip_heater_bit;

    write_user_register(user_reg);
}

} // namespace sensor

#endif //__DEV_SENSOR_HTU21D_HPP__
