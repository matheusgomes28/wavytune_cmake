#ifndef WINDOW_H
#define WINDOW_H

#include "camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <exception>
#include <functional>
#include <memory>
#include <string>

struct GLFWwindow;

namespace wt {
    using GLFWwindow_ptr = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>;

    class WindowCreationException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "error creating the window";
        }
    };

    class Window {
    public:
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
        static void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void resize_callback(GLFWwindow* window, int width, int height);

        Window(std::uint32_t width, std::uint32_t height, std::string const& name);

        /// @brief Processes the frame information. Currently, this function
        /// will poll events and swap the buffers.
        void process_frame();

        /// @brief Queries whether the window was closed
        /// @return true if the window was closed, false otherwise
        [[nodiscard]] bool closed() const;


        /// @biref Gets the current camera state for this window
        /// @return the Camera object with current view state
        Camera camera() const;

        /// @brief gets the current rotation matrix
        /// @return the rotation matrix
        glm::mat4 rotation() const;


    private:
        void _configure_viewport();
        void _configure_input_callbacks();

        std::uint32_t _width;
        std::uint32_t _height;
        std::string _name;
        GLFWwindow_ptr _window;

        // TODO : These are the states of the rendering
        // TODO : which should be elsewhere but here
        Camera _cam;
        glm::mat4 _rot;
        glm::mat4 _curr_rot;
        bool _mouse_down;
        bool _first_mouse;
        float _last_x;
        float _last_y;
    };
} // namespace wt


#endif // WINDOW_H
