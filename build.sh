#!/bin/bash

set -e

mkdir -p build && cd build
conan install .. -s compiler.libcxx=libstdc++11 --build=missing -o is-msgs:build_tests=True
conan build ..
