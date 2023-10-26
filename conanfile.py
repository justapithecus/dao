from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

REQUIRES = (
  "approvaltests.cpp/10.12.2",
  "boost-ext-ut/2.0.0",
  "nlohmann_json/3.11.2"
)


class DaoConanFile(ConanFile):
  name = "Dao"

  settings = "os", "arch", "compiler", "build_type"
  generators = "CMakeDeps"

  requires = REQUIRES

  def generate(self):
    tc = CMakeToolchain(self, generator="Ninja")
    tc.generate()
