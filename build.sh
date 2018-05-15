#!/bin/bash

mkdir -p build
cd build
conan install .. -s compiler.libcxx=libstdc++11 --build=missing
cmake -GNinja ..
ninja 
if ninja help | grep -q "test:"; then
  ninja test
fi