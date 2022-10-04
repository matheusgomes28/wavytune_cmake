from conans import ConanFile

class WavyTuneConan(ConanFile):

	settings = ("os", "compiler", "build_type", "arch")
	generators = ("cmake_find_package")
	requires = (
		"glew/2.1.0",
		"glfw/3.3.2",
		"glm/0.9.9.8",
		"gtest/1.10.0",
		"ms-gsl/3.1.0"
		)


	def imports(self):
		self.copy("*.dll", dst="bin", src="bin")
		self.copy("*.dylib", dst="bin", src="lib")
