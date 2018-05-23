is-msgs
==========

Repository containing all the standard protobuf messages definitions for the *is::* framework. 
Auto generate documentation can be found in the *docs* folder.

## *C++* [Help!](https://developers.google.com/protocol-buffers/docs/reference/cpp-generated)
##### BUILDING FROM SOURCE 
To build this library from source first run the bootstrap script to get build dependencies, then run the build script. - 
```shell
./bootstrap.sh # to get build dependencies
./build.sh # to build library
```
##### USING CONAN PACKAGE MANAGER
A conan artifact is available in our [bintray repository](https://bintray.com/labviros/is/is-msgs%3Ais)

## *Javascript / NodeJS* [Help!](https://developers.google.com/protocol-buffers/docs/reference/javascript-generated)
To use protocol buffers with JavaScript you need the protocol compiler *protoc* download a [pre-built binary on GitHub](https://github.com/google/protobuf/releases).

To generate the javascript files and the bundled/minified version run:
```shell
npm install # to get build dependencies
npm run bundle # to generate js files and minified bundle
```