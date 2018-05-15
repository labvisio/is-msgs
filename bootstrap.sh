#!/bin/bash
	
__USER=$USER

# Get super user privileges
if [[ $EUID != 0 ]]; then
	sudo -E "$0" "$@"
	exit $?
fi

apt update
apt install --no-install-recommends -y \
  git build-essential wget python-pip curl autoconf automake libtool unzip pkg-config ca-certificates nasm

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

if ! conan remote list | grep -q "conan"; then 
  echo "|>>| Installing conan..."; 
  pip install --user conan conan_package_tools
fi

if ! conan remote list | grep -q "is:"; then 
  echo "|>>| Adding 'is' remote"; 
  conan remote add is https://api.bintray.com/conan/picoreti/is 
fi 

if ! conan remote list | grep -q "bincrafters:"; then
  echo "|>>| Adding 'bincrafters' remote"; 
  conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
fi 
