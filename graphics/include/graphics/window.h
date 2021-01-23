#include <GLFW/glfw3.h>

#include <string>

namespace wt
{
	bool window_init();

	GLFWwindow* create_window(int width, int height, std::string const& title);

	void window_uninit();
} // namespace wt