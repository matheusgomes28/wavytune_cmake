from conans import ConanFile

class WavyTuneConan(ConanFile):

	settings = ("os", "compiler", "build_type", "arch")
	generators = ("cmake_find_package")
	requires = (
		"glad/0.1.36",
		"glfw/3.3.2",
		"glm/0.9.9.8",
		"gtest/1.10.0",
		"ms-gsl/3.1.0"
		)


	def configure(self):
		self.options['glad'].shared = False
		self.options['glad'].no_loader = False
		self.options['glad'].spec = 'gl'
		self.options['glad'].gl_profile = 'core'
		self.options['glad'].gl_version = '3.3'

	def imports(self):
		self.copy("*.dll", dst="bin", src="bin")
		self.copy("*.dylib", dst="bin", src="lib")
