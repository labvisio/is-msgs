is-msgs
==========

Repository containing all the standard protobuf messages definitions for the *is::* framework. 
Auto generated documentation can be found in the [docs folder](docs/README.md)

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

Install build dependencies and compile .proto schemas to .js files:
```shell
npm install # to get build dependencies
npm run generate # to generate js files
```

Now to use it on server side (nodejs) simply import and use the generated files, i.e:
```js
const common = require("./is/msgs/common_pb.js");

let tensor = new common.Tensor();
tensor.setDoublesList([1, 2, 3]);
console.log(tensor.toObject());
// ...
```

To use it on the browser, browserify the files by running:
```shell
npm run browserify 
```

Now to use it on the browser, include the browserified bundle, i.e:
```html
<script src="is_msgs.js"></script>
<script>
  var tensor = new proto.is.msgs.common.Tensor();
  tensor.setDoublesList([1, 2, 3]);
  console.log(tensor.toObject());
</script>
// ...
```

## *Python* [Help!](https://developers.google.com/protocol-buffers/docs/reference/python-generated)
To use protocol buffers with Python, simply run:

```shell
pip install --user is-msgs
```

If your project contains messages using a message from this repository, e.g.:

```protobuf
syntax = "proto3";

import "is/msgs/common.proto";

message MyMessage {
  is.common.Pose pose = 1;
}
```

You can build using:

```shell
python -m is_msgs.utils.build <LIST_OF_PROTO_FILES>
```