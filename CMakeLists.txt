cmake_minimum_required(VERSION 3.10.0)
include(GNUInstallDirs)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

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
  "is/msgs/marker.proto"
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
  "is/utils/utils.hpp"
)

list(APPEND sources
  "is/utils/ostream.cpp"
  "is/utils/status.cpp"
  "is/utils/timestamp.cpp"
  "is/utils/validate.cpp"
  ${interfaces}
)

find_package(Protobuf REQUIRED)
find_package(spdlog REQUIRED)

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
  add_executable(timestamp_test is/utils/timestamp.t.cpp)
  set_property(TARGET timestamp_test PROPERTY CXX_STANDARD 11)
  target_link_libraries(timestamp_test PRIVATE is-msgs::is-msgs)
  add_test(NAME timestamp_test COMMAND timestamp_test)

  add_executable(validate_test is/utils/validate.t.cpp)
  set_property(TARGET validate_test PROPERTY CXX_STANDARD 11)
  target_link_libraries(validate_test PRIVATE is-msgs::is-msgs)
  add_test(NAME validate_test COMMAND validate_test)
endif()
