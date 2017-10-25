#!/bin/bash

#mkdir -p ../js ../cpp ../py ../java
#protoc -I `pwd` --js_out=import_style=commonjs,binary:./../js --cpp_out=./../cpp --java_out=./../java --python_out=./../py *.proto 

protoc  --plugin=protoc-gen-doc=$GOPATH/bin/protoc-gen-doc --doc_out=../docs --doc_opt=html,index.html *.proto
