#include <fourier/dft_operations.h>
#include <shaders/shader_program.h>

// More testing for why things arent working
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define MINIAUDIO_IMPLEMENTATION 
extern "C" {
	#include <miniaudio.h>
}

// #include "GLAbstractions/vao.h"
// #include "GLAbstractions/vbo.h"
// #include "GLAbstractions/vertex_attribute.h"

#include <chrono>
#include <cmath>
#include <complex>
#include <iostream>
// #include <mutex>
// #include <thread>

// #include "graphics/draw_buffer.h"
// #include "graphics/draw_data2.h"
// #include "graphics/draw_data3.h"
// #include "graphics/entity.h"
// #include "Renderer/concrete_renderer.h"
// #include "render_builder.h"

// #include "Shaders/vs.glsl.h"
// #include "Shaders/vs_test.glsl.h"
// #include "Shaders/fs.glsl.h"
// #include "Shaders/fs_test.glsl.h"

// #include "DataStructures/byte_array.h"

namespace {
	void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
		if (pDecoder == NULL) {
			return;
		}

		ma_uint64 frames_read = 0;
		ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &frames_read);

		auto* samples = reinterpret_cast<float*>(pOutput);
		ma_uint32 channel_count = pDevice->playback.channels;

		std::vector<float> data;
		for (ma_uint64 i = 0; i < frames_read * channel_count; ++i) {
			data.push_back(samples[i]);
		}

		auto const test = data.size();

		(void)test;
		(void)pInput;
	}
} // namespace

template<class T, size_t N>
constexpr size_t size(T(&)[N])
{
	return N;
}

struct Camera
{
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 transform;


	glm::vec3 getDirection() const
	{
		return transform * glm::vec4(front, 1);
	}

	glm::vec3 getUp() const
	{
		return transform * glm::vec4(up, 1);
	}

	glm::vec3 getRight() const
	{
		return transform * glm::vec4(right, 1);
	}
};


void resizeCallback(GLFWwindow* /* window */, int width, int height)
{
	glViewport(0, 0, width, height);
}

// TODO : encapsulate all of this global look at stuff
Camera cam;
glm::vec3 eye;
glm::vec3 pos;
glm::vec3 up;
glm::mat4 lookAt;

enum class AXIS {
	X,
	Y,
	Z
};
glm::vec3 rotateVector(const glm::vec3& vector, const glm::vec3& from, const AXIS& axis, const float& angle)
{
	const glm::vec3 INV_VECTOR{
		-from.x,
		-from.y,
		-from.z
	};

	switch (axis) {
	case AXIS::X:
	{
		const glm::vec3 ROT_AXIS{ 1, 0, 0 };
		const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
		glm::vec3 retVal = ROT_MATRIX * glm::vec4{ vector.x + INV_VECTOR.x,
			vector.y + INV_VECTOR.y,
			vector.z + INV_VECTOR.z,
			1 };

		return retVal - INV_VECTOR;
	}
	case AXIS::Y:
	{
		const glm::vec3 ROT_AXIS{ 0, 1, 0 };
		const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
		glm::vec3 retVal = ROT_MATRIX * glm::vec4{ vector.x + INV_VECTOR.x,
			vector.y + INV_VECTOR.y,
			vector.z + INV_VECTOR.z,
			1 };
		return retVal - INV_VECTOR;
	}
	case AXIS::Z:
	{
		const glm::vec3 ROT_AXIS{ 0, 0, 1 };
		const glm::mat4& ROT_MATRIX = glm::rotate(angle, ROT_AXIS);
		glm::vec3 retVal = ROT_MATRIX * glm::vec4{ vector.x + INV_VECTOR.x,
			vector.y + INV_VECTOR.y,
			vector.z + INV_VECTOR.z,
			1 };
		return retVal - INV_VECTOR;
	}
	}
}

void keyCallBack(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		cam.transform = glm::rotate(glm::radians(10.f), cam.getRight()) * cam.transform;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		cam.transform = glm::rotate(glm::radians(-10.f), cam.getRight()) * cam.transform;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		cam.transform = glm::rotate(glm::radians(10.f), cam.getUp()) * cam.transform;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		cam.transform = glm::rotate(glm::radians(-10.f), cam.getUp()) * cam.transform;
	}

	// Movements
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		cam.pos += cam.getDirection() * 0.5f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		cam.pos -= cam.getDirection() * 0.5f;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		cam.pos += cam.getRight() * 0.5f;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		cam.pos -= cam.getRight() * 0.5f;
	}
}

unsigned vaoId = 0;
unsigned vertexBufferId = 0;
unsigned normalBufferId = 0;

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
int main(int argc, char** argv)
{
	// MARK: Begin of audio
	ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

	if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    result = ma_decoder_init_file(argv[1], NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", argv[1]);
        return -2;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

	// MARK: End of audio

	using namespace std::chrono_literals;
	// std::chrono::high_resolution_clock c;

	// Getting a signal from sin, with frequency 16Hz
	const double f = 0.25;
	// const double A = 1;
	const double N = 64;
	const double T = 1;
	const double phi = 0;
	std::vector<std::complex<double>> signal;
	for (std::size_t i = 0; i < N; ++i)
	{
		double x = 2.0 * MATH_PI * i * f * T + phi;
		signal.emplace_back(cos(x));
	}

	// Testing the fourier shit
	std::function<double(const std::complex<double>&)> applier = [](const std::complex<double>& v) -> double
	{
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

	// glm::mat4 proj = glm::perspective<float>(
	// 	glm::radians(45.0f),
	// 	800 / 600,
	// 	0.01,
	// 	100
	// 	);


	// Callback from commands
	glfwSetKeyCallback(window, keyCallBack);

	cam.pos = { 0,  0, 10 };
	cam.front = { 0,  0, -1 };
	cam.right = { 1,  0,  0 };
	cam.up = { 0,  1,  0 };
	cam.transform = glm::mat4{ 1 };

	eye = { 0, 0, 5 };
	pos = { 0, 0, -1 };
	up = { 0, 1, 0 };

	// Create a bar renderer
	// RenderBuilder builder;

	// std::vector<std::unique_ptr<ConcreteRenderer>> renderers;
	// for (std::size_t i = 0; i < result.n_rows(); ++i)
	// {
	// 	renderers.push_back(builder.buildBarRenderer(vs, size(vs), fs, size(fs)));
	// 	renderers[i]->set_offset(1.0f * i);
	// 	renderers[i]->send_gpu_data();
	// }

	// Game loop - Main OpenGL rendering
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		lookAt = glm::lookAt(
			cam.pos,
			cam.pos + cam.getDirection(),
			cam.getUp()
		);

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

		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
	return 0;
}