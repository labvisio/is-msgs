include(CMakeFindDependencyMacro)

find_dependency(Protobuf)
find_dependency(spdlog)
find_dependency(Boost)

include("${CMAKE_CURRENT_LIST_DIR}/is-msgsTargets.cmake")
