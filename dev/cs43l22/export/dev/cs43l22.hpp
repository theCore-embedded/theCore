//!
//! \file
//! \brief Contains implementation for CS43L22 driver.
//! \details The CS43L22 is a highly integrated,
//!  low power stereo DAC with headphone and ClassD speaker amplifiers
//!  See: https://www.cirrus.com/products/cs43l22/
//!
//!  Restrictions: Current implementation supports only I2S audio format.
//!  Sample size is fixed and equal to 16 bits. Also, DAC requires external clock (MCLK)
//!  to operate correctly. In many implementations, I2S interface can output MCLK.
//!  It can be used as external clock for DAC.
//!

#ifndef DEV_CS43L22_HPP
#define DEV_CS43L22_HPP

#include <ecl/err.hpp>
#include <dev/bus.hpp>
#include <common/spi.hpp>

#include <functional>

namespace ecl
{

//!  The CS43L22 codec driver implementation.
//! \tparam I2c I2C generic bus driver.
//! \tparam I2s I2S generic bus driver.
//! \tparam Rst_gpio Reset pin represented by generic GPIO class.
//!
template < class I2c, class I2s, class Rst_gpio >
class cs43l22
{
public:

    //! Represents audio channel.
    //!
    enum class channel
    {
        left,
        right,
        all,
    };

    //! Register masks for frequencies supported by beep generator.
    //!
    enum class beep_frequency
    {
        hz260  = 0x0,  //! 260.78 Hz
        hz521  = 0x10, //! 521.74 Hz
        hz585  = 0x20, //! 585.37 Hz
        hz666  = 0x30, //! 666.67 Hz
        hz705  = 0x40, //! 705.88 Hz
        hz774  = 0x50, //! 774.19 Hz
        hz888  = 0x60, //! 888.89 Hz
        hz1000 = 0x70, //! 1000.00 Hz
        hz1043 = 0x80, //! 1043.48 Hz
        hz1200 = 0x90, //! 1200.00 Hz
        hz1333 = 0xA0, //! 1333.33 Hz
        hz1411 = 0xB0, //! 1411.76 Hz
        hz1600 = 0xC0, //! 1600.00 Hz
        hz1714 = 0xD0, //! 1714.29 Hz
        hz2000 = 0xE0, //! 2000.00 Hz
        hz2181 = 0xF0, //! 2181.82 Hz
    };

    //! Register masks for possible values for on duration of the beep signal.
    //!
    enum class beep_on_time
    {
        ms86   = 0x0, //! 86 ms
        ms430  = 0x1, //! 430 ms
        ms780  = 0x2, //! 780 ms
        ms1200 = 0x3, //! 1.20 s
        ms1500 = 0x4, //! 1.50 s
        ms1800 = 0x5, //! 1.80 s
        ms2200 = 0x6, //! 2.20 s
        ms2500 = 0x7, //! 2.50 s
        ms2800 = 0x8, //! 2.80 s
        ms3200 = 0x9, //! 3.20 s
        ms3500 = 0xA, //! 3.50 s
        ms3800 = 0xB, //! 3.80 s
        ms4200 = 0xC, //! 4.20 s
        ms4500 = 0xD, //! 4.50 s
        ms4800 = 0xE, //! 4.80 s
        ms5200 = 0xF, //! 5.20 s
    };

    //! Register masks for possible values for off duration of the beep signal.
    //!
    enum class beep_off_time
    {
        ms1230  = 0x0,  //! 1.23 s
        ms2580  = 0x20, //! 2.58 s
        ms3900  = 0x40, //! 3.90 s
        ms5200  = 0x60, //! 5.20 s
        ms6600  = 0x80, //! 6.60 s
        ms8050  = 0xA0, //! 8.05 s
        ms9350  = 0xC0, //! 9.35 s
        ms10800 = 0xE0, //! 10.80 s
    };

    //! Register masks for operation modes of the beep generator.
    //!
    enum class beep_mode
    {
        single    = 0x40,	//! Play one beep
        multiple  = 0x80,	//! Play beep for on_time, mute for off_time
        continuous = 0xC0,	//! Play continuously
    };

    //!  Maximum value of the master volume.
    //! \details The minimum value (0) represents the signal out the DSP: -102 dB.
    //!  The maximum value (0xE4) represents +12 dB. Step size is 0.5 dB.
    //!
    static constexpr uint8_t max_master_volume = 0xE4;

    //!  Maximum value of the headphone volume.
    //! \details The minimum value (0) represents the signal out the DAC: -127 dB (muted).
    //!  The maximum value (0xFF) represents the signal out the DAC: 0 dB. Step size is 0.5 dB.
    //!
    static constexpr uint8_t max_headphone_volume = 0xFF;

    //!  Maximum value of the beep volume.
    //! \details The minimum value (0) represents the gain: -56 dB.
    //!  The maximum value (0x1F) represents the gain: +6 dB. Step size is 2 dB.
    //!
    static constexpr uint8_t max_beep_volume = 0x1F;

    //!  Describes the user callback.
    //! \details NOTE: The user callback is called in IRQ context.
    //! \param type Represents the type of event: half-transfer (HT) or transfer completed (TC).
    //!
    using user_callback = std::function< void (ecl::bus_event type) >;

    //!  Codec low-level initialization routine.
    //! \details This method performs low-level initialization of the
    //!  codec and underlayer buses. This method must be called once
    //!  before any operations with codec.
    //! \retval Status of the operation.
    //!
    static err init();

    //!  Codec power up sequence. Must not be called if codec is already powered.
    //! \details This method performs power up sequence of the
    //!  codec which is recommended by CS43L22 Reference Manual (RM).
    //!  To use this method, the init() method must be called previously.
    //! \retval Status of the operation.
    //!
    static err power_up();

    //!  Codec power down sequence. Must not be called if codec is not powered.
    //! \details This method performs power down sequence of the
    //!  codec which is recommended by CS43L22 Reference Manual (RM).
    //!  To use this method, the init() method must be called previously.
    //! \retval Status of the operation.
    //!
    static err power_down();

    //!  Send PCM buffer to the codec.
    //! \details This method blocks until all data is transferred.
    //!  The PCM data contains audio samples. Sample size is constant and equal to 16 bit.
    //!  Sampling frequency can be changed with cs43l22< I2c, I2s, Rst_gpio >::set_sampling_frequency() method.
    //!  The buffer format is next: LRLR.... Where L - left channel and R - right channel.
    //! \param buffer The buffer which contains PCM data. Must not be null.
    //! \param count Number of samples in PCM buffer. Must not be null.
    //! \retval Status of the operation.
    //!
    static err send_pcm_buffer(const uint16_t *buffer, size_t count);

    //!  Starts stream of the PCM buffer to the codec.
    //! \details When stream is started, the user callback is called on two events:
    //!  Halt Transfer (HT) and Transfer Completed (TC). When half of samples are
    //!  transferred, the HT is generated and user can write the data into fist
    //!  half of the buffer. After all sample are transferred, the TC is generated.
    //!  After this the codec is continuing streaming from the start of the buffer
    //!  until it is stopped by cs43l22< I2c, I2s, Rst_gpio >::pcm_stream_stop().
    //!  NOTE: The HT and TC events are generated in IRQ context. The user callback
    //!  should not perform blocking operations, busy wait etc.
    //!  The PCM data contains audio samples. Sample size is constant and equal to 16 bit.
    //!  Sampling frequency can be changed with cs43l22< I2c, I2s, Rst_gpio >::set_sampling_frequency() method.
    //!  The buffer format is next: LRLR.... Where L - left channel and R - right channel.
    //! \param buffer The buffer which contains PCM data. Must not be null.
    //! \param count Number of samples in PCM buffer. Must not be null. Must be multiple of 2 and less of equal to 0xFFFF.
    //! \param callback User callback is called on HT and TC events.
    //! \retval Status of the operation.
    //!
    static err pcm_stream_start(const uint16_t *buffer, size_t count, user_callback callback);

    //!  Stop active stream. Must not be called if stream is not active.
    //! \details This method stops active PCM stream. It is recommended to call this method
    //!  during processing of the HT or TC event. In case this method is called during
    //!  processing of the TC event the steam will be stopped immediately. Otherwise, it
    //!  will be stopped after next TC event.
    //! \retval Status of the operation.
    //!
    static err pcm_stream_stop();

    //!  Set master volume for specific channel.
    //! \param ch Audio channel.
    //! \param volume The volume value. Must be in range [0, cs43l22< I2c, I2s, Rst_gpio >::max_master_volume]
    //! \retval Status of the operation.
    //!
    static err set_master_volume(uint8_t volume, channel ch = channel::all);

    //!  Set headphone volume for specific channel.
    //! \param ch Audio channel.
    //! \param volume The volume value. Must be in range [0, cs43l22< I2c, I2s, Rst_gpio >::max_headphone_volume]
    //! \retval Status of the operation.
    //!
    static err set_headphone_volume(uint8_t volume, channel ch = channel::all);

    //!  Mute headphone channel.
    //! \param ch Audio channel.
    //! \retval Status of the operation.
    //!
    static err headphone_mute(channel ch = channel::all);

    //!  Unmute headphone channel.
    //! \param ch Audio channel.
    //! \retval Status of the operation.
    //!
    static err headphone_unmute(channel ch = channel::all);

    //!  Set beep on time. Must not be called if beep is enabled. See RM for details.
    //! \param value The value of beep on time.
    //! \retval Status of the operation.
    //!
    static err set_beep_on_time(beep_on_time value);

    //!  Set beep off time. Must not be called if beep is enabled. See RM for details.
    //! \param value The value of beep off time.
    //! \retval Status of the operation.
    //!
    static err set_beep_off_time(beep_off_time value);

    //!  Set beep frequency. Must not be called if beep is enabled. See RM for details.
    //! \param value The value of beep frequency.
    //! \retval Status of the operation.
    //!
    static err set_beep_frequency(beep_frequency value);

    //!  Set beep Volume. Must not be called if beep is enabled. See RM for details.
    //! \param value The value of beep volume. Must be in range [0, cs43l22< I2c, I2s, Rst_gpio >::max_beep_volume]
    //! \retval Status of the operation.
    //!
    static err set_beep_volume(uint8_t volume);

    //!  Enable beep generator.
    //! \param mode The operation mode for beep generator.
    //! \retval Status of the operation.
    //!
    static err beep_enable(beep_mode mode = beep_mode::multiple);

    //!  Disable beep generator.
    //! \retval Status of the operation.
    //!
    static err beep_disable();

    //!  Set sampling frequency for PCM audio data.
    //! \tparam frequency Audio frequency.
    //! \retval Status of the operation.
    //!
    template <ecl::i2s::audio_frequency frequency>
    static err set_sampling_frequency();

private:
    // The I2C slave address of the codec.
    static constexpr uint8_t i2c_address = 0x94;

    enum codec_register
    {
        init_sequence_reg1      = 0x0, // used only during init sequence
        chip_id                 = 0x01,
        pwr_ctrl1               = 0x02,
        pwr_ctrl2               = 0x04,
        clk_ctrl                = 0x05,
        if_ctrl1                = 0x06,
        if_ctrl2                = 0x07,
        passthrough_a_select    = 0x08,
        passthrough_b_select    = 0x09,
        analog_set              = 0x0A,
        passthrough_gang_ctrl   = 0x0C,
        playback_ctrl1          = 0x0D,
        misc_ctrl               = 0x0E,
        playback_ctrl2          = 0x0F,
        passthrough_a_vol       = 0x14,
        passthrough_b_vol       = 0x15,
        pcm_a_vol               = 0x1A,
        pcm_b_vol               = 0x1B,
        beep_freq_ontime        = 0x1C,
        beep_vol_offtime        = 0x1D,
        beep_tone_cfg           = 0x1E,
        tone_ctrl               = 0x1F,
        master_a_vol            = 0x20,
        master_b_vol            = 0x21,
        hp_a_vol                = 0x22,
        hp_b_vol                = 0x23,
        speak_a_vol             = 0x24,
        speak_b_vol             = 0x25,
        ch_mix_swap             = 0x26,
        limit_ctrl1             = 0x27,
        limit_ctrl2             = 0x28,
        limit_attack            = 0x29,
        ovfl_clk_status         = 0x2E,
        batt_comp               = 0x2F,
        vp_batt_level           = 0x30,
        speak_status            = 0x31,
        init_sequence_reg2      = 0x32, // used only during init sequence
        charge_pump_freq        = 0x34,
        init_sequence_reg3      = 0x47, // used only during init sequence
    };

    // Writes value into internal codec register
    static err register_write(codec_register reg, uint8_t value);

    // Reads internal codec register over I2C
    static err register_read(codec_register reg, uint8_t &value);

    // Returns user callback
    static user_callback &tx_cplt_cb();

    // Internal callback for bus interface
    static void internal_tx_cplt_cb(ecl::bus_channel ch, ecl::bus_event type, size_t total);

    // State flags.
    // This bit is set when coded in inited.
    static constexpr uint8_t inited          = 0x1;
    // This bit is set when coded is powered.
    static constexpr uint8_t powered         = 0x2;
    // This bit is set when there is an active data stream.
    static constexpr uint8_t stream_active   = 0x4;
    // This bit is set when user stopped data streams.
    // Note, that when this bit is set, the stream_active can also be set,
    // until next TC event will be processed.
    static constexpr uint8_t stream_stopped  = 0x8;
    // This bit is set when beep generator is enabled.
    static constexpr uint8_t beep_enabled    = 0x10;

    // Represents the state of the codec.
    static uint8_t m_state;
};

template < class I2c, class I2s, class Rst_gpio > uint8_t cs43l22< I2c, I2s, Rst_gpio >::m_state{0};

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::init()
{
    ecl_assert(!(m_state & inited));

    tx_cplt_cb();

    err rc = I2c::init();
    if (rc != err::ok) {
        return rc;
    }

    rc = I2s::init();

    if (is_ok(rc)) {
        m_state |= inited;
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::power_up()
{
    ecl_assert(m_state & inited);
    ecl_assert(!(m_state & powered));

    Rst_gpio::set();

    uint8_t value {0};

    // power up sequence according to RM
    err rc = register_write(pwr_ctrl1, 0x01);
    //TODO #113 add error checking.
    register_write(init_sequence_reg1, 0x99);
    register_write(init_sequence_reg3, 0x80);

    register_read(init_sequence_reg2, value);
    register_write(init_sequence_reg2, value | 0x80);

    register_read(init_sequence_reg2, value);
    register_write(init_sequence_reg2, value & (~0x80));

    register_write(init_sequence_reg1, 0x0);

    // Turn ON headphone channel A and B
    register_write(pwr_ctrl2, 0xAF);

    // clock auto-detect
    register_write(clk_ctrl, 0x81);

    // Set Audio Word Length to 16 bit, Audio format - I2S.
    register_write(if_ctrl1, 0x7);

    // power up
    register_write(pwr_ctrl1, 0x9e);

    if (is_ok(rc)) {
        m_state |= powered;
    }

    return err::ok;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::power_down()
{
    ecl_assert(m_state & inited);
    ecl_assert(m_state & powered);

    err rc = register_write(pwr_ctrl1, 0x9f);
    if (!is_ok(rc)) {
        return rc;
    }

    Rst_gpio::reset();

    m_state &= ~powered;

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::send_pcm_buffer(const uint16_t *buffer, size_t count)
{
    ecl_assert(m_state & inited);
    ecl_assert(buffer && count);

    I2s::lock();

    I2s::set_buffers(reinterpret_cast< const uint8_t *>(buffer), nullptr, count * sizeof(*buffer));
    err rc = I2s::xfer();

    I2s::unlock();

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::pcm_stream_start(const uint16_t *buffer, size_t count, user_callback cb)
{
    ecl_assert(m_state & inited);
    ecl_assert(buffer && count);
    ecl_assert(cb);

    if (m_state & stream_active) {
        return err::busy;
    }

    m_state |= stream_active;

    tx_cplt_cb() = cb;

    I2s::lock();

    I2s::platform_handle::enable_circular_mode();
    I2s::set_buffers(reinterpret_cast< const uint8_t *>(buffer), nullptr, count * sizeof(*buffer));
    err rc = I2s::xfer(internal_tx_cplt_cb);

    I2s::unlock();

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::pcm_stream_stop()
{
    ecl_assert(m_state & inited);
    ecl_assert(m_state & stream_active);


    // disable circular mode for I2S bus
    I2s::platform_handle::disable_circular_mode();

    m_state |= stream_stopped;

    return err::ok;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::register_write(codec_register reg, uint8_t value)
{
    ecl_assert(m_state & inited);

    uint8_t buffer[] = {static_cast< uint8_t >(reg), value};

    I2c::lock();

	I2c::platform_handle::set_slave_addr(i2c_address);
    I2c::set_buffers(buffer, 0, sizeof(buffer));
    err rc = I2c::xfer();

    I2c::unlock();

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::register_read(codec_register reg, uint8_t &value)
{
    ecl_assert(m_state & inited);

    I2c::lock();

	I2c::platform_handle::set_slave_addr(i2c_address);
    I2c::set_buffers(reinterpret_cast< uint8_t *>(&reg), &value, sizeof(uint8_t));
    err rc = I2c::xfer();

    I2c::unlock();

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
void cs43l22< I2c, I2s, Rst_gpio >::internal_tx_cplt_cb(ecl::bus_channel ch, ecl::bus_event type, size_t total)
{
    (void) total;

    if (ch == ecl::bus_channel::tx) {
        if ((m_state & stream_stopped) && type == ecl::bus_event::tc) {
            m_state &= ~(stream_active | stream_stopped);
            // Reset user callback
            tx_cplt_cb() = user_callback {};
        } else {
            // Call user callback
            tx_cplt_cb()(type);
        }
    }
}

template < class I2c, class I2s, class Rst_gpio >
typename cs43l22< I2c, I2s, Rst_gpio >::user_callback &cs43l22< I2c, I2s, Rst_gpio >::tx_cplt_cb()
{
    static user_callback cb = user_callback {};
    return cb;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::set_master_volume(uint8_t volume, channel ch)
{
    ecl_assert(m_state & inited);
    ecl_assert(volume <= max_master_volume);

    err rc = err::ok;

    // Represents 0dB volume according to RM
    const uint8_t zero_level = 0xCC;
    // Represents -102dB volume according to RM
    const uint8_t lowest_level = 0x34;

    // Volume to bits mapping, see RM for details
    uint8_t bitmask = 0xFF;
    if (volume >= zero_level) {
        bitmask = volume - zero_level;
    } else {
        bitmask = volume + lowest_level;
    }

    if (ch == channel::left) {
        rc = register_write(master_a_vol, bitmask);
    } else if (ch == channel::right) {
        rc = register_write(master_b_vol, bitmask);
    } else { // channel::all
        rc = register_write(master_a_vol, bitmask);
        if (!is_ok(rc)) {
            return rc;
        }
        rc = register_write(master_b_vol, bitmask);
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::set_headphone_volume(uint8_t volume, channel ch)
{
    ecl_assert(m_state & inited);
    ecl_assert(volume <= max_headphone_volume);

    err rc = err::ok;

    // Volume to bits mapping, see RM for details
    uint8_t bitmask = 0xFF;
    if (volume < max_headphone_volume) {
        bitmask = volume + 1;
    } else {
        bitmask = 0x0;
    }

    if (ch == channel::left) {
        rc = register_write(hp_a_vol, bitmask);
    } else if (ch == channel::right) {
        rc = register_write(hp_b_vol, bitmask);
    } else { // channel::all
        rc = register_write(hp_a_vol, bitmask);
        if (!is_ok(rc)) {
            return rc;
        }
        rc = register_write(hp_b_vol, bitmask);
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::headphone_mute(channel ch)
{
    ecl_assert(m_state & inited);

    uint8_t value = 0;
    err rc = register_read(playback_ctrl2, value);
    if (!is_ok(rc)) {
        return rc;
    }

    // Set HP mute bits. See RM for details.
    if (ch == channel::left) {
        value |= 0x40;
    } else if (ch == channel::right) {
        value |= 0x80;
    } else { // channel::all
        value |= 0xC0;
    }

    rc = register_write(playback_ctrl2, value);

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::headphone_unmute(channel ch)
{
    ecl_assert(m_state & inited);

    uint8_t value = 0;
    err rc = register_read(playback_ctrl2, value);
    if (!is_ok(rc)) {
        return rc;
    }

    // Reset HP mute bits. See RM for details.
    if (ch == channel::left) {
        value &= ~0x40;
    } else if (ch == channel::right) {
        value &= ~0x80;
    } else { // channel::all
        value &= ~0xC0;
    }

    rc = register_write(playback_ctrl2, value);

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::set_beep_on_time(beep_on_time value)
{
    ecl_assert(m_state & inited);
    ecl_assert(!(m_state & beep_enabled));

    uint8_t register_value = 0;
    err rc = register_read(beep_freq_ontime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    // According to RM, lower 4 bits represents beep on time. Clear these bits.
    register_value &= ~0xF;

    uint8_t on_time_mask = static_cast< std::underlying_type_t<beep_on_time> >(value) & 0xF;
    register_value |= on_time_mask;

    rc = register_write(beep_freq_ontime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::set_beep_off_time(beep_off_time value)
{
    ecl_assert(m_state & inited);
    ecl_assert(!(m_state & beep_enabled));

    uint8_t register_value = 0;
    err rc = register_read(beep_vol_offtime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    // According to RM, higher 3 bits represents beep off time. Clear these bits.
    register_value &= ~0xE0;

    uint8_t off_time_mask = static_cast< std::underlying_type_t<beep_frequency> >(value) & 0xFF;
    register_value |= off_time_mask;

    rc = register_write(beep_vol_offtime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::set_beep_frequency(beep_frequency value)
{
    ecl_assert(m_state & inited);
    ecl_assert(!(m_state & beep_enabled));

    uint8_t register_value = 0;
    err rc = register_read(beep_freq_ontime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    // According to RM, higher 4 bits represents beep frequency time. Clear these bits.
    register_value &= ~0xF0;

    uint8_t frequency_mask = static_cast< std::underlying_type_t<beep_frequency> >(value) & 0xFF;
    register_value |= frequency_mask;

    rc = register_write(beep_freq_ontime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::set_beep_volume(uint8_t volume)
{
    ecl_assert(m_state & inited);
    ecl_assert(volume <= max_beep_volume);

    uint8_t register_value = 0;
    err rc = register_read(beep_vol_offtime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    // Volume to bits mapping, see RM for details
    const uint8_t zero_level = 0x19;
    const uint8_t lowest_level = 0x7;

    uint8_t bitmask = 0x0;
    if (volume > zero_level) {
        bitmask = volume - zero_level;
    } else {
        bitmask = volume + lowest_level;
    }

    bitmask &= 0x1F;

    register_value |= bitmask;

    rc = register_write(beep_vol_offtime, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::beep_enable(beep_mode mode)
{
    ecl_assert(m_state & inited);
    ecl_assert(!(m_state & beep_enabled));

    uint8_t register_value = 0;
    err rc = register_read(beep_tone_cfg, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    register_value &= ~0xC0;

    uint8_t mode_mask = static_cast< std::underlying_type_t<beep_mode> >(mode) & 0xFF;
    register_value |= mode_mask;
    rc = register_write(beep_tone_cfg, register_value);
    if (!is_ok(rc)) {
        return rc;
    }
    m_state |= beep_enabled;

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
err cs43l22< I2c, I2s, Rst_gpio >::beep_disable()
{
    ecl_assert(m_state & inited);
    ecl_assert(m_state & beep_enabled);

    uint8_t register_value = 0;
    err rc = register_read(beep_tone_cfg, register_value);
    if (!is_ok(rc)) {
        return rc;
    }

    register_value &= 0x3F;

    rc = register_write(beep_tone_cfg, register_value);
    if (!is_ok(rc)) {
        return rc;
    }
    m_state &= ~beep_enabled;

    return rc;
}

template < class I2c, class I2s, class Rst_gpio >
template < ecl::i2s::audio_frequency frequency>
err cs43l22< I2c, I2s, Rst_gpio >::set_sampling_frequency()
{
    ecl_assert(m_state & inited);

    return I2s::platform_handle::template i2s_set_audio_frequency<frequency>();
}

} // namespace ecl

#endif // DEV_CS43L22_HPP
