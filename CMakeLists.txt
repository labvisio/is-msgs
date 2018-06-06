cmake_minimum_required(VERSION 3.10.0)
include(GNUInstallDirs)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

set(enable_tests true CACHE BOOL "enable testing")
if (enable_tests)
  enable_testing()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(author "is")
set(module "msgs")

set(target ${author}-${module})
set(include_dir ${author}/${module})

list(APPEND protos
  "is/msgs/camera.proto"
  "is/msgs/common.proto"
  "is/msgs/image.proto"
  "is/msgs/power.proto"
  "is/msgs/robot.proto"
  "is/msgs/wire.proto"
  "is/msgs/validate.proto"
  "is/msgs/tests.proto"
)

list(APPEND interfaces
  "is/utils/ostream.hpp"
  "is/utils/status.hpp"
  "is/utils/timestamp.hpp"
  "is/utils/validate.hpp"
  "is/utils/io.hpp"
  "is/utils/cv.hpp"
  "is/utils/utils.hpp"
)

list(APPEND sources
  "is/utils/ostream.cpp"
  "is/utils/status.cpp"
  "is/utils/timestamp.cpp"
  "is/utils/validate.cpp"
  "is/utils/io.cpp"
  ${interfaces}
)

list(APPEND tests
  "is/utils/timestamp.t.cpp"
  "is/utils/validate.t.cpp"
  "is/utils/io.t.cpp"
  "is/utils/cv.t.cpp"
)

# public
find_package(Protobuf REQUIRED)
# private
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(opencv REQUIRED)

#######
####
#######

set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
foreach(proto ${protos})
  PROTOBUF_GENERATE_CPP(src hdr ${proto})
  list(APPEND sources ${src} ${hdr})
  list(APPEND interfaces ${hdr} ${proto})
endforeach()

add_library(${target} ${sources})

# compile options
set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)

# link dependencies
target_link_libraries(
  ${target}
 PUBLIC
  protobuf::libprotobuf
 PRIVATE
  spdlog::spdlog
  Boost::filesystem
)

# header dependencies
target_include_directories(
  ${target}
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
)

install(FILES ${interfaces} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${include_dir})

set(export_targets      ${target}Targets)
set(export_targets_file ${export_targets}.cmake)
set(export_namespace    ${author}-${module}::)
set(export_destination  ${CMAKE_INSTALL_LIBDIR}/cmake/${target})
set(export_config_file  ${target}Config.cmake)

install(
  TARGETS   ${target}
  EXPORT    ${export_targets}
  LIBRARY   DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE   DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME   DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# install export target
install(
  EXPORT      ${export_targets}
  FILE        ${export_targets_file}
  NAMESPACE   ${export_namespace}
  DESTINATION ${export_destination}
)

# install export config
install(FILES ${export_config_file} DESTINATION ${export_destination})

# create library alias (less error prone to typos)
set(target_alias ${export_namespace}${target})
add_library(${target_alias} ALIAS ${target})

######
###
######

if (enable_tests)
  enable_testing()
  find_package(GTest REQUIRED)

  function(gtest_add target source)
    add_executable(${target} ${source})
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)
    target_link_libraries(${target} PRIVATE GTest::GTest GTest::Main ${target_alias} opencv::opencv Boost::filesystem)
    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
  endfunction()

  foreach(test ${tests})
    get_filename_component(test_target ${test} NAME_WE)
    gtest_add(${test_target}_test ${test})
  endforeach(test)
endif()
