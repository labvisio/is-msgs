cmake_minimum_required(VERSION 3.0)
project(ismsgs C CXX)

include(GNUInstallDirs)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf 3 REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

file(GLOB protos "protos/*.proto")
foreach(proto ${protos})
  PROTOBUF_GENERATE_CPP(SRC HEADER ${proto})
  set(PROTO_SRCS "${PROTO_SRCS};${SRC}")
  set(PROTO_HEADERS "${PROTO_HEADERS};${HEADER}")
endforeach()

add_library(ismsgs ${PROTO_SRCS})
set_property(TARGET ismsgs PROPERTY CXX_STANDARD 11)
target_link_libraries(
  ismsgs
 PUBLIC 
  ${Protobuf_LIBRARIES}
)

target_include_directories(
  ismsgs 
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

install(
  TARGETS ismsgs
  EXPORT ismsgsTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

#install headers
install(
  FILES ${PROTO_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#install protos
install(
  FILES ${protos}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#install config
install(
  EXPORT ismsgsTargets
  FILE ismsgsConfig.cmake
  NAMESPACE ismsgs::
  DESTINATION lib/cmake/ismsgs
)

# register project in CMake user registry
export(PACKAGE ${PROJECT_NAME})
