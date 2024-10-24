#!/bin/sh

# Setup variables

base_dir=$(pwd)
cmake_subdir=$2
cmake_args=$3
num_cores=$(nproc --all)
DefaultGenerator="Ninja Multi-Config"

cd $1

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    platform="Linux"
    SystemName="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    platform="Mac"
    DefaultGenerator="Xcode"
    SystemName="Darwin"
elif [[ "$OSTYPE" == "cygwin" ]]; then
    platform="Windows"
    SystemName="Windows"
    DefaultGenerator="Visual Studio 17"
elif [[ "$OSTYPE" == "msys" ]]; then
    platform="Windows"
    SystemName="Windows"
    DefaultGenerator="Visual Studio 17"
elif [[ "$OSTYPE" == "win32" ]]; then
    platform="Windows"
    SystemName="Windows"
    DefaultGenerator="Visual Studio 17"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    platform="Linux"
    SystemName="Linux"
else
    echo "Unknown Operating System"
    exit 1
fi

cmake -B "${base_dir}/Build/${platform}" -S "${base_dir}" -G "${DefaultGenerator}"

