#!/bin/bash

mkdir -p js cpp py java
protoc -I `pwd` --js_out=./js --cpp_out=./cpp --java_out=./java --python_out=./py *.proto 

