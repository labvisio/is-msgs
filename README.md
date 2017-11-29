is-msgs
==========

Repository containing all the standard protobuf messages definitions for the IS framework. 
Auto generate documentation can be found in the *docs* folder.

Using the messages
---------------------

*C++*: Building a static library (This is done automatically if you used the installation script) - [Help!](https://developers.google.com/protocol-buffers/docs/reference/cpp-generated)
```shell
mkdir build
cd build
cmake ..
sudo make install
```

*Javascript*: Bundles all messages into a minified file (*gen_js/is_msgs_min.js*) - [Help!](https://developers.google.com/protocol-buffers/docs/reference/javascript-generated)
```shell
npm run bundle
```

*NodeJS*: Generate one .js file for each protobuf definition (*gen_js/..._pb.js*) - [Help!](https://developers.google.com/protocol-buffers/docs/reference/javascript-generated)
```shell
npm run bundle
```

*Python*: Generating python files - [Help!](https://developers.google.com/protocol-buffers/docs/reference/python-generated)
```shell
mkdir gen_py
cd protos
protoc --proto_path=. --python_out=../gen_py *.proto
```