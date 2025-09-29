#include "camera.hpp"
#define GLM_ENABLE_EXPERIMENTAL 1

#include "window.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <cstdint>
#include <iostream>
#include <string>

namespace {
} // namespace


namespace wt {
    Window::Window(std::uint32_t width, std::uint32_t height, std::string const& name)
        : _width{width}, _height{height}, _name{name} {

        GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        _window            = GLFWwindow_ptr(window, [](GLFWwindow* window) { glfwDestroyWindow(window); });


        if (!window) {
            std::cout << "Could not create window. Exiting..." << std::endl;

            // TODO : Need to sort out the catch block where this
            // TODO : gets called
            // glfwTerminate();
            throw WindowCreationException();
        }

        // Useful for getting this info in callbacks
        glfwSetWindowUserPointer(window, this);

        _configure_viewport();
        _configure_input_callbacks();
    }

    void Window::_configure_viewport() {
        glViewport(0, 0, _width, _height);
        glfwSetFramebufferSizeCallback(_window.get(), resize_callback);
        glfwMakeContextCurrent(_window.get());
    }

    void Window::_configure_input_callbacks() {
        glfwSetKeyCallback(_window, Window::keys_callback);
        glfwSetMouseButtonCallback(_window.get(), Window::mouse_button_callback);
        glfwSetCursorPosCallback(_window.get(), Window::cursor_callback);
    }

    void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int /* mods */) {
        auto instance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                // Reset first mouse flag to true when mouse is clicked
                instance->_mouse_down = true;
                instance->_first_mouse = true;
            } else if (action == GLFW_RELEASE) {
                // Save the current rotation when mouse is released
                instance->_mouse_down = false;
                instance->_rot = instance->_curr_rot;
            }
        }
    }

    void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos) {
        auto instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!instance->_mouse_down) {
            return;
        }

        if (instance->_first_mouse) {
            instance->_last_x      = xpos;
            instance->_last_y      = ypos;
            instance->_first_mouse = false;
            return;
        }

        // Calculate mouse movement delta
        float offset_x = xpos - instance->_last_x;
        float offset_y = ypos - instance->_last_y;
        instance->_last_x        = xpos;
        instance->_last_y        = ypos;

        // Sensitivity factor to control rotation speed
        constexpr float SENSITIVITY = 0.005f;
        offset_x *= SENSITIVITY;
        offset_y *= SENSITIVITY;

        // Create rotation matrices for x and y rotation
        glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), offset_y, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), offset_x, glm::vec3(0.0f, 1.0f, 0.0f));

        // Combine rotations (order matters!)
        glm::mat4 const newRotation = yRotation * xRotation;

        // Apply to the existing rotation matrix
        instance->_curr_rot = newRotation * instance->_curr_rot;
    }

    void keys_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) {
        auto instance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        Camera& cam = instance->_cam;

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


    void Window::resize_callback(GLFWwindow* /* window */, int width, int height) {
        glViewport(0, 0, width, height);
    }

}; // namespace wt
