from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

class DaoConanFile(ConanFile):
    name = "Dao"

    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.generate()
