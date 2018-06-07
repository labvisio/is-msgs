#!/bin/bash

mkdir -p build && cd build
conan install .. -s compiler.libcxx=libstdc++11 --build=missing
conan build ..