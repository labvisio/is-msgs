import os
import re

from conan import ConanFile
from conan.tools.files import copy, rmdir, load
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class IsMsgsConan(ConanFile):
    name = "is-msgs"
    license = "MIT"
    url = "https://github.com/labvisio/is-msgs"
    description = "Repository containing the schema for standard ::is messages"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "build_tests": False,
    }
    exports_sources = "*"

    @property
    def version_pattern(self):
        return r"^[0-9]+\.[0-9]+\.[0-9]+$"

    def set_version(self):
        self.version = load(self, ".version").strip()
        if re.match(self.version_pattern, self.version) is None:
            raise RuntimeError(
                f"Package version must matches the pattern '{self.version_pattern}'"
            )

    def requirements(self):
        self.requires("fmt/7.1.3")
        self.requires("boost/1.80.0")
        self.requires("protobuf/3.20.0", force=True)
        if self.options.build_tests:
            self.requires("gtest/1.10.0")
            self.requires("opencv/4.5.5")
            self.requires("libwebp/1.3.0", override=True)
            self.requires("libjpeg-turbo/2.1.5", override=True)

    def build_requirements(self):
        self.tool_requires("protobuf/3.20.0")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
        if self.options.build_tests:
            self.options["opencv"].shared = self.options.shared
            self.options["opencv"].parallel = "tbb"
            self.options["opencv"].with_jpeg = "libjpeg-turbo"
            self.options["opencv"].with_ade = False
            self.options["opencv"].with_gtk = False
            self.options["opencv"].with_quirc = False
            self.options["opencv"].with_ffmpeg = False
            self.options["opencv"].with_msmf = False
            self.options["opencv"].with_msmf_dxva = False
            self.options["opencv"].neon = False
            self.options["opencv"].dnn = True
        self.options["boost"].shared = self.options.shared
        self.options["protobuf"].shared = self.options.shared
        self.options["fmt"].shared = self.options.shared

    def layout(self):
        cmake_layout(self, src_folder="./")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["build_tests"] = self.options.build_tests
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.build_tests:
            cmake.test()

    def package(self):
        copy(self,
             "LICENSE.txt",
             src=self.source_folder,
             dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))
        rmdir(self, os.path.join(self.package_folder, "cmake"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "is-msgs")
        self.cpp_info.set_property("cmake_target_name", "is-msgs::is-msgs")
        self.cpp_info.set_property("pkg_config_name", "is-msgs")
        self.cpp_info.components["is-msgs"].libs = ["is-msgs"]
        self.cpp_info.components["is-msgs"].requires = [
            "fmt::fmt",
            "protobuf::libprotobuf",
            "boost::boost",
        ]
        if self.options.build_tests:
            self.cpp_info.components["is-msgs"].requires.extend([
                "gtest::gtest",
                "opencv::opencv",
            ])
