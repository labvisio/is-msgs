is-msgs
==========

Repository containing all the standard protobuf messages definitions for the *is::* framework. The .proto definitions are used to generate code in other programming languages. In order to use that generated code you need to know the conventions for your language of choice:
  - [C++](https://developers.google.com/protocol-buffers/docs/reference/cpp-generated)
  - [Javascript/ NodeJS](https://developers.google.com/protocol-buffers/docs/reference/javascript-generated)
  - [Python](https://developers.google.com/protocol-buffers/docs/reference/python-generated)

Documentation for the messages and their fields can be found in the [docs folder](docs/README.md).

## *C++*
##### BUILDING FROM SOURCE 
To build this library from source first run the bootstrap script to get build dependencies, then run the build script. 
```shell
./bootstrap.sh # to get build dependencies
./build.sh # to build library
```
##### CONAN PACKAGE
A conan artifact is available in our lab.  **Contact the maintainers to gain access to download from our conan server. It's not public for everyone.**

##### USAGE

```c++
#include <is/msgs/common.pb.h>

is::common::Tensor tensor;
```

## *Javascript / NodeJS*
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

## *Python* 

Documentation about the usage of this package in python can be found at [python/README.md](python/README.md#usage).

## Releasing new versions

First of all, bumps the version on the `.version` file following the pattern `^[0-9]+\.[0-9]+\.[0-9]+$`.

#### Post release steps

After release `is_msgs` in any programing language, the protobuf's documentation available `docs/README.md` file will be automatically updated. Remember to commit and push those changes, as well as creates a tag on git related to the new version. To do so, run the following commands:

```shell
git tag v$(cat .version)
git push origin v$(cat .version)
```

### Python

To release a new python package, check the docs at [python/README.md](python/README.md#releasing-new-versions).

### C++

> Under construction