cmake_minimum_required(VERSION 3.15.0)
project(is-msgs)

include(GNUInstallDirs)

set(build_tests true CACHE BOOL "build tests")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (build_tests)
  enable_testing()
endif()

add_subdirectory(src)