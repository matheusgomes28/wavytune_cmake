#ifndef AUDIO_H
#define AUDIO_H

#include <atomic>
#include <functional>
#include <memory>
#include <miniaudio.h>
#include <string>

extern "C" {
struct ma_decoder;
struct ma_device;
struct ma_device_config;

typedef struct ma_pcm_rb ma_pcm_rb;
}

namespace wt {


    template <typename T>
    using CustomPtr = std::unique_ptr<T, std::function<void(T*)>>;

    using DeviceConfigPtr = CustomPtr<ma_device_config>;
    using DevicePtr       = CustomPtr<ma_device>;
    using DecoderPtr      = CustomPtr<ma_decoder>;
    using RingBufferPtr   = CustomPtr<ma_pcm_rb>;

    class AudioPlayer;

    struct AudioUserData {
        DecoderPtr decoder;
        RingBufferPtr buffer;
        AudioPlayer* instance;
    };

    class AudioPlayer {

    public:
        static void data_callback(ma_device* device, void* output, const void* input, std::uint32_t frame_count);

        AudioPlayer();
        bool play(std::string const& audio_file);

        // std::array<WINDOW_SIZE> current_samples() const;
    private:
        AudioUserData _user_data;
        DevicePtr _device;
        DeviceConfigPtr _device_config;
        std::string _current_file;
        std::atomic_uint8_t _volume;
    };

} // namespace wt

#endif // AUDIO_H
