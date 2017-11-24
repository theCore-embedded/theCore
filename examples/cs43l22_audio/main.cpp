/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief CS43l22 codec usage example.
//! \details Example shows how to use CS43L22 codec to stream PCM data.
//! Incoming data is 16 bps, mono. Before it is scheduled for streaming,
//! it should be converted to stereo. In this example, it happens `on the fly`.
//!
//! Intermediate buffer, used to send data in the codec splitted onto two parts.
//! While one part of the buffer is played, second one is populated with new data.
//! See event_handler() to understand how it works.

#include "target.hpp"

#include <ecl/iostream.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/thread/semaphore.hpp>
#include <algorithm>

//! Audio file in a form of C array
//! 16 bit per sample, mono, 8 kHz
#include "audio.inc"

extern "C"
void board_init()
{
    gpio_init_generated();
}

//! 16 bits per one sample.
static constexpr size_t bps = 16;

//! Total amount of samples in the audio.
//! Note that bytes_per_sample = bps_in / 8
static constexpr size_t samples_in = sizeof(audio_data) / (bps / 8);

//! Amount of channels in the outgoing buffer. Stereo.
static constexpr size_t channels_out = 2;

//! Amount of samples in the outgoing buffer.
static constexpr size_t samples_out = 8192;

//! Amount of samples in 1/2 of the outgoing buffer.
static constexpr size_t samples_out_half = samples_out / 2;

//! Outgoing buffer.
//! 16 bit per sample, stereo, 8 kHz.
static uint16_t samples[samples_out * channels_out] = {};

//! Counter. Counts how many samples left to send.
static size_t samples_left = samples_in;

//! Iterator on the audio data buffer.
static const uint16_t *data_ptr = audio_data;

//! Creates stereo channel in the outgoing buffer from the mono channel.
static void copy_mono_to_stereo(const uint16_t *from, uint16_t *to, size_t count)
{
    for (size_t in = 0; in < count; in++) {
        size_t out = in * 2;
        to[out + 1] = to[out] = from[in];
    }
}

//! Handles events from codec.
static void event_handler(ecl::bus_event type)
{
    // Determine amount of samples to write.
    size_t samples_to_write = std::min(samples_left, samples_out_half);

    // Incoming audio is in mono, must be populated in the buffer as stereo.

    if (type == ecl::bus_event::ht) {

        // Half of transfer is complete. First part of the buffer can be written
        // with samples.

        copy_mono_to_stereo(data_ptr, samples, samples_to_write);
    } else {

        // Second part of the transfer is complete. Second part of the buffer
        // can be populated with samples.

        copy_mono_to_stereo(data_ptr, samples + samples_out_half * channels_out, samples_to_write);
    }

    // Advance counters.

    samples_left -= samples_to_write;
    data_ptr += samples_to_write;

    if (!samples_left) {

        // No more samples to write - let's start from the beginning.

        data_ptr = audio_data;
        samples_left = samples_in;
    }
}

int main()
{
    ecl::err rc;

    ecl::cout << "Starting audio play..." << ecl::endl;

    // Usual init sequence.

    rc = ecl::cs43l22_instance::init();
    ecl_assert(ecl::is_ok(rc));
    rc = ecl::cs43l22_instance::power_up();
    ecl_assert(ecl::is_ok(rc));

    // Set volumes, so audio can be heard.

    rc = ecl::cs43l22_instance::set_master_volume(0x90);
    ecl_assert(ecl::is_ok(rc));
    rc = ecl::cs43l22_instance::set_headphone_volume(0xe0);
    ecl_assert(ecl::is_ok(rc));
    rc = ecl::cs43l22_instance::headphone_unmute();
    ecl_assert(ecl::is_ok(rc));

    // Set audio properties.
    rc = ecl::cs43l22_instance::set_sampling_frequency<ecl::i2s::audio_frequency::k8>();
    ecl_assert(ecl::is_ok(rc));

    // Start audio streaming. Events from codec will be handled in event_handler() routine.
    rc = ecl::cs43l22_instance::pcm_stream_start(samples, sizeof(samples) / (bps / 8), event_handler);
    ecl_assert(ecl::is_ok(rc));

    for(;;);
    return 0;
}
