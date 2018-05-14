#!/bin/bash
	
__USER=$USER

# Get super user privileges
if [[ $EUID != 0 ]]; then
	sudo -E "$0" "$@"
	exit $?
fi

apt update
apt install --no-install-recommends -y \
  git build-essential wget python-pip cmake curl autoconf automake libtool unzip pkg-config ca-certificates nasm

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
