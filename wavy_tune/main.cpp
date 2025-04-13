#include "matrix/matrix.h"
#include <cstring>
#include <fourier/dft_operations.h>
#include <shaders/shader_builder.h>
#include <shaders/shader_program.h>

#include <graphics/concrete_renderer.hpp>
#include <graphics/draw_buffer.hpp>

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

#include <cmath>
#include <complex>
#include <iostream>
#include <optional>
#include <ranges>
// #include <mutex>
// #include <thread>

// #include "graphics/draw_buffer.h"
// #include "graphics/draw_data2.h"
// #include "graphics/draw_data3.h"
// #include "graphics/entity.h"
// #include "Renderer/concrete_renderer.h"
// #include "render_builder.h"

// #include "DataStructures/byte_array.h"

namespace {

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

    void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        AudioData* user_data = (AudioData*) pDevice->pUserData;
        if (user_data == NULL) {
            return;
        }

        ma_uint64 frames_read = 0;
        ma_decoder_read_pcm_frames(&user_data->decoder, pOutput, frameCount, &frames_read);

        
        std::uint32_t write_size;
        void* buffer;
        ma_result result = ma_pcm_rb_acquire_write(&user_data->buffer, &write_size, &buffer);
        if (result == MA_SUCCESS) {
            // error
            const auto written_size = std::min(write_size, frameCount * 4);
            memcpy(buffer, pOutput, written_size);

            ma_pcm_rb_commit_write(&user_data->buffer, written_size);
        }

        (void) pInput;
    }

    template <typename T, typename To = T>
    std::vector<To> matrix_to_vector(wt::matrix::Matrix<T> const& m) {
        auto const rows = m.n_rows();
        auto const cols = m.n_cols();

        std::vector<To> ret;
        ret.reserve(rows * cols);
        for (std::size_t r = 0; r < rows; ++r) {
            for (std::size_t c = 0; c < cols; ++c) {
                ret.push_back(m[r][c]);
            }
        }

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

    // result = ma_pcm_rb_init(ma_format format, ma_uint32 channels, ma_uint32 bufferSizeInFrames, void

    // *pOptionalPreallocatedBuffer, const ma_allocation_callbacks *pAllocationCallbacks, ma_pcm_rb *pRB)
    result = ma_decoder_init_file(args->audio_path.c_str(), NULL, &user_data.decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", argv[1]);
        return -2;
    }

    deviceConfig                   = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = user_data.decoder.outputFormat;
    deviceConfig.playback.channels = user_data.decoder.outputChannels;
    deviceConfig.sampleRate        = user_data.decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &user_data.decoder;

    result = ma_pcm_rb_init(
        user_data.decoder.outputFormat, user_data.decoder.outputChannels, 1000, NULL, NULL, &user_data.buffer);

    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", argv[1]);
        return -2;
    }

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

    // Getting a signal from sin, with frequency 16Hz
    const double f = 0.25;
    // const double A = 1;
    const double N   = 100;
    const double T   = 1;
    const double phi = 0;
    std::vector<std::complex<double>> signal;
    for (std::size_t i = 0; i < N; ++i) {
        double x = 2.0 * MATH_PI * i * f * T + phi;
        signal.emplace_back(sin(x));
    }

    // Testing the fourier shit
    std::function<double(const std::complex<double>&)> applier = [](const std::complex<double>& v) -> double {
        return std::abs(v);
    };
    auto transform = wt::ft::fast_fft(signal);

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

    // Create a bar renderer


    // RenderBuilder builder;
    // std::vector<std::unique_ptr<ConcreteRenderer>> renderers;
    // for (std::size_t i = 0; i < result.n_rows(); ++i)
    // {
    // 	renderers.push_back(builder.buildBarRenderer(vs, size(vs), fs,
    // size(fs))); 	renderers[i]->set_offset(1.0f * i);
    // 	renderers[i]->send_gpu_data();
    // }

    // Game loop - Main OpenGL rendering
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        // lookAt = glm::lookAt(cam.pos, cam.pos + cam.getDirection(), cam.getUp());
        look_at = glm::lookAt(cam.pos, cam.pos + cam.getDirection(), cam.getUp());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Temporary code to make sure we can tran
        // std::unique_lock<std::mutex> buffer_lock{ buffer_mutex };
        // transform = wt::ft::fast_fft(buffer);
        // buffer_lock.unlock();
        auto result = wt::matrix::apply(applier, transform);

        // Render all
        // for (std::size_t i = 0; i < renderers.size(); ++i)
        // {
        // 	renderers[i]->set_height(result[i][0]);
        // 	renderers[i]->render(proj, lookAt);
        // }

        auto const heights = matrix_to_vector<double, float>(result);
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
