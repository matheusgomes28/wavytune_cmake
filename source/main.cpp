#include <GL/glew.h>

#include <graphics/window.h>
#include <matrix/matrix.h>

#include <iostream>

void resize_callback(GLFWwindow * /* window */, int width, int height)
{
  glViewport(0, 0, width, height);
}

int main()
{
  glewInit();

  wt::window_init();
  auto window = wt::create_window(800, 600, "Testing");
  if (!window)
  {
    wt::window_uninit();
    return 0;
  }

  // Attach viewport
  glewInit();
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwMakeContextCurrent(window);

  // VAO vao{"vao testing"};
  wt::matrix::Matrix<double> hello{2, 2};
  // ByteArray hello2(20);

  // wt::ByteArray<10> hello1{10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

  std::cout << "Hello World!" << std::endl;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  wt::window_uninit();
  return 0;
}
