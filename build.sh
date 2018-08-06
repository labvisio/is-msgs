#!/bin/bash

project_dir=$PWD

mkdir -p $project_dir/build && cd $project_dir/build
conan install .. -s compiler.libcxx=libstdc++11 --build=missing -o is-msgs:build_tests=True
conan build ..