from conans import ConanFile, CMake, tools


class IsmsgsConan(ConanFile):
    name = "is-msgs"
    version = "1.1.8"
    license = "MIT"
    url = "https://github.com/labviros/is-msgs"
    description = "Repository containing the schema for standard ::is messages"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "build_tests": [True, False]}
    default_options = "shared=False", "fPIC=True", "build_tests=False"
    generators = "cmake", "cmake_find_package", "cmake_paths"
    requires = ("protobuf/3.6.1@bincrafters/stable", "boost/[>=1.65]@conan/stable",
                "spdlog/[>0.15]@bincrafters/stable")
    exports_sources = "*"

    def build_requirements(self):
        if self.options.build_tests:
            self.build_requires("opencv/[>=3.3]@is/stable")
            self.build_requires("gtest/1.8.0@bincrafters/stable")

    def configure(self):
        if self.options.build_tests:
            self.options["opencv"].with_qt = False

        self.options["spdlog"].fmt_external = False
        if self.options.shared:
            self.options["boost"].fPIC = True

    def build(self):
        cmake = CMake(self, generator='Ninja')
        cmake.definitions["CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.fPIC
        cmake.definitions["enable_tests"] = self.options.build_tests
        cmake.configure()
        cmake.build()
        if self.options.build_tests:
            cmake.test()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["is-msgs"]
