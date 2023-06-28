# is-msgs

[![PyPI](https://img.shields.io/pypi/v/is-msgs.svg?style=for-the-badge)](https://pypi.org/project/is-msgs/)
[![Build](https://img.shields.io/github/actions/workflow/status/labvisio/is-msgs/main.yml?style=for-the-badge)](https://github.com/labvisio/is-msgs/actions)
[![Python suport](https://img.shields.io/pypi/pyversions/is-msgs?style=for-the-badge)](https://pypi.org/project/is-msgs)
[![Downloads](https://img.shields.io/pypi/dm/is-msgs?style=for-the-badge)](https://pypi.org/project/is-msgs/)

Repository containing the schema for standard ::is messages.

## Usage

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


## Releasing new versions

It's important to test your package before release it. You can do that installing it locally running the command bellow. It's recommended to use virtual environments.

```shell
python setup.py install --user
```

Ensure that the new features added are working properly. Afterwards, you'll need do build a tarball file with package's content.

```shell
python setup.py sdist
```

A `.tar.gz` file will be created on `dist/` folder, containing the version on its name. Now, use the `twine` tool to send you package to `PyPi`. Username and password will be required.

```shell
twine upload dist/is_msgs-$(cat .version).tar.gz
```