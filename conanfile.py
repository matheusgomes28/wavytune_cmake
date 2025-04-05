from conan import ConanFile


class WavyTuneConan(ConanFile):
	settings = ("os", "compiler", "build_type", "arch")
	generators = "CMakeDeps", "CMakeToolchain"

	def requirements(self):
		self.requires("glew/2.2.0"),
		self.requires("glfw/3.4"),
		self.requires("glm/1.0.1"),
		self.requires("gtest/1.16.0"),
		self.requires("ms-gsl/4.1.0"),

	# def imports(self):
	# 	self.copy("*.dll", dst="bin", src="bin")
	# 	self.copy("*.dylib", dst="bin", src="lib")
