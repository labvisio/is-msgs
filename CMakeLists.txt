cmake_minimum_required(VERSION 2.8)
add_definitions(-std=c++11)
project(ismsgs C CXX)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf 3 REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

PROTOBUF_GENERATE_CPP(COMMON_PROTO_SRC COMMON_PROTO_HEADER protos/common.proto)
PROTOBUF_GENERATE_CPP(IMAGE_PROTO_SRC IMAGE_PROTO_HEADER protos/image.proto)
PROTOBUF_GENERATE_CPP(ROBOT_PROTO_SRC ROBOT_PROTO_HEADER protos/robot.proto)
PROTOBUF_GENERATE_CPP(CAMERA_PROTO_SRC CAMERA_PROTO_HEADER protos/camera.proto)
PROTOBUF_GENERATE_CPP(MARKER_PROTO_SRC MARKER_PROTO_HEADER protos/marker.proto)
PROTOBUF_GENERATE_CPP(POWER_PROTO_SRC POWER_PROTO_HEADER protos/power.proto)

set(protos_src
${COMMON_PROTO_SRC} 
${IMAGE_PROTO_SRC} 
${ROBOT_PROTO_SRC} 
${CAMERA_PROTO_SRC} 
${MARKER_PROTO_SRC} 
${POWER_PROTO_SRC} 
)

set(protos_headers
${COMMON_PROTO_HEADER} 
${IMAGE_PROTO_HEADER} 
${ROBOT_PROTO_HEADER} 
${CAMERA_PROTO_HEADER} 
${MARKER_PROTO_HEADER} 
${POWER_PROTO_HEADER} 
)

add_library(ismsgs ${protos_src})
target_link_libraries(ismsgs ${PROTOBUF_LIBRARY})
set_property(TARGET ismsgs PROPERTY CXX_STANDARD 11)
install(TARGETS ismsgs DESTINATION lib)
install(FILES ${protos_headers} DESTINATION include/is/msgs)
