include(CMakeFindDependencyMacro)

find_dependency(Protobuf)
find_dependency(Boost COMPONENTS filesystem)

include("${CMAKE_CURRENT_LIST_DIR}/is-msgsTargets.cmake")
