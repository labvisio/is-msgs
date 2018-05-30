#!/bin/bash
	
# Get super user privileges
if [[ $EUID != 0 ]]; then
	sudo -E "$0" "$@"
  exit $?
fi

set -e

apt update
apt install --no-install-recommends -y \
  git build-essential wget python-pip curl python-setuptools autoconf \
  automake libtool unzip pkg-config ca-certificates nasm

invalid_cmake_version=false
if command -v cmake > /dev/null ; then 
  cmake_version=`cmake --version | grep -o -E "([0-9]{1,}\.)+[0-9]{1,}"`
  cmake_version=(`echo $cmake_version | tr . ' '`)
  if [ ${cmake_version[1]} -lt 10 ]; then
    echo "|>>| cmake 3.10+ required"
    invalid_cmake_version=true
  fi;
fi

if [[ -z `command -v cmake` ]] || [[ $invalid_cmake_version == true ]]; then
  echo "|>>| installing cmake..."; 
  wget https://cmake.org/files/v3.11/cmake-3.11.1-Linux-x86_64.sh
  mkdir -p /opt/cmake
  sh cmake-3.11.1-Linux-x86_64.sh --skip-license --prefix=/opt/cmake
  ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
  rm cmake-3.11.1-Linux-x86_64.sh
fi

if ! command -v ninja > /dev/null; then 
  echo "|>>| installing ninja..."; 
  wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
  unzip ninja-linux.zip
  rm ninja-linux.zip
  mv ninja /usr/bin
fi

if ! command -v conan > /dev/null; then 
  echo "|>>| Installing conan..."; 
  pip install conan 
fi

if ! conan remote list | grep -q "bincrafters:"; then
  echo "|>>| Adding 'bincrafters' remote"; 
  conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
fi

if ! conan remote list | grep -q "is:"; then
  echo "|>>| Adding 'is' remote"; 
  conan remote add is https://api.bintray.com/conan/labviros/is
fi