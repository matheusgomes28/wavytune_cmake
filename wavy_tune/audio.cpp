#include "audio.hpp"

#include <fmt/format.h>
#include <gsl/assert>

#define MINIAUDIO_IMPLEMENTATION
extern "C" {
#include <miniaudio.h>
}

namespace {

    wt::AudioUserData make_default_user_data(wt::AudioPlayer* parent_instance) {
        return wt::AudioUserData{.decoder = wt::DecoderPtr(new ma_decoder, [](auto* ptr) { ma_decoder_uninit(ptr); }),
         .buffer                          = wt::RingBufferPtr(new ma_pcm_rb, [](auto* ptr) { ma_pcm_rb_uninit(ptr); }),
         .instance                        = parent_instance};
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

    /*
     * * *
     * @param device the audio device.
     * @param output the buffer with the output audio
     * data.





     * * * * * @param n_frames how many frames the output buffer has.
     * @param vol the volume,
     * must be 0-255.
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

    /**
     * @brief Converts the audio int32 data in source to float32,
     * by normalising all values between
     * [-1.0f, 1.0f].
     * @param destination the destination buffer, must have pre-allcoated
     * * `size
     * * sizeof(float)` bytes ready to be overriden. 
     * @param source
     * the
     * source
     * array of
     * * values to
     * convert, must
     * * * have
     * `size *
     * sizeof(int32)`
     * bytes to be read
     * from.
     */
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

        // Conversion may be needed!
        ma_uint32 const written_size = std::min(frames, write_size);
        if (format == ma_format_f32) {
            memcpy(destination, data, written_size * ma_get_bytes_per_sample(format) * channels);
        } else if (format == ma_format_s32) {
            // TODO : This takes the size in frames, not bytes
            s32_to_f32(destination, data, written_size * channels);
        }

        return ma_pcm_rb_commit_write(buffer, written_size) == MA_SUCCESS;
    }
} // namespace

namespace wt {

    /* Static Methods */
    void AudioPlayer::data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count) {
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


        Expects(device->playback.format == ma_format_f32);
        write_to_buffer(user_data->buffer.get(), output, frames_read, device->playback.format, device->playback.channels);
        (void) input;
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

        ma_result result = MA_SUCCESS;
        result           = ma_decoder_init_file(_current_file.c_str(), NULL, _user_data.decoder.get());
        if (result != MA_SUCCESS) {
            fmt::println("Could not load audio file: {:s}", _current_file);
            return false;
        }

        _device_config->playback.format = _user_data.decoder->outputFormat;
        // deviceConfig.playback.format   = ma_format_f32;
        _device_config->playback.channels = _user_data.decoder->outputChannels;
        _device_config->sampleRate        = _user_data.decoder->outputSampleRate;
        _device_config->dataCallback      = data_callback;
        _device_config->pUserData         = &_user_data;

        return true;
    }

}; // namespace wt
