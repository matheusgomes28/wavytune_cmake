#include "audio.hpp"

#include <fmt/format.h>
#include <gsl/assert>

#define MINIAUDIO_IMPLEMENTATION
extern "C" {
#include <miniaudio.h>
}

#include <array>
#include <cstdint>
#include <limits>
#include <string>

namespace {

    wt::AudioUserData make_default_user_data(wt::AudioPlayer* parent_instance) {
        // clang-format off
        return wt::AudioUserData{
          .decoder  = wt::DecoderPtr(new ma_decoder, [](auto* ptr) { ma_decoder_uninit(ptr); }),
          .buffer   = wt::RingBufferPtr(new ma_pcm_rb, [](auto* ptr) { ma_pcm_rb_uninit(ptr); }),
          .instance = parent_instance
        };
        // clang-format on
    }

    wt::DevicePtr make_default_device() {
        return wt::DevicePtr{new ma_device, ma_device_uninit};
    }

    wt::DeviceConfigPtr make_default_device_config() {

        ma_device_config device_config      = ma_device_config_init(ma_device_type_playback);
        ma_device_config* device_config_ptr = new ma_device_config;

        *device_config_ptr = device_config;
        return wt::DeviceConfigPtr{device_config_ptr, [](ma_device_config* ptr) { delete ptr; }};
    }

    /**
     * Multiplies the outpub audio samples by the volume.
     *
     * @param device the device
     * @param output the output buffer pointer
     * @param n_frames the number of frames
     * @param vol the volume
     */
    void multiply_volume(ma_device* device, void* output, ma_uint64 n_frames, std::uint8_t vol) {
        Expects(output != nullptr);
        Expects(device != nullptr);
        Expects(n_frames > 0);
        Expects(vol <= 255);
        Expects(vol >= 0);

        // TODO : Go through "frames_read" frames * channel count in "pOutput"
        float constexpr max_volume = 127.0f;
        float const volume_level   = vol / max_volume;

        // TODO : Change volume by scaling down the values in output (might be int, or float)
        auto const format = device->playback.format;
        if (format == ma_format_s32) { // signed 32-bit ints
            std::int32_t* buffer = static_cast<std::int32_t*>(output);
            for (ma_uint64 i = 0; i < n_frames * device->playback.channels; ++i) {
                buffer[i] = static_cast<std::int32_t>(volume_level * buffer[i]);
            }
        } else if (format == ma_format_f32) { // signed 32-bit floats
            float* buffer = static_cast<float*>(output);
            for (ma_uint64 i = 0; i < n_frames; ++i) {
                buffer[i] *= volume_level;
            }
        }
    }

    void s32_to_f32(void* destination, void* source, std::size_t size) {
        static_assert(sizeof(ma_float) == sizeof(ma_int32));

        // Running buffer to convert values
        constexpr std::size_t buffer_size = 512;
        const auto int_sample_bytes       = ma_get_bytes_per_sample(ma_format_s32);
        const auto float_sample_bytes     = ma_get_bytes_per_sample(ma_format_f32);

        // calculate how many runs of the buffer we need
        std::size_t const whole_buffers = size / buffer_size;
        std::size_t const remaining     = size - (whole_buffers * buffer_size);
        Expects(remaining < buffer_size);

        std::array<float, buffer_size> buffer{};
        std::size_t buffer_offset = 0;
        for (std::size_t i = 0; i < whole_buffers; ++i, buffer_offset += buffer_size) {

            // convert each one of them
            for (std::size_t j = 0; j < buffer_size; ++j) {
                buffer[j] = static_cast<ma_int32*>(source)[buffer_offset + j] / 2147483648.0f;
            }
            memcpy(static_cast<float*>(destination) + buffer_offset, buffer.data(), buffer_size * int_sample_bytes);
        }

        for (std::size_t j = 0; j < remaining; ++j) {
            buffer[j] = static_cast<ma_int32*>(source)[buffer_offset + j] / 2147483648.0f;
        }
        memcpy(static_cast<float*>(destination) + buffer_offset, buffer.data(), remaining * float_sample_bytes);
    }

    void s16_to_f32(void* destination, void* source, std::size_t size) {
        // Running buffer to convert values
        constexpr std::size_t BUFFER_SIZE = 512;
        constexpr float MAX_VALUE         = std::numeric_limits<std::int16_t>::max();
        const auto in_sample_bytes        = ma_get_bytes_per_sample(ma_format_s16);
        const auto out_float_bytes        = ma_get_bytes_per_sample(ma_format_f32);

        // calculate how many runs of the buffer we need
        std::size_t const whole_buffers = size / BUFFER_SIZE;
        std::size_t const remaining     = size - (whole_buffers * BUFFER_SIZE);
        Expects(remaining < BUFFER_SIZE);

        std::array<float, BUFFER_SIZE> buffer{};
        std::size_t buffer_offset = 0;
        for (std::size_t i = 0; i < whole_buffers; ++i, buffer_offset += BUFFER_SIZE) {

            // convert each one of them
            for (std::size_t j = 0; j < BUFFER_SIZE; ++j) {
                buffer[j] = static_cast<ma_int16*>(source)[buffer_offset + j] / MAX_VALUE;
            }
            memcpy(static_cast<float*>(destination) + buffer_offset, buffer.data(), BUFFER_SIZE * in_sample_bytes);
        }

        for (std::size_t j = 0; j < remaining; ++j) {
            buffer[j] = static_cast<ma_int16*>(source)[buffer_offset + j] / MAX_VALUE;
        }
        memcpy(static_cast<float*>(destination) + buffer_offset, buffer.data(), remaining * out_float_bytes);
    }


    // Works on f32 only!
    bool write_to_buffer(ma_pcm_rb* buffer, void* data, ma_uint32 frames, ma_format format, std::size_t channels) {

        void* destination    = nullptr;
        ma_uint32 write_size = frames;

        ma_result const result = ma_pcm_rb_acquire_write(buffer, &write_size, &destination);
        if (result != MA_SUCCESS) {
            return false;
        }

        if (write_size == 0) {
            // Should be something else;
            return true;
        }

        ma_uint32 const written_size = std::min(frames, write_size);

        switch (format) {
        case ma_format_s32:
            s32_to_f32(destination, data, written_size * channels);
            break;
        case ma_format_s16:
            s16_to_f32(destination, data, written_size * channels);
            break;
        }

        return ma_pcm_rb_commit_write(buffer, written_size) == MA_SUCCESS;
    }
} // namespace

namespace wt {

    /* Static Methods */
    void AudioPlayer::data_callback(ma_device* device, void* output, const void* /* input */, ma_uint32 frame_count) {
        wt::AudioUserData* user_data = (wt::AudioUserData*) device->pUserData;
        Expects(user_data->decoder);
        Expects(user_data->buffer);
        Expects(user_data->instance != nullptr);

        if (user_data == NULL) {
            return;
        }

        ma_uint64 frames_read = 0;
        ma_decoder_read_pcm_frames(user_data->decoder.get(), output, frame_count, &frames_read);

        std::uint8_t const volume = user_data->instance->_volume.load();
        multiply_volume(device, output, frames_read, volume);

        write_to_buffer(
            user_data->buffer.get(), output, frames_read, device->playback.format, device->playback.channels);
    }
    /* End Static Methods */

    AudioPlayer::AudioPlayer()
        : _user_data{make_default_user_data(this)},
          _device{make_default_device()},
          _device_config{make_default_device_config()},
          _current_file{""},
          _volume{50} {}

    bool AudioPlayer::play(std::string const& file_name) {
        Expects(_device);
        Expects(_device_config);
        Expects(_user_data.decoder);

        _current_file = file_name;

        ma_result result = MA_SUCCESS;
        result           = ma_decoder_init_file(_current_file.c_str(), NULL, _user_data.decoder.get());
        if (result != MA_SUCCESS) {
            fmt::println("Could not load audio file: {:s}", _current_file);
            return false;
        }

        _device_config->playback.format   = _user_data.decoder->outputFormat;
        _device_config->playback.channels = _user_data.decoder->outputChannels;
        _device_config->sampleRate        = _user_data.decoder->outputSampleRate;
        _device_config->dataCallback      = data_callback;
        _device_config->pUserData         = &_user_data;

        // TODO : We probably wanna make sure we only do these things
        // TODO : once, even if we play a different file
        result = ma_pcm_rb_init(_device_config->playback.format, _device_config->playback.channels, 1024, nullptr,
            nullptr, _user_data.buffer.get());
        if (result != MA_SUCCESS) {
            fmt::println(": {:s}", _current_file);
            return false;
        }

        if (ma_device_init(NULL, _device_config.get(), _device.get()) != MA_SUCCESS) {
            fmt::println("failed to open playback device");
            ma_decoder_uninit(_user_data.decoder.get());
            ma_pcm_rb_uninit(_user_data.buffer.get());
            return false;
        }

        if (ma_device_start(_device.get()) != MA_SUCCESS) {
            fmt::println("failed to start playback device");
            ma_device_uninit(_device.get());
            ma_decoder_uninit(_user_data.decoder.get());
            ma_pcm_rb_uninit(_user_data.buffer.get());
            return false;
        }
        return true;
    }

}; // namespace wt
