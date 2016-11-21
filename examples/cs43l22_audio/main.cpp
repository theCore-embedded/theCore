#include "target.hpp"

#include <ecl/iostream.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/thread/semaphore.hpp>
#include <algorithm>

// Audio file in a form of C array
#include "audio.inc"

using ecl::cout;
using ecl::endl;
using ecl::semaphore;

extern "C"
void board_init()
{
    gpio_init();
}

static constexpr size_t samples_per_send = 8192;
static constexpr size_t samples_per_part = samples_per_send / 2;
static constexpr size_t total_samples = sizeof(audio_data) / 2; // 16 bit mono
static size_t samples_left = total_samples;

static uint16_t samples[samples_per_send * 2] = {}; // Stereo buffer

static const uint16_t *data_ptr = audio_data;


static void copy_mono_to_stereo(const uint16_t *from, uint16_t *to, size_t count)
{
    for (size_t in = 0, out = 0; in < count; in++, out += 2) {
        to[out + 1] = to[out] = from[in];
    }
}

static void cplt2(ecl::bus_event type)
{
    size_t samples_to_write = std::min(samples_left, samples_per_part);

    // Audio is in mono, should be populated in the buffer as stereo

    if (type  == ecl::bus_event::ht) {
        copy_mono_to_stereo(data_ptr, samples, samples_to_write);
    } else {
        copy_mono_to_stereo(data_ptr, samples + samples_per_part * 2, samples_to_write);
    }

    samples_left -= samples_to_write;
    data_ptr += samples_to_write;

    if (!samples_left) {
        data_ptr = audio_data;
        samples_left = total_samples;
    }
}


int main()
{
    (void) samples;

    ecl::cout << "Starting audio play..." << ecl::endl;
    ecl::cs43l22_instance::init();
    ecl::cs43l22_instance::power_up();

    ecl::cs43l22_instance::set_master_volume(0x90);
    ecl::cs43l22_instance::set_headphone_volume(0xe0);

    ecl::cs43l22_instance::headphone_unmute();
    ecl::cs43l22_instance::set_sampling_frequency<ecl::i2s::audio_frequency::k8>();
    ecl::cs43l22_instance::pcm_stream_start(samples, sizeof(samples)/2, cplt2);

    for(;;);
    return 0;
}
