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

#define MINIAUDIO_IMPLEMENTATION
extern "C" {
#include <miniaudio.h>
}

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

    static std::atomic_uint8_t global_volume = 127;

    struct AudioData {
        ma_decoder decoder;
        ma_pcm_rb buffer;
    };

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

    /**
     * @brief Converts the audio int32 data in source to float32,
     * by normalising all values between

     * * [-1.0f, 1.0f].
     * @param destination the destination buffer, must have pre-allcoated
     * `size *
     *
     * sizeof(float)` bytes ready to be overriden. 
     * @param source the source array of values to convert, must

     * * have
     * `size * sizeof(int32)` bytes to be read from.
     */
    void s32_to_f32(void* destination, void* source, std::size_t size) {
        static_assert(sizeof(ma_float) == sizeof(ma_int32));

        // Running buffer to convert values
        constexpr std::size_t buffer_size = 512;
        std::array<float, buffer_size> buffer{};

        // calculate how many runs of the buffer we need
        std::size_t const whole_buffers = size / buffer_size;
        std::size_t const remaining     = size - (whole_buffers * buffer_size);
        Expects(remaining < buffer_size);

        std::size_t buffer_offset = 0;
        for (std::size_t i = 0; i < whole_buffers; ++i, ++buffer_offset) {

            // convert each one of them
            for (std::size_t j = 0; j < buffer_size; ++j) {
                buffer[j] = static_cast<ma_int32*>(source)[buffer_offset + j] / 2147483648.0f;
            }
            memcpy(static_cast<float*>(destination) + buffer_offset, buffer.data(),
                buffer_size * ma_get_bytes_per_sample(ma_format_s32));
        }

        for (std::size_t j = 0; j < remaining; ++j) {
            buffer[j] = static_cast<ma_int32*>(source)[buffer_offset + j];
        }
        memcpy(static_cast<float*>(destination) + buffer_offset, buffer.data(),
            remaining * ma_get_bytes_per_sample(ma_format_s32));
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

    /**
     * @brief Read data from the current circular audio buffer into the given
     * data buffer.
     * @param
     * buffer the miniaudio circular buffer.
     * @param data the destination for the read data.
     * @param frames
     * the desired number of frames to read.
     * @param format the format of the device playback.
     * @param
     * channels number of channels.
     * @return the number of samples actually read into the data buffer.
     */
    [[nodiscard]] ma_uint32 read_from_buffer(
        ma_pcm_rb* buffer, void* data, ma_uint32 frames, ma_format format, std::size_t channels) {

        void* source        = nullptr;
        ma_uint32 read_size = frames;
        ma_result result    = ma_pcm_rb_acquire_read(buffer, &read_size, &source);

        if (result != MA_SUCCESS) {
            return 0;
        }

        if (read_size == 0) {
            // TODO : Should probably return something else
            return 0;
        }

        ma_uint32 actually_read = std::min(read_size, frames);
        memcpy(data, source, actually_read * ma_get_bytes_per_sample(format) * channels);

        result = ma_pcm_rb_commit_read(buffer, actually_read);
        Expects((result == MA_SUCCESS) || (result == MA_AT_END));
        return actually_read;

        // Read the entire bytes
        // if ((result == MA_SUCCESS) || (result == MA_AT_END)) {
        //     return actually_read;
        // }
        // return (result == MA_SUCCESS) || (result == MA_AT_END);
    }

    /**
     * @brief This function will scale the volume down depending on
     * what volume level is passed in.

     * * @param device the audio device.
     * @param output the buffer with the output audio data.
     * @param
     * frames_read how many frames the output buffer has.
     * @param vol the volume, must be 0-255.
     */
    void multiply_volume(ma_device* device, void* output, ma_uint64 frames_read, std::uint8_t vol) {
        Expects(output != nullptr);
        Expects(device != nullptr);
        Expects(frames_read > 0);

        // TODO : Go through "frames_read" frames * channel count in "pOutput"
        float constexpr max_volume = 127.0f;
        float const volume_level   = vol / max_volume;

        // TODO : Change volume by scaling down the values in output (might be int, or float)
        auto const format = device->playback.format;
        if (format == ma_format_s32) { // signed 32-bit ints
            std::int32_t* buffer = static_cast<std::int32_t*>(output);
            for (ma_uint64 i = 0; i < frames_read * device->playback.channels; ++i) {
                const std::int32_t sample = buffer[i];
                // const std::int32_t modified_sample = static_cast<std::int32_t>(volume_level * sample);
                buffer[i] = static_cast<std::int32_t>(volume_level * sample);
            }
        } else if (format == ma_format_f32) { // signed 32-bit floats
            float* buffer = static_cast<float*>(output);
            for (ma_uint64 i = 0; i < frames_read; ++i) {
                buffer[i] *= volume_level;
            }
        }
    }

    void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count) {
        AudioData* user_data = (AudioData*) device->pUserData;
        if (user_data == NULL) {
            return;
        }

        ma_uint64 frames_read = 0;
        ma_decoder_read_pcm_frames(&user_data->decoder, output, frame_count, &frames_read);

        std::uint8_t const volume = global_volume.load();
        multiply_volume(device, output, frames_read, volume);


        // figure out whether we need a new buffer for
        write_to_buffer(&user_data->buffer, output, frames_read, device->playback.format, device->playback.channels);
        (void) input;
    }

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

struct Camera {
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    glm::mat4 transform;

    glm::vec3 getDirection() const {
        return transform * glm::vec4(front, 1);
    }

    glm::vec3 getUp() const {
        return transform * glm::vec4(up, 1);
    }

    glm::vec3 getRight() const {
        return transform * glm::vec4(right, 1);
    }
};

void resizeCallback(GLFWwindow* /* window */, int width, int height) {
    glViewport(0, 0, width, height);
}

// MARK: Global configs
// TODO : encapsulate all of this global look at stuff
// TODO : YASSS gal, this comment is still valid

// camera state
static Camera cam;
static glm::vec3 eye;
static glm::vec3 pos;
static glm::vec3 up;
static glm::mat4 look_at;

// Mouse related events
static bool mouse_down  = false;
static bool first_mouse = true;
static double last_x    = 0.0;
static double last_y    = 0.0;

// Rotation metrix stuff
static glm::mat4 rot      = glm::mat4(1.0f);
static glm::mat4 curr_rot = glm::mat4(1.0f);
// MARK: end Global configs


enum class AXIS { X, Y, Z };
glm::vec3 rotateVector(const glm::vec3& vector, const glm::vec3& from, const AXIS& axis, const float& angle) {
    const glm::vec3 INV_VECTOR{-from.x, -from.y, -from.z};

    switch (axis) {
    case AXIS::X:
        {
            const glm::vec3 ROT_AXIS{1, 0, 0};
            const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
            glm::vec3 retVal =
                ROT_MATRIX * glm::vec4{vector.x + INV_VECTOR.x, vector.y + INV_VECTOR.y, vector.z + INV_VECTOR.z, 1};

            return retVal - INV_VECTOR;
        }
    case AXIS::Y:
        {
            const glm::vec3 ROT_AXIS{0, 1, 0};
            const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
            glm::vec3 retVal =
                ROT_MATRIX * glm::vec4{vector.x + INV_VECTOR.x, vector.y + INV_VECTOR.y, vector.z + INV_VECTOR.z, 1};
            return retVal - INV_VECTOR;
        }
    case AXIS::Z:
        {
            const glm::vec3 ROT_AXIS{0, 0, 1};
            const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
            glm::vec3 retVal =
                ROT_MATRIX * glm::vec4{vector.x + INV_VECTOR.x, vector.y + INV_VECTOR.y, vector.z + INV_VECTOR.z, 1};
            return retVal - INV_VECTOR;
        }
    }
}

// MARK: Event callbacks (mouse, key)
void key_cb(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */) {
    if (key == GLFW_KEY_W && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.transform = glm::rotate(glm::radians(10.f), cam.getRight()) * cam.transform;
    }
    if (key == GLFW_KEY_S && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.transform = glm::rotate(glm::radians(-10.f), cam.getRight()) * cam.transform;
    }
    if (key == GLFW_KEY_A && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.transform = glm::rotate(glm::radians(10.f), cam.getUp()) * cam.transform;
    }
    if (key == GLFW_KEY_D && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.transform = glm::rotate(glm::radians(-10.f), cam.getUp()) * cam.transform;
    }

    // Movements
    if (key == GLFW_KEY_UP && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.pos += cam.getDirection() * 0.5f;
    }
    if (key == GLFW_KEY_DOWN && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.pos -= cam.getDirection() * 0.5f;
    }
    if (key == GLFW_KEY_RIGHT && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.pos += cam.getRight() * 0.5f;
    }
    if (key == GLFW_KEY_LEFT && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        cam.pos -= cam.getRight() * 0.5f;
    }

    // Audio related
    constexpr std::uint8_t VOLUME_STEP = 2u;
    if (key == GLFW_KEY_J && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        auto const current_volume = global_volume.load();

        constexpr std::uint8_t MAX_VOLUME = 127u;
        if (current_volume < MAX_VOLUME - VOLUME_STEP) {
            global_volume.fetch_add(VOLUME_STEP);
            std::cout << "volume: " << static_cast<int>(current_volume + VOLUME_STEP) << "\n";
        }
    }
    if (key == GLFW_KEY_K && ((action == GLFW_PRESS) || (action == GLFW_REPEAT))) {
        auto const current_volume = global_volume.load();

        constexpr std::uint8_t MIN_VOLUME = 0u;
        if (current_volume > MIN_VOLUME + VOLUME_STEP) {
            global_volume.fetch_sub(VOLUME_STEP);
            std::cout << "volume: " << static_cast<int>(current_volume - VOLUME_STEP) << "\n";
        }
    }
}

void mouse_button_cb(GLFWwindow* /* window */, int button, int action, int /* mods */) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_down = true;
            // Reset first mouse flag to true when mouse is clicked
            first_mouse = true;
        } else if (action == GLFW_RELEASE) {
            mouse_down = false;
            // Save the current rotation when mouse is released
            rot = curr_rot;
        }
    }
}

void cursor_cb(GLFWwindow* /* window */, double xpos, double ypos) {
    if (!mouse_down) {
        return;
    }

    if (first_mouse) {
        last_x      = xpos;
        last_y      = ypos;
        first_mouse = false;
        return;
    }

    // Calculate mouse movement delta
    float xoffset = xpos - last_x;
    float yoffset = ypos - last_y;
    last_x        = xpos;
    last_y        = ypos;

    // Sensitivity factor to control rotation speed
    const float sensitivity = 0.005f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Create rotation matrices for x and y rotation
    glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), yoffset, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), xoffset, glm::vec3(0.0f, 1.0f, 0.0f));

    // Combine rotations (order matters!)
    glm::mat4 newRotation = yRotation * xRotation;

    // Apply to the existing rotation matrix
    curr_rot = newRotation * curr_rot;

    // Print current rotation matrix for debugging
    std::cout << "Current Rotation Matrix:" << std::endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << curr_rot[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}
// MARK: End Event callbacks (mouse, key)

unsigned vaoId          = 0;
unsigned vertexBufferId = 0;
unsigned normalBufferId = 0;

// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR
// pCmdLine, int nCmdShow)
int main(int argc, char** argv) {

    auto const args = parse_program_args(argc, argv);
    if (!args) {
        return 0;
    }

    // MARK: Begin of audio
    ma_result result;
    ma_device_config deviceConfig;
    ma_device device;
    AudioData user_data{};

    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    result = ma_pcm_rb_init(ma_format_f32, 2, 1024, nullptr, nullptr, &user_data.buffer);

    // std::vector<float> test_write(20, 10);
    // std::vector<float> test_read(20);
    // if (!write_to_buffer(&user_data.buffer, test_write.data(), 10, ma_format_f32, 2)) {
    //     return -1;
    // }
    // if (!read_from_buffer(&user_data.buffer, test_read.data(), 20, ma_format_f32, 2)) {
    //     return -1;
    // }

    if (result != MA_SUCCESS) {
        printf("Could not create buffer");
        return -2;
    }


    result = ma_decoder_init_file(args->audio_path.c_str(), NULL, &user_data.decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", argv[1]);
        return -2;
    }

    deviceConfig                 = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = user_data.decoder.outputFormat;
    // deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = user_data.decoder.outputChannels;
    deviceConfig.sampleRate        = user_data.decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &user_data;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&user_data.decoder);
        ma_pcm_rb_uninit(&user_data.buffer);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&user_data.decoder);
        ma_pcm_rb_uninit(&user_data.buffer);
        return -4;
    }

    // MARK: End of audio

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "TestingOut Window", NULL, NULL);
    if (!window) {
        std::cout << "Could not create window. Exiting..." << std::endl;
        glfwTerminate();
        return -1;
    }

    // Create the viewport
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwMakeContextCurrent(window);

    // Create the shader program
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

    std::vector<glm::vec3> vertices{
     // Front face
     {-0.5f, 0.0f, 0.5f}, // Bottom-left
     {0.5f, 0.0f, 0.5f}, // Bottom-right
     {0.5f, 1.0f, 0.5f}, // Top-right
     {-0.5f, 1.0f, 0.5f}, // Top-left

     // Back face
     {-0.5f, 0.0f, -0.5f}, // Bottom-left
     {0.5f, 0.0f, -0.5f}, // Bottom-right 5 2 6
     {0.5f, 1.0f, -0.5f}, // Top-right
     {-0.5f, 1.0f, -0.5f} // Top-left
    };

    std::vector<glm::vec3> normals{// Front face
     {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},

     // Back face (all normals point in -z direction)
     {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}};

    std::vector<glm::vec4> colors{// Front face
     {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f},

     // Back face
     {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 0.5f, 0.0f, 1.0f}};

    // Assuming we're looking from the front up
    std::vector<std::uint32_t> indices{// Front face
     0, 3, 1, 1, 3, 2,
     // Back face
     5, 4, 7, 5, 6, 7,
     // Top face
     2, 7, 6, 2, 3, 7,
     // Bottom Face
     1, 4, 0, 1, 5, 4,
     // Left face
     0, 4, 3, 4, 7, 3,
     // Right face
     1, 2, 5, 5, 2, 6};

    DrawBufferBuilder buffer_builder;
    DrawBuffer buffer = buffer_builder.add_vertices(std::move(vertices))
                            .add_normals(std::move(normals))
                            .add_colors(std::move(colors))
                            .add_indices(std::move(indices))
                            .build();
    renderer.queue_data(std::move(buffer));
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

    // Callback from commands
    glfwSetKeyCallback(window, key_cb);
    glfwSetMouseButtonCallback(window, mouse_button_cb);
    glfwSetCursorPosCallback(window, cursor_cb);

    cam.pos       = {0, 0, 10};
    cam.front     = {0, 0, -1};
    cam.right     = {1, 0, 0};
    cam.up        = {0, 1, 0};
    cam.transform = glm::mat4{1};

    eye = {0, 0, 5};
    pos = {0, 0, -1};
    up  = {0, 1, 0};


    std::array<float, wt::analysis::WINDOW_SIZE> raw_audio{}; // TODO : needs to have double signal size

    wt::analysis::FftAnalyzer analyzer;
    auto const hann_coefficients = wt::analysis::make_hann_coefficients<wt::analysis::WINDOW_SIZE>();
    // analyzer.set_preprocessor([&](std::array<std::complex<float>, wt::analysis::WINDOW_SIZE>& buffer) {
    //     // TODO : We want to apply the hann coefficients to the array
    //     for (std::size_t i = 0; i < wt::analysis::WINDOW_SIZE; ++i) {
    //         buffer[i] *= hann_coefficients[i];
    //     }
    // });
    analyzer.set_postprocessor([](std::array<std::complex<float>, wt::analysis::WINDOW_SIZE>& buffer) {
        for (std::size_t i = 0; i < wt::analysis::WINDOW_SIZE; ++i) {
            std::complex<float> res = buffer[i];
            buffer[i].real(std::norm(res));
            buffer[i].imag(0.0f);
        }
    });

    // Game loop - Main OpenGL rendering
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    std::size_t sample_counter = 0;
    while (!glfwWindowShouldClose(window)) {

        // TODO : Need to figure out why read_from_buffer is reading
        // TODO : dodgy data from the output into the raw_audio
        // TODO : buffer.
        std::size_t samples_to_read = raw_audio.size() - sample_counter;
        sample_counter += read_from_buffer(&user_data.buffer, raw_audio.data() + sample_counter, samples_to_read / 2,
        ma_format_f32, device.playback.channels);

        sample_counter = sample_counter != raw_audio.size() ? samples_to_read : 0;

        // lookAt = glm::lookAt(cam.pos, cam.pos + cam.getDirection(), cam.getUp());
        look_at = glm::lookAt(cam.pos, cam.pos + cam.getDirection(), cam.getUp());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // MARK: Sound analysis

        // This will transform the complex output of the FFT
        // analysis into an array of floats representing the
        // frequency amount
        // auto const transform_complex = analyzer.analyze(wt::test::sin_10);
        std::array<float, 100> heights{};
        if (sample_counter == 0) {
            auto const transform_complex = analyzer.analyze(raw_audio);
            std::array<float, wt::analysis::WINDOW_SIZE> transform{};

            std::transform(begin(transform_complex), end(transform_complex), begin(transform),
            [](std::complex<float> in) { return in.real(); });

            heights = normalize(bin_pack<100>(transform), 10.0f);
        }
        // MARK: Sound analysis


        // std::vector<float> heights(100);
        Expects(heights.size() == 100);
        Expects(offsets.size() == 100);
        renderer.render(proj, look_at, curr_rot, heights, offsets);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ma_device_uninit(&device);
    ma_decoder_uninit(&user_data.decoder);
    ma_pcm_rb_uninit(&user_data.buffer);
    return 0;
}
