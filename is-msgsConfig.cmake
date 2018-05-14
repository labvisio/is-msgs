include(CMakeFindDependencyMacro)

find_dependency(Protobuf)
find_dependency(spdlog)

include("${CMAKE_CURRENT_LIST_DIR}/is-msgsTargets.cmake")
