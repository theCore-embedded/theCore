#ifndef __HTU21_HPP__
#define __HTU21_HPP__

namespace sensor {

template <class i2c_dev>
class htu21 {
public:
    htu21() = delete;
    ~htu21() = delete;

    //! \brief Inits sensor and underlying I2C platform bus.
    //! \retval Status of operation.
    //!
    static ecl::err init();

    //! \brief Performs soft reset of the sensor.
    //!
    static void soft_reset();

    //! \brief Reads sensor user register.
    //! \details Allows to get useful information about sensor state
    //! \retval User register content.
    //!
    static uint8_t read_user_register();

    //! \brief Writes to the sensor user register.
    //! \details Allows to configure sensor resolution.
    //!
    static void write_user_register(uint8_t value);

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
    //! \retval Temperature in C degree.
    //!
    static float get_temperature();

    //! \brief Reads sample from sensor and converts to a physical value.
    //! \retval Relative humidity in %.
    //!
    static float get_humidity();

    //! Sensor I2C address. Cannot be changed.
    static constexpr uint8_t i2_addr = 0x80;

private:
    // sensor I2c commands
    enum {
        HTU21_TRIGGER_TEMPERATURE_HM = 0xE3,
        HTU21_TRIGGER_HUMIDITY_HM = 0xE5,
        HTU21_TRIGGER_TEMPERATURE = 0xF3,
        HTU21_TRIGGER_HUMIDITY = 0xF5,
        HTU21_WRITE_USER_REG = 0xE6,
        HTU21_READ_USER_REG = 0xE7,
        HTU21_SOFT_RESET = 0xFE
    };

    //! \briefReads sample from sensor in I2C hold master mode.
    //! \details In this mode sensor holds SCL until measurements is finished
    static uint16_t i2c_get_sample_hold_master(uint8_t cmd);
};

template <class i2c_dev>
ecl::err htu21<i2c_dev>::init()
{
    return i2c_dev::init();
}

template <class i2c_dev>
void htu21<i2c_dev>::soft_reset()
{
    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    uint8_t cmd = HTU21_SOFT_RESET;

    i2c_dev::lock();
    i2c_dev::set_buffers(&cmd, nullptr, 1);
    i2c_dev::xfer();
    i2c_dev::unlock();
}

template <class i2c_dev>
uint8_t htu21<i2c_dev>::read_user_register()
{
    uint8_t value = 0;

    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    i2c_dev::lock();
    i2c_dev::set_buffers(nullptr, &value, 1);
    i2c_dev::xfer();
    i2c_dev::unlock();

    return value;
}

template <class i2c_dev>
void htu21<i2c_dev>::write_user_register(uint8_t value)
{
    i2c_dev::platform_handle().set_slave_addr(i2_addr);

    i2c_dev::lock();
    i2c_dev::set_buffers(&value, nullptr, 1);
    i2c_dev::xfer();
    i2c_dev::unlock();
}

template <class i2c_dev>
uint16_t htu21<i2c_dev>::i2c_get_sample_hold_master(uint8_t cmd)
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
uint16_t htu21<i2c_dev>::get_sample_temperature()
{
    return i2c_get_sample_hold_master(HTU21_TRIGGER_TEMPERATURE_HM);
}

template <class i2c_dev>
uint16_t htu21<i2c_dev>::get_sample_humidity()
{
    return i2c_get_sample_hold_master(HTU21_TRIGGER_HUMIDITY_HM);
}

template <class i2c_dev>
float htu21<i2c_dev>::get_temperature()
{
    uint16_t sample = get_sample_temperature();

    // clear last 2 bits according to RM,
    // since they contain status information
    sample &= ~3;

    return -46.85f + 175.72f * (static_cast<float>(sample) / (1 << 16));
}

template <class i2c_dev>
float htu21<i2c_dev>::get_humidity()
{
    uint16_t sample = get_sample_humidity();

    // clear last 2 bits according to RM,
    // since they contain status information
    sample &= ~3;

    return -6.0f + 125.0f * (static_cast<float>(sample) / (1 << 16));
}

} // namespace sensor

#endif //__HTU21_HPP__
