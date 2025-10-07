#include "audio.hpp"
#include "cube.hpp"
#include "window.hpp"

#include <shaders/shader_builder.h>
#include <shaders/shader_program.h>

#include <analysis/analysis.hpp>
#include <analysis/hann_window.hpp>
#include <graphics/concrete_renderer.hpp>
#include <graphics/draw_buffer.hpp>

// #include "test.hpp"


// More testing for why things arent working
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gsl/assert>

#include <cxxopts.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


// #include "GLAbstractions/vao.h"
// #include "GLAbstractions/vbo.h"
// #include "GLAbstractions/vertex_attribute.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <complex>
#include <cstring>
#include <iostream>
#include <optional>
#include <ranges>
// #include <mutex>

// #include "graphics/draw_buffer.h"
// #include "graphics/draw_data2.h"
// #include "graphics/draw_data3.h"
// #include "graphics/entity.h"
// #include "Renderer/concrete_renderer.h"
// #include "render_builder.h"

// #include "DataStructures/byte_array.h"

namespace {

    static std::atomic_uint8_t global_volume = 50;

    struct ProgramArgs {
        std::string vs_path;
        std::string gs_path;
        std::string fs_path;
        std::string audio_path;
    };

    auto parse_program_args(int argc, char** argv) -> std::optional<ProgramArgs> {
        cxxopts::Options options("WavyTune", "Simple audio visualiser");

        // clang-format off
        options.add_options()
          ("h,help", "Show help", cxxopts::value<std::string>())
          ("v,vertex-shader", "Vertex shader path", cxxopts::value<std::string>())
          ("f,frag-shader", "Fragment shader", cxxopts::value<std::string>())
          ("g,geo-shader", "Geometry shader", cxxopts::value<std::string>())
          ("a,audio-file", "Audio file", cxxopts::value<std::string>());
        // clang-format on

        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return std::nullopt;
        }

        ProgramArgs args;
        if (!result.count("vertex-shader")) {
            return std::nullopt;
        }
        if (!result.count("frag-shader")) {
            return std::nullopt;
        }
        if (!result.count("audio-file")) {
            return std::nullopt;
        }
        if (result.count("geo-shader")) {
            args.gs_path = result["geo-shader"].as<std::string>();
        }

        args.vs_path    = result["vertex-shader"].as<std::string>();
        args.fs_path    = result["frag-shader"].as<std::string>();
        args.audio_path = result["audio-file"].as<std::string>();

        return args;
    }

    // [[nodiscard]] ma_uint32 read_from_buffer(
    //     ma_pcm_rb* buffer, void* data, ma_uint32 frames, ma_format format, std::size_t channels) {
    //
    //     void* source        = nullptr;
    //     ma_uint32 read_size = frames;
    //     ma_result result    = ma_pcm_rb_acquire_read(buffer, &read_size, &source);
    //
    //     if (result != MA_SUCCESS) {
    //         return 0;
    //     }
    //
    //     if (read_size == 0) {
    //         // TODO : Should probably return something else
    //         return 0;
    //     }
    //
    //     ma_uint32 actually_read = std::min(read_size, frames);
    //     memcpy(data, source, actually_read * ma_get_bytes_per_sample(format) * channels);
    //
    //     result = ma_pcm_rb_commit_read(buffer, actually_read);
    //     Expects((result == MA_SUCCESS) || (result == MA_AT_END));
    //     return actually_read;
    // }

    /**
     * @brief This function will scale the volume down depending on
     * what volume level is passed in.

 *

     * * *
     * @param device the audio device.
     * @param output the buffer with the output audio data.
     *

     * * @param
     *
     * frames_read how many frames the output buffer has.
     * @param vol the volume, must be

     * * 0-255.
     */

    // TODO : This may ignore items in the end if the input size,
    // TODO : isn't divible exactly by output size
    // Packs the array into an array of length Size
    template <std::size_t OutSize, std::size_t InSize>
    std::array<float, OutSize> bin_pack(std::array<float, InSize> const& data) {
        static_assert(InSize > OutSize);
        constexpr std::size_t window_size = InSize / OutSize;

        std::array<float, OutSize> ret;
        for (std::size_t i = 0; i < OutSize; ++i) {

            float total = 0;
            for (std::size_t j = 0; j < window_size; ++j) {
                total += data[i * window_size + j];
            }

            ret[i] = total / window_size;
        }

        return ret;
    }

    template <typename T, std::size_t Size>
    std::array<T, Size> normalize(std::array<T, Size> const& in, T const& multiplier) {
        static_assert(Size > 0);

        auto const max = *std::max_element(begin(in), end(in));
        std::array<T, Size> ret;

        std::transform(begin(in), end(in), begin(ret), [=](auto const& val) { return multiplier * val / max; });
        return ret;
    }
} // namespace

template <class T, size_t N>
constexpr size_t size(T (&)[N]) {
    return N;
}


// MARK: Global configs
// TODO : encapsulate all of this global look at stuff
// TODO : YASSS gal, this comment is still valid

// camera state
static glm::vec3 eye;
static glm::vec3 pos;
static glm::vec3 up;
static glm::mat4 look_at;


// enum class AXIS { X, Y, Z };
// glm::vec3 rotateVector(const glm::vec3& vector, const glm::vec3& from, const AXIS& axis, const float& angle) {
//     const glm::vec3 INV_VECTOR{-from.x, -from.y, -from.z};
//
//     switch (axis) {
//     case AXIS::X:
//         {
//             const glm::vec3 ROT_AXIS{1, 0, 0};
//             const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
//             glm::vec3 retVal =
//                 ROT_MATRIX * glm::vec4{vector.x + INV_VECTOR.x, vector.y + INV_VECTOR.y, vector.z + INV_VECTOR.z, 1};
//
//             return retVal - INV_VECTOR;
//         }
//     case AXIS::Y:
//         {
//             const glm::vec3 ROT_AXIS{0, 1, 0};
//             const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
//             glm::vec3 retVal =
//                 ROT_MATRIX * glm::vec4{vector.x + INV_VECTOR.x, vector.y + INV_VECTOR.y, vector.z + INV_VECTOR.z, 1};
//             return retVal - INV_VECTOR;
//         }
//     case AXIS::Z:
//         {
//             const glm::vec3 ROT_AXIS{0, 0, 1};
//             const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
//             glm::vec3 retVal =
//                 ROT_MATRIX * glm::vec4{vector.x + INV_VECTOR.x, vector.y + INV_VECTOR.y, vector.z + INV_VECTOR.z, 1};
//             return retVal - INV_VECTOR;
//         }
//     }
// }
//
// MARK: Event callbacks (mouse, key)

// MARK: End Event callbacks (mouse, key)

unsigned vaoId          = 0;
unsigned vertexBufferId = 0;
unsigned normalBufferId = 0;

// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR
// pCmdLine, int nCmdShow)
int main(int argc, char** argv) {

    auto const args = parse_program_args(argc, argv);
    if (!args) {
        std::cout << "could not parse arguments";
        return 0;
    }

    wt::AudioPlayer player;
    player.play(args->audio_path);

    // Testing the fourier shit
    std::function<double(const std::complex<double>&)> applier = [](const std::complex<double>& v) -> double {
        return std::abs(v);
    };


    // TODO : Where is this from?
    // auto result = apply(applier, transform);

    glewExperimental = GL_TRUE;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    auto window = wt::Window{800, 600, "Testing Out Stream"};

    glewInit(); // Initialise all the openGL macros

    // MARK: Creating shader & renderers
    ShaderBuilder shader_builder;

    auto shader_program = shader_builder.set_vertex_shader(args->vs_path)
                              .set_geometry_shader(args->gs_path)
                              .set_fragment_shader(args->fs_path)
                              .build();

    if (!shader_program) {
        std::cout << "could not build the shader program\n";
        return -1;
    }

    ConcreteRenderer renderer{std::move(shader_program)};

    renderer.queue_data(wt::unit_cube());
    renderer.send_gpu_data();

    // creating all the offsets
    std::vector<glm::vec3> offsets;
    offsets.reserve(100);
    for (auto const& elem :
        std::views::iota(0, 100) | std::views::transform([](auto const& i) { return glm::vec3{i, 0.0f, 0.0}; })) {
        offsets.push_back(std::move(elem));
    }


    // MARK: End shader objects


    // MARK: Projection maths
    glm::mat4 proj = glm::perspective<float>(glm::radians(45.0f), 800 / 600.0, 0.01, 100);
    // MARK: End projection maths

    eye = {0, 0, 5};
    pos = {0, 0, -1};
    up  = {0, 1, 0};


    std::array<std::int32_t, wt::analysis::WINDOW_SIZE> raw_audio{}; // TODO : needs to have double signal size

    wt::analysis::FftAnalyzer analyzer;
    auto const hann_coefficients_input  = wt::analysis::make_hann_coefficients<wt::analysis::WINDOW_SIZE>();
    auto const hann_coefficients_output = wt::analysis::make_hann_coefficients<wt::analysis::WINDOW_SIZE / 2 + 1>();
    analyzer.set_preprocessor([&](std::array<float, wt::analysis::WINDOW_SIZE>& buffer) {
        // TODO : We want to apply the hann coefficients to the array
        for (std::size_t i = 0; i < wt::analysis::WINDOW_SIZE; ++i) {
            buffer[i] *= hann_coefficients_input[i];
        }
    });
    // analyzer.set_postprocessor([](std::array<std::complex<float>, wt::analysis::WINDOW_SIZE>& buffer) {
    //     for (std::size_t i = 0; i < wt::analysis::WINDOW_SIZE; ++i) {
    //         std::complex<float> res = buffer[i];
    //         buffer[i].real(std::norm(res));
    //         buffer[i].imag(0.0f);
    //     }
    // });

    // Game loop - Main OpenGL rendering
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    std::size_t sample_counter = 0;
    std::array<float, 100> heights;
    while (!window.closed()) {

        // TODO : Need to figure out why read_from_buffer is reading
        // TODO : dodgy data from the output into the raw_audio
        // TODO : buffer.
        // std::uint32_t samples_to_read = std::max(2u, static_cast<std::uint32_t>(raw_audio.size() - sample_counter)) / 2;
        // sample_counter += read_from_buffer(&user_data.buffer, raw_audio.data() + sample_counter, samples_to_read,
        //                       device.playback.format, device.playback.channels)
        //                 * device.playback.channels;

        // TODO : There's an issue where we have left over frames
        sample_counter = sample_counter < raw_audio.size() ? sample_counter : 0;

        // lookAt = glm::lookAt(cam.pos, cam.pos + cam.getDirection(), cam.getUp());
        auto const cam = window.camera();
        look_at        = glm::lookAt(cam.pos, cam.pos + cam.getDirection(), cam.getUp());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // MARK: Sound analysis

        // This will transform the complex output of the FFT
        // analysis into an array of floats representing the
        // frequency amount
        // auto const transform_complex = analyzer.analyze(wt::test::sin_10);
        if (sample_counter == 0) {
            // convert integer to floats for analysis
            // std::array<float, wt::analysis::WINDOW_SIZE> audio_floats;
            // s32_to_f32(audio_floats.data(), raw_audio.data(), raw_audio.size());

            // auto const transform_complex = analyzer.analyze(audio_floats);
            // std::array<float, wt::analysis::WINDOW_SIZE / 2 + 1> transform{};
            //
            // std::transform(begin(transform_complex), end(transform_complex), begin(transform),
            //     [](std::complex<float> in) { return std::abs(in); });
            //
            // // Remove the energy at (0)
            // for (int i = 0; i < transform.size(); i++) {
            //     transform[i] *= hann_coefficients_output[i];
            // }
            // heights = bin_pack<100>(transform);
            // heights = normalize(heights, 10.0f);
            for (int i = 0; i < 100; ++i) {
              heights[i] = 1.0f;
            }
        }
        // MARK: Sound analysis


        // std::vector<float> heights(100);
        Expects(heights.size() == 100);
        Expects(offsets.size() == 100);

        renderer.render(proj, look_at, window.rotation(), heights, offsets);
        glBindVertexArray(0);

        window.process_frame();
    }

    return 0;
}
