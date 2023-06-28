#!/bin/bash
	
# Get super user privileges
if [[ $EUID != 0 ]]; then
  export wasnt_root=true
  sudo -E "$0" "$@"
fi

if [[ $EUID == 0 ]]; then
  echo "[$EUID] |>>| installing distro packages"
  apt-get update
  apt-get install --no-install-recommends -y \
    python3                                  \
    python3-pip                              \
    python3-venv                             \
    python3-setuptools                       \
    git                                      \
    wget                                     \
    curl                                     \
    nasm                                     \
    sudo                                     \
    unzip                                    \
    libtool                                  \
    autoconf                                 \
    automake                                 \
    pkg-config                               \
    build-essential                          \
    ca-certificates

  invalid_cmake_version=false
  if command -v cmake > /dev/null ; then 
    cmake_version=`cmake --version | grep -o -E "([0-9]{1,}\.)+[0-9]{1,}"`
    cmake_version=(`echo $cmake_version | tr . ' '`)
    if [ ${cmake_version[1]} -lt 15 ]; then
      echo "[$EUID] |>>| cmake 3.15+ required"
      invalid_cmake_version=true
    fi;
  fi

  if [[ -z `command -v cmake` ]] || [[ $invalid_cmake_version == true ]]; then
    echo "[$EUID] |>>| installing cmake..."; 
    wget https://cmake.org/files/v3.20/cmake-3.20.0-linux-x86_64.sh
    mkdir -p /opt/cmake
    sh cmake-3.20.0-linux-x86_64.sh --skip-license --prefix=/opt/cmake
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
    rm cmake-3.20.0-linux-x86_64.sh
  fi
fi

if [[ $EUID != 0 ]] || [[ !$wasnt_root ]]; then
  pip3 install conan --upgrade
  if [[ ! -f $HOME/.conan2/profiles/default ]]; then
     conan profile detect
  fi

  if ! grep -q "tools.system.package_manager:mode=install" $HOME/.conan2/global.conf; then
    echo -e "tools.system.package_manager:mode=install" | tee -a $HOME/.conan2/global.conf
  fi

  if ! grep -q "tools.system.package_manager:sudo=True" $HOME/.conan2/global.conf; then
    echo -e "tools.system.package_manager:sudo=True" | tee -a $HOME/.conan2/global.conf
  fi
fi