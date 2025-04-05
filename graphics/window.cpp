#include <GLFW/glfw3.h>

#include <graphics/window.h>

bool wt::window_init()
{
  int result = glfwInit();
  return result == GLFW_TRUE ? true : false;
}

void wt::window_uninit() { glfwTerminate(); }

GLFWwindow *wt::create_window(int width, int height, std::string const &title)
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  return glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
}